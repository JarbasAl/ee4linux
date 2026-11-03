// C:\EEX\Empire Earth\Database\EEDatabase.h (RECONSTRUCTION, PARTIAL; class and member names PROVISIONAL). Found
// through the drive-less include path in the original ("\eex\Empire Earth\Database\EEDatabase.h", assert text
// 0x4bc668): every includer constructs its own header-static lock (dynamic initializer: CreateCriticalSection with
// ESVERIFY on line 27, atexit DestroyCriticalSection). Includers: the units with chains at 0xd5458, 0xd7314,
// 0xd9c2b, 0xda5c0, 0xdb205, 0x23b624 (+0x23b66e: a second object in that unit). Include it by bare name
// (ee.EEX_INCLUDE_DIRS); a unit in this folder finds it next to itself ("C:\EEX\..." __FILE__) unless it is reached
// through another include-path header or <...>: check the unit's assert string. Line numbers count: keep the
// ESVERIFY on line 27. Requires lle/TSScheduler.h.
#ifndef EE_DATABASE_EEDATABASE_H
#define EE_DATABASE_EEDATABASE_H

// A critical section created on construction (not polymorphic: the lock is at +0; the dynamic initializer pushes
// the object's address). Each unit's copy is used with Enter/Leave around its table accesses (0xd56b8..).
class EEDatabaseLock {
public:
    EEDatabaseLock();
    ~EEDatabaseLock();

    TSCriticalSection* mLock;               // +0x00
};

//
//
//
inline EEDatabaseLock::EEDatabaseLock()
{
    ESVERIFY_AT(TSScheduler::CreateCriticalSection(this->mLock) == keseOK, 27);
}

inline EEDatabaseLock::~EEDatabaseLock()
{
    TSScheduler::DestroyCriticalSection(this->mLock);
}

static EEDatabaseLock sDatabaseLock;        // one per includer (0x8e60e0, 0x8e8430, 0x8e8458, 0x96445c, ...)

#endif
