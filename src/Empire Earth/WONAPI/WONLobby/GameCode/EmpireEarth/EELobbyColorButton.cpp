// C:\EEX\Empire Earth\WONAPI\WONLobby\GameCode\EmpireEarth\EELobbyColorButton.cpp (name PROVISIONAL: no __FILE__; it
// sorts before EELobbyConnectDialog.cpp, and .data opens with the colour table before the two classes' type
// descriptors). A player's colour button in the game room and the 4 x 4 colour chooser popup it opens; the lobby's
// sixteen player colours: 0x290578..0x2909a1 (the first lobby unit: Component's and Button's inline virtuals are its
// COMDATs). Module flags.
#include <windows.h>
#include <string>
#include <list>
#include <vector>

#include "lle/LowLevelEngine.h"

#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/EELobbyGameCode.h"

using namespace WONAPI;

// 0x8d01f4 (filled by EELobbyInit.cpp from the game's player colours, 0x00rrggbb)
unsigned long gEELobbyColors[16] = {
    0x0000ff, 0xff0000, 0xffff00, 0xff00ff, 0xff8800, 0x008800, 0x00ffff, 0x880000,
    0x888888, 0x000088, 0xbbbbbb, 0xdddddd, 0xffff88, 0x88ff88, 0x8800ff, 0xff8888,
};

namespace WONAPI {

// the colour chooser popup: 4 x 4 swatches of 20 pixels, 28 apart, 8 from the edge (0x48 bytes; vtable 0x84c140)
class EELobbyColorChooser : public Component {
public:
    EELobbyColorChooser();
    virtual ~EELobbyColorChooser();
    virtual void Virtual0C(int& outWidth, int& outHeight);
    virtual void Paint(Graphics& inGraphics);
    virtual void Virtual24(int inA, int inB);
    virtual void Virtual28();
    virtual void Virtual2C(int inA, int inB, int inC);
    virtual void Virtual34(int inX, int inY);
    virtual void Virtual6C();

    int mHover;                             // +0x30 (the swatch under the mouse, -1)
    int mSelected;                          // +0x34 (the chosen one, -1)
    bool mTaken[16];                        // +0x38 (another player's colour)
};

}

// FUNCTION: EXE 0x690578
EELobbyColorButton::EELobbyColorButton()
{
    mColor = 0;
    Fn2F3370(0x40, true);
}

// FUNCTION: EXE 0x6905ca
EELobbyColorButton::~EELobbyColorButton()
{
}

// FUNCTION: EXE 0x6905e8
bool EELobbyColorButton::SetColor(int inColor)
{
    if (mColor != inColor && inColor >= 0 && inColor < 16) {
        mColor = inColor;
        Invalidate();
        return true;
    }
    return false;
}

// FUNCTION: EXE 0x69060b
void EELobbyColorButton::Paint(Graphics& inGraphics)
{
    Component::Paint(inGraphics);
    inGraphics.SetColor(gEELobbyColors[mColor]);
    inGraphics.FillRect(0, 0, Width(), Height());
    if (!IsDisabled())
        LobbyDraw3DFrame(inGraphics, 0, 0, Width(), Height(), mButtonFlag5);
}

// FUNCTION: EXE 0x690669
bool EELobbyColorButton::VirtualAC()
{
    if (mChooser.mPtr == NULL) {
        mChooser = new EELobbyColorChooser;
        mChooser->Fn2F4E90(0, 0);
        mChooser->AddListener(new ComponentListenerComp(this));
    } else if (mChooser->mParent != NULL) {
        GetRoot()->ClosePopup(mChooser.mPtr);
        return false;
    }
    mChooser->mSelected = -1;
    int theX = (Width() - mChooser->Width()) / 2;
    int theY = Height();
    Fn2F56D0(theX, theY);
    PopupParams theParams(mChooser.mPtr, 0x23e7, this, theX, theY);
    theParams.mAvoidRect.height = Height();
    theParams.mAvoidRect.y = theY - Height();
    theParams.mInputComponent = this;
    GetRoot()->OpenPopup(theParams);
    return false;
}

