// Genuine SDK MS control constructor completion; support0.
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
#include "ee/wonapi/WONLobbyRoomServer.h"
namespace WONAPI {
// SUPPORT: EXE 0x006ff0c0; native86B, true Direction retained after real Button base.
MSArrowButton::MSArrowButton(Direction direction)
{
    mDirection=direction;
    Fn2F3370(2,true);
}
// SUPPORT: EXE 0x006fee20; native135B, text setup precedes fill flags and vertical alignment.
MSTabButton::MSTabButton(const GUIString& text,bool fillWhole,int lower)
{
    mLowerAmount=lower;
    if(text.length()!=0) {
        mText->SetText(text);
        Fn2F4E90(0,0);
    }
    mIsTransparent=!fillWhole;
    Fn2F3370(0x48,true);
    SetVertAlign(VertAlign_Center);
}
// FUNCTION: WON 0x00700780
MS3DFrame::MS3DFrame(int type,bool up)
{
    mType=type;
    mIsUp=up;
    SetComponentFlags(0x800,true);
}
// FUNCTION: WON 0x007006a0
MSSeperator::MSSeperator(int height)
{
    Fn2F4B90(Width(),height);
}
}

// Genuine MSControls.cpp explicit setup and retained child composition.
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
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
#include "ee/wonapi/WONLobbyMSRemainingPainters.h"
namespace WONAPI {
// FUNCTION: WON 0x006febf0
void MSButton::Init()
{
    mLeftTextPad=mRightTextPad=8;
    mTopTextPad=mBottomTextPad=4;
    SetComponentFlags(8,true);
    Fn2F3370(0x400,true);
    mUseFrame2=false;
}
// SUPPORT: EXE 0x006fec30; explicit SDK default constructor,73B.
MSButton::MSButton() { Init(); }
// SUPPORT: EXE 0x006feca0; explicit SDK text constructor,99B.
MSButton::MSButton(const GUIString& text)
{
    Init();
    mText->SetText(text);
    Fn2F4E90(0,0);
}
// SUPPORT: EXE 0x006fef40; explicit SDK header button constructor,139B.
MSHeaderButton::MSHeaderButton(const GUIString& text)
{
    SetComponentFlags(8,false);
    SetUseFrame2(true);
    IncrementHorzTextPad(2,0);
    SetHorzAlign(HorzAlign_Left);
    if(text.length()!=0) {
        mText->SetText(text);
        Fn2F4E90(0,0);
    }
}
// SUPPORT: EXE 0x00700d10; fullb0 real owner,35B.
MSHeaderControl::MSHeaderControl(MultiListArea* area) : MultiListHeader(area)
{
    mTextLeftPad=0;
}
// FUNCTION: WON 0x00700dd0
void MSHeaderControl::SetColumn(int column,const GUIString& title)
{
    SmartPtr<MSButton> button=new MSHeaderButton(title);
    SetColumnItem(column,button.get());
}
// SUPPORT: EXE 0x00700e80; fulla8 retained header child composition,277B.
MSListCtrl::MSListCtrl(MultiListArea* area)
    : MSScrollContainer(area ? area : new MultiListArea)
{
    mHeader=new MSHeaderControl(GetListArea());
    Fn2F6A60(mHeader.get());
    mHeaderHeight=15;
}
// FUNCTION: WON 0x007010b0
void MSListCtrl::Virtual08()
{
    MSScrollContainer::Virtual08();
    mHeader->Fn2F4BF0(mScrollArea->Left(),mScrollArea->Top()-mHeaderHeight,
                     mScrollArea->Width(),mHeaderHeight);
}
}

