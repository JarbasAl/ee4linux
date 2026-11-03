// C:\EEX\Low-Level Engine\Thread System\TSCriticalSection.h (RECONSTRUCTION; layout proven by the engine's own
// TSCriticalSection.cpp 0x53ca1..0x53cee and TSScheduler::CreateCriticalSection operator new(0x20)).
#ifndef LLE_TSCRITICALSECTION_H
#define LLE_TSCRITICALSECTION_H

class LLE_API TSCriticalSection {
    friend class TSScheduler;               // only the scheduler creates/destroys (private ctor/dtor exported)
public:
    void Enter();
    void Leave();
private:
    TSCriticalSection();
    ~TSCriticalSection();

    CRITICAL_SECTION mSection;              // +0x00
    unsigned long mOwnerThread;             // +0x18 thread id while held (set on first Enter)
    unsigned long mLockCount;               // +0x1c recursion count
};

#endif
