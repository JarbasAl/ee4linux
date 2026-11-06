// Original configuration loading and recursive component initialization.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <cstdio>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/WONPerfTimer.h"
#include "ee/wonapi/WONMSMessageBoxAPI.h"

// Actual original file-static data shared with ComponentConfigInstructions.cpp;
// no duplicate state is allocated by this split translation unit.
extern bool gShowParseTime;
extern int gLinesParsed;

namespace WONAPI {
// FUNCTION: WON 0x007828c0
bool ComponentConfig::ReadConfig_Unsafe(Component* component,const char* fileName,
                                      ResourceConfigTable* table,ConfigParser* parser)
{
    Virtual34(component);
    WONAPI::WindowManager::GetDefaultWindowManager()->SetCacheImages(true);
    SmartPtr<ResourceConfigTable> oldTable=mResourceConfigTable;
    if(table!=NULL) mResourceConfigTable=table;
    else mResourceConfigTable=new ResourceConfigTable;

    bool oldShowParseTime=gShowParseTime;
    gShowParseTime=false;
    std::string path=GetResourceFile(fileName);
    ConfigParser localParser;
    ConfigParser& activeParser=parser?*parser:localParser;
    activeParser.mWarningsAsErrors=true;
    if(!ParseTopLevel(activeParser,path.c_str())) {
        activeParser.CloseFile();
        mConfigError=activeParser.GetAbortReason();
        return false;
    }

    try {
        mResourceConfigTable->SafeGlueResources();
        InitComponents();
    } catch(ConfigObjectException& error) {
        mConfigError=GUIString(error.mText);
        return false;
    }

    if(gShowParseTime) {
        char text[512];
        sprintf(text,"Lines parsed: %d\nConfig Parse Time: %.2f (%.2f) (= %%%d)",
                     gLinesParsed,PerfTimer::GetElapsedTime(),PerfTimer::GetCountedTime(),
                     (int)(PerfTimer::GetPercentTime()*100));
        MSMessageBox::Msg(Window::GetDefaultWindow(),GUIString("Config Parse Time"),
                          GUIString(text),gEmptyGUIString,gEmptyGUIString,gEmptyGUIString);
    }
    gShowParseTime=oldShowParseTime;
    WONAPI::WindowManager::GetDefaultWindowManager()->SetCacheImages(false);
    mResourceConfigTable=oldTable;
    return true;
}

// FUNCTION: WON 0x00782cc0
void ComponentConfig::InitComponents()
{
    if(mHaveInitedComponents) return;
    mHaveInitedComponents=true;
    mComponent->InitComponent(ComponentConfigInit(this));
    std::map<std::string,SmartPtr<ComponentConfig>,StringLessNoCase>::iterator it=mConfigMap.begin();
    while(it!=mConfigMap.end()) {
        it->second->InitComponents();
        ++it;
    }
}
}

#include <stdlib.h>
namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK failure policy ported to existing target API spellings.
void ComponentConfig::ReadConfig(Component* theComponent,const char* theFileName,ResourceConfigTable* theTable,ConfigParser* theParser)
{
    if(!ReadConfig_Unsafe(theComponent,theFileName,theTable,theParser))
    {
        // SDK InitSystemColorScheme inline wrapper: target virtual binds its own retained scheme.
        WindowManager::GetDefaultWindowManager()->GetSystemColorScheme();
        MSMessageBox::Msg(Window::GetDefaultWindow(),GUIString("Config Error"),mConfigError,
                          GUIString("Ok"),gEmptyGUIString,gEmptyGUIString);
        WindowManager::GetDefaultWindowManager()->Fn32D5B0();
        exit(0);
    }
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK explicit initialization, adapted to current plain ComponentInit base.
ComponentConfigInit::ComponentConfigInit(ComponentConfig* theConfig)
{
    mType = 2;
    mConfig = theConfig;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0078aa40
// (public: __thiscall WONAPI::ComponentConfigInit::ComponentConfigInit(class WONAPI::ComponentConfig *)    [??0ComponentConfigInit@WONAPI@@QAE@PAVCompone)
