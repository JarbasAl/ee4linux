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
// FUNCTION: WON 0x00717c70
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

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00717e90
// (public: __thiscall std::map<class WONAPI::Font *,class WONAPI::SmartPtr<class WONAPI::WrappedText>,struct std::less<class WONAPI::Font *>,class std::a)
// TEMPLATE: WON 0x00717df0
// (public: virtual void * __thiscall WONAPI::WONButton::`scalar deleting destructor'(unsigned int))
