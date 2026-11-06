// PROVISIONAL original WONGUI prerequisite family; original filename/object boundaries unproved.
// SOURCE ONLY: near/generated support remain original-carved; no runtime substitution authority.
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
#include "ee/wonapi/WONLobbyLayout.h"
namespace WONAPI {

// FUNCTION: WON 0x00737100
void ChildLayoutInstruction::Execute(Component*)
{
    Component* child = mChild;
    Component* reference = mReference;
    int left, top;
    if(reference == child->mParent)
    {
        left = 0;
        top = 0;
    }
    else
    {
        left = reference->Left();
        top = reference->Top();
    }
    int referenceWidth = reference->Width();
    int referenceHeight = reference->Height();
    int right = left + referenceWidth;
    int bottom = top + referenceHeight;
    int x = child->Left();
    int y = child->Top();
    int width = child->Width();
    int height = child->Height();
    for(int flag = 1; flag < 0x100000; flag <<= 1)
    {
        if((mFlags & flag) == 0)
            continue;
        switch(flag)
        {
        case 0x00001: width = mWidth + referenceWidth; break;
        case 0x00002: height = mHeight + referenceHeight; break;
        case 0x00100: y = mY - height + top; break;
        case 0x00200: y = mY + bottom; break;
        case 0x00400: x = mX + right; break;
        case 0x00800: x = mX - width + left; break;
        case 0x01000: x = mX + left; break;
        case 0x02000: x = mX - width + right; break;
        case 0x04000: y = mY + top; break;
        case 0x08000: y = mY - height + bottom; break;
        case 0x10000: width = mWidth - x + right; break;
        case 0x20000: width = mWidth - x + left; break;
        case 0x40000: height = mHeight - y + top; break;
        case 0x80000: height = mHeight - y + bottom; break;
        }
    }
    child->Fn2F4BF0(x, y, width, height);
}
// FUNCTION: WON 0x006f61b0
bool ChildLayoutInstruction::UsesComponent(Component* component)
{
    return mChild == component || mReference == component;
}


// FUNCTION: WON 0x00736d20
void ChildLayoutManager::ExecuteInstructions(Component* container)
{
    for(InstructionList::iterator itr = mInstructions.begin(); itr != mInstructions.end(); ++itr)
        if((*itr)->mEnabled)
            (*itr)->Execute(container);
}
// FUNCTION: WON 0x00736d60
void ChildLayoutManager::AddInstruction(ChildLayoutInstructionBase* instruction)
{
    mInstructions.push_back(instruction);
}
// FUNCTION: WON 0x00736e90
void ChildLayoutManager::AddLayoutPoint(LayoutPoint* point)
{
    mLayoutPoints.push_back(point);
}
// FUNCTION: WON 0x00736fc0
void ChildLayoutManager::RemoveComponent(Component* component)
{
    InstructionList::iterator itr = mInstructions.begin();
    while(itr != mInstructions.end())
    {
        if((*itr)->UsesComponent(component))
            mInstructions.erase(itr++);
        else
            ++itr;
    }
}
// FUNCTION: WON 0x00737030
void ChildLayoutManager::Clear()
{
    mInstructions.clear();
    mLayoutPoints.clear();
}
// FUNCTION: WON 0x007370c0
void ChildLayoutManager::EnableInstructions(int group, bool enabled)
{
    for(InstructionList::iterator itr = mInstructions.begin(); itr != mInstructions.end(); ++itr)
        if((*itr)->mGroupId == group)
            (*itr)->mEnabled = enabled;
}

}
