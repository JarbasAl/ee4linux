// C:\EEX\Low-Level Engine\File System\FSPersistentList.h (file name inferred): the registry of FSPersistent
// prototypes by tag (a lazily created singleton). FSPersistentList.cpp 0xbdb4..0xc129; vtable 0x1009762c.
// Requires <map>, TSLockable.h and lle/FSPersistent.h.
#ifndef LLE_FSPERSISTENTLIST_H
#define LLE_FSPERSISTENTLIST_H

#include <map>

class LLE_API FSPersistentList : public TSLockable {   // sizeof 0x14 (operator new in Instance)
public:
    // implicit copy ctor (0x4574) and operator= (0x45b7), emitted with File System.cpp's inlines
    virtual ~FSPersistentList();                                                // 0xbe25
    FSPersistent* CreateDefaultPersistent(long inTagID);                        // 0xbe88
    static void Shutdown(bool inForced);                                        // 0xbed8
    static FSPersistentList* Instance();                                        // 0xbf05
    long RegisterPersistent(FSPersistent* inPersistent);                        // 0xbf54

private:
    FSPersistentList();                                                         // 0xbdfb

    std::map<long, FSPersistent*> mPersistents;   // +0x08

    static TSLockable sCreateSingletonLock;       // 0x1010f778
    static FSPersistentList* sSingleton;          // 0x1010f780
};

#endif
