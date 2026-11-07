// WONAPI GUI/resource classes used by the game's lobby glue (C:\EEX\Empire Earth\WONAPI\WONLobby\GameCode\EmpireEarth\)
//. WONAPI proper (0x29deb0..) is not decompiled yet: only what the GameCode
// units' codegen proves is declared here; every class and member name is PROVISIONAL (by behaviour), layouts are
// undecoded unless a member is listed. Requires <string>, ee/EEWonNet.h (RefCount, SmartPtr).
#ifndef EE_WONAPI_WONLOBBYRESOURCE_H
#define EE_WONAPI_WONLOBBYRESOURCE_H
#include <set>
#include <map>
#include "ee/wonapi/WONConfigObject.h"
#include "ee/wonapi/WONLobbyBackground.h"

namespace WONAPI {
class Component;
class ComponentConfig;
class ResourceConfigTable;
void WONGUIConfig_Init(ResourceConfigTable*); //39a1a0 genuine SDK registration external API
void ImageConfig_Init(ResourceConfigTable*); //38adf0 genuine SDK registration external API
void FontConfig_Init(ResourceConfigTable*); //38a080 genuine SDK registration external API
class Font; // owned font results in resource readers; GUI definition follows this header
class Background;
class StringParser;
void StringToUpperCaseInPlace(std::string&); //2a69a0 external utility

class Sound;
class Image;

// the shared, reference-counted text of a GUIString (0x10 bytes: `new` in 0x328450; ctor 0x327840 from a wide string)
class GUIStringData : public RefCount {
public:
    explicit GUIStringData(unsigned short inCapacity); // 0x3274e0
    virtual ~GUIStringData();               // 0x327980
    GUIStringData(const wchar_t* inText);  // 0x327840 (WONAPI)
    GUIStringData(const char* inText);     // 0x3277e0 (WONAPI)
    GUIStringData(const char*,unsigned short); //genuine SDK bounded ctor; SOURCE_ONLY_NO_NATIVE_ID
    GUIStringData(const wchar_t*,unsigned short); //genuine SDK bounded ctor; SOURCE_ONLY_NO_NATIVE_ID
    GUIStringData(const std::string& inText);      // 0x3278a0
    GUIStringData(const std::wstring& inText);     // 0x327910
    GUIStringData(const GUIStringData& theCopy);   // 0x327600
    GUIStringData(const GUIStringData* inOther, unsigned short inStart, unsigned short inLength); // 0x3276f0
    void Init(); //3274b0 actual14B helper
    void AssignStr(const char*,int); //327a20 unsigned-byte widening
    void AssignWStr(const wchar_t*,int); //327ac0
    void assign(const GUIStringData*,unsigned short,unsigned short); //327c10
#ifdef EE_DINKUM_SXL233
    void resize(int inLength, bool inGrow) { Reserve(inLength,inGrow); mLength=inLength; mText[mLength]=0; } // 0x30a1d0
#else
    void resize(int inLength, bool inGrow); // 0x30a1d0; distinct from Reserve
#endif
    void Fill(int,unsigned short,unsigned short); //327b60 actual external data Fill
    int ToInt(unsigned short inStart) const; // 0x3281e0
    int find(int inChar, int inStart) const; // 0x328280; -1 if absent
    int find(const GUIStringData* inText, int inStart) const; // 0x3282c0
    int findNoCase(const GUIStringData* inText, int inStart) const; // 0x328340
    int rFind(int,int) const; //328410, actual reverse character search55B
    int rFind(const GUIStringData*,int) const; //genuine SDK Source-only reverse substring search
    int findOneOf(const GUIStringData*,int) const;
    void erase(int,int);
    void insert(int,const GUIStringData*);
    int setAt(int,int);
    void toUpper();                        // 0x3279d0
    void toLowerCase(); //genuine SDK Latin-byte case algorithm; SOURCE_ONLY_NO_NATIVE_ID
    int Compare(const GUIStringData* inOther) const;    // 0x327fa0
    int compare(const char* theStr, int theOffset) const; //genuine SDK narrowcase-sensitive API, native3280f0..32814c/92
    // 0x292419
    void append(int inChar) { resize(mLength + 1, true); mText[mLength - 1] = (wchar_t)inChar; } //genuine SDK explicit inline data API; native3022d0/157 context, support0
#ifdef EE_DINKUM_SXL233 // the WON library objects inline it (ChatCommandParser::AddHelpEntry 0x757800); the exe's lobby
    // glue calls an out-of-line copy of an older revision (0x292419: Reserve + mLength + terminator)
    void append(const GUIStringData* inText, unsigned short inStart, unsigned short inLength)
    { int theLength = mLength; resize(theLength + inLength, true); memcpy(mText + theLength, inText->mText + inStart, inLength * sizeof(wchar_t)); }
#else
    void append(const GUIStringData* inText, unsigned short inStart, unsigned short inLength);
#endif
    int Compare(const GUIStringData* inOther, bool inCaseSensitive) const;     // 0x328340 (WONAPI)
    int CompareNoCase(const GUIStringData* inOther) const;  // 0x328030
    int CompareNoCase(const char* inText, int inOffset) const; // 0x328150
    std::string GetString() const;          // 0x327d90 (WONAPI, /GX): narrowed
    std::wstring GetWString() const;        // 0x327f30

