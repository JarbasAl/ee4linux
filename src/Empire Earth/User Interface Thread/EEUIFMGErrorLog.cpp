// C:\EEX\Empire Earth\User Interface Thread\EEUIFMGErrorLog.cpp (file name inferred: no __FILE__; RTTI class
// EEUIFMGErrorLog). The in-game form's error log list box: 0x267ad9..0x267b77 (ctor, ??_G, MouseLButtonDown).
// Unit flags: the exe module set.
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
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "lle/UIScrollBar.h"
#include "lle/UIListBox.h"
#include "lle/UILBString.h"
#include "ee/EEUIFMGErrorLog.h"

// FUNCTION: EXE 0x667ad9
// Creates the in-game error log list box: transparent text, no selection highlight
EEUIFMGErrorLog::EEUIFMGErrorLog(const GERectangle& inRectangle, UIForm* inForm, GEFont* inFont,
                                 std::vector<UWideString>& inData, unsigned long inIndex, unsigned long inId)
    : UILBString(inId, inData, inFont, false, inRectangle, inForm, inIndex, 0)
{
    GEText* theText = this->GetGEText();
    theText->SetFgColor(GEColor());
    theText->SetIsTransparent(true);
    this->GetSelectedBackground()->SetOpacity(0.0f);
}

// SYNTHETIC: EXE 0x667b33
// (the scalar deleting destructor, emitted with the vtable; slot EEUIFMGErrorLog+0x0)

// FUNCTION: EXE 0x667b56
// Handles a click in the error log, releasing the mouse unless the scroll bar holds it
long EEUIFMGErrorLog::MouseLButtonDown()
{
    long theResult = UIListBox<UWideString>::MouseLButtonDown();
    if (!this->GetScrollBarCaptured())
        this->ReleaseCapturedMouse();
    return theResult;
}
