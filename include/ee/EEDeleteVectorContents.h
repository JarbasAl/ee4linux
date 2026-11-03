// EEDeleteVectorContents.h (RECONSTRUCTION; file name inferred). The DeleteVectorContents
// template of ee/EECalamityManager.h (cdecl template COMDATs there 0xaea54/0xaeb53), also instantiated by
// EEScenarioManager.cpp (0x1b5d53 triggers, 0x1b5d90 conditions, 0x1b5dcd effects, 0x1b5e0a objects, 0x1b5e3a areas).
#ifndef EE_EEDELETEVECTORCONTENTS_H
#define EE_EEDELETEVECTORCONTENTS_H

#ifndef EE_DELETEVECTORCONTENTS_DEFINED
#define EE_DELETEVECTORCONTENTS_DEFINED
// Deletes every element of ioList and empties it; the number deleted
template<class T> unsigned long DeleteVectorContents(T& ioList)
{
    unsigned long theCount = 0;
    for (T::iterator theIterator = ioList.begin(); theIterator != ioList.end(); ++theIterator) {
        delete *theIterator;
        theCount++;
    }
    ioList.clear();
    return theCount;
}
#endif

#endif
