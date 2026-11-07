// Native-backed client peer authentication operation and tracked asynchronous flow.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <time.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONAuthPeerData.h"
namespace WONAPI {
// FUNCTION: WON 0x006e6a10
PeerAuthOp::PeerAuthOp(const IPAddr &theAddr, AuthContext *theAuthContext, AuthType theType, 
					   BlockingSocket *theSocket, bool useAuth2) : mConnectAddr(theAddr)
{
	mAuthContext = theAuthContext;
	mAuthType = theType;
	mSocket = theSocket;
	mPeerAuthClient.SetUseAuth2(useAuth2);
}

// FUNCTION: WON 0x006e6d10
PeerAuthOp::~PeerAuthOp()
{
}

// FUNCTION: WON 0x006e6ed0
bool PeerAuthOp::CallbackHook(AsyncOp *theOp, int theParam)
{
	switch(theParam)
	{
		case PeerAuth_Track_Socket:
		{
			if(theOp->GetStatus()!=0)
			{
				Fn29F220(theOp->GetStatus());
				break;
			}

			if(mAuthType==AUTH_TYPE_NONE) // just connecting
			{
				Fn29F220(0);
				break;
			}

			RecvMsgOp *aRecvOp = dynamic_cast<RecvMsgOp*>(theOp);
			if(aRecvOp==NULL)
				break;

			ByteBufferPtr aSendMsg;
			ByteBufferPtr aRecvMsg = aRecvOp->GetMsg();
			WONStatus aStatus = mPeerAuthClient.HandleRecvMsg(aRecvMsg->data(),aRecvMsg->length(),aSendMsg);
			if(aStatus==(-4) || aStatus==(-19)) // expired certificate
			{
				mPeerAuthClient.Reset();
				switch(mGetCertStatus)
				{
					case 100004: AsyncRefreshCert(); break;
					case 100000: break;
					case 0: AsyncRetry(); break;
					default: Fn29F220(100053);
				}	

				break;
			}
			else if(CheckStatus(aStatus))
				break;

			if(aSendMsg.get()==NULL) 
				Success(); 				// done
			else
				AsyncSend(aSendMsg.get()); 	// Need to send more stuff
		}
		break;


		case PeerAuth_Track_GetCert:
		{
			LightGetCertOp *aGetCert = (LightGetCertOp*)theOp;
			mGetCertStatus = aGetCert->GetStatus();
			mPeerData = aGetCert->GetPeerData();
			if(mPeerAuthClient.GetState()==PeerAuthClient::STATE_NOT_STARTED)
			{
				if(mGetCertStatus!=0)
				{
					Fn29F220(100053);
					break;
				}
				else
					AsyncRetry();
			}
		}
		break;

		default:
			return false;
	}

	return true;
}



// FUNCTION: WON 0x006e71a0
void PeerAuthOp::AsyncRetry()
{
	mGetCertStatus = (-4); 
	AsyncConnect();
	AsyncSend(mPeerAuthClient.Start(mPeerData.get(),mAuthType,mLengthFieldSize).get());
}

// FUNCTION: WON 0x006e7240
void PeerAuthOp::AsyncSend(const ByteBuffer *theMsg)
{
	mSocket->Fn2B4740((SocketOp*)Track(new SendMsgOp(theMsg),PeerAuth_Track_Socket), SendTime());
	mSocket->Fn2B4740((SocketOp*)Track(new RecvMsgOp,PeerAuth_Track_Socket), RecvTime());
}

// FUNCTION: WON 0x006e7370
void PeerAuthOp::AsyncConnect()
{
	mSocket->Fn2B4740((SocketOp*)Track(new ConnectOp(mConnectAddr),PeerAuth_Track_Socket), ConnectTime());
}

// FUNCTION: WON 0x006e74a0
void PeerAuthOp::AsyncRefreshCert()
{
	mGetCertStatus = 100000;
	SmartPtr<LightGetCertOp> aGetCert = new LightGetCertOp;
	Track(aGetCert.get(),PeerAuth_Track_GetCert);
	mAuthContext->RefreshAsync(aGetCert.get());
}

// FUNCTION: WON 0x006e75f0
void PeerAuthOp::AsyncRun()
{
	mGetCertStatus = 100004;
	if(mAuthType!=AUTH_TYPE_NONE)
	{
		if(mPeerData->CertificateExpired(120))
			AsyncRefreshCert();

		if(mPeerData->CertificateExpired(30)) // don't even try until get the certificate
			return;
	}

	AsyncConnect();
	if(mAuthType!=AUTH_TYPE_NONE)
		AsyncSend(mPeerAuthClient.Start(mPeerData.get(),mAuthType,mLengthFieldSize).get());		
}

// FUNCTION: WON 0x006e76d0
void PeerAuthOp::Virtual04()
{	
	mLengthFieldSize = mSocket->GetLengthFieldSize();
	mPeerAuthClient.SetClientKeySize(mClientKeySize, mUseClientKey);

	if(mAuthType!=AUTH_TYPE_NONE)
		mPeerData = mAuthContext->GetPeerData();

	// Asynchronous operation
	if(mRunMode==OP_MODE_ASYNC)
	{
		AsyncRun();
		return;
	}

	// Blocking operation

	// Refresh certificate if necessary
	if(mAuthType!=AUTH_TYPE_NONE && mPeerData->CertificateExpired(30))
	{
		mGetCertStatus = mAuthContext->RefreshBlock(TimeLeft());
		if(mGetCertStatus!=0)
		{
			Fn29F220(100053);
			return;
		}

		mPeerData = mAuthContext->GetPeerData();
	}

	for(int i=0; i<2; i++)
	{
		WONStatus aStatus;

		// Connect
		aStatus = mSocket->Connect(mConnectAddr, ConnectTime());
		if(CheckStatus(aStatus))
			return;

		// If no auth then done
		if(mAuthType==AUTH_TYPE_NONE)
		{
			Fn29F220(0);
			return;
		}

		// Send peer auth request
		ByteBufferPtr aSendMsg = mPeerAuthClient.Start(mPeerData.get(),mAuthType,mLengthFieldSize);
		aStatus = mSocket->SendMsg(aSendMsg.get(), SendTime());
		if(CheckStatus(aStatus))
			return;

		// Receive challenge1
		ByteBufferPtr aRecvMsg;
		aStatus = mSocket->RecvMsg(aRecvMsg, RecvTime());
		if(CheckStatus(aStatus))
			return;

		// Check challenge 1
		aStatus = mPeerAuthClient.HandleRecvMsg(aRecvMsg->data(),aRecvMsg->length(),aSendMsg);
		if(i==0 && (aStatus==(-4) || aStatus==(-19))) // expired certificate
		{
			mPeerAuthClient.Reset();
			mGetCertStatus = mAuthContext->RefreshBlock(TimeLeft());
			if(mGetCertStatus!=0)
			{
				Fn29F220(100053);
				return;
			}

			mPeerData = mAuthContext->GetPeerData();
			continue; // try one more time
		}
		else if(CheckStatus(aStatus))
			return;


		// Send challenge 2
		aStatus = mSocket->SendMsg(aSendMsg.get(), SendTime());
		if(CheckStatus(aStatus))
			return;

		// Receive peer auth complete
		aStatus = mSocket->RecvMsg(aRecvMsg, RecvTime());
		if(CheckStatus(aStatus))
			return;

		// Check peer auth complete
		aStatus = mPeerAuthClient.HandleRecvMsg(aRecvMsg->data(),aRecvMsg->length(),aSendMsg);
		if(CheckStatus(aStatus))
			return;

		// Success... add encryption protocol if necessary
		Success();
		return;
	}
}

// FUNCTION: WON 0x006e7c40
void PeerAuthOp::Success()
{
	if(mAuthType==AUTH_TYPE_SESSION || mAuthType==AUTH_TYPE_PERSISTENT) // add crypt protocol
		mSocket->Fn2B0620(new CryptTransform(GetSession()));

	Fn29F220(0);
}



}

namespace WONAPI {
// FUNCTION: WON 0x006e8fa0
bool PeerAuthOp::CheckStatus(WONStatus status)
{
    if(status==0)
        return false;
    Fn29F220(status);
    return true;
}
// FUNCTION: WON 0x006e7cb0
void PeerAuthOp::Virtual08()
{
    AsyncOpWithTracker::Virtual08();
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e7580
// (public: virtual __thiscall WONAPI::LightGetCertOp::~LightGetCertOp(void)    [??1LightGetCertOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e7560
// (public: virtual void * __thiscall WONAPI::LightGetCertOp::`scalar deleting destructor'(unsigned int)    [??_GLightGetCertOp@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e6b90
// (protected: virtual void * __thiscall WONAPI::PeerAuthOp::`scalar deleting destructor'(unsigned int)    [??_GPeerAuthOp@WONAPI@@MAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e6bb0
// (public: __thiscall WONAPI::PeerAuthClient::~PeerAuthClient(void)    [??1PeerAuthClient@WONAPI@@QAE@XZ])
