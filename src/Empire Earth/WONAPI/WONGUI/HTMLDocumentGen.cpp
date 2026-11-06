// HTML parameter document generation and percent substitution.
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
#include "ee/wonapi/WONLobbyHTMLFromText.h"
namespace WONAPI {

// FUNCTION: WON 0x0073F020
bool HTMLDocumentGen::AddDocPrv(const GUIString &theText, int theFlags, bool addToVector)
{
	SmartPtr<HTMLDocument> aDocument;
	HTMLFromText aGen;
	aDocument = aGen.Generate(theText,theFlags);
	
	if(addToVector)
		mDocumentVector.push_back(aDocument);
	else
		mDocument->AddDocument(aDocument.get());

	return aDocument.get()!=NULL;
}

// FUNCTION: WON 0x0073f170
// (??1HTMLFromText@WONAPI@@QAE@XZ)

// FUNCTION: WON 0x0073F230
void HTMLDocumentGen::printf(const GUIString &theText)
{
	int aStartPos = 0;
	while(true)
	{
		int anEndPos = theText.find('%',aStartPos);
		if(anEndPos<0)
			break;

		if(theText[anEndPos+1]=='%')
		{
			anEndPos++;
			AddDoc(theText.substr(aStartPos,anEndPos-aStartPos),HTMLDocGenFlag_IsHTML);
			aStartPos = anEndPos+1;
			continue;
		}
			
		AddDoc(theText.substr(aStartPos,anEndPos-aStartPos),HTMLDocGenFlag_IsHTML);

		anEndPos++;
		aStartPos = anEndPos;
		while(anEndPos<theText.length() && theText[anEndPos]!='%')
			anEndPos++;

		GUIString aNumber = theText.substr(aStartPos,anEndPos-aStartPos);
		int aParamNum = aNumber.mData->ToInt(0)-1;
		if(aParamNum>=0 && aParamNum<mDocumentVector.size())
			mDocument->AddDocument(mDocumentVector[aParamNum].get());

		aStartPos = anEndPos+1;
		if(aStartPos>=theText.length())
			break;
	}

	if(aStartPos<theText.length())
		AddDoc(theText.substr(aStartPos),HTMLDocGenFlag_IsHTML);
}


// FUNCTION: WON 0x0073f1f0
bool HTMLDocumentGen::AddDoc(const GUIString &theText, int theFlags)
{
	return AddDocPrv(theText,theFlags,false);
}
// FUNCTION: WON 0x0073f210
bool HTMLDocumentGen::AddDocParam(const GUIString &theText, int theFlags)
{
	return AddDocPrv(theText,theFlags,true);
}
}

namespace WONAPI {
// Genuine SDK authored constructor/API, support only; no implicit lifetime body.
// FUNCTION: WON 0x0073efc0
HTMLDocumentGen::HTMLDocumentGen(HTMLDocument* document)
{
    SetDocument(document);
}
// FUNCTION: WON 0x0073f440
void HTMLDocumentGen::SetDocument(HTMLDocument* document)
{
    if(document==NULL) mDocument=new HTMLDocument(gEmptyGUIString);
    else mDocument=document;
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable API body, zero Native/primary/physical credit.
void HTMLDocumentGen::AddTag(HTMLTag *theTag)
{
	if(theTag!=NULL)
		mDocument->AddTag(theTag);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable API body, zero Native/primary/physical credit.
void HTMLDocumentGen::AddTagParam(HTMLTag *theTag)
{
	SmartPtr<HTMLDocument> aDoc = new HTMLDocument(gEmptyGUIString);
	if(theTag!=NULL)
		aDoc->AddTag(theTag);

	mDocumentVector.push_back(aDoc);
}
}
