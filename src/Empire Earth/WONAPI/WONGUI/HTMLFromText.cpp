// Text-to-HTML character stream, prefix matching and URL-aware text insertion.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <cstdio>
#include <wctype.h>
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
// FUNCTION: WON 0x0073f6e0
// (_$E74)
// FUNCTION: WON 0x0073f6f0
// (_$E71)
// FUNCTION: WON 0x0073f740
// (_$E73)
// FUNCTION: WON 0x0073f750
// (_$E72)
extern const WONAPI::GUIString gProtocols[5];
extern const WONAPI::GUIString gProtocolPrepends[5];
const WONAPI::GUIString gProtocols[] = { "HTTP://", "HTTPS://", "FTP://", "MAILTO:", "WWW." };
// FUNCTION: WON 0x0073f770
// (_$E79)
// FUNCTION: WON 0x0073f780
// (_$E76)
// FUNCTION: WON 0x0073f7d0
// (_$E78)
// FUNCTION: WON 0x0073f7e0
// (_$E77)
const WONAPI::GUIString gProtocolPrepends[] = { "", "", "", "", "HTTP://" };
namespace WONAPI {

// FUNCTION: WON 0x0073F610
int HTMLFromText::GetChar()
{
	int aChar = 0;
	if(mBackStr.length()==0)
	{
		if(mFile)
			aChar = fgetc(mFile);
		else 
			aChar = -1;
	}
	else
	{
		aChar = mBackStr[mBackStr.length()-1];
		mBackStr.resize(mBackStr.length()-1);
	}

	return aChar;
}

// FUNCTION: WON 0x0073F800
bool HTMLFromText::StartsWith(const GUIString &s1, int p1, const GUIString &s2, int p2)
{
	int c1, c2;
	do
	{
		c1 = towupper(s1[p1++]);
		c2 = towupper(s2[p2++]);
		if(c2=='\0')
			return true;
		else if(c1!=c2)
			return false;

	} while(c1!='\0');

	return false;
}

// FUNCTION: WON 0x0073fce0
// (??_GHTMLTextTag@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0073fd00
// (??1HTMLTextTag@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x0073FD60
void HTMLFromText::AddStr(GUIString &theStr)
{
	int aPos = 0;

	while(aPos<theStr.length())
	{
		if(CheckURL(theStr,aPos))
			continue;
		
//		if(CheckEmoticon(aStr,aPos))
//			continue;

//		if(CheckFontInstruction(aStr,aPos,aCurFont))
//			continue;

		// check change color instruction
//		if(CheckChangeColor(aStr,aPos))
//			continue;

		aPos++;
	}

	if(theStr.length()!=0)
		mDocument->AddTag(new HTMLTextTag(theStr));
}


// FUNCTION: WON 0x0073f580
void HTMLFromText::CloseFile()
{
	mBackStr = GetEmptyGUIStringData();
	if(mFile!=NULL)
	{
		fclose(mFile);
		mFile = NULL;
	}
}
// FUNCTION: WON 0x0073f5f0
bool HTMLFromText::EndOfFile()
{	
	if(!mFile || feof(mFile))
		return mBackStr.length()==0;
	else
		return false;
}
}

namespace WONAPI {
// Genuine SDK constructor; member owners use ordinary implicit construction.
// FUNCTION: WON 0x0073f510
HTMLFromText::HTMLFromText()
{
    mFile=NULL;
}
}
