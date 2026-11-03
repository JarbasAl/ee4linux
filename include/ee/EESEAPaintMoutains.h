// C:\EEX\Empire Earth\Scenario Editor Actions\EESEAPaintMoutains.h (RECONSTRUCTION; file name inferred, the RTTI
// spelling). The editor's "paint mountains" action, which removes the units on the painted tiles
// (RTTI EESEAPaintMoutains > EESEAPaint > EEAction; vtable 0x4459dc). The inline ctor/??_G 0x1ab48a are emitted by
// EEUIFSETerrain.cpp; Virtual4 0x1a86ff and SetArea 0x1a88ea are in EESEAPaintMoutains.cpp. Names are provisional.
// Requires ee/EESEAPaint.h, <deque>.
#ifndef EE_EESEAPAINTMOUTAINS_H
#define EE_EESEAPAINTMOUTAINS_H

class EESEAPaintMoutains : public EESEAPaint {
public:
    EESEAPaintMoutains() {}
    virtual ~EESEAPaintMoutains() {}        // +0x00 (??_G 0x1ab48a)
    virtual void Virtual4();                // +0x04 0x1a86ff
    virtual void Virtual8();                // +0x08 (0xe123d: an empty body)
    virtual bool SetArea(long inLeft, long inTop, long inRight, long inBottom);   // +0x0c 0x1a88ea

    std::deque<EESEAPaintRect> mPendingAreas;  // +0x0c the areas still to clear
};

#endif
