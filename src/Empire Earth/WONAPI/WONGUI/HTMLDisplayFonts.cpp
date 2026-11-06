// HTML font stack, point-size selection and alignment lookup.
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
// FUNCTION: WON 0x007386f0
int HTMLDisplayGen::GetFontStyle()
{
    int style=0;
    if(mBoldCount) style|=FontStyle_Bold;
    if(mItalicCount) style|=FontStyle_Italic;
    if(mUnderlineCount) style|=FontStyle_Underline;
    return style;
}
// FUNCTION: WON 0x00738720
bool HTMLDisplayGen::GetIsFontMonospace()
{
    return mFontStack.empty()?false:mFontStack.back().mIsMonoSpace;
}
// FUNCTION: WON 0x00738750
void HTMLDisplayGen::CalcFont()
{
    static int FONT_SIZE_TABLE[5][8]={
        {4,6,8,9,10,12,16,24},
        {5,7,9,10,12,16,20,30},
        {6,8,10,12,14,18,24,36},
        {8,10,12,14,16,20,28,42},
        {10,12,14,16,18,24,32,48}
    };
    if(mFontStack.empty()) return;
    FontInfo info=mFontStack.back();
    int size=info.mSize-1;
    if(size<0) size=0;
    else if(size>6) size=6;
    if(!info.mIsMonoSpace) size++;
    int pointSize=FONT_SIZE_TABLE[mFontSize][size];
    FontDescriptor descriptor(info.mFace,GetFontStyle(),pointSize);
    mCurFont=mWindow->GetFont(descriptor);
    if(info.mColor==-1) mCurColor=mDefaultTextColor; else mCurColor=info.mColor;
}
// FUNCTION: WON 0x00738920
// (??1FontInfo@HTMLDisplayGen@WONAPI@@QAE@XZ)

// FUNCTION: WON 0x00738960
void HTMLDisplayGen::PushFont(const char* face,int size,int color,bool mono)
{
    FontInfo info;
    if(!mFontStack.empty()) info=mFontStack.back();
    else
    {
        info.mColor=-1;
        info.mSize=3;
        info.mFace=mDefaultFontFace;
        info.mIsMonoSpace=false;
    }
    if(face!=NULL) info.mFace=face;
    if(size>=1 && size<=7) info.mSize=size;
    if(color!=-1) info.mColor=color;
    info.mIsMonoSpace=mono;
    mFontStack.push_back(info);
    CalcFont();
}
// FUNCTION: WON 0x00738b30
void HTMLDisplayGen::PopFont()
{
    if(mFontStack.size()>1)
    {
        mFontStack.pop_back();
        CalcFont();
    }
}
// FUNCTION: WON 0x00738ba0
HTMLAlign HTMLDisplayGen::GetCurAlign()
{
    return mAlignStack.empty()?mDefaultAlign:mAlignStack.back();
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0074c8a0
// (public: class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInf)
// TEMPLATE: WON 0x0074c920
// (public: class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInf)
// TEMPLATE: WON 0x007500f0
// (public: void __thiscall std::vector<struct WONAPI::HTMLDisplayGen::FontInfo,class std::allocator<struct WONAPI::HTMLDisplayGen::FontInfo> >::insert(cl)
// TEMPLATE: WON 0x007543c0
// (class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInfo &,stru)
// TEMPLATE: WON 0x00754880
// (protected: struct WONAPI::HTMLDisplayGen::FontInfo * __thiscall std::vector<struct WONAPI::HTMLDisplayGen::FontInfo,class std::allocator<struct WONAPI)
// TEMPLATE: WON 0x007548c0
// (void __cdecl std::fill(class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisp)
// TEMPLATE: WON 0x00754a30
// (class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInfo &,stru)
// TEMPLATE: WON 0x00754e40
// (void __cdecl std::_Construct(struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInfo const &))
