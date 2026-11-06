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
