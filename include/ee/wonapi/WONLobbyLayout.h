// Original-backed prerequisite contracts; no new construction, copying or vtable authority.
#ifndef EE_WONAPI_WONLOBBYLAYOUT_H
#define EE_WONAPI_WONLOBBYLAYOUT_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Six concrete layout owners promoted only after actual allocations/inline constructors/normal destructors proof.
// Complete Param40; genuine SDK explicit construction, borrowed fields and natural POD copy/lifetime.
class PercentLayoutParam {
public:
    PercentLayoutParam(Component *left, Component *top, Component *right, Component *bottom, int px, int py, int pw = -1, int ph = -1, int lp = -1, int tp = -1, int rp = -1, int bp = -1); //337320 genuine SDK explicit constructor, complete40
    PercentLayoutParam(Component *ref,int px,int py,int pw=-1,int ph=-1,int lp=-1,int tp=-1,int rp=-1,int bp=-1);
    PercentLayoutParam(Component *ref1,Component *ref2,int px,int py,int pw=-1,int ph=-1,int lp=-1,int tp=-1,int rp=-1,int bp=-1);
    void Init(Component*,Component*,Component*,Component*,int,int,int,int,int,int,int,int); //3372c0 genuine SDK explicit initializer
    void SetPercentages(int,int,int,int); //3373f0 genuine SDK scalar setter
    void SetPadding(int,int,int,int); //337410 genuine SDK scalar setter
    void SetBorders(Component*, Component*, Component*, Component*);
    void DoCalc(Component*, Component*);
    Component *left, *top, *right, *bottom;
    int px, py, pw, ph;
    int lp, tp, rp, bp;
    int dx, dy, dw, dh; //30..3c, real POD tail copied by native factory; DoCalc does not read it
};
// SOURCE_ONLY_NO_NATIVE_ID: complete same-snapshot SDK owner; no inferred Native allocation/VT identity.
class VerticalFlowLayout : public ChildLayoutInstructionBase {
protected:
    struct ItemInfo {
        int mTopPad;
        int mLeftPad;
        Component* mComponent;
        ItemInfo(Component* component,int topPad,int leftPad)
            : mTopPad(topPad),mLeftPad(leftPad),mComponent(component) {}
    };
    Component* mTopComp;
    typedef std::list<ItemInfo> ComponentList;
    ComponentList mComponentList;
public:
    VerticalFlowLayout(Component*);
    VerticalFlowLayout* Add(Component*,int=0,int=0);
    virtual void Execute(Component*);
};
class SameSizeLayout : public ChildLayoutInstructionBase {
public:
    SameSizeLayout(bool horz=true,bool vert=true) : mHorz(horz),mVert(vert) {} //actual388e56..ea1
    SameSizeLayout(Component* first,bool horz=true,bool vert=true) : mHorz(horz),mVert(vert) { Add(first); } //actual301964..301999: first insertion is construction-owned
    SameSizeLayout* Add(Component*); //3377a0 genuine external native algorithm
    virtual ~SameSizeLayout(); // external original generated lifetime
    virtual void Execute(Component*);
    std::list<Component*> mComponentList; //0c
    bool mHorz, mVert; //18,19; full1c
};
class ClearRightLayout : public ChildLayoutInstructionBase {
public:
    ClearRightLayout(Component* component,int pad=0) : mComponent(component),mPad(pad) {} //3890d8..116
    ClearRightLayout* Add(Component* component) { mComponentList.push_back(component); return this; }
    virtual ~ClearRightLayout();
    virtual void Execute(Component*);
    Component* mComponent; //0c borrowed
    int mPad; //10
    std::list<Component*> mComponentList; //14; full20
};
class SpaceEvenlyLayout : public ChildLayoutInstructionBase {
public:
    SpaceEvenlyLayout(Component* box,int left=0,int right=0,bool horz=true) : mSpaceBox(box),mLeftPad(left),mRightPad(right),mIsHorz(horz) {} //3892ea..337
    SpaceEvenlyLayout* Add(Component* component) { mComponentList.push_back(component); return this; }
    virtual void Execute(Component*);
    Component* mSpaceBox; //0c borrowed
    int mLeftPad, mRightPad; //10,14
    bool mIsHorz; //18
    std::list<Component*> mComponentList; //1c; full28
};
class NoOverlapLayout : public ChildLayoutInstructionBase {
public:
    NoOverlapLayout(Component* target,int type,Component* ref,int horz=0,int vert=0) : mRef(ref),mTarget(target),mType(type),mHorzPad(horz),mVertPad(vert) {} //389608..64d
    NoOverlapLayout* Add(Component* component) { mComponentList.push_back(component); return this; }
    virtual void Execute(Component*);
    Component *mRef, *mTarget; //0c,10 borrowed
    int mType, mHorzPad, mVertPad; //14,18,1c
    std::list<Component*> mComponentList; //20; full2c
};
class SizeAtLeastLayout : public ChildLayoutInstructionBase {
public:
    SizeAtLeastLayout(Component* target,int horz=0,int vert=0) : mTarget(target),mHorzPad(horz),mVertPad(vert) {} //3898c3..909
    SizeAtLeastLayout* Add(Component* component) { mComponentList.push_back(component); return this; }
    virtual void Execute(Component*);
    Component* mTarget; //0c borrowed
    int mHorzPad, mVertPad; //10,14
    std::list<Component*> mComponentList; //18; full24
};
class ChildLayoutInstruction : public ChildLayoutInstructionBase {
public:
    virtual void Execute(Component*);
    virtual bool UsesComponent(Component*);
    int mFlags;
    Component* mChild;
    Component* mReference;
    int mX, mY, mWidth, mHeight;
};
// Complete ChildLayoutManager20 declaration is in WONLobbyGUI.h before Container.
}
#endif
