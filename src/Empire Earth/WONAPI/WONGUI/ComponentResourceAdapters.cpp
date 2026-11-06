// Complete actual Component resource/input adapters; all helper/getter/setter quota0.
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
namespace WONAPI {
// Original2f4af0/14, borrowed field accessor support0.
// FUNCTION: WON 0x006f4af0
BackgroundGrabber* Component::GetGrabBG()
{
    return mExtraStuff!=NULL ? mExtraStuff->mGrabBG : NULL;
}
// Original2f4b00/14, borrowed field accessor support0.
// FUNCTION: WON 0x006f4b00
Cursor* Component::Fn2F4B00()
{
    return mExtraStuff!=NULL ? mExtraStuff->mDefaultCursor.get() : NULL;
}
// Original2f4b10/89, existing external Extra allocation plus true owning setter support0.
// FUNCTION: WON 0x006f4b10
void Component::AddListener(ComponentListener* listener)
{
    AllocateExtraStuff();
    mExtraStuff->mListener=listener;
}
// Original2f4c60/89, existing external Extra allocation plus true owning setter support0.
// FUNCTION: WON 0x006f4c60
void Component::SetDefaultCursor(Cursor* cursor)
{
    AllocateExtraStuff();
    mExtraStuff->mDefaultCursor=cursor;
}
// Original2f5360/28, manager forwarding support0.
void Component::Fn2F5360(bool subscribe)
{
    GetRoot()->mManager->SubscribeToBroadcast(this,subscribe);
}
// Original2f5770/39, coordinate adapting helper support0.
// FUNCTION: WON 0x006f5770
void Component::GetMousePos(int& x,int& y)
{
    Window* window=GetRoot();
    x=window->mMouseX;
    y=window->mMouseY;
    RootToChild(x,y);
}
// Original2f57a0/9, borrowed window scalar accessor support0.
// FUNCTION: WON 0x006f57a0
int Component::GetKeyMod()
{
    return GetRoot()->mKeyMod;
}
// Original2f5960/14, borrowed own scheme getter support0.
// FUNCTION: WON 0x006f5960
ColorScheme* Component::GetOwnColorScheme()
{
    return mExtraStuff!=NULL ? mExtraStuff->mColorScheme.get() : NULL;
}
// Original2f5970/44, inherited scheme getter support0.
// FUNCTION: WON 0x006f5970
ColorScheme* Component::GetColorScheme()
{
    if(mExtraStuff!=NULL && mExtraStuff->mColorScheme.get()!=NULL)
        return mExtraStuff->mColorScheme.get();
    if(mParent!=NULL) return mParent->GetColorScheme();
    return GetRoot()->mManager->mColorScheme.get();
}
// Original2f59a0/89, existing external Extra allocation plus true owning setter support0.
// FUNCTION: WON 0x006f59a0
void Component::Fn2F59A0(ColorScheme* scheme)
{
    AllocateExtraStuff();
    mExtraStuff->mColorScheme=scheme;
}
// Original2f5b20/13, borrowed font forwarding support0.
// FUNCTION: WON 0x006f5b20
Font* Component::Fn2F5B20()
{
    return GetRoot()->mManager->GetDefaultFont();
}
// Original2f5b30/23, borrowed font forwarding support0.
Font* Component::GetNamedFont(const std::string& name)
{
    return GetRoot()->mManager->GetNamedFont(name);
}
// Original2f5b50/28, borrowed font forwarding support0.
// FUNCTION: WON 0x006f5b50
Font* Component::Fn2F5B50(int style,int size)
{
    return GetRoot()->mManager->GetDefaultFontMod(style,size);
}
}
