#ifndef EE_WONAPI_WONLOBBYSKINLISTCONTROLS_H
#define EE_WONAPI_WONLOBBYSKINLISTCONTROLS_H
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyWONButton.h"
#include "ee/wonapi/WONLobbySkinnedContainers.h"
namespace WONAPI {
// Actual fieldless100 renderer derived from true WONButton100, VT450798/48slots.
// Borrowed-only, no receiver allocation/construction/copy/lifecycle source.
class WONHeaderButton : public WONButton {
public:
    WONHeaderButton(); //3191a0 external fieldless100, bounded F allocation
    virtual ~WONHeaderButton(); //319240 original external normal/deleting319220
    virtual void Paint(Graphics&); //3193c0 actual18
    WONHeaderButton* Duplicate(); //319350 genuine SDK raw duplicate provider
};
// Complete actual WONHeaderControlb4, real MultiListHeaderac plus textpadac/SkinSPb0.
// Complete b4 constructor source uses genuine MultiListHeader default base and retained skin; no copy authority.
class WONSkin;
class WONHeaderControl : public MultiListHeader {
public:
    WONHeaderControl(WONSkin* =NULL); //31a060 original external RET4
    // Genuine SDK implicit virtual destructor31a120; automatic SkinSP/MultiListHeader cleanup.
    void SetColumn(int,const GUIString&,int* =NULL); //31a1c0 real provider
protected:
    int mTextLeftPad; //ac
    SmartPtr<WONSkin> mSkin; //b0 retained, fullb4
};
// Actualc0 borrowed list control; truebc WONScrollContainer plus retained headerbc.
class WONListCtrl : public WONScrollContainer {
public:
    WONListCtrl(); //31a270 external fullc0, bounded F allocation
    // Genuine SDK implicit virtual destructor31a2b0; automatic HeaderSP/WONScrollContainer cleanup.
    virtual void Virtual08(); //31a580 support0 geometry forwarding
    WONListCtrl* Duplicate(); //31a520 original external raw result
    void SetHeader(WONHeaderControl*); //31a3e0 original external RET4
    WONHeaderControl* GetHeader() { return mHeader.get(); } //genuine SDK inline
    MultiListArea* GetListArea() { return (MultiListArea*)GetScrollArea(); } //genuine SDK inline, borrowed84
    void SetStandarColumnHeader(int,const GUIString&,int); //31a450 genuine SDK spelling
    void SetColumnHeader(int,Component*,int); //31a4c0
    SmartPtr<WONHeaderControl> mHeader; //bc..bf
};
}
#endif
