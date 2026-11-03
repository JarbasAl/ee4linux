// C:\EEX\Low-Level Engine\User Interface\UILineGraph.h (RECONSTRUCTION). A UIGraph that draws each enabled series as
// connected line segments; grid values print as "%.2f". Vtable 0x100996f0 (UIGraph's 46 slots plus DrawSeries at
// 0xb8). No members of its own (sizeof 0x230). The implicit copy ctor 0x100670dd, operator= 0x100670f5 and ??_E are
// emitted by UIControlsTable.cpp; UILineGraph.cpp (0x100785cd..0x10078a70) holds the key chains and the rest.
// Requires lle/UIGraph.h.
#ifndef LLE_UILINEGRAPH_H
#define LLE_UILINEGRAPH_H

class UGraphSeries;

class LLE_API UILineGraph : public UIGraph {
public:
    UILineGraph(unsigned long inId, const GERectangle& inRectangle, UIForm* inForm, unsigned long inIndex,
                unsigned long inBorderThickness);
    virtual ~UILineGraph();
    virtual long DrawPolygons();
    virtual long DrawSeries(const UGraphSeries& inSeries);  // slot 0xb8
    // identical bodies, ICF-folded (both exports 0x10078a53)
    virtual void FormatXValues(float inValue, UWideString& outString);
    virtual void FormatYValues(float inValue, UWideString& outString);
};

#endif
