// Native WONGUI MSFont; SDK corroborates GDI resource and text contracts.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
using namespace WONAPI;

// FUNCTION: WON 0x007cee60
MSFont::MSFont(DisplayContext* context,const FontDescriptor& descriptor)
 : Font(context,descriptor) {
    mDC=CreateCompatibleDC(context->mDC);
    int height=-MulDiv(descriptor.mSize,96,72);
    int style=descriptor.mStyle;
    mFont=CreateFontA(height,0,0,0,(style&1)?FW_BOLD:FW_NORMAL,
        (style&2)?TRUE:FALSE,FALSE,FALSE,ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,
        descriptor.mFace.c_str());
    SelectObject(mDC,mFont);
    GetTextMetricsA(mDC,&mTextMetrics);
}

// FUNCTION: WON 0x007cef30
MSFont::~MSFont() {
    DeleteObject(mFont);
    DeleteDC(mDC);
}

// FUNCTION: WON 0x007cef60
void MSFont::Virtual04(DisplayContext* context) {
    SelectObject(context->mDC,mFont);
}

// FUNCTION: WON 0x007cef80
void MSFont::Virtual08(DisplayContext* context,const GUIString& text,
    unsigned short offset,unsigned short length,int x,int y) {
    TextOutW(context->mDC,x,y,text.mData->mText+offset,length);
    if(mDescriptor.mStyle&4) {
        Graphics* g=context->mGraphics;
        if(g) {
            int descent=GetDescent();
            int mod;
            if(descent<3) mod=0;
            else if(descent==3) mod=1;
            else mod=descent>>1;
            int yy=y+GetHeight()-mod-1;
            g->DrawLineHook(x,yy,x+GetStringWidth(text,offset,length),yy);
        }
    }
}

// FUNCTION: WON 0x007cf040
int MSFont::GetStringWidth(const GUIString& text,unsigned short offset,unsigned short length) {
    if(length==0) return 0;
    SIZE size;
    GetTextExtentPoint32W(mDC,text.mData->mText+offset,length,&size);
    return size.cx;
}

// FUNCTION: WON 0x007cf090
int MSFont::Virtual10(int character) {
    int width=0;
    GetCharWidthW(mDC,character,character,&width);
    return width;
}

// FUNCTION: WON 0x007cf0c0
int MSFont::GetHeight() { return mTextMetrics.tmHeight; }
// FUNCTION: WON 0x007cf0d0
int MSFont::GetAscent() { return mTextMetrics.tmAscent; }
// FUNCTION: WON 0x007cf0e0
int MSFont::GetDescent() { return mTextMetrics.tmDescent; }

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007cef10
// (public: virtual void * __thiscall WONAPI::MSFont::`scalar deleting destructor'(unsigned int)    [??_GMSFont@WONAPI@@UAEPAXI@Z])
