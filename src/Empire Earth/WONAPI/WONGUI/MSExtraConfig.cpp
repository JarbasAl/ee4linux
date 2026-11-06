// Actual MS control configuration dispatch on original-owned borrowed controls.
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
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x0078f810
bool MSComboBoxConfig::Virtual24(const std::string& key)
{
    if(key=="DRAWFRAME") { bool on=Fn3837F0(); mMSComboBox->SetDrawFrame(on); }
    else return ComboBoxConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x0078f880
bool MSScrollbarConfig::Virtual24(const std::string& key)
{
    if(key=="TRANSPARENT") mMSScrollbar->SetTransparent(Fn3837F0());
    else return ScrollbarConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x0078fa40
bool MSEtchedFrameGroupConfig::Virtual24(const std::string& key)
{
    if(key=="TITLE") mFrame->SetLabelText(Fn3842F0(false));
    else if(key=="CONTAINEDCOMPONENT") mFrame->AddContainedComponent(ReadComponent());
    else return ContainerConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x0078fb30
bool MSTabCtrlConfig::Virtual24(const std::string& key)
{
    if(key=="TRANSPARENT") mMSTabCtrl->SetTransparent(Fn3837F0());
    else if(key=="ADDSTANDARDTAB") {
        GUIString text=Fn3842F0(false); EnsureComma();
        Container* container=SafeConfigGetComponent<Container*>(ReadComponent(),"Container");
        mMSTabCtrl->AddStandardTab(text,container);
    } else return TabCtrlConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x0078fd30
bool MSScrollContainerConfig::Virtual24(const std::string& key)
{
    if(key=="DRAWFRAME") mScrollContainer->SetDrawFrame(Fn3837F0());
    else if(key=="DRAW3DFRAME") mScrollContainer->SetDraw3DFrame(Fn3837F0());
    else if(key=="TRANSPARENT") { bool on=Fn3837F0(); mScrollContainer->SetTransparent(on); }
    else return ScrollbarScrollerConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x0078ff00
bool MSHeaderControlConfig::Virtual24(const std::string& key)
{
    if(key=="SETSTANDARDCOLUMN") {
        int column=ReadInt(); EnsureComma(); GUIString title=Fn3842F0(false);
        mMSHeader->SetColumn(column,title);
    } else return MultiListHeaderConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x0078ffd0
bool MSListCtrlConfig::Virtual24(const std::string& key)
{
    if(key=="HEADERHEIGHT") { int height=ReadInt(); mListCtrl->SetHeaderHeight(height); }
    else return MSScrollContainerConfig::Virtual24(key);
    return true;
}
}
