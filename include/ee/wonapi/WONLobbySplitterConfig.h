#ifndef EE_WONAPI_WONLOBBYSPLITTERCONFIG_H
#define EE_WONAPI_WONLOBBYSPLITTERCONFIG_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Original-owned Splitter typed field contract, actual native307060 member order.
// This version predates SDK's moved-to-top doubles. No new construction,
// allocation, sizeof, copy or own lifetime implementation authority.
class Splitter : public Container {
public:
    Splitter(); //307060 genuine explicit SDK constructor, complete0x118 owner
    enum QuadrantPosition { Position_Left=1,Position_Top=2,Position_Right=4,Position_Bottom=8,
        Position_TopLeft=3,Position_TopRight=6,Position_BottomLeft=9,Position_BottomRight=12 };
    enum SplitterType { Splitter_LeftRight=5,Splitter_UpDown=10,Splitter_LeftRight_Top=0x15,
        Splitter_LeftRight_Bottom=0x25,Splitter_UpDown_Left=0x4a,Splitter_UpDown_Right=0x8a,Splitter_Center=0x10f };
    void AddQuadrant(Component*,QuadrantPosition); //307440
    void AddSplitter(Component*,SplitterType); //307670
    void AddInvisibleSplitter(SplitterType,int=10); //307760
    void AddComplexSplitter(Container*,SplitterType,Component* =NULL); //307690
    void SetMinLeftRight(int,int); //307c30 support0
    void SetMinTopBottom(int,int); //307c50 support0
    void SetPercentages(int,int); //307c70 support0
    void MaxQuadrant(QuadrantPosition); //307810
    void UnMaxQuadrant(); //307990
    void SetSplitterPos(int,int,bool=true); //307c90
    void RegisterMaxButton(Button*,QuadrantPosition); //307300
    bool CheckMaxButton(ComponentEvent*); //3080b0
protected:
    virtual ~Splitter(); //actual normal3071e0, external lifetime only
    virtual void Virtual08(); //307de0 SizeChanged
    virtual void Virtual2C(int,int,int); //307f30 MouseDown, existing canonical int button ABI
    virtual void Virtual30(int,int,int); //307ff0 MouseUp discovery gap
    virtual void Virtual38(int,int); //308030 MouseDrag
    virtual void HandleComponentEvent(ComponentEvent*); //308140 actual7c override
    struct Quadrant {
        Component* mComponent; //node+08 borrowed, Container owns actual child
        QuadrantPosition mPosition; //node+0c
        Quadrant(Component* c,QuadrantPosition p):mComponent(c),mPosition(p) {}
    };
    typedef std::list<Quadrant> QuadrantList;
    QuadrantList mQuadrantList; //84 allocator,88 header,8c count; native node10
    struct SplitterParam {
        Component* mComponent; //node+08 borrowed, Container owns actual child
        SplitterType mType; //node+0c
        SmartPtr<Component> mDragComponent; //node+10 retained, native node14
        SplitterParam(Component* c,SplitterType t):mComponent(c),mType(t) {}
    };
    typedef std::list<SplitterParam> SplitterList;
    SplitterList mSplitterList; //90 allocator,94 header,98 count
    typedef std::list<SmartPtr<Button> > ButtonList;
    ButtonList mButtonList; //9c allocator,a0 header,a4 count; native node0c
    WONRectangle mOldBounds; //a8..b7 real POD rectangle
    bool mLeftRightDrag,mUpDownDrag; //b8,b9; naturalba/bb alignment
    int mDragX,mDragY; //bc,c0
    int mMinLeft,mMinRight,mMinTop,mMinBottom; //c4,c8,cc,d0
    bool mUpDownMax,mLeftRightMax; //d4,d5; naturald6/d7 alignment
    double mLeftPercent,mTopPercent; //d8,e0, actual old target order
    bool mSplitCoordsValid; //e8, naturale9..eb alignment
    int mSplitX,mSplitY,mSplitLeft,mSplitRight,mSplitTop,mSplitBottom; //ec..100
    int mLeftRightSplitterWidth,mUpDownSplitterHeight; //104,108
    bool mHaveMaxQuadrant; //10c, natural10d..10f alignment
    QuadrantPosition mCurMaxQuadrant; //110, natural alignment, no size claim
    void CalcQuadrants(); //307a50
    void CalcSplitterPositions(); //307ad0
    void AddSplitterPrv(Component*,SplitterType,Component*); //3074b0
};
// Actual9c: complete existing ContainerConfig98 plus borrowed pointer98.
// No config construction/factory/allocation or lifetime implementation.
class SplitterConfig : public ContainerConfig {
public:
    static ComponentConfig* CfgFactory(); //39b410 genuine SDK static API, original external provider only
    static Component* CompFactory(); //39b3b0 genuine SDK static API, original external provider only
    virtual void Virtual34(Component*); //38f960 original external SetComponent
protected:
    virtual bool Virtual24(const std::string&); //395450 actual protected SDK handler24
    Splitter* mSplitter; //98
    Splitter::QuadrantPosition ReadQuadrant(); //394ed0
    Splitter::SplitterType ReadSplitterType(); //3951b0
};
}
#endif
