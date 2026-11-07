// ChooseNetworkAdapterDialog family source intent; pending full MultiListItem28 contract.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyDialogs.h"
#include "LostDebug.h"
using namespace WONAPI;
// Actual RTTI is global NetworkAdapterListItem, not WONAPI namespace.
class NetworkAdapterListItem : public MultiListItem {
public:
    NetworkAdapterListItem(long inHost) : mHost(inHost) {}
    virtual int Virtual10(const ListItem* inOther,ListArea* inList) const;
    long mHost; //28; complete size2c, inherited vector ownership at18
};
// FUNCTION: WON 0x7f9c00
int NetworkAdapterListItem::Virtual10(const ListItem* inOther,ListArea* inList) const
{
    long aHost=static_cast<const NetworkAdapterListItem*>(inOther)->mHost;
    if(mHost==aHost) return 0;
    if(mHost==0) return -1;
    if(aHost==0) return 1;
    return MultiListItem::Virtual10(inOther,inList);
}
namespace WONAPI {

ChooseNetworkAdapterDialog::ChooseNetworkAdapterDialog() {}
// FUNCTION: WON 0x7f9c80
ChooseNetworkAdapterDialog::~ChooseNetworkAdapterDialog() {}
// FUNCTION: WON 0x7f9d50
void ChooseNetworkAdapterDialog::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* aConfig=static_cast<ComponentConfigInit&>(inInit).mConfig;
    SafeGetComponent(aConfig,mAdapterList,"AdapterList",__FILE__,60);
    SafeGetComponent(aConfig,mActivateButton,"ActivateButton",__FILE__,61);
}
// FUNCTION: WON 0x7f9da0
void ChooseNetworkAdapterDialog::Reset()
{
    // LOST-DEBUG: none: exact only
    EE_LOST_DEBUG(1);
    mAdapterList->Fn2FAA00();
    mAdapterList->Fn2FAA50(false);
    mAdapterList->InsertItem(new NetworkAdapterListItem(0),-1);
    mAdapterList->SetString(0,0,::ChooseNetworkAdapterDialog_Default_String,NULL);
    if(WONAPI::IPAddr::GetLocalHost()==0) mAdapterList->SetSelItem(0,false);
    std::list<LobbyNetAddress> anAddresses;
    LobbyGetNetAddresses(anAddresses,true);
    for(std::list<LobbyNetAddress>::const_iterator anItr=anAddresses.begin();anItr!=anAddresses.end();++anItr) {
        const LobbyNetAddress& aNet=*anItr;
        IPAddr anAddress(aNet.mHandle,0);
        GUIString aName(aNet.mName);
        if(aName.length()==0) aName=::ChooseNetworkAdapterDialog_Unknown_String;
        mAdapterList->InsertItem(new NetworkAdapterListItem(aNet.mHandle),-1);
        mAdapterList->SetString(mAdapterList->GetNumItems()-1,0,aName,NULL);
        mAdapterList->SetString(mAdapterList->GetNumItems()-1,1,anAddress.GetHostString(true),NULL);
        if(WONAPI::IPAddr::GetLocalHost()==aNet.mHandle) mAdapterList->SetSelItem(mAdapterList->GetNumItems()-1,true);
    }
    mAdapterList->Fn2FAA10();
}
// Zero-credit selected-address getter: original3fa1d0,26B.
// FUNCTION: WON 0x007fa1d0
unsigned long ChooseNetworkAdapterDialog::GetSelectedAddress()
{
    NetworkAdapterListItem* anItem=static_cast<NetworkAdapterListItem*>(mAdapterList->mSelItem);
    return anItem==NULL ? WONAPI::IPAddr::GetLocalHost() : anItem->mHost;
}
// Actual shared13B Dialog delegate, zero substantive credit.
void ChooseNetworkAdapterDialog::HandleComponentEvent(ComponentEvent* inEvent)
{
    Dialog::HandleComponentEvent(inEvent);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f9c40
// (public: __thiscall WONAPI::ChooseNetworkAdapterDialog::ChooseNetworkAdapterDialog(void)    [??0ChooseNetworkAdapterDialog@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007f9c60
// (public: virtual void * __thiscall WONAPI::ChooseNetworkAdapterDialog::`scalar deleting destructor'(unsigned int)    [??_GChooseNetworkAdapterDialog@WO)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007fa110
// (public: __thiscall WONAPI::MultiListItem::MultiListItem(void)    [??0MultiListItem@WONAPI@@QAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007fa0f0
// (public: virtual void * __thiscall NetworkAdapterListItem::`scalar deleting destructor'(unsigned int))
// TEMPLATE: WON 0x007fa160
// (public: virtual __thiscall NetworkAdapterListItem::~NetworkAdapterListItem(void))
