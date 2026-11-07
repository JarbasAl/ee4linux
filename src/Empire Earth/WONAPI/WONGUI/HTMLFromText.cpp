// Text-to-HTML character stream, prefix matching, URL-aware text insertion and document generation.
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
#include "ee/wonapi/WONLobbyHTMLParser.h"
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
static int gNumProtocols = 5;
namespace WONAPI {
// Genuine SDK constructor; member owners use ordinary implicit construction.
// FUNCTION: WON 0x0073f510
HTMLFromText::HTMLFromText()
{
    mFile=NULL;
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
// FUNCTION: WON 0x0073fc30
// (??_GHTMLAnchorTag@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0073fc50
// (??1HTMLAnchorTag@WONAPI@@UAE@XZ)
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


// FUNCTION: WON 0x0073fe40
HTMLDocument* HTMLFromText::Generate(bool wantBody)
{
    mDocument=new HTMLDocument(gEmptyGUIString);
    if(wantBody) mDocument->AddTag(new HTMLBodyTag);
    GUIString current((unsigned short)32);
    while(!EndOfFile()) {
        int ch=GetChar();
        if(ch=='\n') {
            if(current.length()!=0) {
                AddStr(current);
                current.erase();
                current.reserve(32);
            }
            mDocument->AddTag(new HTMLLineBreakTag);
        } else if(ch!=-1) current.append(ch);

        else break;
    }
    if(current.length()!=0) AddStr(current);
    if(wantBody) {
        HTMLBodyTag* tag=new HTMLBodyTag;
        tag->SetFlags(HTMLTagFlag_IsEndTag,true);
        mDocument->AddTag(tag);
    }
    CloseFile();
    return mDocument.get();
}

// FUNCTION: WON 0x007400a0
HTMLDocument* HTMLFromText::GenerateFromFile(const GUIString& path,int flags)
{
    CloseFile();
    if(path.empty()) return NULL;
    std::string narrowPath=path;
    mFile=fopen(narrowPath.c_str(),"r");
    if(mFile==NULL) return NULL;
    bool isHTML=(flags&HTMLDocGenFlag_IsHTML)!=0;
    if(flags&HTMLDocGenFlag_IsOptionalHTML) {
        int first=fgetc(mFile);
        if(first=='<') {
            CloseFile();
            isHTML=true;
        } else if(!(flags&HTMLDocGenFlag_SkipFirstCharIfNotHTML)) ungetc(first,mFile);
    }
    if(isHTML) {
        HTMLParser parser;
        parser.ParseFromFile(path);
        mDocument=parser.GetDocument();
        // Preserve the original forced-HTML file-handle state; no extra fclose.
        return mDocument.get();
    }
    return Generate(false);
}

// FUNCTION: WON 0x00740340
// (??1HTMLParser@WONAPI@@QAE@XZ)

// FUNCTION: WON 0x00740440
HTMLDocument* HTMLFromText::GenerateFromText(const GUIString& text,int flags)
{
    CloseFile();
    bool isHTML=(flags&HTMLDocGenFlag_IsHTML)!=0;
    if((flags&HTMLDocGenFlag_IsOptionalHTML) && text.length()!=0 && text[0]=='<')
        isHTML=true;
    if(isHTML) {
        HTMLParser parser;
        parser.ParseFromText(text);
        mDocument=parser.GetDocument();
        return mDocument.get();
    }
    mBackStr.resize(0);
    mBackStr.reserve(text.length());
    for(int i=text.length()-1;i>=0;--i) mBackStr.append(text[i]);
    if(flags&HTMLDocGenFlag_IsOptionalHTML) {
        if((flags&HTMLDocGenFlag_SkipFirstCharIfNotHTML) && mBackStr.length()>0)
            mBackStr.resize(mBackStr.length()-1);
    }
    return Generate(false);
}

// FUNCTION: WON 0x00740710
HTMLDocument* HTMLFromText::Generate(const GUIString& text,int flags)
{
    if(flags&HTMLDocGenFlag_IsFile) return GenerateFromFile(text,flags);
    return GenerateFromText(text,flags);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0074b130
// (public: __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::GUIString>,struct std::_Mapk<class)
// TEMPLATE: WON 0x0074b580
// (public: __thiscall std::pair<class WONAPI::GUIString const ,class WONAPI::GUIString>::~pair<class WONAPI::GUIString const ,class WONAPI::GUIString>(vo)
// TEMPLATE: WON 0x0074bdd0
// (public: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::GUIString>,struct std::_Mapk<class WONA)
// TEMPLATE: WON 0x0074dec0
// (public: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::GUIString>,struct std::_Mapk<class WONA)
// TEMPLATE: WON 0x0074e3e0
// (protected: void __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::GUIString>,struct std::_Ma)
// TEMPLATE: WON 0x00754fe0
// (public: void * __thiscall std::pair<class WONAPI::GUIString const ,class WONAPI::GUIString>::`scalar deleting destructor'(unsigned int))
