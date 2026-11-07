// Actual native frame parsing and original-owned skin control instructions.
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
#include "ee/wonapi/WONLobbyStretchImage.h"
#include "ee/wonapi/WONLobbyWONFrameValueConfig.h"
#include "LostDebug.h"
using namespace WONAPI;
// FUNCTION: WON 0x007980a0
static WONFrame ReadFrame(ComponentConfig* config)
{
    SmartPtr<Image> image=config->ReadImage(false);
    StretchImage* stretch=dynamic_cast<StretchImage*>(image.get());
    if(stretch!=NULL) return stretch;
    config->EnsureComma();
    int left=config->ReadInt();
    if(config->EndOfString()) return WONFrame(image.get(),left);
    config->EnsureComma();
    int top=config->ReadInt(); config->EnsureComma();
    int right=config->ReadInt(); config->EnsureComma();
    int bottom=config->ReadInt();
    return WONFrame(image.get(),left,top,right,bottom);
}
namespace WONAPI {
// Exact genuine SDK out-of-line copy constructor. SUPPORT0/unmarked; no native entry claimed.
WONFrame::WONFrame(const WONFrame& from)
{
    Copy(from);
}
// FUNCTION: WON 0x00797f70
bool WONComboBoxConfig::Virtual24(const std::string& key)
{
    if(key=="DRAWFRAME") { bool on=Fn3837F0(); mComboBox->mDrawFrame=on; }
    else if(key=="BUTTONINFRAME") { bool on=Fn3837F0(); mComboBox->mButtonInFrame=on; }
    else if(key=="FRAME") mComboBox->SetFrame(ReadFrame(this));
    else return ComboBoxConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x007981f0
bool WONTabCtrlConfig::Virtual24(const std::string& key)
{
    if(key=="TRANSPARENT") mTabCtrl->SetTransparent(Fn3837F0());
    else if(key=="ADDSTANDARDTAB") {
        GUIString name=Fn3842F0(false); EnsureComma();
        Container* tab=SafeConfigGetComponent<Container*>(ReadComponent(),"Container");
        mTabCtrl->AddStandardTab(name,tab);
    } else if(key=="FRAME") mTabCtrl->SetFrame(ReadFrame(this));
    else return TabCtrlConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x007987e0
bool WONScrollContainerConfig::Virtual24(const std::string& key)
{
    if(key=="DRAWFRAME") {
        if(!Fn3837F0()) mScrollContainer->SetFrame(WONFrame());
    } else if(key=="TRANSPARENT") { bool on=Fn3837F0(); mScrollContainer->mTransparent=on; }
    else if(key=="FRAME") mScrollContainer->SetFrame(ReadFrame(this));
    else if(key=="CORNER") mScrollContainer->SetCorner(ReadImage(false));
    else return ScrollbarScrollerConfig::Virtual24(key);
    return true;
}
}
