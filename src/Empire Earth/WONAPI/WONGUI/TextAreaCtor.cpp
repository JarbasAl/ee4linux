// Complete native3117c0 owner initialization, support0.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <deque>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyTextAreaConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x007117c0
TextArea::TextArea()
{
    mSearchLine=new TextLine;
    mTextFlags=TextFlag_WordWrap|TextFlag_AllowSelection|TextFlag_ShowPartialLines|TextFlag_ScrollOnBottom;
    mHasBeenVisible=false;
    mTextColor=0x8000000e;
    mFont=GetNamedFont("TextArea");
    mLinkColor=0x0000ff;
    mLinkFont=GetNamedFont("TextAreaLink");
    if(mLinkFont.get()==GetRoot()->mManager->GetDefaultFont())
    {
        FontDescriptor descriptor=mFont->mDescriptor;
        descriptor.mStyle|=FontStyle_Underline;
        mLinkFont=GetFont(descriptor);
    }
    SetComponentFlags(8,true);
    mMaxChars=80*2000;
    mMaxLines=0;
    mWrapIndentSize=0;
    mLineAlign=HorzAlign_Left;
    mTopPad=mLeftPad=mRightPad=mBottomPad=5;
    Reset(false);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00711b00
// (public: virtual void * __thiscall WONAPI::TextArea::`scalar deleting destructor'(unsigned int)    [??_GTextArea@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00711b20
// (public: __thiscall std::map<class WONAPI::GUIString,int,struct std::less<class WONAPI::GUIString>,class std::allocator<struct std::pair<class WONAPI::)
