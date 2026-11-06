// Register child layout state and execute the immediate group before retaining it.
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
// FUNCTION: WON 0x006f61d0
void Container::AddChildLayout(ChildLayoutInstructionBase* theInstr, int theId)
{
    theInstr->mGroupId = theId;
    if(theId == 0) {
        theInstr->mEnabled = true;
        theInstr->Execute(this);
    }
    else
        theInstr->mEnabled = false;
    mChildLayout.AddInstruction(theInstr);
}
}

#include "ee/wonapi/WONLobbyLayout.h"
namespace WONAPI {
// Genuine SDK Container::AddChildLayout component overload, existing reduced API spelling.
// FUNCTION: WON 0x006f6140
void Container::Fn2F6140(Component* child,int flags,Component* reference,
    int x,int y,int width,int height,int groupId)
{
    ChildLayoutInstruction* instruction=new ChildLayoutInstruction;
    instruction->mFlags=flags;
    instruction->mChild=child;
    instruction->mReference=reference;
    instruction->mX=x;
    instruction->mY=y;
    instruction->mWidth=width;
    instruction->mHeight=height;
    AddChildLayout(instruction,groupId);
}
}
