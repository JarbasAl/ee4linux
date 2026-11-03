// C:\EEX\Low-Level Engine\User Interface\UIScrollButton.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x1007d013..0x1007d139: the ctor and ButtonDown only. No U2D key init chains: the unit does not include the
// rasterizer header. The header-inline members are emitted first by UICheckBox.cpp (0x1005f272..).
#include <windows.h>

#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GEBitmap.h"
#include "lle/GERhombus.h"
#include "lle/TMPoll.h"
#include "lle/UIControl.h"
#include "lle/UIButton.h"
#include "lle/UISlider.h"
#include "lle/UIScrollButton.h"

// FUNCTION: LLE 0x1007d013
// Creates a 20x20 scroll arrow button that steps a slider down or up, with a 500 ms delay and 100 ms repeat
UIScrollButton::UIScrollButton(unsigned long inId, GEBitmap* inNormalBitmap, GEBitmap* inPressedBitmap,
                               const GE2DPoint& inLocation, UIForm* inForm, UISlider* inSlider,
                               unsigned long inDecrement, unsigned long inIndex)
    : UIButton(inId, inNormalBitmap, NULL, inPressedBitmap,
               GERectangle(inLocation, GE2DPoint(inLocation.GetX() + 19.0f,
                                                 inLocation.GetY() + 19.0f)),
               inForm, inIndex, 0)
{
    this->mDecrement = inDecrement;
    this->mSlider = inSlider;
    this->mDelayPoll = new TMPoll(500, 0, false);
    this->mRepeatPoll = new TMPoll(100, 0, false);
}

// FUNCTION: LLE 0x1007d0c3
// Steps the slider while held: once on press, again after the 500 ms delay, then every 100 ms
void UIScrollButton::ButtonDown()
{
    if (this->mDelayPoll->GetIsRunning()) {
        if (this->mDelayPoll->Poll()) {
            this->mRepeatPoll->Start();
            this->mDelayPoll->Stop();
            this->mSlider->SetCurrentRangeValue(this->mSlider->GetCurrentRangeValue() + (this->mDecrement ? -1 : 1));
        }
    } else if (this->mRepeatPoll->GetIsRunning()) {
        if (this->mRepeatPoll->Poll())
            this->mSlider->SetCurrentRangeValue(this->mSlider->GetCurrentRangeValue() + (this->mDecrement ? -1 : 1));
    } else {
        this->mDelayPoll->Start();
        this->mSlider->SetCurrentRangeValue(this->mSlider->GetCurrentRangeValue() + (this->mDecrement ? -1 : 1));
    }
    UIButton::ButtonDown();
}
