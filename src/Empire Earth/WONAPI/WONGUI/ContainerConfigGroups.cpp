// Native typed percent-layout and color-scheme child groups.
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
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/WONLayoutConfigs.h"
#include "ee/wonapi/WONPercentLayout.h"
namespace WONAPI {
// FUNCTION: WON 0x007885d0
bool ContainerConfig::Virtual04(ConfigParser* parser,const std::string& type)
{
    if(type=="PERCENTLAYOUT") {
        SmartPtr<PercentLayoutConfig> config=new PercentLayoutConfig;
        if(ParseChild(*parser,config.get(),false,true)) {
            int i;
            for(i=0;i<4;++i) if(config->refs[i]!=NULL) break;
            if(i==4) config->refs[0]=mComponent.get();
            PercentLayoutParam param(config->refs[0],config->refs[1],config->refs[2],config->refs[3],
                config->params[0],config->params[1],config->params[2],config->params[3],
                config->params[4],config->params[5],config->params[6],config->params[7]);
            mContainer->AddChildLayout(new PercentLayout(config->target,param),mCurLayoutId);
        }
        return true;
    }
    if(type=="COLORSCHEME") {
        SmartPtr<ColorSchemeConfig> config=new ColorSchemeConfig;
        if(ParseChild(*parser,config.get(),false,true))
            mContainer->Fn2F59A0(config->mColorScheme.get());
        return true;
    }
    return ComponentConfig::Virtual04(parser,type);
}
}
