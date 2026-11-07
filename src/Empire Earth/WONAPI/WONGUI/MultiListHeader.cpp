// Multi-list header behavior and genuine completeac setup; original destructor retained.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Original302bc0/23B reciprocal borrowed linkage, support0.
// FUNCTION: WON 0x00702bc0
void MultiListHeader::SetMultiListArea(MultiListArea* area)
{
    mListArea=area;
    if(mListArea!=NULL) mListArea->mListHeader=this;
}
// Original302cb0/52B conditional field update, support0.
// FUNCTION: WON 0x00702cb0
void MultiListHeader::SetSortColumn(int column,ListSortType type)
{
    if(column<0 || column>=mColumnVector.size()) return;
    mColumnVector[column].mSortType=type;
    mCurSortCol=column;
}
// FUNCTION: WON 0x00702cf0
void MultiListHeader::SetColumnItem(int column,Component* item)
{
    mDragColumn=-1;
    if(mColumnVector.size()<=column) mColumnVector.resize(column+1);
    Component* old=mColumnVector[column].mComponent;
    if(old!=NULL) VirtualC4(old);
    item->mControlId=column;
    item->SetComponentFlags(0x2000,true);
    mColumnVector[column].mComponent=item;
    Fn2F6A60(item);
    if(mListArea!=NULL && column<mListArea->mColumnInfoVector.size() && mListArea->GetColumnWidth(column)<=0) {
        int width,height;
        item->Virtual0C(width,height);
        mListArea->SetColumnWidth(column,width,false);
        if(mListArea->mSortColumn==column) {
            mCurSortCol=column;
            mColumnVector[column].mSortType=static_cast<ListSortType>(mListArea->mSortType);
        }
    } else NotifyColumnChange();
}
// FUNCTION: WON 0x00702e10
void MultiListHeader::NotifyColumnChange()
{
    if(mListArea==NULL) return;
    std::vector<MultiListArea::ColumnInfo>& columns=mListArea->mColumnInfoVector;
    int x=-mListArea->mHorzOffset;
    int size=mColumnVector.size();
    for(int i=0;i<size;++i) {
        Component* component=mColumnVector[i].mComponent;
        int width=-1;
        if(i<columns.size()) width=columns[i].mWidth;
        if(component!=NULL) {
            if(width==-1) {
                component->SetEnabled(false);
                width=0;
            } else component->SetEnabled(true);
            component->SetVisible(width!=0);
            component->Fn2F4BF0(x,0,width,Height());
        }
        x+=width;
    }
    if(mColumnVector.size()>0) {
        Component* component=mColumnVector[mColumnVector.size()-1].mComponent;
        component->SetVisible(true);
        component->Fn2F4BF0(component->Left(),0,Width(),Height());
    }
    Invalidate();
}
// Original302fb0/17B base size-change and notify, support0.
// FUNCTION: WON 0x00702fb0
void MultiListHeader::Virtual08()
{
    Container::Virtual08();
    NotifyColumnChange();
}
// Original302fd0/55B raw child lookup, support0.
// FUNCTION: WON 0x00702fd0
int MultiListHeader::GetColumnIndex(Component* component)
{
    for(int i=0;i<mColumnVector.size();++i)
        if(mColumnVector[i].mComponent==component) return i;
    return -1;
}
// Original303010/64B guarded field getter, support0.
// FUNCTION: WON 0x00703010
ListSortType MultiListHeader::GetSortType(int column)
{
    if(column<0 || column>=mColumnVector.size() || column!=mCurSortCol)
        return ListSortType_None;
    return mColumnVector[column].mSortType;
}
}

