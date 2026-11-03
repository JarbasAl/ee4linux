// C:\EEX\Empire Earth\UI Events\EEUIECameraEvents.cpp (file name inferred: no __FILE__; the draft header's name).
// The mouse-move switch and the free-camera revolve/rotate events: 0x1e6711..0x1e688c (their ??_G are emitted by
// their creator 0x19de39.. elsewhere). Unit flags: the exe module set.
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

#include "lle/GETransformation.h"
#include "lle/GE3DPoint.h"
#include "lle/ISEntity.h"

#include "ee/EmpireEarth.h"
#include "ee/RTSWorld.h"
#include "ee/EECameraControl.h"
#include "ee/EEWorld.h"
#include "ee/EEMouse.h"
#include "ee/EEUIECameraEvents.h"

// FUNCTION: EXE 0x5e6711
// Freezes the game mouse while the camera is in free mode (mode 2)
long EEUIEDisableMouseMove::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    if (gWorld.mCameraControl.mMode == 2)
        gMouse->mIsFrozen = true;
    return kErrorOK;
}

// FUNCTION: EXE 0x5e672b
// Unfreezes the game mouse while the camera is in free mode (mode 2)
long EEUIEEnableMouseMove::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    if (gWorld.mCameraControl.mMode == 2)
        gMouse->mIsFrozen = false;
    return kErrorOK;
}

// FUNCTION: EXE 0x5e6745
// Revolves the free camera by the mouse movement (half a degree per unit, inverted) if free camera is on
long EEUIERevolveCamera::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    GETransformation theTransformation;
    ISEntity* theX;
    ISEntity* theY;
    gISMouse->GetEntity(kMouseXAxis, theX);
    gISMouse->GetEntity(kMouseYAxis, theY);
    float theXAngle = -theX->GetValue() * 0.008726646f;
    float theYAngle = -theY->GetValue() * 0.008726646f;
    if (gWorld.mCameraControl.mMode == 2 && gFreeCamera)
        gWorld.mCameraControl.Revolve(GE3DPoint(theXAngle, theYAngle, 0.0f));
    return kErrorOK;
}

// FUNCTION: EXE 0x5e67dd
// Rotates the free camera by the mouse movement (half a degree per unit, inverted at zoom 0) if free camera is on
long EEUIERotateCamera::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    GETransformation theTransformation;
    ISEntity* theX;
    ISEntity* theY;
    gISMouse->GetEntity(kMouseXAxis, theX);
    gISMouse->GetEntity(kMouseYAxis, theY);
    float theXAngle = theX->GetValue() * 0.008726646f;
    float theYAngle = theY->GetValue() * 0.008726646f;
    if (gWorld.mCameraControl.mZoom == 0.0f) {
        theXAngle = -theXAngle;
        theYAngle = -theYAngle;
    }
    if (gWorld.mCameraControl.mMode == 2 && gFreeCamera)
        gWorld.mCameraControl.Rotate(GE3DPoint(theXAngle, theYAngle, 0.0f));
    return kErrorOK;
}