    wchar_t* mText;                         // +0x08
    unsigned short mLength;                 // +0x0c
    unsigned short mCapacity;               // +0x0e
    // 0x2911c9 (a COMDAT of EELobbyFileDialog.cpp): room for inLength characters and the terminator (doubled when
    // inGrow), the text kept
#ifdef EE_DINKUM_SXL233
    void Reserve(int inLength, bool inGrow)
    {
        inLength++;
        if (mCapacity < inLength) {
            wchar_t* theOld = mText;
            if (inGrow)
                inLength += inLength;
            mText = new wchar_t[inLength];
            if (theOld != NULL) {
                memcpy(mText, theOld, (mLength + 1) * sizeof(wchar_t));
                delete[] theOld;
            }
            mCapacity = inLength;
        }
    }
#else
    void Reserve(int inLength, bool inGrow);
#endif
};

// 0x328450 (WONAPI, /GX): the shared empty string (a function-local static, created on first use)
GUIStringData* GetEmptyGUIStringData();

// a lobby text: one reference to its shared data (4 bytes: the resource unit's 73 globals)
class GUIString {
public:
#ifdef EE_DINKUM_SXL233 // WON library view: inline
    GUIString() : mData(GetEmptyGUIStringData()) {}
#else
    GUIString();                            // 0x29171b (out of line: every `$E` ctor jumps to it)
#endif
    explicit GUIString(unsigned short capacity) : mData(new GUIStringData(capacity)) {} //genuineSDKinline, native3398f7..339929
    void erase() { mData=GetEmptyGUIStringData(); } //genuineSDKinline noCOW, actualemptySP replacement
    int ToInt(int start=0) const { return mData->ToInt(start); }
    void reserve(int length) { mData->Reserve(length,false); } //genuineSDKinline, deliberately noCOW; native33ff52/34053e
#ifdef EE_DINKUM_SXL233 // WON library view: built inline (ChatCommandParser::AddHelpEntry 0x757800, GetHelp 0x7579c0)
    GUIString(const char* inText) : mData(new GUIStringData(inText)) {}
    GUIString(const wchar_t* inText) : mData(new GUIStringData(inText)) {}
#else
    GUIString(const char* inText);         // 0x29115e (new GUIStringData(inText); no caller expands it)
#endif
    GUIString(const char* text,unsigned short length) :mData(new GUIStringData(text,length)) {} //genuine SDK bounded constructor;0Native
    GUIString(const wchar_t* text,unsigned short length) :mData(new GUIStringData(text,length)) {} //genuine SDK bounded constructor;0Native
#ifdef EE_DINKUM_SXL233
    GUIString(const std::string& inText) : mData(new GUIStringData(inText)) {}
    GUIString(const std::wstring& inText) : mData(new GUIStringData(inText)) {}
#else
    GUIString(const std::string& inText);  // 0x29848e
    GUIString(const std::wstring& inText); // 0x29258f
#endif
    // (EELobbyGame 0x292e9f: GetEmptyGUIStringData() into the SmartPtr ctor COMDAT 0x293cd4)
    GUIString(GUIStringData* inData) : mData(inData) {}
    GUIString& operator=(const GUIString& theCopy) { mData = theCopy.mData; return *this; }
    GUIString& operator=(GUIStringData* theData) { mData = theData; return *this; }
    int Compare(const GUIString& inOther, bool inCaseSensitive) const
    {
        return mData->Compare(inOther.mData.get(), inCaseSensitive);
    }
    int compare(const char* theStr, int theOffset=0) const { return mData->compare(theStr,theOffset); } //genuine SDK inline read-only forwarder
    int CompareNoCase(const GUIString& inOther) const { return mData->CompareNoCase(inOther.mData.get()); }
    int CompareNoCase(const char* inText, int inOffset = 0) const { return mData->CompareNoCase(inText, inOffset); }
    //: the lobby's saved-games directory 0x2988e9 expands append(const GUIString&) (the length is read
    // before the copy-on-write call) and calls CopyOnWrite 0x29121b / GUIStringData::append 0x292419 out of line
#ifdef EE_DINKUM_SXL233 // WON library view: expanded inline (AddHelpEntry 0x757800: cmp [data+4],1; new 0x10)
    void CopyOnWrite() { if (mData->GetRefCount() > 1) mData = new GUIStringData(*mData.mPtr); }
#else
    void CopyOnWrite();                     // 0x29121b
#endif
    void Fill(int ch,unsigned short offset,unsigned short length) { CopyOnWrite(); mData->Fill(ch,offset,length); } //actual SDKinline; support0
#ifdef EE_DINKUM_SXL233
    void resize(int inLength) { CopyOnWrite(); mData->resize(inLength,false); }
    void assign(const GUIString& s,unsigned short o,unsigned short n) { CopyOnWrite(); mData->assign(s.mData,o,n); }
    void assign(const GUIString& s,unsigned short o) { unsigned short off=o; unsigned short l=s.length(); assign(s,off,l-off); }
#else
    void resize(int inLength);              // 0x29119a
#endif
    unsigned short length() const { return mData->mLength; }
    bool empty() const { return mData->mLength==0; }
    int operator[](int inPos) const { return mData->mText[inPos]; }
    unsigned short at(int inPos) const { return mData->mText[inPos]; }
    GUIString substr(unsigned short inStart, unsigned short inLength) const; // 0x2f8b90
    GUIString substr(unsigned short inStart) const; // 0x339c80
    int find(int inChar, int inStart) const { return mData->find(inChar, inStart); }
    int find(const GUIString& inText, int inStart) const { return mData->find(inText.mData.get(), inStart); }
    int findNoCase(const GUIString& inText, int inStart) const { return mData->findNoCase(inText.mData.get(), inStart); }
    int rFind(int ch,int start=-1) const { return mData->rFind(ch,start); } //actual SDK inline, support0
    int rFind(const GUIString& text,int start=-1) const { return mData->rFind(text.mData.get(),start); }
    int findOneOf(const GUIString& text,int start=0) const { return mData->findOneOf(text.mData.get(),start); }
    int setAt(int pos,int ch) { CopyOnWrite(); return mData->setAt(pos,ch); }
    void erase(int start,int length=0) { CopyOnWrite(); mData->erase(start,length); }
    void insert(int start,const GUIString& text) { CopyOnWrite(); mData->insert(start,text.mData.get()); }
    void toUpper() { CopyOnWrite(); mData->toUpper(); }
    void toLowerCase() { CopyOnWrite(); mData->toLowerCase(); } //genuine SDK COW forwarder;0Native
    void append(int inChar) { CopyOnWrite(); mData->append(inChar); } //genuine SDK int API; existing algorithm retained through true data owner
    void append(const GUIString& inText) { append(inText, 0, inText.length()); }
    void append(const GUIString& inText, unsigned short inStart) { append(inText, inStart, inText.length()-inStart); }
    void append(const GUIString& inText, unsigned short inStart, unsigned short inLength)
    {
        CopyOnWrite();
        mData->append(inText.mData.get(), inStart, inLength);
    }
    // a std::set<GUIString> key (0x298c87: `key.mData->Compare(node.mData) < 0`)
    bool operator<(const GUIString& inOther) const { return mData->Compare(inOther.mData.get()) < 0; }
    // (EELobbyGame 0x292e9f: the version and saved-player name checks, Compare 0x327fa0 against 0)
    bool operator==(const GUIString& inOther) const { return mData->Compare(inOther.mData.get()) == 0; }
    bool operator!=(const GUIString& inOther) const { return mData->Compare(inOther.mData.get()) != 0; }
    // the narrowed text (EELobbyGameOptionsCtrl 0x2948f5 passes one to a std::string parameter; PROVISIONAL shape)
    operator std::string() const { return mData->GetString(); }
    //: 0x296c32 narrows a GUIString through a std::wstring temporary
    operator std::wstring() const { return mData->GetWString(); }

