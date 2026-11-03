// C:\EEX\Empire Earth\Actions\EEAScroll.h (RECONSTRUCTION; file name inferred). The edge-scroll action
// (RTTI EEAScroll > EEAction > UReferenceCounter; vtable 0x43907c; sizeof 0x50; EEAScroll.cpp 0x470f..0x4a0b): ctor
// 0x470f, ??_G 0x4764 (also EEAServerMigration's: ICF), the implicit dtor 0x4780 (~EEAction expanded), Virtual4 0x478e
// (moves the camera), Update 0x4935 (the mouse position against the scroll edges; posts the action). Names are
// provisional
// (by behaviour). Requires ee/EEAction.h, lle/TMPoll.h, lle/GE2DPoint.h, lle/GERectangle.h.
#ifndef EE_EEASCROLL_H
#define EE_EEASCROLL_H

class EEAScroll : public EEAction {
public:
    EEAScroll();
    virtual ~EEAScroll();
    virtual void Virtual4();                // +0x04 (0x478e)
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)
    bool Update(const GE2DPoint& inPoint);  // 0x4935 (called from 0x19e72c; true while the mouse is on an edge)

    // +0x0c the inner rectangle: outside it the camera scrolls (x <= left, y <= top, x >= right, y >= bottom); the
    // ctor's stores (+0x18, +0x10, +0x0c, +0x14) are GERectangle()'s top, left, bottom, right
    GERectangle mEdges;
    TMPoll mPoll;                           // +0x20 (10 ms, not started)
    unsigned long mElapsed;                 // +0x40 the last poll's elapsed time
    bool mScrollLeft;                       // +0x44
    bool mScrollRight;                      // +0x45
    bool mScrollUp;                         // +0x46
    bool mScrollDown;                       // +0x47
    bool mAccelerate;                       // +0x48 (= true in the ctor body)
};

#endif
