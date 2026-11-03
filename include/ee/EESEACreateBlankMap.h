// C:\EEX\Empire Earth\Scenario Editor Actions\EESEACreateBlankMap.h (RECONSTRUCTION; file name inferred). The editor's
// "create a blank map" action (RTTI EESEACreateBlankMap > EEAction; vtable 0x445904, sizeof 0x18; ctor 0x1a5881 out of
// line in EESEACreateBlankMap.cpp: stores its arguments, then posts itself to the UI thread). EEUIFSEMap::CreateMap
// news it. Requires ee/EEAction.h.
#ifndef EE_EESEACREATEBLANKMAP_H
#define EE_EESEACREATEBLANKMAP_H

class EESEACreateBlankMap : public EEAction {
public:
    EESEACreateBlankMap(long inWidth, long inHeight, long inTerrain);   // 0x1a5881
    virtual void Virtual4();                // +0x04 (0x1a58bc)
    virtual void Virtual8() {}              // +0x08 (the shared empty slot 0x1720c6)
    // 0x1a5ae7: makes the new map, from the selected elevation file when there is one (provisional name)
    void CreateMap();

    unsigned long mWidth;                   // +0x0c (unsigned: fild qword in CreateMap)
    unsigned long mHeight;                  // +0x10 (CreateMap sets it from the elevation file's aspect)

    long mTerrain;                          // +0x14 (the terrain table entry's +0x50)
};

#endif
