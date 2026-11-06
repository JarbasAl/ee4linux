// Complete nonvirtual native HTMLParser2c owner with actual original constructor342ae0.
// Typed cleanup remains natural implicit member cleanup; no fabricated fclose/dtor provider.
// Construction via original constructor is permitted, no new ctor body or by-value copy authority.
#ifndef EE_WONAPI_WONLOBBYHTMLPARSER_H
#define EE_WONAPI_WONLOBBYHTMLPARSER_H
#include <cstdio>
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyHTMLTag.h"
namespace WONAPI {
class HTMLParser {
protected:
    FILE* mFile; //00, older target usesCRT FILE* rather than laterSDK FileReader20
    GUIString mBackStr; //04
    int mLineNum; //08
    HTMLTagAttributes mTagAttributes; //0c..17
    SmartPtr<HTMLDocument> mDocument; //18, original-owned external virtual lifetime
    HTMLTagList mTagList; //1c..27
    typedef HTMLTag* (*TagFactoryFunc)(HTMLTagType);
    struct TagLookupInfo {
        HTMLTagType mType;
        TagFactoryFunc mFactoryFunc;
        TagLookupInfo() : mType(HTMLTagType_None),mFactoryFunc(NULL) {}
        TagLookupInfo(TagFactoryFunc f,HTMLTagType t) : mType(t),mFactoryFunc(f) {}
    };
    typedef std::map<GUIString,TagLookupInfo,GUIStringLessNoCase> TagNameMap;
    static TagNameMap& GetTagNameMap(); //340740 actual externaltableinitializer
    TagNameMap& mTagNameMap; //28
    void CloseFile(); //342bb0
    bool EndOfFile() { if(mFile==NULL || feof(mFile)) return (mBackStr.length()==0); else return false; } //343ff0 = its out-of-line COMDAT
    int GetChar(); //3434d0
    int PeekChar(); //344010..3441f3 complete483B inclmis-scannedtail344039
    void UngetChar(int theChar) { if(theChar==EOF) return; if(theChar=='\n') mLineNum--; mBackStr.append(theChar); } //3430f0 = its out-of-line COMDAT
    void UngetString(const GUIString&);
    void UngetString(const GUIString&,int);
    bool SkipWhitespace(); //344200
    int ReadEntity(); //342c10
    bool ReadWord(GUIString&,const char* =NULL,bool=true,bool=false,bool=true); //343280
    void SkipTag(const GUIString&); //3435e0
    bool GetActualTag(); //3438a0
    bool GetNextTag(); //3443f0
    bool Parse(); //3445f0, complete80 HTMLDocument allocation via actual constructor
public:
    HTMLParser(); //342ae0 external native owner creation only
    void SetDocument(HTMLDocument* document) { mDocument=document; } //genuine SDK inline owner assignment
    HTMLDocument* GetDocument() { return mDocument.get(); } //genuine SDK borrowed getter
    bool ParseFromFile(const GUIString&); //3446e0
    bool ParseFromText(const GUIString&); //344850
};
}
#endif
