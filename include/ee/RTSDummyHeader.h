// C:\EEX\RTS Engine\RTS Engine\RTSDummyHeader.h (RECONSTRUCTION; file name inferred).
// A polymorphic list head (RTTI RTSDummyHeader<RTSUnit>, vtable 0x4421f8; RTSDummyUnit, vtable 0x4421f0). sizeof 8.
// All inline: RTSWorld's ctor 0x147dfd stores +4 = 0 and then RTSDummyUnit's vptr (the base vptr store is dead),
// its dtor 0x147eb6 re-stores the base vptr; both classes share the ??_G 0x148274 (emitted after RTSWorld's dtor,
// its first user). Member name is provisional.
#ifndef EE_RTSDUMMYHEADER_H
#define EE_RTSDUMMYHEADER_H

template <class T>
class RTSDummyHeader {
public:
    RTSDummyHeader() : mFirst(NULL) {}
    virtual ~RTSDummyHeader() {}

    T* mFirst;                              // +0x04 (provisional name)
};

class RTSUnit;

class RTSDummyUnit : public RTSDummyHeader<RTSUnit> {
};

#endif
