// SOURCE_ONLY_NO_NATIVE_ID; proposed missing-unit family profile, zero Native/primary credit.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONSocketThreadExWindows.h"
using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SocketThreadEx::SocketThreadEx()
{
	AsyncSocket::StartWinsock();
	mSignalSocket = new AsyncSocket(AsyncSocket::UDP);

	// Bind SignalSocket and connect it to itself
	IPAddr anAddr = IPAddr::GetLocalAddr();
	anAddr.SetPort(0); // choose random port
	mSignalSocket->Fn2B0C90(anAddr,false); // SDK Bind address/reuse default
	anAddr.SetPort(mSignalSocket->GetLocalPort());
	mSignalSocket->Fn2B0E60(anAddr); // current Connect spelling

	for(int i=0; i<3; i++)
	{
		mSocketArray[i].reserve(64);
		mOpArray[i].reserve(64);
	}

	mSocketArray[0 /* SDK SocketEvent_Read */].push_back(mSignalSocket->mSocket /* genuine SDK borrowed descriptor getter */);
	mOpArray[0 /* SDK SocketEvent_Read */].push_back(NULL);
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SocketThreadEx::~SocketThreadEx()
{
	PurgeOps();
	AsyncSocket::StopWinsock();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SocketThreadEx::PurgeOps()
{
	for(int i=0; i<3; i++)
	{
		OpMap::iterator anItr = mOpMap[i].begin();
		while(anItr!=mOpMap[i].end())
		{
			SocketOp *anOp = anItr->second;
			if(anOp->mB44)
			{
				anOp->mB44 = false;
				anOp->Kill(true); // explicit genuine SDK finish default
			}

			++anItr;
		}

		mOpMap[i].clear();
		if(i!=0 /* SDK SocketEvent_Read */)
		{
			mSocketArray[i].clear();
			mOpArray[i].clear();
		}
	}	
	
	// Leave signal socket
	mSocketArray[0 /* SDK SocketEvent_Read */].resize(1);
	mOpArray[0 /* SDK SocketEvent_Read */].resize(1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SocketThreadEx::AddSocketOp(SocketOp *theOp)
{
	AutoCrit aCrit(mDataCrit);
	SOCKET aSocket = theOp->GetSocket()->mSocket /* genuine SDK borrowed descriptor getter */;
	for(int i=0; i<3; i++)
	{
		if(theOp->mB58[i])
		{
			std::pair<OpMap::iterator,bool> aRet;
			aRet = mOpMap[i].insert(OpMap::value_type(aSocket,theOp));
			if(!aRet.second) // old op still here
			{
				aRet.first->second->Kill(true); // explicit genuine SDK finish default
				RemoveSocketOp(aRet.first->second,i);
				aRet.first->second = theOp;
			}

			mOpMap[i][aSocket] = theOp;

			theOp->mL48[i] = mSocketArray[i].size();
			mSocketArray[i].push_back(aSocket);
			mOpArray[i].push_back(theOp);
		}
	}

	theOp->mB44 = true;
	Signal();
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void SocketThreadEx::RemoveSocketOp(SocketOp *theOp)
{
	RemoveSocketOp(theOp, -1);
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void SocketThreadEx::RemoveSocketOp(SocketOp *theOp, int dontErase)
{
	AutoCrit aCrit(mDataCrit);
	SmartPtr<SocketOp> anOp = theOp;
	for(int i=0; i<3; i++)
	{
		if(theOp->mB58[i])
		{
			int aPos = theOp->mL48[i];
			if(aPos>=0)
			{
				if(dontErase!=i)
					mOpMap[i].erase(mSocketArray[i][aPos]);

				int aSize = mSocketArray[i].size();
				
				if(aPos!=aSize-1)
				{
					swap(mSocketArray[i][aPos],mSocketArray[i][aSize-1]);
					swap(mOpArray[i][aPos],mOpArray[i][aSize-1]);

					mOpArray[i][aPos]->mL48[i] = aPos;
				}

				mSocketArray[i].pop_back();
				mOpArray[i].pop_back();
				theOp->mL48[i] = -1;
			}
		}
	}

	theOp->mB44 = false;
	Signal();
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void SocketThreadEx::Signal()
{
	static char aBuf = 1;
	mSignalSocket->Send(&aBuf,1,NULL); // SDK optional sent-length default
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void SocketThreadEx::Pump(DWORD theWaitTime)
{

	AutoCrit aCrit(mDataCrit);
	int i,j;

	for(i=0; i<3; i++)
		m_fdset[i].Copy(mSocketArray[i]);

	aCrit.Leave();
	
	timeval timeoutval;
	timeoutval.tv_sec = theWaitTime/1000;
	timeoutval.tv_usec = (theWaitTime%1000)*1000;

	int aVal = select(0, m_fdset[0].mSet, m_fdset[1].mSet, m_fdset[2].mSet, &timeoutval);

	if(aVal>0)
	{
		aCrit.Enter();
		OpMap::iterator anItr;
		for(i=0; i<3; i++)
		{
			fd_set *aSet = m_fdset[i].mSet;
			for(j=0; j<aSet->fd_count; j++)
			{
				anItr = mOpMap[i].find(aSet->fd_array[j]);
				if(anItr!=mOpMap[i].end())
				{
					if(DoOp(anItr->second))
					{
						RemoveSocketOp(anItr->second, i);
						mOpMap[i].erase(anItr);
					}
				}
			}
		}
		aCrit.Leave();

		static char aBuf;
		while(mSignalSocket->Fn2B15E0(&aBuf,1,NULL)==0 /* WS_Success */)
		{
		}
	}
	else if(aVal<0)
	{
		aCrit.Enter();
		OpMap::iterator anItr;
		for(i=0; i<3; i++)
		{
			anItr = mOpMap[i].begin();
			while(anItr!=mOpMap[i].end())
			{
				if(DoOp(anItr->second))
				{
					RemoveSocketOp(anItr->second, i);
					mOpMap[i].erase(anItr++);
				}
				else
					++anItr;
			}
		}
		aCrit.Leave();
	}
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void SocketThreadEx::ThreadFunc()
{
	while(!mStopped)
		Pump(1000);
}

