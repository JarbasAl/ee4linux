// WONAPI GUI/resource classes used by the game's lobby glue (C:\EEX\Empire Earth\WONAPI\WONLobby\GameCode\EmpireEarth\)
// (RECONSTRUCTION; r12-misc-h2, round 12). WONAPI proper (0x29deb0..) is not decompiled yet: only what the GameCode
// units' codegen proves is declared here; every class and member name is PROVISIONAL (by behaviour), layouts are
// undecoded unless a member is listed. Requires <string>, ee/EEWonNet.h (RefCount, SmartPtr).
#ifndef EE_WONAPI_WONLOBBYRESOURCE_H
#define EE_WONAPI_WONLOBBYRESOURCE_H

namespace WONAPI {

class Sound;

// the shared, reference-counted text of a GUIString (0x10 bytes: `new` in 0x328450; ctor 0x327840 from a wide string)
class GUIStringData : public RefCount {
public:
    GUIStringData(const wchar_t* inText);  // 0x327840 (WONAPI)
    GUIStringData(const char* inText);     // 0x3277e0 (WONAPI)
    GUIStringData(const std::string& inText);      // 0x3278a0 (WONAPI; r12-misc-h5)
    GUIStringData(const std::wstring& inText);     // 0x327910 (WONAPI; r12-misc-h5)
    GUIStringData(const GUIStringData& theCopy);   // 0x327600 (WONAPI; GUIString::CopyOnWrite; r12-misc-h5)
    int Compare(const GUIStringData* inOther) const;    // 0x327fa0 (WONAPI; GUIString's operator<; r12-misc-h5)
    // 0x292419 (a COMDAT of the lobby glue: its first user is in 0x290f66..0x2942c8; r12-misc-h5)
    void append(const GUIStringData* inText, unsigned short inStart, unsigned short inLength);
    int Compare(const GUIStringData* inOther, bool inCaseSensitive) const;     // 0x328340 (WONAPI)
    int CompareNoCase(const GUIStringData* inOther) const;  // 0x328030 (WONAPI; towupper compare; r28 C4)
    std::string GetString() const;          // 0x327d90 (WONAPI, /GX): narrowed
    std::wstring GetWString() const;        // 0x327f30 (WONAPI; r12-misc-h5)

    wchar_t* mText;                         // +0x08 (r12-misc-h5: 0x292419)
    unsigned short mLength;                 // +0x0c
    unsigned short mCapacity;               // +0x0e (r12-misc-h7: Reserve 0x2911c9)
    // 0x2911c9 (a COMDAT of EELobbyFileDialog.cpp): room for inLength characters and the terminator (doubled when
    // inGrow), the text kept
    void Reserve(int inLength, bool inGrow);
};

// 0x328450 (WONAPI, /GX): the shared empty string (a function-local static, created on first use)
GUIStringData* GetEmptyGUIStringData();

// a lobby text: one reference to its shared data (4 bytes: the resource unit's 73 globals)
class GUIString {
public:
    GUIString();                            // 0x29171b (out of line: every `$E` ctor jumps to it)
    GUIString(const char* inText);         // 0x29115e (new GUIStringData(inText); no caller expands it)
    GUIString(const std::string& inText);  // 0x29848e (likewise; r12-misc-h5)
    GUIString(const std::wstring& inText); // 0x29258f (likewise; r12-misc-h5)
    // r23 (EELobbyGame 0x292e9f: GetEmptyGUIStringData() into the SmartPtr ctor COMDAT 0x293cd4)
    GUIString(GUIStringData* inData) : mData(inData) {}
    GUIString& operator=(const GUIString& theCopy) { mData = theCopy.mData; return *this; }
    GUIString& operator=(GUIStringData* theData) { mData = theData; return *this; }
    int Compare(const GUIString& inOther, bool inCaseSensitive) const
    {
        return mData->Compare(inOther.mData.get(), inCaseSensitive);
    }
    int CompareNoCase(const GUIString& inOther) const { return mData->CompareNoCase(inOther.mData.get()); }
    // r12-misc-h5: the lobby's saved-games directory 0x2988e9 expands append(const GUIString&) (the length is read
    // before the copy-on-write call) and calls CopyOnWrite 0x29121b / GUIStringData::append 0x292419 out of line
    void CopyOnWrite();                     // 0x29121b
    void resize(int inLength);              // 0x29119a (r12-misc-h7: EELobbyFileDialog cuts the extension off)
    unsigned short length() const { return mData->mLength; }
    void append(const GUIString& inText) { append(inText, 0, inText.length()); }
    void append(const GUIString& inText, unsigned short inStart, unsigned short inLength)
    {
        CopyOnWrite();
        mData->append(inText.mData.mPtr, inStart, inLength);
    }
    // a std::set<GUIString> key (0x298c87: `key.mData->Compare(node.mData) < 0`; r12-misc-h5)
    bool operator<(const GUIString& inOther) const { return mData->Compare(inOther.mData.get()) < 0; }
    // r23 (EELobbyGame 0x292e9f: the version and saved-player name checks, Compare 0x327fa0 against 0)
    bool operator==(const GUIString& inOther) const { return mData->Compare(inOther.mData.get()) == 0; }
    bool operator!=(const GUIString& inOther) const { return mData->Compare(inOther.mData.get()) != 0; }
    // the narrowed text (EELobbyGameOptionsCtrl 0x2948f5 passes one to a std::string parameter; PROVISIONAL shape)
    operator std::string() const { return mData->GetString(); }
    // r12-misc-h5: 0x296c32 narrows a GUIString through a std::wstring temporary
    operator std::wstring() const { return mData->GetWString(); }