    SmartPtr<GUIStringData> mData;
};

// Actual call-only clipboard utility; no objects, state, lifetime, allocation or implementation.
class Clipboard {
public:
    static GUIString Get(); //3cea00 cdecl hidden return
    static void Set(const GUIString&); //3ceb90 cdecl constref
};

// 0x377f40: borrowed static resource string by32-bit index. SDKglobalenum spelling is recorded separately.

// Actual ResourceConfig is a complete nativeb0 ConfigObject descendant.
// AddConfig393bb0 allocates exactlyb0 and calls the actual external noarg390740 constructor.
// Constructor/destructor implementations and own vtable remain original providers; no copy/byvalue authority.
// Target has NO laterSDK mConfigTable tail atb0.
class ResourceConfig : public ConfigObject {
protected:
    virtual ~ResourceConfig(); //ordinary390d10/deleting390870, external native
    bool mAllowOverwrite; //54, natural55..57
    int mParseCount; //58 actual393340 increment, capturedbeforekeyread
    template<class T> class ResMap : public std::map<std::string,std::pair<T,int>,StringLessNoCase> {};
    typedef ResMap<GUIString> StringMap;
    typedef ResMap<SmartPtr<Image> > ImageMap;
    typedef ResMap<SmartPtr<Font> > FontMap;
    typedef ResMap<DWORD> ColorMap;
    typedef ResMap<Background> BackgroundMap;
    typedef ResMap<int> IntMap;
    typedef ResMap<SmartPtr<Sound> > SoundMap;
    StringMap mStringMap; //5c..67
    ImageMap mImageMap; //68..73
    FontMap mFontMap; //74..7f
    ColorMap mColorMap; //80..8b
    BackgroundMap mBackgroundMap; //8c..97, actualBackground14+generation4
    IntMap mIntMap; //98..a3
    SoundMap mSoundMap; //a4..af, actualendb0; no laterSDKmConfigTable
    bool HandleString(StringParser&); //392ac0 actual borrowed original provider
    bool HandleImage(StringParser&); //392d90
    bool HandleSound(StringParser&); //392820
    bool HandleFont(StringParser&); //392410
    bool HandleColor(StringParser&); //3923d0
    bool HandleBackground(StringParser&); //393020
public:
    ResourceConfig(); //390740 actualexternal294B, noarg/RET0, AddConfig393bb0 allocatesb0
    virtual bool Virtual08(ConfigParser*,const std::string&,StringParser&); //393350 actualHandleKeyVal
    virtual bool Virtual0C(ConfigParser*); //393340 actualPreParse
    void SetAllowOverwrite(bool value) { mAllowOverwrite=value; } //genuine SDKinline54 setter, support0
    bool GetString(const std::string&,GUIString&); //3934c0
    bool GetImage(const std::string&,Image*&); //393570
    bool GetFont(const std::string&,Font*&); //3935d0
    bool GetSound(const std::string&,Sound*&); //393630
    bool GetColor(const std::string&,DWORD&); //3936a0
    bool GetBackground(const std::string&,Background&); //393710
    bool GetInt(const std::string&,int&); //393820
    const GUIString& SafeGetString(const std::string&); //393890, owned resource lifetime remains original
    Image* SafeGetImage(const std::string&); //3938b0 genuineSDK borrowedpayload; nativeproviderexternal
    Sound* SafeGetSound(const std::string&); //3938d0 genuineSDK borrowedpayload; nativeproviderexternal
    void AddSound(const std::string&,Sound*); //3934a0
    void AddString(const std::string&,const GUIString&);
    void AddImage(const std::string&,Image*);
    void AddFont(const std::string&,Font*);
    void AddColor(const std::string&,DWORD);
    void AddBackground(const std::string&,const Background&);
    void AddInt(const std::string&,int);
    Font* SafeGetFont(const std::string&);
    DWORD SafeGetColor(const std::string&);
    const Background& SafeGetBackground(const std::string&);
    int SafeGetInt(const std::string&);
    static void EnsureComma(StringParser&); //390f20
    static void EnsureCloseParen(StringParser&); //391060
    static int SafeReadColor(StringParser&); //3911a0
    static int SafeReadFontStyle(StringParser&); //3916c0
    static SmartPtr<Font> SafeReadFont(StringParser&); //391870
    static SmartPtr<Sound> SafeReadSound(StringParser&); //391ab0
    static SmartPtr<Image> SafeReadImage(StringParser&); //391ea0
    static Background SafeReadBackground(StringParser&); //392050
};
// Actual complete ResourceConfigTable20 :RefCount8, native3938f0/393aa0.
// Original constructor/destructor and parser/factory providers stay external.
class ResourceConfigTable : public RefCount {
public:
    typedef void (*GlueResourceFunc)(ResourceConfigTable*);
    typedef ComponentConfig* (*ComponentConfigFactory)();
    typedef Component* (*ComponentFactory)();
    ResourceConfigTable(); //3938f0 full20
    ResourceConfig* AddConfig(const std::string&,bool* = NULL); //393bb0 ret8, actual bool* output
    ResourceConfig* SafeGetConfig(const std::string&); //393e50 ret4, throws missing
    ResourceConfig* GetConfig(const std::string&);
    void ClearGlueResourceFuncs();
    void ClearAllocators();
    void ClearControlIds();
    void ClearResources();
    void ClearAll();
    void RegisterGlueResourceFunc(GlueResourceFunc); //393f40
    void SafeGlueResources(); //393fe0 actual callback walk
    void RegisterControlId(const std::string&,int); //3941d0 support0
    void AddAllocator(const std::string&,ComponentConfigFactory,ComponentFactory); //394040
    bool GetControlId(const std::string&,int&); //3942f0
    bool AllocateConfig(const std::string&,SmartPtr<ComponentConfig>&,SmartPtr<Component>&); //3941f0
protected:
    virtual ~ResourceConfigTable(); //ordinary393aa0/deleting3939c0, sole slot0
    typedef std::map<std::string,SmartPtr<ResourceConfig>,StringLessNoCase> ResourceMap;
    ResourceMap mResourceMap; //08..13, actual owning VC6 map0c
    typedef std::set<GlueResourceFunc> GlueResourceFuncSet;
    GlueResourceFuncSet mGlueResourceFuncSet; //14..1f, full20
    typedef std::map<std::string,int,StringLessNoCase> ControlIdMap;
    static ControlIdMap mControlIdMap; //original566af8 declaration only
    struct ConfigAllocator {
        ComponentConfigFactory mConfigFactory;
        ComponentFactory mComponentFactory;
        ConfigAllocator(ComponentConfigFactory a,ComponentFactory b) : mConfigFactory(a),mComponentFactory(b) { }
    };
    typedef std::map<std::string,ConfigAllocator,StringLessNoCase> ConfigAllocatorMap;
    static ConfigAllocatorMap mConfigAllocatorMap; //original566b08 declaration only
};
typedef SmartPtr<ResourceConfigTable> ResourceConfigTablePtr; //actualSDK type, no legacycompat alias

// CriticalSection: ee/EEWonNet.h

// 0x2a6590 (WONAPI string utilities, /GX): widens a string (inCodePage < 0: the default code page 0x2a6570)
std::wstring StringToWString(const std::string& inString, int inCodePage);
// 0x2a6410 (WONAPI string utilities, /GX): narrows a wide string
std::string WStringToString(const std::wstring& inString, int inCodePage);

// a file name split into its parts (0x70 bytes: EELobbyFileDialog 0x29136a has it at [ebp-0x80] below a string
// temporary at [ebp-0x10]; six strings, then the file's details read on demand; class name and
// members PROVISIONAL). Its implicit dtor is out of line (0x29141e, a COMDAT of the lobby glue).
class WONFile {
public:
    WONFile(const std::string& inPath);     // 0x2a7610 (WONAPI, /GX)
    bool NormalizePath();
    std::string GetRelativePath(const WONFile&);
    static WONFile GetWorkingDirectory();
    bool SetWorkingDirectory();
    static const char PATH_SEP;
    void Reset();                          // 0x2a76c0
    void CalcFileInfo();                   // 0x2a79f0
    void CalcFileDirectory();              // 0x2a7a60
    void CalcFullPath();                   // 0x2a7c30
    void CalcFileExtension();              // 0x2a7d80
    bool SetReadOnly(bool inReadOnly);      // actual native file attributes
    bool Exists();
    bool IsDir();
    bool CreateAsDirectory(bool inCreateAll = true);
    const std::string& GetFilePath();
    const std::string& GetFileDirectory();
    const std::string& GetFileExtension();
    const std::string& GetFullPath();
    void SetFilePath(const std::string& inPath); // 0x2a7840
    const std::string& GetFileName();       // 0x2a7ff0 (+0x20, parsed on first use)
    long GetModifyTime();                   // 0x2a8070
    unsigned long GetSize();                // 0x2a8050 (+0x60, read on first use; PROVISIONAL)
    bool Delete(bool inFlag);               // 0x2a7960

