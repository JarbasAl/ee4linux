// HTML document traversal setup and original global default font owner.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
namespace WONAPI {
// Actual566278 real string16: native338000 constructs this literal,338090 destroys it.
// FUNCTION: WON 0x00737ff0
// (_$E74)
// FUNCTION: WON 0x00738080
// (_$E73)
// FUNCTION: WON 0x00738090
// (_$E72)
static std::string gGlobalDefaultFontFace("Times New Roman");
// FUNCTION: WON 0x007380e0
void HTMLDisplayGen::SetGlobalDefaultFontFace(const std::string& face)
{
    gGlobalDefaultFontFace=face;
}
// FUNCTION: WON 0x007391c0
bool HTMLDisplayGen::FindBody()
{
    HTMLTag* tag=GetNextTag();
    if(tag==NULL) return false;
    if(tag->GetType()!=HTMLTagType_Body)
    {
        SmartPtr<HTMLBodyTag> body=new HTMLBodyTag;
        HandleTag(body.get());
    }
    --mTagListItr;
    return true;
}
// FUNCTION: WON 0x0073c430
void HTMLDisplayGen::Generate(HTMLDocument* document,MultiLineElement* display,Table* container)
{
    mTagList=&document->GetTagList();
    mTagListItr=document->GetBodyStart();
    mTagListEnd=document->GetBodyEnd();
    mWindow=WONAPI::WindowManager::GetDefaultWindowManager()->mDefaultWindow;
    mDisplay=display;
    mDisplayContainer=container;
    mAlignStack.clear();
    mFontStack.clear();
    mDefaultTextColor=(int)(0x80000000u|StandardColor_Text);
    mDefaultFontFace=gGlobalDefaultFontFace;
    mBoldCount=mItalicCount=mUnderlineCount=0;
    mDefaultLinkColor=(int)(0x80000000u|StandardColor_Link);
    mDefaultALinkColor=(int)(0x80000000u|StandardColor_LinkDown);
    mParsingTableData=false;
    mDefaultAlign=HTMLAlign_Left;
    if(!FindBody()) return;
    GenerateBase();
    LineSegment::mCounter=0;
    mDisplay->SetSelIndex();
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00738000
// (_$E71)
// TEMPLATE: WON 0x0074c9a0
// (public: class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInf)
// TEMPLATE: WON 0x0074cb40
// (public: class std::_Ptrit<enum WONAPI::HTMLAlign,int,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign &,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign)
// TEMPLATE: WON 0x007544e0
// (struct WONAPI::HTMLDisplayGen::FontInfo * __cdecl std::copy(class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGe)
// TEMPLATE: WON 0x007546a0
// (enum WONAPI::HTMLAlign * __cdecl std::copy(class std::_Ptrit<enum WONAPI::HTMLAlign,int,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign &,enum WONAPI:)
