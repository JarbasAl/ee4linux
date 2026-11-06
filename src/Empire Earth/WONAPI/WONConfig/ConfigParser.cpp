// C:\EEX\Empire Earth\WONAPI\WONConfig\ConfigParser.cpp (RECONSTRUCTION).
// Reference/token/file subset of original ConfigParser family; exact filename/TU boundary provisional.
// Full target storage0x88=136B; native ctor/dtor now proved and implemented below.
// Only independently proved nested32 define-map construction is admitted for the additional define behaviors.

// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <stdio.h>
#include <ctype.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyConfig.h"
namespace WONAPI {

int ConfigParser::mLineCount = 0;
// FUNCTION: WON 0x007b1f90
// Original3b1f90/32, inherits only flags when parent is nonnull.
void ConfigParser::SetParent(ConfigParser* inParent)
{
    mParent = inParent;
    if (inParent != NULL) {
        mIgnorePathOnIncludeFile = inParent->mIgnorePathOnIncludeFile;
        mWarningsAsErrors = inParent->mWarningsAsErrors;
    }
}
// FUNCTION: WON 0x007b32b0
bool ConfigParser::EndOfFile()
{
    if(!mFileReader.EndOfFile()) return false;
    return mBackStr.empty();
}
// FUNCTION: WON 0x007b32d0
// Original3b32d0/469. CR recursively normalizes through GetChar, including its original CR-CR behavior.
int ConfigParser::GetChar()
{
    int ch;
    if (mBackStr.empty()) {
        if (mIsUnicode) {
            int low = mFileReader.TryReadByte();
            int high = mFileReader.TryReadByte();
            ch = low | (high << 8);
        } else {
            ch = mFileReader.TryReadByte();
        }
    } else {
        ch = mBackStr[mBackStr.length() - 1];
        mBackStr.erase(mBackStr.length() - 1);
    }
    if (ch == '\r') {
        int next = GetChar();
        if (next != '\n') {
            UngetChar(next);
            ++mLineCount;
            ++mLineNum;
        }
        return '\n';
    }
    if (ch == '\n') {
        ++mLineCount;
        ++mLineNum;
    }
    return ch;
}
// FUNCTION: WON 0x007b34b0
// Original3b34b0/56. Closed-reader gate remains even if back-string was populated manually.
int ConfigParser::PeekChar()
{
    if ((mFileReader.mFile != NULL || mFileReader.mBuffer != NULL) && !EndOfFile()) {
        int ch = GetChar();
        UngetChar(ch);
        return ch;
    }
    return -1;
}
// FUNCTION: WON 0x007b34f0
// Original3b34f0/230. EOF is not pushed; line counters are decremented before append can throw.
void ConfigParser::UngetChar(int inChar)
{
    if (inChar == -1) return;
    if (inChar == '\n') {
        --mLineNum;
        --mLineCount;
    }
    mBackStr += (wchar_t)inChar;
}
// FUNCTION: WON 0x007b35e0
// Original3b35e0/51; plain signed-char promotion preserved before wide back-string insertion.
void ConfigParser::UngetStr(const std::string& inString)
{
    for (int i = (int)inString.length() - 1; i >= 0; --i)
        UngetChar(inString[i]);
}
// FUNCTION: WON 0x007b3c50
// Original3b3c50/133; comments include #, // and /* */, with two-character pushback for noncomment slash.
void ConfigParser::SkipWhitespace()
{
    while (!EndOfFile()) {
        int ch = GetChar();
        if ((unsigned int)ch < 256 && isspace(ch))
            continue;
        if (ch == '#') {
            SkipLine();
            continue;
        }
        if (ch == '/') {
            int next = GetChar();
            if (next == '/') {
                SkipLine();
                continue;
            }
            if (next == '*') {
                SkipBlockComment();
                continue;
            }
            UngetChar(next);
        }
        UngetChar(ch);
        return;
    }
}
// FUNCTION: WON 0x007b3ce0
// Original3b3ce0/37; consumes terminating newline rather than pushing it back.
void ConfigParser::SkipLine()
{
    while (!EndOfFile())
        if (GetChar() == '\n') return;
}
// FUNCTION: WON 0x007b3d10
// Original3b3d10/51; only immediately preceding star closes the comment.
void ConfigParser::SkipBlockComment()
{
    bool star = false;
    while (!EndOfFile()) {
        int ch = GetChar();
        if (star && ch == '/') return;
        star = ch == '*';
    }
}
// FUNCTION: WON 0x007b1fb0
// Original3b1fb0/307; append policy and temporary lifetime are deliberate.
void ConfigParser::SetError(const std::string& inError, bool inAppend)
{
    if (!mAbortReason.empty() && !inAppend) return;
    char location[1024];
    sprintf(location, " (%s:%d)", mFileNameAndPath.c_str(), mLineNum);
    mAbortReason += inError + location;
}
// FUNCTION: WON 0x007b20f0
// Original3b20f0/541; warnings-as-errors still appends a warning after setting the error.
void ConfigParser::AddWarning(const std::string& inWarning)
{
    if (!mAbortReason.empty()) return;
    if (mWarningsAsErrors) SetError(inWarning, false);
    char warning[1024];
    sprintf(warning, "Warning (%s: %d) - %s\n", mFileNameAndPath.c_str(), mLineNum, inWarning.c_str());
    mWarnings += warning;
}
// FUNCTION: WON 0x007b2310
// Original3b2310/265; append without clearing, including self-copy behavior from basic_string.
void ConfigParser::CopyWarnings(ConfigParser& inSource)
{
    mWarnings += inSource.mWarnings;
}
// FUNCTION: WON 0x007b2420
// Original3b2420/232; lazy error construction makes this more than a passive accessor.
const std::string& ConfigParser::GetAbortReason()
{
    if (mAbortReason.empty()) SetError("Parse aborted.", false);
    return mAbortReason;
}
// FUNCTION: WON 0x007b3840
// Original3b3840/598; reference record layout actual0pos/4path/14line/18back, size28hex.
void ConfigParser::GetState(ConfigParserState& outState)
{
    outState.mBackStr = mBackStr;
    outState.mFileNameAndPath = mFileNameAndPath;
    outState.mFilePos = mFileReader.pos();
    outState.mLineNum = mLineNum;
}
// FUNCTION: WON 0x007b3aa0
// Original3b3aa0/427; failed reopen is not rolled back; line assignment precedes back-string copy.
bool ConfigParser::SetState(const ConfigParserState& inState)
{
    if (!mFileReader.IsOpen() ||
        inState.mFileNameAndPath != mFileNameAndPath) {
        if (!OpenFile(inState.mFileNameAndPath)) return false;
    }
    mFileReader.SetPos(inState.mFilePos);
    mLineNum = inState.mLineNum;
    mBackStr = inState.mBackStr;
    return true;
}
// Original3b3830/8, pure forwarding support0.
// FUNCTION: WON 0x007b3830
void ConfigParser::CloseFile()
{
    mFileReader.Close();
}
// FUNCTION: WON 0x007b3620

bool ConfigParser::OpenFile(const std::string& inPath)
{
    CloseFile();
    mLineNum = 1;
    mIsUnicode = false;
    mBackStr.erase();
    mFileNameAndPath = inPath;
    std::string::size_type sep = mFileNameAndPath.rfind('/');
    if (sep == std::string::npos) sep = mFileNameAndPath.rfind('\\');
    if (sep == std::string::npos) mFilePath = "";
    else mFilePath = mFileNameAndPath.substr(0, sep + 1);
    mFileReader.OpenText(inPath.c_str());
    if (mFileReader.mFile != NULL || mFileReader.mBuffer != NULL) {
        int first = mFileReader.TryReadByte();
        int second = mFileReader.TryReadByte();
        mFileReader.TryReadByte();
        int fourth = mFileReader.TryReadByte();
        if (first == 0xff && second == 0xfe && fourth == 0) {
            mFileReader.Open(inPath.c_str());
            mFileReader.SkipBytes(2);
            mIsUnicode = true;
        } else {
            mFileReader.Rewind();
        }
    }
    mFileReader.ReadIntoMemory();
    return mFileReader.mFile != NULL || mFileReader.mBuffer != NULL;
}
// Additional original ConfigParser define family; complete nested32 type independently reviewed.
// Target differs from archived SDK: narrow map values are std::string, not DefineStruct.

// FUNCTION: WON 0x007b2510
// Original3b2510/158, actual null SmartPtr temporary and vector insertion.
void ConfigParser::PushDefineStack()
{
    ConfigParser* root=this;
    while(root->mParent!=NULL) root=root->mParent;
    root->mDefineStack.push_back(NULL);
}
// FUNCTION: WON 0x007b25b0
// Original3b25b0/93: guard empty stack, erase last owned pointer.
void ConfigParser::PopDefineStack()
{
    ConfigParser* root=this;
    while(root->mParent!=NULL) root=root->mParent;
    if(!root->mDefineStack.empty()) root->mDefineStack.pop_back();
}
// FUNCTION: WON 0x007b2610
// Original3b2610/947; literals retain original misspelling.
bool ConfigParser::AddDefine(const std::string& name,const std::string& value,bool redefine)
{
    ConfigParser* root=this;
    while(root->mParent!=NULL) root=root->mParent;
    if(name.find(' ')!=std::string::npos) {
        root->AddWarning("Not allowed to have spaces in define names.");
        return true;
    }
    if(root->mDefineStack.empty()) root->mDefineStack.push_back(new DefineMapContainer);
    if(root->mDefineStack.back().get()==NULL) root->mDefineStack.back()=new DefineMapContainer;
    DefineMapContainer::DefineMap& defines=root->mDefineStack.back()->mDefineMap;
    if(redefine) defines[name]=value;
    else if(!defines.insert(std::make_pair(name,value)).second)
        root->AddWarning("Redefintion of "+name);
    return true;
}
// FUNCTION: WON 0x007b2b00
// Original3b2b00/218. Newest-to-oldest stack search skips null owned pointers.
bool ConfigParser::GetDefine(const std::string& name,std::string& outValue)
{
    ConfigParser* root=this;
    while(root->mParent!=NULL) root=root->mParent;
    for(DefineStack::reverse_iterator i=root->mDefineStack.rbegin();i!=root->mDefineStack.rend();++i) {
        if(i->get()==NULL) continue;
        DefineMapContainer::DefineMap& defines=(*i)->mDefineMap;
        DefineMapContainer::DefineMap::iterator found=defines.find(name);
        if(found!=defines.end()) {outValue=found->second;return true;}
    }
    return false;
}
// FUNCTION: WON 0x007b2be0
// Original3b2be0/320, genuine method (not constructor/helper): narrow temporary then conversion.
bool ConfigParser::GetDefine(const std::string& name,std::wstring& outValue)
{
    std::string value;
    if(!GetDefine(name,value)) return false;
    outValue=StringToWString(value);
    return true;
}
// FUNCTION: WON 0x007b2d20
// Original3b2d20/935. Local state exists even when caller supplies one or parent handles it.
bool ConfigParser::AddGroupDefine(const std::string& name,ConfigParserState* state)
{
    if(name.find(' ')!=std::string::npos) {
        AddWarning("Not allowed to have spaces in define names.");
        return true;
    }
    ConfigParserState localState;
    if(state==NULL) {GetState(localState);state=&localState;}
    if(mParent!=NULL) return mParent->AddGroupDefine(name,state);
    if(mDefineStack.empty()) mDefineStack.push_back(new DefineMapContainer);
    if(mDefineStack.back().get()==NULL) mDefineStack.back()=new DefineMapContainer;
    if(!mDefineStack.back()->mGroupDefineMap.insert(std::make_pair(name,*state)).second)
        AddWarning("Redefintion of "+name);
    return true;
}
// FUNCTION: WON 0x007b31a0
// Original3b31a0/259, explicit state copy order matches original fields0/4/14/18.
bool ConfigParser::GetGroupDefine(const std::string& name,ConfigParserState& outState)
{
    ConfigParser* root=this;
    while(root->mParent!=NULL) root=root->mParent;
    for(DefineStack::reverse_iterator i=root->mDefineStack.rbegin();i!=root->mDefineStack.rend();++i) {
        if(i->get()==NULL) continue;
        DefineMapContainer::GroupDefineMap& groups=(*i)->mGroupDefineMap;
        DefineMapContainer::GroupDefineMap::iterator found=groups.find(name);
        if(found!=groups.end()) {outState=found->second;return true;}
    }
    return false;
}

// Native full ConfigParser136 lifetime, independently proven. Existing Unicode5d remains intentionally uninitialized.
// FUNCTION: WON 0x007b1d20
ConfigParser::ConfigParser()
{
    mLineNum=0;
    mParent=NULL;
    mStopped=false;
    mWarningsAsErrors=false;
    mIgnorePathOnIncludeFile=false;
}
// FUNCTION: WON 0x007b1e10
ConfigParser::~ConfigParser() {CloseFile();}

}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: compatible same-snapshot reset/pushback APIs, not later DefineStruct storage.
void ConfigParser::Reset(void)
{
//	mDefineMap.clear();
	mDefineStack.clear();
	mAbortReason.erase();
	mWarnings.erase();
	mBackStr.erase();

	mLineNum = 1;
	mStopped = false;
//	if(mFile)
//		rewind(mFile);
	mFileReader.Rewind();
}

void ConfigParser::UngetStr(const std::wstring &theStr)
{
	for(int i=theStr.length()-1; i>=0; i--)
		UngetChar(theStr[i]);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007b1df0
// (public: virtual void * __thiscall WONAPI::ConfigParser::`scalar deleting destructor'(unsigned int)    [??_GConfigParser@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007b30d0
// (public: __thiscall std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct WONAPI::ConfigParserState>)
// TEMPLATE: WON 0x007b3d90
// (public: __thiscall std::map<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<char,struc)
// TEMPLATE: WON 0x007b3de0
// (public: __thiscall std::map<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct WONAPI::ConfigParserState,s)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007b29d0
// (public: virtual void * __thiscall WONAPI::ConfigParser::DefineMapContainer::`scalar deleting destructor'(unsigned int)    [??_GDefineMapContainer@Conf)
// TEMPLATE: WON 0x007b29f0
// (public: virtual __thiscall WONAPI::ConfigParser::DefineMapContainer::~DefineMapContainer(void)    [??1DefineMapContainer@ConfigParser@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007b3e30
// (public: struct std::pair<class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<cla)
