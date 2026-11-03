// C:\EEX\Empire Earth\Scenario Editor Actions\EESEAPaintArea.h (RECONSTRUCTION; file name inferred). The
// editor's area painting action (RTTI EESEAPaintArea > EEAction; vtable 0x4438a4, sizeof 0x2c; the ctor 0x17d991
// (inX, inY: both corners, then the rectangle) is emitted by its first user). Requires ee/EEAction.h, lle/GERectangle.h.
#ifndef EE_EESEAPAINTAREA_H
#define EE_EESEAPAINTAREA_H

class EESEAPaintArea : public EEAction {
public:
    EESEAPaintArea(long inX, long inY);
    virtual void Virtual4();                // +0x04 (0x1a8284)
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)
    // 0x1a8299: moves the second corner; the rectangle spans both corners; posts itself (provisional name)
    void SetCorner(long inX, long inY);

    GERectangle mR0C;                       // +0x0c
    long mStartX;                           // +0x1c the first corner
    long mStartY;                           // +0x20
    long mEndX;                             // +0x24 the second corner
    long mEndY;                             // +0x28
};

#endif
