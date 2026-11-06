// Complete original-backed Component source completion; all selected methods support0.
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
// Genuine SDK implicit destructor supplied by actual complete class/real constructor; no authored body.
// FUNCTION: WON 0x00705010
void ScrollArea::SetScroller(Scroller* scroller)
{
    mScroller=scroller;
    if(scroller!=NULL) scroller->SetScrollArea(this);
}
// FUNCTION: WON 0x00705180
void ScrollArea::HandleComponentEvent(ComponentEvent* event)
{
    Component::HandleComponentEvent(event);
}
// FUNCTION: WON 0x00705c10; native95B, nested viewport translation and visible-rectangle invalidation.
void ScrollAreaViewport::InvalidateIfRectVisible(WONRectangle& rect)
{
    if(mParent==mScrollArea) {
        if(mScrollArea->IsRectVisible(rect)) mScrollArea->Invalidate();
    } else {
        Container* parent=mParent;
        if(parent!=NULL) {
            rect.x+=Left();
            rect.y+=Top();
            static_cast<ScrollAreaViewport*>(parent)->InvalidateIfRectVisible(rect);
        }
    }
}
// FUNCTION: WON 0x00705c70
void ScrollAreaViewport::Reset()
{
    mMouseChild=mFocusChild=NULL;
    mChildTimerSet.clear();
    mChildList.clear();
    WindowManager* manager=WONAPI::WindowManager::GetDefaultWindowManager();
    if(manager!=NULL) manager->CheckEndPopup(this);
}
// Genuine SDK implicit destructor supplied by actual complete class/real constructor; no authored body.
// FUNCTION: WON 0x00705db0
void ScrollAreaViewport::VirtualB4(Component* child) { Container::VirtualB4(child); }
// FUNCTION: WON 0x00705dc0
void ScrollAreaViewport::VirtualB8(Component* child) { Container::VirtualB8(child); }
// FUNCTION: WON 0x00705dd0
bool ScrollAreaViewport::Virtual44(int delta) { return Container::Virtual44(delta); }
// FUNCTION: WON 0x00705de0
void ScrollAreaViewport::Virtual78(Cursor* cursor) { mScrollArea->Virtual78(cursor); }
// FUNCTION: WON 0x00705e00
void ScrollAreaViewport::Paint(Graphics& graphics) { Container::Paint(graphics); }
}

namespace WONAPI {
// FUNCTION: WON 0x00705d70
bool ScrollAreaViewport::VirtualC0(Component* child)
{
    WONRectangle bounds=child->GetBounds();
    InvalidateIfRectVisible(bounds);
    return false;
}
}

namespace WONAPI {
// Genuine SDK ChildChanged empty default, sharednative2fa500/3; primary/physicalnew0.
void ScrollAreaViewport::VirtualBC(Component*) {}
}
