// Original plain-text/optional-HTML generation on the complete existing stream owner.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <cstdio>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyHTMLTag.h"
#include "ee/wonapi/WONLobbyHTMLFromText.h"
#include "ee/wonapi/WONLobbyHTMLParser.h"
namespace WONAPI {
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