// FUNCTION: EXE 0x690751
void EELobbyColorButton::HandleComponentEvent(ComponentEvent* inEvent)
{
    if (inEvent->mType == 30) {
        if (SetColor(mChooser->mSelected))
            Fn2F5380(100064, 0);
    } else
        Button::HandleComponentEvent(inEvent);
}

// FUNCTION: EXE 0x690789
EELobbyColorChooser::EELobbyColorChooser() : mHover(-1), mSelected(-1)
{
}

// FUNCTION: EXE 0x6907c2
EELobbyColorChooser::~EELobbyColorChooser()
{
}

// FUNCTION: EXE 0x6907cd
void EELobbyColorChooser::Virtual6C()
{
    Component::Virtual6C();
    mHover = -1;
    EELobbyGame* theGame = (EELobbyGame*)GetMyLobbyGame();
    if (theGame != NULL)
        theGame->GetTakenColors(mTaken, -1);
}

// FUNCTION: EXE 0x6907f1
void EELobbyColorChooser::Virtual24(int inA, int inB)
{
    Component::Virtual24(inA, inB);
}

// FUNCTION: EXE 0x690801
void EELobbyColorChooser::Virtual34(int inX, int inY)
{
    bool theOutside = false;
    inX -= 8;
    if (inX < 0 || inX % 28 >= 20)
        theOutside = true;
    else
        inX /= 28;
    inY -= 8;
    if (inY < 0 || inY % 28 >= 20)
        theOutside = true;
    else
        inY /= 28;
    int theHover = theOutside ? -1 : inX + inY * 4;
    if (theHover != mHover) {
        mHover = theHover;
        Invalidate();
    }
}

// FUNCTION: EXE 0x69086e
void EELobbyColorChooser::Virtual2C(int inA, int inB, int inC)
{
    if (mHover >= 0 && mHover < 16 && !mTaken[mHover]) {
        mSelected = mHover;
        GetRoot()->ClosePopup(this);
    }
}

// FUNCTION: EXE 0x690894
void EELobbyColorChooser::Virtual28()
{
    Component::Virtual28();
    if (mHover != -1) {
        mHover = -1;
        Invalidate();
    }
}

// FUNCTION: EXE 0x6908af
void EELobbyColorChooser::Virtual0C(int& outWidth, int& outHeight)
{
    outWidth = 120;
    outHeight = 120;
}

// FUNCTION: EXE 0x6908c1
// NEAR: equal but for the colour constant 0x888888, which build/exe/xref_noreloc.txt takes for an address (a tool
// false positive: the push is an immediate in both)
void EELobbyColorChooser::Paint(Graphics& inGraphics)
{
    Component::Paint(inGraphics);
    inGraphics.SetSysColor(11);
    inGraphics.FillRect(0, 0, Width(), Height());
    LobbyDrawFrame(inGraphics, 0, 0, Width(), Height(), false);
    int theIndex = 0;
    int theY = 8;
    for (int theRow = 0; theRow < 4; theRow++) {
        int theX = 8;
        for (int theColumn = 0; theColumn < 4; theColumn++) {
            if (!mTaken[theIndex]) {
                inGraphics.SetColor(gEELobbyColors[theIndex]);
                inGraphics.FillRect(theX, theY, 20, 20);
                inGraphics.SetColor(0x888888);
                inGraphics.DrawRect(theX, theY, 20, 20);
                if (theIndex == mHover)
                    LobbyDraw3DFrame(inGraphics, theX, theY, 20, 20, false);
            }
            theX += 28;
            theIndex++;
        }
        theY += 28;
    }
}

// The unit's compiler-generated functions:
// SYNTHETIC: EXE 0x69059d
// (Component::Virtual3C, inline; ICF also Virtual50/54/58)
// SYNTHETIC: EXE 0x6905a2
// (Component::VirtualA4, inline; ICF also VirtualA8)
// SYNTHETIC: EXE 0x6905a8
// (Button::GetText, inline)
// SYNTHETIC: EXE 0x6905ae
// (??_G EELobbyColorButton)
// (0x6907a3 Component::Virtual30, inline, is ICF-shared with game code: named there)
// SYNTHETIC: EXE 0x6907a6
// (??_G EELobbyColorChooser)

//: the ICF-shared copy LINK kept in this unit's range
// SYNTHETIC: EXE 0x6907a3
