// HTML heading, preformatted, font and paragraph style handling.
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
// FUNCTION: WON 0x00739e80
void HTMLDisplayGen::HandleHeading(HTMLHeadingTag* tag)
{
    if(tag->IsStart())
    {
        StartFreshParagraph();
        PushAlign(tag->mAlign);
        ++mBoldCount;
        PushFont(mDefaultFontFace.c_str(),HTMLTagType_H6-tag->GetType()+1,-1);
    }
    else
    {
        --mBoldCount;
        PopFont();
        PopAlign();
        StartFreshParagraph();
    }
}
// FUNCTION: WON 0x0073a220
void HTMLDisplayGen::HandlePre(HTMLTag* tag)
{
    if(tag->IsStart())
    {
        StartFreshParagraph();
        mInPre=true;
        ++mNoBreak;
        PushFont("Courier New",-1,-1,true);
    }
    else
    {
        PopFont();
        mInPre=false;
        --mNoBreak;
        StartFreshParagraph();
    }
}
// FUNCTION: WON 0x0073a280
void HTMLDisplayGen::HandleCenter(HTMLTag* tag)
{
    StartFreshLine();
    if(tag->IsStart()) PushAlign(HTMLAlign_Center);
    else PopAlign();
}
// FUNCTION: WON 0x0073a2c0
void HTMLDisplayGen::HandleModifyFontStyle(HTMLTag* tag,int style)
{
    int increment=tag->IsStart()?1:-1;
    if(style&FontStyle_Bold) mBoldCount+=increment;
    if(style&FontStyle_Italic) mItalicCount+=increment;
    if(style&FontStyle_Underline) mUnderlineCount+=increment;
    CalcFont();
}
// FUNCTION: WON 0x0073a300
void HTMLDisplayGen::HandleModifyFont(HTMLTag* tag,const char* face,int size,int color,bool mono)
{
    if(tag->IsStart()) PushFont(face,size,color,mono);
    else PopFont();
}
// FUNCTION: WON 0x0073a340
void HTMLDisplayGen::HandleFont(HTMLFontTag* tag)
{
    if(tag->IsStart())
    {
        if(tag->mFace.empty()) PushFont(NULL,tag->mSize,tag->mColor);
        else
        {
            std::string face=tag->mFace;
            PushFont(face.c_str(),tag->mSize,tag->mColor);
        }
    }
    else PopFont();
}
// FUNCTION: WON 0x0073a4a0
void HTMLDisplayGen::EndParagraph()
{
    PopAlign();
    StartFreshParagraph();
    mInParagraph=false;
}
// FUNCTION: WON 0x0073a4c0
void HTMLDisplayGen::HandleBlock(HTMLBlockTag* tag)
{
    if(tag->IsStart())
    {
        if(tag->GetType()==HTMLTagType_Paragraph)
        {
            StartFreshParagraph();
            mInParagraph=true;
        }
        else StartFreshLine();
        PushAlign(tag->mAlign);
    }
    else
    {
        if(tag->GetType()!=HTMLTagType_Paragraph)
        {
            StartFreshLine();
            PopAlign();
        }
        else if(mInParagraph) EndParagraph();
    }
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00702370
// (public: __thiscall WONAPI::GUIString::operator class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >(void)const )
