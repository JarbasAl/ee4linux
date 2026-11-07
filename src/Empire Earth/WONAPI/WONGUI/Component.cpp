// PROVISIONAL Component event-propagation and geometry family; original filename unproved.
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
namespace WONAPI {
// FUNCTION: WON 0x006f52d0
void Component::Fn2F52D0(ComponentEvent* event)
{
    SmartPtr<ComponentEvent> eventRef = event;
    event->mBroadcast = true;
    Virtual80(event);
}

// FUNCTION: WON 0x006f53f0
void Component::Fn2F53F0(ComponentEvent* event, bool sendNow)
{
    Window* window = Fn2F5940();
    if(window == NULL || window->mWindowState != 0)
    {
        SmartPtr<ComponentEvent> eventRef = event;
        return;
    }
    if(event->mComponent == NULL)
        event->SetComponent(this);
    if(sendNow)
    {
        SmartPtr<ComponentEvent> eventRef = event;
        if(event->mBroadcast)
        {
            WindowManager* aManager = window->mManager;
            aManager->Fn32F680(event);
        }
        else
            Fn2F54E0(event);
    }
    else
        window->Virtual48(event);
}

// FUNCTION: WON 0x006f4b70
void Component::Fn2F4B70(int x, int y)
{
    mBounds.x = x;
    mBounds.y = y;
    Virtual04();
}

// FUNCTION: WON 0x006f4b90
void Component::Fn2F4B90(int width, int height)
{
    if(width < 0) width = 0;
    if(height < 0) height = 0;
    if(width != Width() || height != Height())
    {
        mBounds.height = height;
        mBounds.width = width;
        Virtual08();
    }
}

// FUNCTION: WON 0x006f4bc0
void Component::Fn2F4BC0(int width, int height)
{
    if(width < Width()) width = Width();
    if(height < Height()) height = Height();
    if(width > Width() || height > Height())
        Fn2F4B90(width,height);
}


// FUNCTION: WON 0x006f4bf0
void Component::Fn2F4BF0(int x, int y, int width, int height)
{
    Fn2F4B70(x,y);
    Fn2F4B90(width,height);
}

// FUNCTION: WON 0x006f4e90
void Component::Fn2F4E90(int extraWidth,int extraHeight)
{
    int width = 0, height = 0;
    Virtual0C(width,height);
    Fn2F4B90(width+extraWidth,height+extraHeight);
}

// FUNCTION: WON 0x006f4ee0
void Component::SetDesiredWidth(int padding)
{
    int width = 0, height = 0;
    Virtual0C(width,height);
    Fn2F4B90(width+padding,Height());
}

// FUNCTION: WON 0x006f56d0
void Component::Fn2F56D0(int& x,int& y)
{
    x += Left();
    y += Top();
    for(Component* parent=mParent; parent!=NULL; parent=parent->mParent)
    {
        x += parent->Left();
        y += parent->Top();
    }
}

// FUNCTION: WON 0x006f50e0
void Component::Virtual60(WONRectangle& rect)
{
    if(mParent != NULL)
    {
        rect.x += Left();
        rect.y += Top();
        mParent->Virtual60(rect);
    }
}

// FUNCTION: WON 0x006f5660
void Component::Virtual8C(const ControlIdSet& ids,bool enabled)
{
    if(mControlId!=0)
    {
        int id=mControlId;
        if(ids.find(id)!=ids.end()) SetEnabled(enabled);
    }
}

// FUNCTION: WON 0x006f5080
bool Component::Contains(int x,int y)
{
    return x>=Left() && x<Left()+Width() && y>=Top() && y<Top()+Height();
}

// FUNCTION: WON 0x006f55b0
void Component::SetVisible(bool visible)
{
    if(visible != IsVisible()) {
        SetComponentFlags(1,!visible);
        mInvalidState=0;
        if(mParent!=NULL) mParent->VirtualBC(this);
        Invalidate();
        Virtual84(visible);
    }
}

// FUNCTION: WON 0x006f5610
void Component::SetEnabled(bool enabled)
{
    if(enabled != !IsDisabled()) {
        SetComponentFlags(0x80,!enabled);
        Virtual74(enabled);
        if(mParent!=NULL) mParent->VirtualBC(this);
        Invalidate();
    }
}
}

// Genuine SDK Component.cpp constructor: real embedded Rectangle16 initializes at+10 before body.
// FUNCTION: WON 0x006f49e0
namespace WONAPI {
Component::Component()
{
    mParent=NULL;
    mComponentBits=0;
    mInvalidState=0;
    mControlId=0;
    mTimerFlags=0;
    mFocusId=0;
    mExtraStuff=NULL;
}
}

namespace WONAPI {
// FUNCTION: WON 0x006f50c0
bool Component::Intersects(Component* component)
{
    return mBounds.Intersects(component->mBounds);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f4a50
// (public: virtual void * __thiscall WONAPI::Component::`scalar deleting destructor'(unsigned int)    [??_GComponent@WONAPI@@UAEPAXI@Z])
