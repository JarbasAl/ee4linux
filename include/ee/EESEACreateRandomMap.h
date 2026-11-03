// C:\EEX\Empire Earth\Scenario Editor Actions\EESEACreateRandomMap.h (RECONSTRUCTION; file name inferred). The
// editor's "create a random map" action (RTTI EESEACreateRandomMap > EEAction; vtable 0x445944, sizeof 0x14; ctor
// 0x1a66ae out of line in EESEACreateRandomMap.cpp: stores its arguments, then posts itself to the UI thread).
// EEUIFSEMap::CreateMap news it. Requires ee/EEAction.h.
#ifndef EE_EESEACREATERANDOMMAP_H
#define EE_EESEACREATERANDOMMAP_H

class EESEACreateRandomMap : public EEAction {
public:
    EESEACreateRandomMap(unsigned long inSize, unsigned long inStyle);   // 0x1a66ae
    virtual void Virtual4();                // +0x04 (0x1a66e2)
    virtual void Virtual8() {}              // +0x08 (the shared empty slot 0x1720c6)

    unsigned long mSize;                    // +0x0c (the random map size combo's selection)
    unsigned long mStyle;                   // +0x10 (the map style combo's selection: a generator)
};

#endif
