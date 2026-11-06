// Genuine default-member construction precedes the FontTextMap and body.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyWONButton.h"
namespace WONAPI {
// FUNCTION: WONLobby.exe 0x317c70
// Original348 bytes; lifecycle/default setup, support0.
WONButton::WONButton()
{
    mLeftTextPad=mRightTextPad=8;
    mTopTextPad=mBottomTextPad=4;
    mLeftFocusPad=mRightFocusPad=4;
    mTopFocusPad=3;
    mBottomFocusPad=4;
    mPushDX=mPushDY=1;
    mHasCheck=false;
    mScaleImage=true;
    mAdjustHeightToFitText=false;
    mWrapText=false;
    mDrawGrayText=true;
    mIsTransparent=false;
    mHAlign=HorzAlign_Center;
    mVAlign=VertAlign_Center;
    mFocusStyle=1;
    Fn2F3370(1,true);
    SetComponentFlags(8,true);
    mDefaultTextColor=0x80000005;
}
}
