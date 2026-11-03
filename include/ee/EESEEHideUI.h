// EESEEHideUI.h (RECONSTRUCTION; file name inferred). The scenario editor's "hide UI" check box
// event (RTTI EESEEHideUI > UIEvent > UReferenceCounter; vtable 0x44518c; sizeof 0x2c: EEUIFScenarioEditor's ctor
// 0x19d615 news 0x2c bytes and sets it as the check box 0x12's two trigger events). The inline ctor is the COMDAT
// 0x1a015a (UIEvent's stores, then the two default GERectangles); ProcessEvent 0x1af4c1 sits in the editor events'
// range. Member names are provisional. Requires lle/UIEvent.h, lle/GERectangle.h.
#ifndef EE_EESEEHIDEUI_H
#define EE_EESEEHIDEUI_H

class EESEEHideUI : public UIEvent {
public:
    EESEEHideUI() {}
    virtual ~EESEEHideUI() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x1af4c1

    GERectangle mR0C;                       // +0x0c
    GERectangle mR1C;                       // +0x1c
};

// The viewport change EESEEHideUI posts (RTTI EESEEChangeViewport > UIEvent > UReferenceCounter; vtable 0x445f98;
// sizeof 0x20: operator new(0x20) at 0x1af506, the ctor expanded there). ProcessEvent 0x1af55a, ??_G 0x1af900, both in
// the editor events' range. Names are provisional.
class EESEEChangeViewport : public UIEvent {
public:
    EESEEChangeViewport(const GERectangle& inRectangle, bool inHidden) : mRectangle(inRectangle), mHidden(inHidden) {}
    virtual ~EESEEChangeViewport() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x1af55a

    GERectangle mRectangle;                 // +0x0c
    bool mHidden;                           // +0x1c
};

#endif
