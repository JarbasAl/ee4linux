// CreditsDialog configuration behavior; original CreditsDialog.cpp anchor3f76b0.
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
namespace WONAPI {
// FUNCTION: WON 0x7f7780
bool CreditsDialogConfig::Virtual24(const std::string& inInstruction)
{
    if(inInstruction!="ADDLINE") return DialogConfig::Virtual24(inInstruction);
    GUIString aLine=Fn3842F0(false);
    DWORD aColor=-1;
    if(!EndOfString()) {
        SkipComma();
        aColor=Fn385510();
    }
    mCredits->AddLine(aLine,Fn386860(std::string("DialogComponent")),aColor);
    return true;
}
// Actual25B SetComponent bridge38f100, zero helper quota.
void CreditsDialogConfig::Virtual34(Component* inComponent)
{
    DialogConfig::Virtual34(inComponent);
    mCredits=static_cast<CreditsDialog*>(inComponent);
}
// FUNCTION: WON 0x7f7660
void CreditsDialog::AddLine(const GUIString& inText,Component* inComponent,unsigned long inColor)
{
    Font* aFont=Fn2F5B20();
    int aWidth=aFont->GetStringWidth(inText,0,inText.length())+20;
    if(aWidth>inComponent->Width()) inComponent->Fn2F4B90(aWidth,inComponent->Height());
    mScrollingText->AddLine(inText,inColor);
}
// FUNCTION: WON 0x7f76b0
void CreditsDialog::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* aConfig=static_cast<ComponentConfigInit&>(inInit).mConfig;
    SafeGetComponent(aConfig,mCreditsButton,"CreditsButton",__FILE__,271);
    SafeGetComponent(aConfig,mCloseButton,"CancelButton",__FILE__,272);
    Component* aParent=mCreditsButton->mParent;
    Fn2F6140(mScrollingText.get(),0x5003,mCreditsButton.get(),aParent->Left(),aParent->Top(),0,0,0);
    Fn2F6A60(mScrollingText.get());
    mCreditsButton->SetVisible(false);
    UpdateCredits();
}
}

// R5 primary-SDK/global color algorithm and actual credits text/list/timer/paint behavior.
// FUNCTION: WON 0x007f7100
DWORD DimColor(DWORD color,int percent)
{
    if(percent<=0) return 0xffffff;
    if(percent>=100) return color;
    int red=(color>>16)&255,green=(color>>8)&255,blue=color&255;
    red=255-((255-red)*percent/100);
    green=255-((255-green)*percent/100);
    blue=255-((255-blue)*percent/100);
    return (red<<16)|(green<<8)|blue;
}
namespace WONAPI {
// FUNCTION: WON 0x007f6f90
void ScrollingTextComponent::AddLine(const GUIString& text,unsigned long color)
{
    if(color!=static_cast<DWORD>(-1)) mLastColor=color;
    mLines.push_back(CreditLineInfo(text,mLastColor));
}
// FUNCTION: WON 0x007f70b0
bool ScrollingTextComponent::Virtual44(int delta)
{
    mTimeSinceLastScroll+=delta;
    if(mTimeSinceLastScroll>=mScrollSpeed) {
        if(--mScrollTop < -mTotalTextHeight) mScrollTop=Height();
        Invalidate();
        mTimeSinceLastScroll=0;
    }
    return Component::Virtual44(delta);
}
// FUNCTION: WON 0x007f71b0
void ScrollingTextComponent::Paint(Graphics& g)
{
    Component::Paint(g);
    g.SetFont(Fn2F5B20());
    g.SetColor(0xffffff);
    g.FillRect(2,2,Width()-4,Height()-4);
    g.SetSysColor(2);
    g.DrawRect(0,0,Width(),Height());
    g.DrawRect(1,1,Width()-1,Height()-1);
    g.SetSysColor(3);
    g.DrawRect(0,0,Width()-1,Height()-1);
    int top=mScrollTop;
    for(CreditsList::iterator i=mLines.begin();i!=mLines.end();++i) {
        CreditLineInfo& line=*i;
        int height=g.mFont->GetHeight();
        if(top<Height()/3) g.SetColor(DimColor(line.GetColor(),top*100/(Height()/3)));
        else if(Height()-top<=height) g.SetColor(0xffffff);
        else if(top>(Height()*2/3)-height)
            g.SetColor(DimColor(line.GetColor(),(Height()-(top+height))*100/(Height()/3)));
        else g.SetColor(line.GetColor());
        if(g.mColor!=0xffffff && top>2 && top<Height()-height-2) {
            int width;
            {GUIString text=line.GetLine();width=g.mFont->GetStringWidth(text,0,text.length());}
            g.DrawString(line.GetLine(),(Width()-width)/2,top);
        }
        top+=height;
    }
    mTotalTextHeight=top-mScrollTop;
}
}

