// C:\EEX\Low-Level Engine\User Interface\UIMovie.h (RECONSTRUCTION). A control that shows a GEMovie. UIMovie.cpp
// (0x1007ba37..0x1007badf) holds the ctor, dtor, CopyData, DrawPolygons and DrawTextA; the inline overrides of
// UIControl's pure slots and ??_E are emitted by UIControlsTable.cpp, the first includer (??_E 0x100671b0; the
// overrides are ICF-folded `return 0` / `ret` stubs). Vtable 0x100997b0. sizeof 0xa4. Requires lle/UIControl.h,
// lle/GEMovie.h.
#ifndef LLE_UIMOVIE_H
#define LLE_UIMOVIE_H

#include "lle/UIControl.h"

class GEMovie;

class LLE_API UIMovie : public UIControl {
public:
    UIMovie(unsigned long inId, GEMovie* inMovie, const GERectangle& inRectangle, UIForm* inForm,
            unsigned long inIndex);
    virtual ~UIMovie();
    virtual long CopyData();
    virtual long DrawPolygons();
    virtual long DrawTextA();
    virtual long UpdateKeyboard() { return kErrorOK; }
protected:
    // protected in the original (exported ?MouseLButtonDown@UIMovie@@MAEJXZ etc.); UpdateKeyboard is public
    virtual long MouseLButtonDown() { return kErrorOK; }
    virtual long MouseLButtonUp() { return kErrorOK; }
    virtual long MouseMove(const GE2DPoint& inLocation) { return kErrorOK; }
    virtual long MouseRButtonDown() { return kErrorOK; }
    virtual long MouseRButtonUp() { return kErrorOK; }
    virtual void TabbedTo() {}
    virtual void TabbedFrom() {}

    GEMovie* mMovie;                        // +0xa0 (referenced)
};

#endif
