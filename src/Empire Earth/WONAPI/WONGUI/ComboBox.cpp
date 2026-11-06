// Whole native ComboBox source draft; uncompiled and unpromoted.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Native2f3970..2f3a2b187B; base/member/default setup conservative support0.
ComboBox::ComboBox()
{
    SetComponentFlags(8,true);
    SetOnlyChildInput(true); //actual byte7c, GUI contract pending
    mListBoxVisible=false;
    mNoType=false;
    mShowButtonWhenDisabled=true;
    mListBoxWidth=200;
    mListBoxHeight=150;
}
// Native2f3a50..2f3b6e286B; detach before automatic owned cleanup.
// FUNCTION: WON 0x006f3a50
ComboBox::~ComboBox()
{
    if(mListBox.get()!=NULL) mListBox->AddListener(NULL);
}
// Native2f3b70..2f3ba351B.
// FUNCTION: WON 0x006f3b70
ListItem* ComboBox::AddString(const GUIString& inText,int inPos)
{
    if(mListArea.get()!=NULL) {
        ListItem* item=mListArea->InsertString(inText,inPos);
        CalcListBoxSize();
        return item;
    }
    return NULL;
}
// Native2f3bb0..2f3c0383B; null-list path takes and immediately drops one owner.
// FUNCTION: WON 0x006f3bb0
void ComboBox::InsertItem(TextListItem* inItem,int inPos)
{
    if(mListArea.get()!=NULL) {
        mListArea->InsertItem(inItem,inPos);
        CalcListBoxSize();
    } else {
        SmartPtr<TextListItem> held=inItem;
    }
}
// Native2f3c10..2f3c3234B.
// FUNCTION: WON 0x006f3c10
void ComboBox::DeleteItem(int inPos)
{
    if(mListArea.get()!=NULL) {
        mListArea->DeleteItem(inPos);
        CalcListBoxSize();
    }
}
// Native2f3c40..2f3c5723B; guarded forwarding support0.
// FUNCTION: WON 0x006f3c40
void ComboBox::UpdateItem(TextListItem* inItem)
{
    if(mListArea.get()!=NULL) mListArea->Fn2FC730(inItem);
}
// Native2f3c60..2f3d37215B; empty-string owner ends before size calculation.
// FUNCTION: WON 0x006f3c60
void ComboBox::Clear(bool inClearInput)
{
    mListArea->Fn2FAA50(true);
    if(inClearInput) mEdit->SetText(GUIString(new GUIStringData("")),true);
    CalcListBoxSize();
}
// Native2f3d40..2f3d5b27B; existing index-return contract, support0.
// FUNCTION: WON 0x006f3d40
int ComboBox::GetSelItem()
{
    ListItem* item=mListArea->mSelItem;
    return item==NULL ? -1 : mListArea->GetItemPos(item);
}
// Native2f3d60..2f3e68264B; selection callback occurs before fresh member read.
// FUNCTION: WON 0x006f3d60
void ComboBox::SetSelItem(int inPos,bool inNotify)
{
    if(mListArea.get()==NULL) return;
    mListArea->SetSelItem(inPos,true);
    if(mListArea->mSelItem==NULL)
        mEdit->SetText(GUIString(new GUIStringData("")),inNotify);
    else SetInputBoxToListSelection(false,inNotify);
}
// Native2f3e70..2f3ee3115B. Preserve input dereference before its late null check.
// FUNCTION: WON 0x006f3e70
void ComboBox::SetListBox(Scroller* inListBox)
{
    mListBox=inListBox;
    if(mListArea.get()!=NULL && inListBox->GetScrollArea()!=mListArea.get() && inListBox!=NULL)
        mListArea->SetScroller(mListBox.get());
}
// Native2f3ef0..2f3f0e30B.
// FUNCTION: WON 0x006f3ef0
void ComboBox::Virtual6C()
{
    if(mEdit->mParent!=this) AddControls();
    Container::Virtual6C();
}
// Native2f3f10..2f3f9c140B; real complete listener allocation c, no widget new.
// FUNCTION: WON 0x006f3f10
void ComboBox::AddControls()
{
    Fn2F6A60(mEdit.get());
    Fn2F6A60(mButton.get());
    mListArea->Fn2FB600(0x1000,true);
    mListArea->SetComponentFlags(8,false);
    mListBox->AddListener(new ComponentListenerComp(this));
    mButton->Fn2F3370(0x40,true);
    mButton->SetComponentFlags(8,false);
}
// Native2f3fa0..2f4010112B; exact source dimensions require GUI reference fields.
// FUNCTION: WON 0x006f3fa0
void ComboBox::CalcListBoxSize()
{
    if(mListBoxVisible) return;
    int width=mListBoxWidth;
    int contentWidth=mListBox->Width()-mListArea->Width()+mListArea->GetRightPad()+
        mListArea->GetLeftPad()+mListArea->GetTotalWidth();
    if(width>contentWidth) width=contentWidth;
    if(width<Width()) width=Width();
    int height=mListArea->GetTotalHeight()+4;
    if(height>mListBoxHeight) height=mListBoxHeight;
    mListBox->Fn2F4B90(width,height);
}
// Native2f4010..2f402117B; forwarding support0.
// FUNCTION: WON 0x006f4010
void ComboBox::Virtual08() { Container::Virtual08(); CalcListBoxSize(); }
// Native2f4030/28,4050/18,4070/18 state-set+size forwards; provisional support0.
// FUNCTION: WON 0x006f4030
void ComboBox::SetListBoxDimensions(int w,int h) { mListBoxWidth=w;mListBoxHeight=h;CalcListBoxSize(); }
// FUNCTION: WON 0x006f4050
void ComboBox::SetListBoxWidth(int w) { mListBoxWidth=w;CalcListBoxSize(); }
// FUNCTION: WON 0x006f4070
void ComboBox::SetListBoxHeight(int h) { mListBoxHeight=h;CalcListBoxSize(); }
// Native2f4090..2f40ac28B.
// FUNCTION: WON 0x006f4090
void ComboBox::Virtual48()
{
    Container::Virtual48();
    if(mFocusChild==NULL) mEdit->Fn2F5890();
}
// Native2f40b0..2f40b55B; inherited forwarding support0.
// FUNCTION: WON 0x006f40b0
void ComboBox::Virtual4C() { Container::Virtual4C(); }
// Native2f40c0..2f419d221B. Borrowed selected item snapshot survives SetText callback.
// FUNCTION: WON 0x006f40c0
void ComboBox::SetInputBoxToListSelection(bool inActive,bool inFireEvent)
{
    TextListItem* item=static_cast<TextListItem*>(mListArea->mSelItem);
    if(item==NULL) return;
    mEdit->SetText(item->mText,true);
    if(item->mL10>mEdit->Width()-10) mEdit->SelectRange(item->mText.length(),0);
    else mEdit->SelectRange(0,-1);
    if(inFireEvent) Fn2F53F0(new ComboSelChangedEvent(this,item,inActive),false);
}
// Native2f42b0..2f42d335B. Two forwards, conservative support0.
// FUNCTION: WON 0x006f42b0
void ComboBox::SetText(const GUIString& inText,bool inNotify)
{
    mEdit->SetText(inText,inNotify);
    Invalidate();
}
// Native2f42e0..2f4362130B. Count snapshot once; current list/item each iteration.
// FUNCTION: WON 0x006f42e0
void ComboBox::SetTextAndSel(const GUIString& inText,bool inNotify)
{
    if(mListArea.get()!=NULL) {
        int count=mListArea->GetNumItems();
        for(int i=0;i<count;++i) {
            TextListItem* item=static_cast<TextListItem*>(mListArea->GetItem(i));
            if(item!=NULL && item->mText==inText) {
                SetSelItem(i,inNotify);
                return;
            }
        }
    }
    SetText(inText,inNotify);
}
// Native2f4370..2f44d1353B. PopupParams full2c typed contract pending GUI G.
// FUNCTION: WON 0x006f4370
void ComboBox::ShowList()
{
    if(mListBoxVisible) return;
    CalcListBoxSize();
    Window* window=GetRoot();
    int x=0,y=Height();
    Fn2F56D0(x,y);
    PopupParams params(mListBox.get(),0x2347,this,x,y);
    params.mInputComponent=this; //actual borrowed input-component08
    params.mAvoidRect.y=y-Height();
    params.mAvoidRect.height=Height();
    ListItem* selected=mListArea->mSelItem;
    if(selected!=NULL) {
        TextListItem* textItem=static_cast<TextListItem*>(selected);
        if(textItem->mText!=mEdit->GetText()) {
            int i=0;
            while(i<mListArea->GetNumItems()) {
                TextListItem* item=static_cast<TextListItem*>(mListArea->GetItem(i));
                if(item->mText==mEdit->GetText()) {
                    mListArea->SetSelItem(i,false);
                    mListArea->EnsureVisible(item);
                    break;
                }
                ++i;
            }
            if(i==mListArea->GetNumItems()) mListArea->SetSelItem(-1,false);
        } else mListArea->EnsureVisible(selected);
    }
    window->OpenPopup(params);
    mListBoxVisible=true;
}
// Native2f44e0..2f45f5277B. Button sender comparison precedes all event-kind reads.
// FUNCTION: WON 0x006f44e0
void ComboBox::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mComponent==mButton.get()) {
        if(!mListBoxVisible) {
            mUndoStr=mEdit->GetText();
            ShowList();
            return;
        }
        mListBoxVisible=false;
        GetRoot()->ClosePopup(mListBox.get());
        return;
    }
    if(inEvent->mType==23) {
        if(static_cast<ListItemClickedEvent*>(inEvent)->mButton!=0) return;
        SetInputBoxToListSelection(true,true);
        mListBoxVisible=false;
        GetRoot()->ClosePopup(mListBox.get());
        return;
    }
    if(inEvent->mType==30) {
        if(mListBoxVisible) mListBoxVisible=false;
        return;
    }
    if(inEvent->mType==21) {
        Fn2F5380(inEvent->mType,1);
        return;
    }
    if(inEvent->mType==20) {
        Fn2F5380(inEvent->mType,1);
        return;
    }
    Component::HandleComponentEvent(inEvent);
}
// Native2f4600..2f464c76B. Current mouse child and no-type checks AFTER base call.
// FUNCTION: WON 0x006f4600
void ComboBox::Virtual2C(int x,int y,int button)
{
    Container::Virtual2C(x,y,button);
    if(button==0) {
        if(mMouseChild==mEdit.get() && mNoType) mButton->Fn2F3440();
        Fn2F5890();
    }
}
// Native2f4650..2f467234B. Guarded forwarding, conservative support0.
// FUNCTION: WON 0x006f4650
bool ComboBox::Virtual3C(int amount)
{
    return mListBoxVisible ? mListArea->Virtual3C(amount) : false;
}
// Native2f4680..2f4799281B. Popup navigation and undo paths preserve current-member reloads.
// FUNCTION: WON 0x006f4680
bool ComboBox::Virtual54(int key)
{
    if(key==40 && !mListBoxVisible) {
        mUndoStr=mEdit->GetText();
        SetInputBoxToListSelection(false,true);
        ShowList();
        return true;
    }
    if(mListBoxVisible) {
        if(key==40 || key==38 || key==33 || key==34) {
            mListArea->Virtual54(key);
            SetInputBoxToListSelection(false,true);
            return true;
        }
        if(key==13) {
            SetInputBoxToListSelection(true,true);
            mButton->Fn2F3440();
            return true;
        }
        if(key==27) {
            mEdit->SetText(mUndoStr,true);
            mButton->Fn2F3440();
            return true;
        }
    }
    return Container::Virtual54(key);
}
// Native2f47a0..2f47ff95B. Setter callbacks precede current focus-child comparison.
// FUNCTION: WON 0x006f47a0
void ComboBox::SetNoType(bool inNoType)
{
    if(inNoType==mNoType) return;
    mNoType=inNoType;
    mEdit->SetComponentFlags(8,!inNoType);
    mEdit->Fn2F84B0(!inNoType);
    if(inNoType && mFocusChild==mEdit.get()) VirtualB4(NULL);
}
// Native2f4800..2f4888136B. Closing popup changes flag BEFORE GetRoot callback.
// FUNCTION: WON 0x006f4800
void ComboBox::SetEnabled(bool inEnabled)
{
    if(inEnabled==!IsDisabled()) return;
    Component::SetEnabled(inEnabled);
    mEdit->SetEnabled(inEnabled);
    mButton->SetEnabled(inEnabled);
    if(!inEnabled && mListBoxVisible) {
        mListBoxVisible=false;
        GetRoot()->ClosePopup(mListBox.get());
    }
    if(!mShowButtonWhenDisabled) {
        mButton->SetVisible(inEnabled);
        Fn2F6290();
    }
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f3a30
// (public: virtual void * __thiscall WONAPI::ComboBox::`scalar deleting destructor'(unsigned int)    [??_GComboBox@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006f4250
// (public: virtual __thiscall WONAPI::ComponentEvent::~ComponentEvent(void)    [??1ComponentEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f41a0
// (public: virtual void * __thiscall WONAPI::ComboSelChangedEvent::`scalar deleting destructor'(unsigned int)    [??_GComboSelChangedEvent@WONAPI@@UAEPAX)
// TEMPLATE: WON 0x006f41c0
// (public: virtual __thiscall WONAPI::ComboSelChangedEvent::~ComboSelChangedEvent(void)    [??1ComboSelChangedEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f3970
// (public: __thiscall WONAPI::ComboBox::ComboBox(void)    [??0ComboBox@WONAPI@@QAE@XZ])
