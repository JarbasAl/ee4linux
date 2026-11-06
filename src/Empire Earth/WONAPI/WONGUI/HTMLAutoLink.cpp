// Original protocol-table references and complete plain-text hyperlink recognition.
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
#include "ee/wonapi/WONLobbyHTMLTag.h"
#include "ee/wonapi/WONLobbyHTMLFromText.h"
// Genuine original file-static arrays: declaration-only references, no replacement
// definition, initializer, destructor registration or source-owned storage.
extern const WONAPI::GUIString gProtocols[5]; //5662cc actual five4B GUIString objects
extern const WONAPI::GUIString gProtocolPrepends[5]; //5662a4 actual five4B objects
static int gNumProtocols = 5;
namespace WONAPI {
// FUNCTION: WON 0x0073F8B0
bool HTMLFromText::CheckURL(GUIString &aStr, int &aPos)
{
	for(int i=0; i<gNumProtocols; i++)
	{
		if(StartsWith(aStr,aPos,gProtocols[i],0) &&
		   (aPos==0 || aStr[aPos-1]== ' '))
		{
			int anEndPos = aPos;
			int anItr = aPos;
			while(anItr<aStr.length())
			{
				int aChar = aStr[anItr];
				if(aChar>='0' && aChar<='9' || aChar>='a' && aChar<='z' || aChar>='A' && aChar<='Z')
					anEndPos = anItr+1;
				else if(aChar==' ')
					break;
				
				anItr++;
			}
			
			if(aPos>0)
				mDocument->AddTag(new HTMLTextTag(aStr.substr(0,aPos)));

			HTMLAnchorTag *anAnchor = new HTMLAnchorTag;
			GUIString aLink = aStr.substr(aPos,anEndPos-aPos);
			GUIString anHRef = gProtocolPrepends[i];
			anHRef.append(aLink);

			anAnchor->mHRef = anHRef;
			mDocument->AddTag(anAnchor);
			mDocument->AddTag(new HTMLTextTag(aLink));

			anAnchor = new HTMLAnchorTag;
			anAnchor->SetFlags(HTMLTagFlag_IsEndTag,true);
			mDocument->AddTag(anAnchor);

			aStr = aStr.substr(anEndPos);
			aPos = 0;
			return true;
		}
	}

	return false;
}
}

// FUNCTION: WON 0x0073fc30
// (??_GHTMLAnchorTag@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0073fc50
// (??1HTMLAnchorTag@WONAPI@@UAE@XZ)
