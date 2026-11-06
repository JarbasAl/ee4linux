#ifndef EE_WONAPI_WONLOBBYHTMLFROMTEXT_H
#define EE_WONAPI_WONLOBBYHTMLFROMTEXT_H
#include <cstdio>
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
enum HTMLDocGenFlags {
    HTMLDocGenFlag_IsHTML=0x0001,HTMLDocGenFlag_IsFile=0x0002,
    HTMLDocGenFlag_IsOptionalHTML=0x0004,HTMLDocGenFlag_SkipFirstCharIfNotHTML=0x0008
};
// Complete0c stack value owner, no virtual slots: native ctor33f510/98,
// natural implicit dtor33f170/128 releases GUIString8 then DocumentSP0.
// Raw FILE*4 remains governed by original Generate/CloseFile protocol.
class HTMLFromText {
protected:
    SmartPtr<HTMLDocument> mDocument; //00
    FILE* mFile; //04
    GUIString mBackStr; //08
    HTMLDocument* Generate(bool); //33fe40 actualRET4
    HTMLDocument* GenerateFromFile(const GUIString&,int); //3400a0 actualRET8
    HTMLDocument* GenerateFromText(const GUIString&,int); //340440 actualRET8
    void CloseFile(); //33f580 support0 cleanup
    bool EndOfFile(); //33f5f0 support0 EOF query
    int GetChar(); //33f610
    bool StartsWith(const GUIString&,int,const GUIString&,int); //33f800
    bool CheckURL(GUIString&,int&); //33f8b0 actual original callback provider
    void AddStr(GUIString&); //33fd60
public:
    HTMLFromText(); //33f510 original external constructor
    HTMLDocument* Generate(const GUIString&,int=0); //340710 original raw return retained by this owner
    // Natural implicit destructor matches original; no invented explicit empty stub.
};
}
#endif
