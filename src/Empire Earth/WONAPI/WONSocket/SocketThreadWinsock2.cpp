// SOURCE_ONLY_NO_NATIVE_ID: literal same-SDK Windows backend, no backend-selection/runtime claim.
// Missing-unit proposed family profile: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include "ee/wonapi/WONSocketThreadWinsock2.h"
#include <crtdbg.h>
using namespace WONAPI;


static CriticalSection gDataCrit;
static bool gThreadConstructed = false;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SocketWaitThread::SocketWaitThread(CriticalSection &theCrit) 
{
	mNumObjects = 1;
#ifndef __WON_SINGLETHREADED__
	mWaitArray[0] = mSignalEvent.mEvent;
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SocketWaitThread::~SocketWaitThread()
{
	AutoCrit aCrit(gDataCrit);
	for(int i=1; i<mNumObjects; i++)
	{
		AsyncSocket *aSocket = mSocketArray[i];
		RemoveSocket(aSocket);
	}

	ReleaseSockets();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SocketWaitThread::ReleaseSockets(void) 
{
	AutoCrit aCrit(gDataCrit);

	if(mReleaseSet.empty())
		return;

	ReverseSet::iterator anItr = mReleaseSet.begin();
	int aCount = 0;
	while(anItr!=mReleaseSet.end()) 
	{
		int aPos = *anItr;

		CloseHandle(mWaitArray[aPos]);
//		mSocketArray[aPos]->SetThreadData(NULL);

		if(aPos!=mNumObjects-1)
		{
			// Swap released handled with the last handle (mNumObjects-1)
			AsyncSocket* aSocket = mSocketArray[mNumObjects-1];
			mSocketArray[aPos] = aSocket;

			// Maintain invariant on the non-released handle
			SocketData* aData = (SocketData*)aSocket->mThreadData.get();
			aData->mArrayPos = aPos;
			mWaitArray[aPos] = aData->mEvent;
		}

		mNumObjects--;
		mSocketArray[mNumObjects] = NULL;

		++anItr;
		++aCount;
	}

	mReleaseSet.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SocketWaitThread::ThreadFunc()
{
	int aHandleIndex;
	while(true)
	{
		DWORD aWaitResult = WaitForMultipleObjects(mNumObjects, mWaitArray, false, INFINITE);
		switch(aWaitResult) 
		{
			case WAIT_FAILED:				
				return; 

			case WAIT_TIMEOUT:
				break;

			default:
			{
				AutoCrit aCrit(gDataCrit);

				aHandleIndex = (aWaitResult - WAIT_OBJECT_0);
				if(aHandleIndex==0)  // REHUP
				{
					if(mStopped)
						return;
				}
				else
				{
					SmartPtr<AsyncSocket> aSocket = mSocketArray[aHandleIndex];
					aCrit.Leave();

					// Make sure to enter the socket critical section first since DoOp will probably enter
					// the socket critical section as well.  When SocketOps run immediately, we may be in
					// the socket's critical section when we try to enter the thread's critical section.
					AutoCrit aSocketCrit(aSocket->mDataCrit);
					aCrit.Enter();
//					SocketThreadWinsock2::Notify(aSocket);
					SocketWaitThread::SocketData* aData = (SocketWaitThread::SocketData *)aSocket->mThreadData.get();
					if(aData==NULL)
						break;

					SocketWaitThread::OpList::iterator anItr = aData->mOps.begin();
					while(anItr!=aData->mOps.end())
					{
						if(SocketThread::DoOp(*anItr))
							aData->mOps.erase(anItr++);
						else
							++anItr;
					}
				}
			}
		}

		ReleaseSockets();
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SocketWaitThread::AddSocket(AsyncSocket *theSocket)
{
	AutoCrit aCrit(gDataCrit);

	if(mNumObjects==MAXIMUM_WAIT_OBJECTS)
		return false;

	int aPos = mNumObjects;

	SocketDataPtr aData = new SocketData(this);
	aData->mEvent = CreateEvent(NULL,false,false,NULL);
	WSAEventSelect(theSocket->mSocket, aData->mEvent, FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT | FD_ACCEPT);
	aData->mArrayPos = aPos;
	theSocket->mThreadData = aData.get();

	mSocketArray[aPos] = theSocket;
	mWaitArray[aPos] = aData->mEvent;
	mNumObjects++;

	aCrit.Leave();
	mSignalEvent.Set();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SocketWaitThread::RemoveSocket(AsyncSocket *theSocket)
{
	AutoCrit aCrit(gDataCrit);

	SocketData* aData = (SocketData*)theSocket->mThreadData.get();
	if(aData==NULL)
		return;

	mReleaseSet.insert(aData->mArrayPos);
	OpList::iterator anItr = aData->mOps.begin();
	while(anItr!=aData->mOps.end())
	{
		(*anItr)->Kill(true);
		++anItr;
	}
	aData->mOps.clear();
	theSocket->mThreadData = NULL;
	theSocket->mThreadCloseCallback = NULL;

	aCrit.Leave();
	mSignalEvent.Set();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SocketWaitThread::SocketData::SocketData(SocketWaitThread *theThread)
{
	mThread = theThread;
	mEvent = NULL;
	mArrayPos = -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SocketThreadWinsock2::SocketThreadWinsock2()
{
	gThreadConstructed = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SocketThreadWinsock2::~SocketThreadWinsock2()
{
	gThreadConstructed = false;
	PurgeOps();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SocketThreadWinsock2::PurgeOps()
{
	ThreadList::iterator anItr = mThreadList.begin();
	while(anItr!=mThreadList.end())
	{
		SocketWaitThread *aThread = *anItr;
		aThread->Stop();
		delete aThread;
		++anItr;
	}
	mThreadList.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SocketThreadWinsock2::StaticSocketCloseCallback(AsyncSocket *theSocket)
{
//	if(!gThreadConstructed)
//		return;

	AutoCrit aCrit(gDataCrit);
	SocketWaitThread::SocketData *aData = (SocketWaitThread::SocketData *)theSocket->mThreadData.get();
	if(aData!=NULL)
	{
		SocketWaitThread *aThread = aData->mThread;
		aThread->RemoveSocket(theSocket);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SocketThreadWinsock2::AddSocketOp(SocketOp *theOp)
{
	AutoCrit aCrit(gDataCrit);

	AsyncSocket* aSocket = theOp->GetSocket();
	SocketWaitThread::SocketData* aData = (SocketWaitThread::SocketData *)aSocket->mThreadData.get();
	bool foundThread = false;
	if(aData==NULL)
	{
		aSocket->mThreadCloseCallback = StaticSocketCloseCallback;
		ThreadList::iterator anItr = mThreadList.begin();
		while(anItr!=mThreadList.end())
		{
			if((*anItr)->AddSocket(aSocket))
			{
				foundThread = true;
				if(anItr!=mThreadList.begin())
				{
					mThreadList.push_front(*anItr);
					mThreadList.erase(anItr++);
				}
				break;
			}
			else
				++anItr;
		}

		if(!foundThread)
		{
			mThreadList.push_front(new SocketWaitThread(gDataCrit));
			mThreadList.front()->Start();
			mThreadList.front()->AddSocket(aSocket);
		}
	}

	aData = (SocketWaitThread::SocketData *)aSocket->mThreadData.get();
	aData->mOps.insert(theOp);
	theOp->mB44 = true;
	SetEvent(aData->mEvent);
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void SocketThreadWinsock2::RemoveSocketOp(SocketOp *theOp)
{
	AutoCrit aCrit(gDataCrit);
	AsyncSocket* aSocket = theOp->GetSocket();
	SocketWaitThread::SocketData* aData = (SocketWaitThread::SocketData *)aSocket->mThreadData.get();
	theOp->mB44 = false;
	if(aData!=NULL)
	{
		aData->mOps.erase(theOp);

//		if(aData->mOps.empty())
//			aData->mThread->RemoveSocket(aSocket);
	}	
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
void SocketThreadWinsock2::Notify(AsyncSocket *theSocket)
{
	SocketWaitThread::SocketData* aData = (SocketWaitThread::SocketData *)theSocket->mThreadData.get();
	if(aData==NULL)
		return;

	SocketWaitThread::OpList::iterator anItr = aData->mOps.begin();
	while(anItr!=aData->mOps.end())
	{
		if(DoOp(*anItr))
			aData->mOps.erase(anItr++);
		else
			++anItr;
	}
}

*/