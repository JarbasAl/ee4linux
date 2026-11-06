// Genuine SDK existing-owner anchor/link helpers and new anchor instruction.
// SOURCE_ONLY_NO_NATIVE_ID; no Native/primary/physical claim.
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
#include "ee/wonapi/WONLobbyTextAreaConfig.h"
namespace WONAPI {
void TextArea::AddAnchor(const GUIString &theName, int theLineNum)
{
	mAnchorMap[theName] = theLineNum;
}

bool TextArea::GotoAnchor(const GUIString &theName)
{
	if(GetNumLines()<=0)
		return false;

	AnchorMap::iterator anItr = mAnchorMap.find(theName);
	if(anItr==mAnchorMap.end())
		return false;

	int aLineNum = anItr->second;
	if(aLineNum>=mLineVector.size())
		aLineNum = mLineVector.size()-1;

	if(aLineNum<0)
		aLineNum = 0;

	TextLine *aLine = mLineVector[aLineNum];
	mVertOffset = aLine->my;
	VirtualCC(false);
	return true;
}

void TextArea::AddLink(const GUIString &theText, const GUIString &theLinkParam, bool lineBreak)
{
	SmartPtr<TextLink> aSeg = new TextLink;
	aSeg->mText = theText;
	aSeg->mLinkParam = theLinkParam;
	aSeg->mLineBreak = lineBreak;
	aSeg->mColor = mLinkColor;
	aSeg->mFont = mLinkFont;
	AddSegment(aSeg);
}

void TextAnchorInstruction::PerformInstructions(TextArea *theTextArea)
{
	theTextArea->AddAnchor(mAnchorName, theTextArea->GetNumLines()-1);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00712b40
// (public: bool __thiscall WONAPI::GUIString::operator<(class WONAPI::GUIString const &)const     [??MGUIString@WONAPI@@QBE_NABV01@@Z])
