// Complete original SDK stateful child-layout helper; SOURCE_ONLY_NO_NATIVE_ID.
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
#include "ee/wonapi/WONLobbyLayoutHelper.h"
namespace WONAPI {
LayoutHelper::LayoutHelper(Container *theParent)
{
	mContainer = theParent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LayoutHelper::Add()
{
	ChildLayoutInstruction *anInstr = new ChildLayoutInstruction;
	anInstr->mFlags				= mType;	
	anInstr->mChild			= mTarget;
	anInstr->mReference			= mReference;
	anInstr->mX		= mHorzPosPad;
	anInstr->mY		= mVertPosPad;
	anInstr->mWidth		= mHorzSizePad;
	anInstr->mHeight		= mVertSizePad;
	mContainer->AddChildLayout(anInstr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LayoutHelper::Add(Component *target, int theType, Component *ref, int horzPosPad, int vertPosPad, int horzSizePad, int vertSizePad)
{
	mType = theType;
	mTarget = target;
	mReference = ref;
	mHorzPosPad = horzPosPad;
	mVertPosPad = vertPosPad;
	mHorzSizePad = horzSizePad;
	mVertSizePad = vertSizePad;
	Add();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LayoutHelper::Repeat(Component *target)
{
	mTarget = target;
	Add();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LayoutHelper::Repeat(Component *target, Component *ref)
{
	mTarget = target;
	mReference = ref;
	Add();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LayoutHelper::Repeat(Component *target, int horzPosPad, int vertPosPad, int horzSizePad, int vertSizePad)
{
	mTarget = target;
	mHorzPosPad = horzPosPad;
	mVertPosPad = vertPosPad;
	mHorzSizePad = horzSizePad;
	mVertSizePad = vertSizePad;
	Add();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LayoutHelper::FlowRepeat(Component *target)
{
	mReference = mTarget;
	Repeat(target);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LayoutHelper::FlowRepeat(Component *target, int horzPosPad, int vertPosPad, int horzSizePad, int vertSizePad)
{	
	mReference = mTarget;
	Repeat(target,horzPosPad,vertPosPad,horzSizePad,vertSizePad);
}

}
