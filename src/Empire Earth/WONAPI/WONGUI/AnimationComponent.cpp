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
#include "ee/wonapi/WONLobbyAnimationComponent.h"

using namespace WONAPI;

// FUNCTION: WON 0x00706e10
AnimationComponent::AnimationComponent(Animation* animation,bool startNow)
    :mAnimation(animation)
{
    mDelayAcc=0;
    mStarted=false;
    SetAnimation(animation);
    if(animation!=NULL)
    {
        Fn2F4B90(animation->GetWidth(),animation->GetHeight());
        if(startNow)
            Start();
    }
}

// FUNCTION: WON 0x00706f40
void AnimationComponent::SetAnimation(Animation* animation)
{
    mAnimation=animation;
}

// FUNCTION: WON 0x00706fa0
void AnimationComponent::Start(bool restart)
{
    if(restart)
    {
        mAnimation->Rewind();
        mDelayAcc=0;
    }
    mStarted=true;
    Fn2F57D0(true);
}

// FUNCTION: WON 0x00706fd0
void AnimationComponent::Virtual6C()
{
    Component::Virtual6C();
    if(mStarted)
        Fn2F57D0(true);
}

// FUNCTION: WON 0x00706ff0
void AnimationComponent::Virtual08()
{
    Component::Virtual08();
}

// FUNCTION: WON 0x00707000
void AnimationComponent::Paint(Graphics& g)
{
    Component::Paint(g);
    mAnimation->Draw(g,0,0);
}

// FUNCTION: WON 0x00707020
bool AnimationComponent::Virtual44(int delta)
{
    mDelayAcc+=delta;
    if(mDelayAcc>mAnimation->GetFrameDelayTime())
    {
        if(!mAnimation->NextFrame())
            return false;
        mDelayAcc=0;
        Invalidate();
    }
    return true;
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00706ed0
// (public: virtual __thiscall WONAPI::AnimationComponent::~AnimationComponent(void)    [??1AnimationComponent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00706eb0
// (public: virtual void * __thiscall WONAPI::AnimationComponent::`scalar deleting destructor'(unsigned int)    [??_GAnimationComponent@WONAPI@@UAEPAXI@Z])