namespace WONAPI {
// FUNCTION: WON 0x00702be0
void MultiListHeader::HandleComponentEvent(ComponentEvent* event)
{
    if(event->mType==15 && mListArea!=NULL) {
        Component* child=event->mComponent;
        Container* parent=child->mParent;
        while(parent!=this && parent!=NULL) {
            child=parent;
            parent=child->mParent;
        }
        if(parent==NULL) return;
        int column=child->mControlId;
        if(column>=0 && column<(int)mListArea->mColumnInfoVector.size() && column<mColumnVector.size()) {
            ListSortType type=mColumnVector[column].mSortType;
            if(column==mCurSortCol || type==ListSortType_None)
                type=type==ListSortType_Ascending ? ListSortType_Descending : ListSortType_Ascending;
            mListArea->SetSortColumn(column,type);
            return;
        }
    }
    Container::HandleComponentEvent(event);
}
// FUNCTION: WON 0x00703050
ListSortType MultiListHeader::GetSortTypeForButton(Component* button)
{
    while(true) {
        Container* parent=button->mParent;
        if(parent==NULL) return ListSortType_None;
        MultiListHeader* header=dynamic_cast<MultiListHeader*>(parent);
        if(header==NULL) {
            button=parent;
            continue;
        }
        if(header->mDrawSortArrow) return header->GetSortType(header->GetColumnIndex(button));
        return ListSortType_None;
    }
}
// FUNCTION: WON 0x007030b0
Component* MultiListHeader::GetDragComponent(int x)
{
    mDragColumn=-1;
    mDragComponent=NULL;
    std::vector<MultiListArea::ColumnInfo>& columns=mListArea->mColumnInfoVector;
    for(int i=0;i<mColumnVector.size();++i) {
        Component* component=mColumnVector[i].mComponent;
        if(component!=NULL && x>=component->Left() && x<=component->Right()) {
            int width=component->Width()/4;
            if(width>5) width=5;
            if(x-component->Left()<width && i!=0) mDragColumn=i-1;
            else if(component->Right()-x<width && i!=mColumnVector.size()) mDragColumn=i;
            else return NULL;
            if(mDragColumn>=columns.size()) {
                mDragColumn=-1;
                return NULL;
            }
            MultiListArea::ColumnInfo& info=columns[mDragColumn];
            if(info.mSizeMode==0 || (info.mSizeMode==2 && (info.mPartnerColumn<0 || info.mPartnerColumn>=columns.size()))) {
                mDragColumn=-1;
                return NULL;
            }
            mDragX=x-mListArea->mColumnInfoVector[mDragColumn].mWidth;
            mDragComponent=mColumnVector[mDragColumn].mComponent;
            return mDragComponent;
        }
    }
    return NULL;
}
// Original303260/47B dispatch gating, support0.
// FUNCTION: WON 0x00703260
void MultiListHeader::Virtual2C(int x,int y,int button)
{
    if(button==0 && GetDragComponent(x)!=NULL) return;
    Container::Virtual2C(x,y,button);
}
// FUNCTION: WON 0x00703290
void MultiListHeader::Virtual34(int x,int y)
{
    Container::Virtual34(x,y);
    if(GetDragComponent(x)!=NULL) Virtual78(Cursor::GetStandardCursor(StandardCursor_EastWest));
    else if(mCurrentCursor.get()==Cursor::GetStandardCursor(StandardCursor_EastWest)) Virtual78(NULL);
}
// FUNCTION: WON 0x007032f0
void MultiListHeader::Virtual38(int x,int y)
{
    if(mDragColumn>=0) {
        int width=x-mDragX;
        if(width<0) width=0;
        mListArea->SetColumnWidth(mDragColumn,width,false);
        Virtual78(Cursor::GetStandardCursor(StandardCursor_EastWest));
    } else Container::Virtual38(x,y);
}
// Original303350/51B dispatch gating, support0.
// FUNCTION: WON 0x00703350
void MultiListHeader::Virtual30(int x,int y,int button)
{
    if(button==0 && mDragColumn!=-1) mDragColumn=-1;
    else Container::Virtual30(x,y,button);
}
// Original303390/15B reset and base dispatch, support0.
// FUNCTION: WON 0x00703390
void MultiListHeader::Virtual28()
{
    mDragColumn=-1;
    Container::Virtual28();
}
}

namespace WONAPI {
// FUNCTION: WON 0x00702a90
MultiListHeader::MultiListHeader(MultiListArea* area)
{
    SetComponentFlags(0x2000,true);
    mDragColumn=-1;
    mCurSortCol=-1;
    mDrawSortArrow=true;
    SetMultiListArea(area);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00702b30
// (public: virtual void * __thiscall WONAPI::MultiListHeader::`scalar deleting destructor'(unsigned int)    [??_GMultiListHeader@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0071d040
// (public: class std::_Ptrit<struct WONAPI::MultiListHeader::ColumnInfo,int,struct WONAPI::MultiListHeader::ColumnInfo *,struct WONAPI::MultiListHeader::)
// TEMPLATE: WON 0x007238c0
// (public: class std::_Ptrit<struct WONAPI::MultiListHeader::ColumnInfo,int,struct WONAPI::MultiListHeader::ColumnInfo *,struct WONAPI::MultiListHeader::)
// TEMPLATE: WON 0x007249f0
// (protected: struct WONAPI::MultiListHeader::ColumnInfo * __thiscall std::vector<struct WONAPI::MultiListHeader::ColumnInfo,class std::allocator<struct )
// TEMPLATE: WON 0x00724a30
// (void __cdecl std::fill(class std::_Ptrit<struct WONAPI::MultiListHeader::ColumnInfo,int,struct WONAPI::MultiListHeader::ColumnInfo *,struct WONAPI::Mu)
// TEMPLATE: WON 0x00724a60
// (class std::_Ptrit<struct WONAPI::MultiListHeader::ColumnInfo,int,struct WONAPI::MultiListHeader::ColumnInfo *,struct WONAPI::MultiListHeader::ColumnIn)
// TEMPLATE: WON 0x00724aa0
// (struct WONAPI::MultiListHeader::ColumnInfo * __cdecl std::copy(class std::_Ptrit<struct WONAPI::MultiListHeader::ColumnInfo,int,struct WONAPI::MultiLi)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00702b50
// (public: virtual __thiscall WONAPI::MultiListHeader::~MultiListHeader(void)    [??1MultiListHeader@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071cf80
// (public: void __thiscall std::vector<struct WONAPI::MultiListHeader::ColumnInfo,class std::allocator<struct WONAPI::MultiListHeader::ColumnInfo> >::res)
// TEMPLATE: WON 0x00720f70
// (public: void __thiscall std::vector<struct WONAPI::MultiListHeader::ColumnInfo,class std::allocator<struct WONAPI::MultiListHeader::ColumnInfo> >::ins)
// TEMPLATE: WON 0x00721280
// (public: class std::_Ptrit<struct WONAPI::MultiListHeader::ColumnInfo,int,struct WONAPI::MultiListHeader::ColumnInfo *,struct WONAPI::MultiListHeader::)
// TEMPLATE: WON 0x00723780
// (protected: void __thiscall std::vector<struct WONAPI::MultiListHeader::ColumnInfo,class std::allocator<struct WONAPI::MultiListHeader::ColumnInfo> >::)
