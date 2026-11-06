#ifndef EE_WONAPI_WONAPICORE_H
#define EE_WONAPI_WONAPICORE_H
#include "ee/wonapi/WONTimerThread.h"
namespace WONAPI {
// Complete actual d0 owner, exact native Core/Ex three-slot hierarchy.
class WONAPICore {
public:
    static void QueueOp(AsyncOp*); //existing canonical spelling, actual2f2e60 SDK QueueInContainer
    static void AddTimerOp(AsyncOp*,unsigned long); //existing2f2ea0 SDK AddToTimerThread
    static void RemoveTimerOp(AsyncOp*); //existing2f2ed0 SDK RemoveFromTimerThread
    static const std::string& GetDefaultFileDirectory(); //2f2da0
    virtual ~WONAPICore(); //slot0 actual ordinary2f2980
    virtual void Virtual04(); //slot4 actual Startup2f2b50
    virtual void Virtual08(); //slot8 actual Shutdown2f2c80
    void Pump(unsigned long,int=0); //2f2d20 actual SDK argument types
    static WONAPICore* GetInstance() { return mInstance; }
    static bool IsStarted(); //2f2d00 support0
protected:
    friend class SocketOp;
    static void AddToSocketThread(SocketOp*); //2f2ef0 actual protected SDK dispatch, support0
    static void RemoveFromSocketThread(SocketOp*); //2f2f20 support0
    WONAPICore(const char*); //2f28d0 actual173, support0
    void Init(); //2f2750 actual321, state initializer support0
    void WriteVersion(); //2f2a80
    static WONAPICore* mInstance; //actual external static5661fc
    AsyncOpContainer mOpContainer; //04..3c, true38
    TimerThread mTimerThread; //3c..8c, true50
    SocketThread* mSocketThread; //8c owned rawpointer, virtual deletion
    bool mIsStarted; //90
    bool mDoSocketAndTimerThreads; //91
    bool mDoPumpThread; //92,naturalpad93
    static bool mHaveWrittenVersion; //actual external static566200
    std::string mVersionString; //94..a4
    std::string mFileDirectory; //a4..b4
    class PumpThread : public Thread {
        virtual void ThreadFunc(); //slot10 actual2f2fb0, support0
    };
    PumpThread mPumpThread; //b4..d0, actual1c, noextra fields
private:
    WONAPICore(WONAPICore&);
    WONAPICore& operator=(WONAPICore&);
};
class WONAPICoreEx : public WONAPICore {
public:
    WONAPICoreEx(); //actual2f2f40 support0, implicitdtor2f2f80 support0
    void SetDefaultFileDirectory(const char*);
    void SetSocketThread(SocketThread*);
    void SetDoSocketAndTimerThreads(bool);
    void Fn2F2F90(bool); //existing canonical actual setter2f2f90 support0
};
}
#endif