namespace WONAPI {
// FUNCTION: WON 0x007f6de0
ScrollingTextComponent::ScrollingTextComponent()
    :mLastColor(0),mTimeSinceLastScroll(0),mScrollSpeed(60),mScrollTop(0),mTotalTextHeight(0)
{
    SetComponentFlags(0x2000,true);
}
// FUNCTION: WON 0x007f7490
CreditsDialog::CreditsDialog()
{
    mScrollingText=new ScrollingTextComponent;
}
}

namespace WONAPI {
// SUPPORT-ONLY: EXE 0x007f6e90 (182B genuine SDK explicit destructor)
// Native3f6eb2 stores ownVT456018 before list34/Font30/Component cleanup.
// FUNCTION: WON 0x007f6e90
ScrollingTextComponent::~ScrollingTextComponent() {}
}

namespace WONAPI {
// FUNCTION: WON 0x007f6f50
void ScrollingTextComponent::Start()
{
    Fn2F57D0(true);
    Reset();
}
// FUNCTION: WON 0x007f6f70
void ScrollingTextComponent::Stop()
{
    Fn2F57D0(false);
}
// FUNCTION: WON 0x007f6f80
void ScrollingTextComponent::Reset()
{
    mScrollTop=Height();
}
// FUNCTION: WON 0x007f7750
void CreditsDialog::StartScrolling()
{
    mScrollingText->Start();
}
// FUNCTION: WON 0x007f7760
void CreditsDialog::StopScrolling()
{
    mScrollingText->Stop();
}
// FUNCTION: WON 0x007f7770
void CreditsDialog::ResetScrolling()
{
    mScrollingText->Reset();
}
}

namespace WONAPI {
// Genuine SDK CreditLineInfo owned GUIString accessor; complete original local class.
// SUPPORT: EXE 0x007f7450 (zero primary quota)
// FUNCTION: WON 0x007f7450
GUIString CreditLineInfo::GetLine()
{
    return mLine;
}
}

namespace WONAPI {
// Genuine SDK UpdateCredits moved its content to configuration.
// SUPPORT: EXE 0x008142a0 (shared one-byte body; zero primary/physical novelty claim)
void CreditsDialog::UpdateCredits()
{
}
// Genuine SDK base event delegation, actual Credits vtable slot7c.
// FUNCTION: WON 0x007fa1f0
void CreditsDialog::HandleComponentEvent(ComponentEvent* event)
{
    Dialog::HandleComponentEvent(event);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f6e70
// (public: virtual void * __thiscall WONAPI::ScrollingTextComponent::`scalar deleting destructor'(unsigned int)    [??_GScrollingTextComponent@WONAPI@@UA)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007fa7a0
// (void __cdecl std::_Construct(class WONAPI::CreditLineInfo *,class WONAPI::CreditLineInfo const &)    [?_Construct@std@@YAXPAVCreditLineInfo@WONAPI@@AB)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f7560
// (public: virtual void * __thiscall WONAPI::CreditsDialog::`scalar deleting destructor'(unsigned int)    [??_GCreditsDialog@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007f7580
// (public: virtual __thiscall WONAPI::CreditsDialog::~CreditsDialog(void)    [??1CreditsDialog@WONAPI@@UAE@XZ])
