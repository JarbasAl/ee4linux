// C:\EEX\Empire Earth\User Interface Thread\EEUICPing.cpp (file name inferred: no __FILE__; RTTI class EEUICPing).
// The lobby's ping meter: 0x258464..0x25861c (ctor, ??_G, dtor, DrawPolygons, SetPing; EEUICPortrait's ctor at
// 0x25861c). Unit flags: the exe module set.
#include <windows.h>
#include <mbstring.h>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GESurface.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "GEWindow.h"
#include "lle/GEScreen.h"
#include "lle/GEViewport.h"
#include "lle/GEVertexArray.h"
#include "lle/GERasterizer.h"
#include "lle/GERhombus.h"
#include "lle/GEBitmap.h"
#include "lle/FSFileSpec.h"
#include "lle/FSArchive.h"
#include "lle/FSPath.h"
#include "lle/TMPoll.h"
#include "lle/TSCriticalSection.h"
#include "lle/TSScheduler.h"
#include "lle/SESample.h"
#include "lle/SESMInstanceNode.h"
#include "lle/SESoundManager.h"
#include "lle/Input System.h"
#include "lle/ISKeyboard.h"
#include "lle/ISMouse.h"
#include "lle/UIControl.h"
#include "lle/UIEvent.h"
#include "lle/UIEntityGroup.h"
#include "lle/UIBackground.h"
#include "lle/UIFontTable.h"
#include "lle/UIHotKey.h"
#include "lle/UIIMECandidate.h"
#include "lle/UIForm.h"
#include "lle/UIManager.h"
#include "lle/UIButton.h"
#include "lle/NEMessage.h"
#include "lle/NESynchronousCommunications.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEPlayer.h"
#include "ee/EEUnit.h"
#include "ee/EEMessage.h"
#include "ee/EECommunications.h"
#include "ee/EmpireEarth.h"
#include "ee/EEUICPing.h"

// FUNCTION: EXE 0x658464
// Creates the lobby's ping meter: five 10x10 bars 12 pixels apart (all green), none lit
EEUICPing::EEUICPing(unsigned long inId, const GE2DPoint& inLocation, UIForm* inForm, unsigned long inIndex)
    : UIControl(kControlTypeCustom, inId, GERectangle(inLocation, 60.0f, 10.0f), inForm, inIndex, 0)
{
    GERectangle theRect;
    theRect.SetTop(inLocation.GetY());
    theRect.SetBottom(inLocation.GetY() + 10.0f);
    for (unsigned long i = 0; i < 5; i++) {
        theRect.SetLeft(inLocation.GetX() + i * 12);
        theRect.SetRight(theRect.GetLeft() + 10.0f);
        this->mBars[i].SetRectangle(theRect);
        if (i <= 2)
            this->mBars[i].SetColor(GEColor(0.0f, 1.0f, 0.0f));
        else
            this->mBars[i].SetColor(GEColor(0.0f, 1.0f, 0.0f));
    }
    this->mNumBars = 0;
}

// SYNTHETIC: EXE 0x65855c
// (the scalar deleting destructor, emitted with the vtable; slot EEUICPing+0x0)

// FUNCTION: EXE 0x658578
// Destroys the ping meter
EEUICPing::~EEUICPing()
{
}

// FUNCTION: EXE 0x6585a6
// Draws the meter's bars up to and including the lit count (so at least one)
long EEUICPing::DrawPolygons()
{
    long theResult = UIControl::DrawPolygons();
    for (unsigned long i = 0; i < 5; i++) {
        if (i <= this->mNumBars) {
            unsigned long theScreen = gScreenIndex;
            theResult = this->GetRasterizer()->Draw(&this->mBars[i], theScreen);
        }
    }
    return theResult;
}

// FUNCTION: EXE 0x6585e7
// Sets the number of lit bars from a ping quality of 0..1 (times 5), redrawing on change
void EEUICPing::SetPing(float inPing)
{
    unsigned long theBars = (unsigned long)(inPing * 5.0f);
    if (theBars != this->mNumBars) {
        this->SetDirty(true);
        this->mNumBars = theBars;
    }
}
