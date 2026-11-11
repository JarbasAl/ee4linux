// C:\EEX\Empire Earth\Scenario Editor\EESEUITechListBox.h (RECONSTRUCTION; file name inferred). The player tab's two
// technology list boxes (ids 0x154 and 0x155: RTTI EESEUITechListBox > UILBString; vtable 0x444a6c, sizeof 0x15c,
// ctor 0x19764b; its unit 0x19764b..0x199afd). Each holds the tech ids of its lines
// (mTechs); CopyData collects the visible lines that depend on (0x154) or are prerequisites of (0x155) the other list's
// selection, and DrawPolygons highlights them. Names are provisional. Requires lle/UILBString.h, <vector>.
#ifndef EE_EESEUITECHLISTBOX_H
#define EE_EESEUITECHLISTBOX_H

class EESEUITechListBox : public UILBString {
public:
    // 0x19764b (EESEPlayerScreen's control builder 0x17509c news 0x15c twice): the rectangle from the location and
    // the unsigned size (GERectangle's inline ctor), the selection colour (0, 0.38, 0)
    EESEUITechListBox(unsigned long inID, std::vector<UWideString>& inStrings, GEFont* inFont, bool inAutoSelect,
                      const GE2DPoint& inLocation, unsigned long inWidth, unsigned long inHeight, UIForm* inForm,
                      unsigned long inIndex, unsigned long inBorderThickness);
#ifdef EE_PORT
    EESEUITechListBox(unsigned long inID, const std::vector<UWideString>& inStrings, GEFont* inFont, bool inAutoSelect, const GE2DPoint& inLocation, unsigned long inWidth, unsigned long inHeight, UIForm* inForm, unsigned long inIndex, unsigned long inBorderThickness)
        : EESEUITechListBox(inID, const_cast<std::vector<UWideString>&>(inStrings), inFont, inAutoSelect, inLocation, inWidth, inHeight, inForm, inIndex, inBorderThickness)
    {
    }
#endif
    virtual ~EESEUITechListBox() {}         // +0x00 (??_G 0x199aa4 calls the out-of-line copy 0x199ac0)
    virtual long CopyData();                // +0x14 0x19771c
    virtual long DrawPolygons();            // +0x18 0x19789f
    void AddTech(long inID);                // 0x197709 (EESEPlayerScreen::Fn1760AD)
    void Clear();                           // 0x197885 (mTechs, then DeleteAllEntries)
    // 0x1979c1 (EESEATechTree passes its +0x0c, +0x10: the first and last changed lines) (provisional name)
    void Fill(long inFirst, long inLast);
    // 0x197c9f: sorts mTechs (by epoch: checkbox 0x15e; unit or tech names: radio button 0x14d) and rebuilds the
    // lines (EESEPlayerScreen::Fn1760AD/Fn1766ED) (provisional name)
    void Rebuild();

    std::vector<long> mTechs;               // +0x13c the tech ids, one per line
    std::vector<unsigned long> mDependents; // +0x14c the visible lines to highlight (CopyData)
};

#endif