#include "ee/wonapi/WONLobbyMSRemainingPainters.h"
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
namespace WONAPI {

// FUNCTION: WON 0x006ff3b0
void MSCheckbox::Init()
{
    mStandardTextColor=StandardColor_Text;
    mTextColor=-1;
    mHAlign=HorzAlign_Left;
    mFocusStyle=FocusStyle_AroundText;
    mLeftTextPad=mCheckboxWidth+4;
    SetComponentFlags(8,true);
}
// FUNCTION: WON 0x006ff3e0
MSCheckbox::MSCheckbox() { Init(); }
inline void MSRadioButton::Init()
{
    Fn2F3370(8,true);
    mHAlign=HorzAlign_Left;
    mFocusStyle=FocusStyle_AroundText;
    mLeftTextPad=mRadioWidth+4;
}
// FUNCTION: WON 0x006ff530
MSRadioButton::MSRadioButton() { Init(); }
// SUPPORT: EXE 0x006ff6c0; genuine explicit SDK constructor,107B.
MSLabel::MSLabel()
{
    mStandardTextColor=StandardColor_Text;
    mTextColor=-1;
    mTopTextPad=mRightTextPad=mLeftTextPad=mBottomTextPad=0;
    mHAlign=HorzAlign_Left;
    Fn2F3370(0x200,true);
}
// SUPPORT: EXE 0x006ff740; genuine explicit SDK text constructor,131B.
MSLabel::MSLabel(const GUIString& text)
{
    mStandardTextColor=StandardColor_Text;
    mTextColor=-1;
    mTopTextPad=mRightTextPad=mLeftTextPad=mBottomTextPad=0;
    Fn2F3370(0x200,true);
    SetText(text,true);
    Fn2F4E90(0,0);
}
// SUPPORT: EXE 0x006ff200; genuine explicit SDK constructor,77B.
MSCloseButton::MSCloseButton() { Fn2F4B90(16,14); }

// SUPPORT: EXE 0x007008d0; genuine default constructor,201B.
MSEtchedFrameGroup::MSEtchedFrameGroup()
{
    mLabel=new Label;
    mLabel->SetHorzAlign(HorzAlign_Center);
}
}

#include "ee/wonapi/WONLobbyMSMessageBox.h"
#include "ee/wonapi/WONLobbyMSPainters.h"
namespace WONAPI {
// SUPPORT: EXE 0x00700550; genuine SDK explicit child composition,191B.
MSDialog::MSDialog()
{
    SmartPtr<MSDlgBack> back=new MSDlgBack;
    Fn2F6140(back.get(),3,this,0,0,0,0,0);
    Fn2F6A60(back.get());
}
}

