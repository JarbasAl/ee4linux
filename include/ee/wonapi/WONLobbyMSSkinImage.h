#ifndef EE_WONAPI_WONLOBBYMSSKINIMAGE_H
#define EE_WONAPI_WONLOBBYMSSKINIMAGE_H
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyImageHelpers.h"
#include "ee/wonapi/WONLobbySkinFrame.h"
#include "ee/wonapi/WONLobbyWONButton.h"
namespace WONAPI {
class WONHeaderButton; class WONScrollbar; class WONInputBox; class WONComboBox;
class WONScrollContainer; class WONListCtrl; class WONTabCtrl; class WONFrameComponent;
// Actual80 original-owned outer skin, complete SDK field graph/native3152d0.
// Complete80 base member graph: natural base/member initialization permitted
// for the reviewed WONMSSkin constructor. Original own lifetime/copy providers retained.
class WONSkin : public RefCount {
public:
    WONInputBox* GetInputBox(); //315890 genuine SDK external API; call-only
    WONFrameComponent* GetDialogBack(); //3155f0 genuine SDK external API; call-only
    WONFrameComponent* GetPopupBack(); //315580 genuine SDK external API; call-only
    WONSimpleButton* GetCloseButton(); //315540 genuine SDK external API; call-only
    WONHeaderButton* GetHeaderButton(); //315520 genuine SDK external API; call-only
    WONButton* GetRadioButton(); //315510 genuine SDK external API; call-only
    WONButton* GetCheckbox(); //315500 genuine SDK external API; call-only
    WONButton* GetPushButton(); //3154f0 genuine SDK external API; call-only
    virtual ~WONSkin(); //3152d0 original external normal
    bool CopyButtonType(const GUIString&,WONButton*); //315c90 original external
    bool CopySimpleButtonType(const GUIString&,WONSimpleButton*); //315d00 original external
    WONListCtrl* GetListCtrl(); //315b20 actual225B composition
    WONButton* GetTabButton(); //315530 true original8B raw owned duplicate factory
    WONScrollbar* GetScrollbar(bool); //3156a0 true493B raw-result composition
    WONComboBox* GetComboBox(); //3158a0 true329B raw-result composition
    WONScrollContainer* GetScrollContainer(); //315ae0/53 support0
    WONTabCtrl* GetTabCtrl(); //315c10/114 support0
    WONSimpleButton* GetArrowButton(Direction); //315550 actual raw duplicate provider
    WONSimpleButton* GetScrollThumb(bool); //315660 actual raw duplicate provider
    WONSimpleButton* GetScrollGutter(bool); //315680 actual raw duplicate provider
    SmartPtr<WONButton> mPushButton,mCheckbox,mRadioButton;
    SmartPtr<WONHeaderButton> mHeaderButton;
    SmartPtr<WONButton> mTabButton;
    SmartPtr<WONSimpleButton> mCloseButton,mComboButton;
    SmartPtr<WONSimpleButton> mScrollThumb[2],mScrollGutter[2],mArrowButton[4];
    SmartPtr<WONScrollbar> mScrollbar[2];
    SmartPtr<WONInputBox> mInputBox;
    SmartPtr<WONComboBox> mComboBox;
    SmartPtr<WONScrollContainer> mScrollContainer;
    SmartPtr<WONListCtrl> mListCtrl;
    SmartPtr<WONTabCtrl> mTabCtrl;
    SmartPtr<WONFrameComponent> mPopupBack,mDialogBack;
    typedef std::map<GUIString,SmartPtr<WONButton>,GUIStringLessNoCase> ButtonTypeMap;
    typedef std::map<GUIString,SmartPtr<WONSimpleButton>,GUIStringLessNoCase> SimpleButtonTypeMap;
    ButtonTypeMap mButtonTypeMap; //68..73 actual VC6 tree0c
    SimpleButtonTypeMap mSimpleButtonTypeMap; //74..7f
};
class WONMSSkin : public WONSkin {
public:
    WONMSSkin(); //315d70 complete runtime state-image/widget/frame composition
    // Genuine SDK implicit destructor: no derived owned fields; automatic WONSkin base cleanup.
    enum MSImageFlag { MSImageFlag_Down=1,MSImageFlag_Default=2,MSImageFlag_Disabled=4,MSImageFlag_Checked=8 };
    enum MSImageType {
        MSImageType_Frame1,MSImageType_Frame2,MSImageType_Frame3,
        MSImageType_LeftArrow,MSImageType_RightArrow,MSImageType_UpArrow,MSImageType_DownArrow,
        MSImageType_PushButton,MSImageType_Checkbox,MSImageType_RadioButton,
        MSImageType_ScrollThumb,MSImageType_ScrollGutter,MSImageType_ScrollCorner,
        MSImageType_ListHeader,MSImageType_TabButton,MSImageType_TabCtrlFrame,
        MSImageType_ComboPopupFrame,MSImageType_CloseButton
    };
    // Actual14 nested renderer, ProgramImage10 +two bytes10/11 +natural padding.
    // Borrowed-only receiver. Natural implicit destructor matches generic328770;
    // no allocation/constructor/duplicate body is authored by this cohort.
    class MSImage : public ProgramImage {
    public:
        unsigned char mImageType,mImageFlags;
        MSImage(unsigned char t,unsigned char f=0,int w=0,int h=0) : ProgramImage(w,h),mImageType(t),mImageFlags(f) {} //genuineSDKinline, actualnew14/ProgramImage328790+bytes10/11
        virtual SmartPtr<Image> Duplicate(); //315a10 original external
        virtual void Paint(Graphics&); //3172b0 actual14
        void DrawPushButton(Graphics&,bool,bool=false);
        void DrawArrowButton(Graphics&,Direction);
        void DrawCloseButton(Graphics&);
        bool FlagSet(unsigned char flags) { return mImageFlags&flags?true:false; }
    };
};
}
#endif
