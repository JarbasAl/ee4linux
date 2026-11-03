// The main game form's unit-hack panel (RECONSTRUCTION; class and header names inferred: no RTTI, not polymorphic).
// Owned by EEUIFMainGame (+0x4e8, new'd 0x2c bytes by its ctor 0x262e73, deleted by its dtor 0x2647e1);
// all members are in EEUIFMainGame.cpp (0x2628e5..0x262e73). It lists the creatable unit classes by name in the form's
// combo box 98 and sends EEMHackANewUnit (place a unit) / EEMLoadUnitData (reload a class). Requires <vector>,
// lle/UWideString.h.
#ifndef EE_EEUIFMGUNITHACK_H
#define EE_EEUIFMGUNITHACK_H

class EEUIFMainGame;
class GE2DPoint;

class EEUIFMGUnitHack {
public:
    EEUIFMGUnitHack(EEUIFMainGame* inForm);     // 0x2628e5 (the sorted class names)
    ~EEUIFMGUnitHack();                         // 0x262a91
    void PlaceUnit(const GE2DPoint& inTile);    // 0x262aa5 (EEMHackANewUnit for the selected player)
    void FillComboBox();                        // 0x262c6f
    void LoadUnitData();                        // 0x262cdc (EEMLoadUnitData for the selected class)
    void Update(const GE2DPoint& inLocation);   // 0x262d66 (the left mouse button over the game view)
    unsigned long GetSelection();               // 0x262e29 (mMode 1: combo 98, 2: list 99)
    void Fn157269(const GE2DPoint& inTile);    // (ICF-folded into 0x157269: an empty body; provisional name)
    void FnE123D();                             // (ICF-folded into 0xe123d: an empty body; provisional name)

    std::vector<UWideString> mNames;            // +0x00 the class names, sorted
    std::vector<long> mClassIDs;                // +0x10 the gUnitClassTable index of each name
    EEUIFMainGame* mForm;                       // +0x20
    unsigned long mMode;                        // +0x24 (unsigned: `cmp 0; jbe`)
    long mL28;                                  // +0x28 (= -1)
};

#endif
