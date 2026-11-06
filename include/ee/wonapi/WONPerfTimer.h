// Reference-only actual PerfTimer static API view. No instance layout, construction,
// destruction, sizeof or lifetime authority is granted by this declaration.
#ifndef EE_WONAPI_WONPERFTIMER_H
#define EE_WONAPI_WONPERFTIMER_H
#include <string>
// R11 promotes the former static-only view to the complete genuine SDK value owner.
// Historical restriction above described the old view; new instance methods/fields are
// SOURCE_ONLY_NO_NATIVE_ID, not a Native instance size/allocation or ABI identity claim.
namespace WONAPI {
class PerfTimer {
protected:
    bool mStarted;
    const char* mName;
public:
    PerfTimer(const char* theName=0);
    ~PerfTimer();
    void Start();
    void Stop();
    static void DumpTimesToStr(std::string&);
    static void DumpTimesToFile(const char*);
    static void GlobalStart(const char* name=0); //2a3940, genuine cdecl char-pointer/ret0
    static void GlobalStop(const char* name=0); //2a3990, genuine cdecl char-pointer/ret0
    static double GetCountedTime(const char* name=0); //2a3820 actualcdecl/FPU result
    static double GetElapsedTime(const char* name=0); //2a3900 actualcdecl/FPU result
    static double GetPercentTime(const char* name=0); //2a3910 actualcdecl/FPU result
};
}
#endif