#include "ee/wonapi/WONLobbyMSBorderPainters.h"
#include "ee/wonapi/WONLobbyMSRemainingPainters.h"
namespace WONAPI {
// SUPPORT: EXE 0x006fed10; genuine explicit SDK body, 11B native.
MSButton::~MSButton() {}
// SUPPORT: EXE 0x006ff140; genuine explicit SDK body, 11B native.
MSArrowButton::~MSArrowButton() {}
// SUPPORT: EXE 0x006ff420; genuine explicit SDK body, 11B native.
MSCheckbox::~MSCheckbox() {}
// SUPPORT: EXE 0x006ff5b0; genuine explicit SDK body, 11B native.
MSRadioButton::~MSRadioButton() {}
// SUPPORT: EXE 0x006ff350; genuine explicit SDK body, 11B native.
MSScrollThumb::~MSScrollThumb() {}
// SUPPORT: EXE 0x006ff310; genuine explicit SDK body, 18B native.
MSScrollThumb::MSScrollThumb() {}
// FUNCTION: WON 0x00700840
MSEtchedFrame::MSEtchedFrame() { SetComponentFlags(0x800,true); }
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006fec30
// (public: __thiscall WONAPI::MSButton::MSButton(void)    [??0MSButton@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006fec80
// (public: virtual void * __thiscall WONAPI::MSButton::`scalar deleting destructor'(unsigned int)    [??_GMSButton@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006feca0
// (public: __thiscall WONAPI::MSButton::MSButton(class WONAPI::GUIString const &)    [??0MSButton@WONAPI@@QAE@ABVGUIString@1@@Z])
// TEMPLATE: WON 0x006fed10
// (public: virtual __thiscall WONAPI::MSButton::~MSButton(void)    [??1MSButton@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006fee20
// (public: __thiscall WONAPI::MSTabButton::MSTabButton(class WONAPI::GUIString const &,bool,int)    [??0MSTabButton@WONAPI@@QAE@ABVGUIString@1@_NH@Z])
// TEMPLATE: WON 0x006fef40
// (public: __thiscall WONAPI::MSHeaderButton::MSHeaderButton(class WONAPI::GUIString const &)    [??0MSHeaderButton@WONAPI@@QAE@ABVGUIString@1@@Z])
// TEMPLATE: WON 0x006feff0
// (public: virtual __thiscall WONAPI::MSHeaderButton::~MSHeaderButton(void)    [??1MSHeaderButton@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ff0c0
// (public: __thiscall WONAPI::MSArrowButton::MSArrowButton(enum WONAPI::Direction)    [??0MSArrowButton@WONAPI@@QAE@W4Direction@1@@Z])
// TEMPLATE: WON 0x006ff120
// (public: virtual void * __thiscall WONAPI::MSArrowButton::`scalar deleting destructor'(unsigned int)    [??_GMSArrowButton@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006ff140
// (public: virtual __thiscall WONAPI::MSArrowButton::~MSArrowButton(void)    [??1MSArrowButton@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ff200
// (public: __thiscall WONAPI::MSCloseButton::MSCloseButton(void)    [??0MSCloseButton@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006ff290
// (public: virtual __thiscall WONAPI::MSCloseButton::~MSCloseButton(void)    [??1MSCloseButton@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ff310
// (public: __thiscall WONAPI::MSScrollThumb::MSScrollThumb(void)    [??0MSScrollThumb@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006ff330
// (public: virtual void * __thiscall WONAPI::MSScrollThumb::`scalar deleting destructor'(unsigned int)    [??_GMSScrollThumb@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006ff350
// (public: virtual __thiscall WONAPI::MSScrollThumb::~MSScrollThumb(void)    [??1MSScrollThumb@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ff400
// (public: virtual void * __thiscall WONAPI::MSCheckbox::`scalar deleting destructor'(unsigned int)    [??_GMSCheckbox@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006ff420
// (public: virtual __thiscall WONAPI::MSCheckbox::~MSCheckbox(void)    [??1MSCheckbox@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ff590
// (public: virtual void * __thiscall WONAPI::MSRadioButton::`scalar deleting destructor'(unsigned int)    [??_GMSRadioButton@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006ff5b0
// (public: virtual __thiscall WONAPI::MSRadioButton::~MSRadioButton(void)    [??1MSRadioButton@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ff6c0
// (public: __thiscall WONAPI::MSLabel::MSLabel(void)    [??0MSLabel@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006ff730
// (public: virtual __thiscall WONAPI::MSTabButton::~MSTabButton(void)    [??1MSTabButton@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ff740
// (public: __thiscall WONAPI::MSLabel::MSLabel(class WONAPI::GUIString const &)    [??0MSLabel@WONAPI@@QAE@ABVGUIString@1@@Z])
// TEMPLATE: WON 0x00700550
// (public: __thiscall WONAPI::MSDialog::MSDialog(void)    [??0MSDialog@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x00700610
// (public: virtual void * __thiscall WONAPI::MSDialog::`scalar deleting destructor'(unsigned int)    [??_GMSDialog@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007008d0
// (public: __thiscall WONAPI::MSEtchedFrameGroup::MSEtchedFrameGroup(void)    [??0MSEtchedFrameGroup@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x00700d10
// (public: __thiscall WONAPI::MSHeaderControl::MSHeaderControl(class WONAPI::MultiListArea *)    [??0MSHeaderControl@WONAPI@@QAE@PAVMultiListArea@1@@Z])
// TEMPLATE: WON 0x00700e80
// (public: __thiscall WONAPI::MSListCtrl::MSListCtrl(class WONAPI::MultiListArea *)    [??0MSListCtrl@WONAPI@@QAE@PAVMultiListArea@1@@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006feeb0
// (public: virtual void * __thiscall WONAPI::MSTabButton::`scalar deleting destructor'(unsigned int)    [??_GMSTabButton@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006fefd0
// (public: virtual void * __thiscall WONAPI::MSHeaderButton::`scalar deleting destructor'(unsigned int)    [??_GMSHeaderButton@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006ff270
// (public: virtual void * __thiscall WONAPI::MSCloseButton::`scalar deleting destructor'(unsigned int)    [??_GMSCloseButton@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007009a0
// (public: virtual void * __thiscall WONAPI::MSEtchedFrameGroup::`scalar deleting destructor'(unsigned int)    [??_GMSEtchedFrameGroup@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00700d40
// (public: virtual void * __thiscall WONAPI::MSHeaderControl::`scalar deleting destructor'(unsigned int)    [??_GMSHeaderControl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00700fa0
// (public: virtual void * __thiscall WONAPI::MSListCtrl::`scalar deleting destructor'(unsigned int)    [??_GMSListCtrl@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ff250
// (public: virtual void __thiscall WONAPI::MSCloseButton::Virtual0C(int &,int &))
// TEMPLATE: WON 0x00700700
// (public: virtual void __thiscall WONAPI::MSSeperator::Virtual0C(int &,int &))
// TEMPLATE: WON 0x007009c0
// (public: virtual __thiscall WONAPI::MSEtchedFrameGroup::~MSEtchedFrameGroup(void))
// TEMPLATE: WON 0x00700fc0
// (public: virtual __thiscall WONAPI::MSListCtrl::~MSListCtrl(void))
// TEMPLATE: WON 0x00700d60
// (public: virtual __thiscall WONAPI::MSHeaderControl::~MSHeaderControl(void))