    SmartPtr<GUIStringData> mData;
};

// one section of a resource file (GetString reads the section's string map at +0x5c)
class ResourceConfigTable {
public:
    ResourceConfigTable* GetTable(const std::string& inName);       // 0x393e50 (WONAPI)
    void AddSound(const std::string& inName, Sound* inSound);        // 0x3934a0 (WONAPI; "LobbyMusic"; r12-misc-h5)
    const GUIString& GetString(const std::string& inName);          // 0x393890 (WONAPI)
};

// the list of resource loaders the lobby runs when it reads its resource file (0x20 bytes)
class ResourceCollection : public RefCount {
public:
    typedef void (*LoadFunc)(ResourceConfigTable* inTable);

    ResourceCollection();                                           // 0x3938f0 (WONAPI)
    void AddLoadFunc(LoadFunc inFunc);                              // 0x393f40 (WONAPI)
    // 0x393bb0 (WONAPI): a section by name ("LobbyGlobal"; r12-misc-h5, name PROVISIONAL)
    ResourceConfigTable* GetTable(const std::string& inName, bool inFlag);
    // 0x394040 (WONAPI): a component type the resource file may use, with its config and component factories
    void AddComponentType(const std::string& inName, RefCount* (*inConfigFactory)(),
                          RefCount* (*inComponentFactory)());

    char _gap08[0x18];
};

typedef SmartPtr<ResourceCollection> ResourceCollectionPtr;

// CriticalSection: ee/EEWonNet.h (r25pre-won-2: ServerContext has one)

// 0x2a6590 (WONAPI string utilities, /GX): widens a string (inCodePage < 0: the default code page 0x2a6570)
std::wstring StringToWString(const std::string& inString, int inCodePage);
// 0x2a6410 (WONAPI string utilities, /GX): narrows a wide string (inCodePage < 0: the default; r12-misc-h5)
std::string WStringToString(const std::wstring& inString, int inCodePage);

// a file name split into its parts (0x70 bytes: EELobbyFileDialog 0x29136a has it at [ebp-0x80] below a string
// temporary at [ebp-0x10] (r12-misc-h7; was 0x84); six strings, then the file's details read on demand; class name and
// members PROVISIONAL, r12-misc-h5). Its implicit dtor is out of line (0x29141e, a COMDAT of the lobby glue).
class WONFile {
public:
    WONFile(const std::string& inPath);     // 0x2a7610 (WONAPI, /GX)
    const std::string& GetFileName();       // 0x2a7ff0 (+0x20, parsed on first use)
    unsigned long GetSize();                // 0x2a8050 (+0x60, read on first use; PROVISIONAL)
    bool Delete(bool inFlag);               // 0x2a7960 (r12-misc-h7: the file dialog's delete button; PROVISIONAL)

    std::string mPath;                      // +0x00
    std::string mS10;                       // +0x10
    std::string mFileName;                  // +0x20
    std::string mS30;                       // +0x30
    std::string mS40;                       // +0x40
    std::string mS50;                       // +0x50
    char _gap60[0x10];
};

}

#endif
