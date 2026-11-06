// Complete native FontConfig98 parser/cache owner; base lifetime remains original external.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyParser.h"
namespace WONAPI {
// Full actual constructor38a1c0 remains original external; no newconstruction here.
// FUNCTION: WON 0x0078a300
Font* FontConfig::GetFont()
{
    if(mNeedRecalcFont) {
        if(mFont.get()==NULL || mFont->mDescriptor!=mDescriptor)
            mFont=Window::GetDefaultWindow()->GetFont(mDescriptor).get();
    }
    return mFont.get();
}
// FUNCTION: WON 0x0078a3e0
void FontConfig::SetFont(Font* font)
{
    mFont=font;
    if(font!=NULL) mDescriptor=font->mDescriptor;
    mNeedRecalcFont=false;
}
// FUNCTION: WON 0x0078a580
bool FontConfig::Virtual2C(const std::string& name,int& value)
{
    if(mFont.get()==NULL) return false;
    if(name=="SIZE") value=mDescriptor.mSize;
    else if(name=="HEIGHT") value=mFont->GetHeight();
    else return false;
    return true;
}
// FUNCTION: WON 0x0078a5f0
bool FontConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="SRC") SetFont(ReadFont().get());
    else if(instruction=="SIZE") {
        mNeedRecalcFont=true;
        mDescriptor.mSize=ReadInt();
    } else if(instruction=="STYLE") {
        while(true) {
            mNeedRecalcFont=true;
            bool add=true;
            GUIString flag=ReadFlag(&add);
            if(flag.length()==0) break;
            int style=0;
            if(flag.CompareNoCase("Bold")==0) style=FontStyle_Bold;
            else if(flag.CompareNoCase("Italic")==0) style=FontStyle_Italic;
            else if(flag.CompareNoCase("Plain")==0) style=FontStyle_Plain;
            else if(flag.CompareNoCase("Underline")==0) style=FontStyle_Underline;
            else throw ConfigObjectException("Unknown font style: "+(std::string)flag);
            if(add) mDescriptor.mStyle|=style;
            else mDescriptor.mStyle&=~style;
        }
    } else if(instruction=="SETDEFAULTFONT") WONAPI::WindowManager::GetDefaultWindowManager()->SetDefaultFont(ReadFont().get());
    else if(instruction=="SETNAMEDFONT") {
        GUIString name=Fn3842F0(false);
        EnsureComma();
        SmartPtr<Font> font=ReadFont();
        WONAPI::WindowManager::GetDefaultWindowManager()->SetNamedFont(name,font.get());
    } else return false;
    return true;
}
// Actual PostParse3099f0 shared true hook, support0.
bool FontConfig::Virtual10(ConfigParser*) {return true;}
}


namespace WONAPI {
// FUNCTION: WON 0x0078a080
void FontConfig_Init(ResourceConfigTable* table)
{
    table->AddAllocator("Font",FontConfig::CfgFactory,FontConfig::CompFactory);
}
}

// Round8 B: genuine static callbacks used by this original SDK registration unit.
namespace WONAPI {

// SUPPORT-ONLY: EXE 0x006c7b10 (genuine SDK shared NULL callback; zero new native/primary credit)
Component* FontConfig::CompFactory()
{
    return NULL;
}

}

// Private Round9 genuine SDK FontConfig additions; apply only under Root exact lease.
namespace WONAPI {
// FUNCTION: WON 0x0078a160
ComponentConfig* FontConfig::CfgFactory()
{
    return new FontConfig;
}
// FUNCTION: WON 0x0078a1c0
FontConfig::FontConfig()
{
    mHaveInitedComponents=true;
    mNeedRecalcFont=true;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0078a080
// (void __cdecl WONAPI::FontConfig_Init(class WONAPI::ResourceConfigTable *)    [?FontConfig_Init@WONAPI@@YAXPAVResourceConfigTable@1@@Z])
// TEMPLATE: WON 0x0078a250
// (public: virtual __thiscall WONAPI::FontConfig::~FontConfig(void)    [??1FontConfig@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0078a230
// (public: virtual void * __thiscall WONAPI::FontConfig::`scalar deleting destructor'(unsigned int)    [??_GFontConfig@WONAPI@@UAEPAXI@Z])
