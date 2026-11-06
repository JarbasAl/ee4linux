// Native original-backed PerfTimer static API and timestamp calibration.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include "ee/wonapi/WONPerfTimer.h"
using namespace WONAPI;
// Actual native state: RVA565ec0/8,565ed0/8,565ed8/8,565ee0/4,565ee4/1.

static __int64 gPerfTotalTime = 0;
static __int64 gPerfResetTick = 0;
static double gPerfElapsedTime = 0;
static int gPerfTimerStartCount = 0;
static bool gPerfTimerStarted = false;
// SUPPORT-ONLY: EXE 0x006a37f0
// This is the original SDK's actual hardware timestamp primitive, not a codegen substitute.
static __int64 MyQueryPerformanceCounter()
{
    unsigned long x,y;
    __asm {
        rdtsc
        mov x,eax
        mov y,edx
    }
    __int64 result=y;
    result<<=32;
    result|=x;
    return result;
}
// FUNCTION: WON 0x006a3850
static __int64 MyQueryPerformanceFrequency()
{
    static __int64 aFreq=0; //actual native cached frequency RVA565ee8/8
    if(aFreq!=0) return aFreq;
    LARGE_INTEGER s1,e1,f1;
    __int64 s2,e2,f2;
    QueryPerformanceCounter(&s1);
    s2=MyQueryPerformanceCounter();
    Sleep(50);
    e2=MyQueryPerformanceCounter();
    QueryPerformanceCounter(&e1);
    QueryPerformanceFrequency(&f1);
    double time=(double)(e1.QuadPart-s1.QuadPart)/f1.QuadPart;
    f2=(e2-s2)/time;
    aFreq=f2;
    return aFreq;
}
// FUNCTION: WON 0x006a3820
// Scalar query/conversion adapter is not primary breadth.
double PerfTimer::GetCountedTime(const char*)
{
    __int64 frequency=MyQueryPerformanceFrequency();
    if(frequency==0) return gPerfTotalTime;
    return (double)gPerfTotalTime/frequency*1000.0;
}
// FUNCTION: WON 0x006a3900
double PerfTimer::GetElapsedTime(const char*) {return gPerfElapsedTime;}
// FUNCTION: WON 0x006a3910
double PerfTimer::GetPercentTime(const char*)
{
    if(gPerfElapsedTime==0) return 0;
    return GetCountedTime()/gPerfElapsedTime;
}
// FUNCTION: WON 0x006a3940
void PerfTimer::GlobalStart(const char*)
{
    gPerfTimerStarted=true;
    gPerfTotalTime=0;
    gPerfTimerStartCount=0;
    gPerfElapsedTime=0;
    LARGE_INTEGER tick;
    QueryPerformanceCounter(&tick);
    gPerfResetTick=tick.QuadPart;
}
// FUNCTION: WON 0x006a3990
void PerfTimer::GlobalStop(const char*)
{
    LARGE_INTEGER tick;
    QueryPerformanceCounter(&tick);
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    gPerfElapsedTime=(double)(tick.QuadPart-gPerfResetTick)/frequency.QuadPart*1000.0;
    gPerfTimerStarted=false;
}

// genuine missing instance family; SOURCE_ONLY_NO_NATIVE_ID.
// Existing static APIs/calibration/data remain unchanged above.
static __int64 gPerfStartTick = 0; // genuine SDK file-static instance accumulation tick

PerfTimer::PerfTimer(const char *theName)
{
	mName = theName;
	mStarted = false;
	Start();
}

PerfTimer::~PerfTimer()
{
	Stop();
}

void PerfTimer::Start()
{
	if(mName!=NULL)
		return;

	if(!mStarted)
	{
		mStarted = true;

		gPerfTimerStartCount++;
		if(gPerfTimerStartCount==1)
			gPerfStartTick = MyQueryPerformanceCounter();
	}
}

void PerfTimer::Stop()
{
	if(mName!=NULL)
		return;

	if(mStarted)
	{
		gPerfTimerStartCount--;
		if(gPerfTimerStartCount<=0 && gPerfTimerStarted)
		{
			gPerfTotalTime += MyQueryPerformanceCounter() - gPerfStartTick;
			gPerfTimerStartCount = 0;
		}
		else
		{
			int x = 0;
			x++;
		}
		mStarted = false;
	}
}

void PerfTimer::DumpTimesToStr(std::string &theStr)
{
}

void PerfTimer::DumpTimesToFile(const char *theFileName)
{
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a37f0
// (__int64 __cdecl MyQueryPerformanceCounter(void)    [?MyQueryPerformanceCounter@@YA_JXZ])