    std::string mPath;                      // +0x00
    std::string mS10;                       // +0x10
    std::string mFileName;                  // +0x20
    std::string mS30;                       // +0x30
    std::string mS40;                       // +0x40
    std::string mS50;                       // +0x50
    unsigned long mSize;                   // +0x60, _stat st_size
    long mModifyTime;                       // +0x64, _stat st_mtime
    bool mB68;                              // +0x68, _stat success
    bool mB69;                              // +0x69, original mode-derived predicate
    bool mB6A;                              // +0x6a, directory predicate
    bool mB6B;                              // +0x6b, string10 cache flag
    bool mB6C;                              // +0x6c, string20 cache flag
    bool mB6D;                              // +0x6d, string40 cache flag
    bool mStatValid;                        // +0x6e
    bool mB6F;                              // +0x6f, string50 cache flag
};








// F actual Server resources; per-global producer/lifetime packet retained.

extern unsigned long gLobbyTimeout4E0EBC; //actual DWORD20000; writablePE, provisional nominalname
std::string StringToLowerCase(const std::string&); //2a66f0 external342, helperquota0
std::string StringToUpperCase(const std::string&);
std::wstring WStringToLowerCase(const std::wstring&);
void StringToLowerCaseInPlace(std::string&);
void WStringToUpperCaseInPlace(std::wstring&);
void WStringToLowerCaseInPlace(std::wstring&);

// Individually producer/lifetime-proved popup text; no definitions.


}

#include "ee/wonapi/WONLobbyResourceGlobals.h"

#endif
