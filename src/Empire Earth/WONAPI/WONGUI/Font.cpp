// WONGUI Font descriptor behavior and complete descriptor ownership.
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
#include "ee/wonapi/WONStringCompare.h"
using namespace WONAPI;

// FUNCTION: WON 0x007267f0
bool FontDescriptor::operator<(const FontDescriptor& other) const {
    if(mSize<other.mSize) return true;
    if(mSize>other.mSize) return false;
    if(mStyle<other.mStyle) return true;
    if(mStyle>other.mStyle) return false;
    return StringCompareNoCase(mFace,other.mFace)<0;
}

// FUNCTION: WON 0x00726840
bool FontDescriptor::operator==(const FontDescriptor& other) const {
    if(mSize!=other.mSize) return false;
    if(mStyle!=other.mStyle) return false;
    return StringCompareNoCase(mFace,other.mFace)==0;
}

// FUNCTION: WON 0x00726880
bool FontDescriptor::operator!=(const FontDescriptor& other) const {
    return !(*this==other);
}

// FUNCTION: WON 0x007268a0
Font::Font(DisplayContext*,const FontDescriptor& descriptor)
 : mDescriptor(descriptor) {}

// FUNCTION: WON 0x00726a60
Font::~Font() {}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00726a40
// (public: virtual void * __thiscall WONAPI::Font::`scalar deleting destructor'(unsigned int)    [??_GFont@WONAPI@@UAEPAXI@Z])
