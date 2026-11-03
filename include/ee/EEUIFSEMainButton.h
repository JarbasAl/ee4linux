// C:\EEX\Empire Earth\Scenario Editor\EEUIFSEMainButton.h (RECONSTRUCTION; file name inferred). A tab button of
// the scenario editor (RTTI EEUIFSEMainButton > UIRadioButton; vtable 0x4452ac; sizeof 0x128 from the `new` in
// 0x19eba9; ctor 0x1a147f: a 32x32 push button whose normal and pressed bitmaps are 32x32 cells of one texture).
// Check (slot 0x94) switches the editor's tab through the UI thread (or directly while there is no
// gUserInterface). Requires lle/UIRadioButton.h, lle/GEBitmap.h.
#ifndef EE_EEUIFSEMAINBUTTON_H
#define EE_EEUIFSEMAINBUTTON_H

#include "lle/UIRadioButton.h"

class EEUIFScenarioEditor;
class GEFont;
class GETexture;

extern GEFont* gU531930;                    // 0x531930 (a UI font; Empire Earth.cpp's globals)

class EEUIFSEMainButton : public UIRadioButton {
public:
    EEUIFSEMainButton(unsigned long inId, unsigned char inFlag, GETexture* inTexture, const GE2DPoint& inNormalCell,
                      const GE2DPoint& inPressedCell, const GE2DPoint& inPosition, EEUIFScenarioEditor* inEditor,
                      long inTab, unsigned long inIndex, bool inIsChecked);
    virtual ~EEUIFSEMainButton();
    virtual void Check();                   // slot 0x94
    void CheckDirect();                     // 0x1a1885 (ExitEditor; provisional name)
    virtual void UnCheck();                 // slot 0x98
    GEBitmap* mNormalBitmap2;               // +0x114
    GEBitmap* mPressedBitmap2;              // +0x118
    EEUIFScenarioEditor* mEditor;           // +0x11c
    long mTab;                              // +0x120
    unsigned char mFlag;                    // +0x124
};

#endif
