// WONGUI multicolumn lookup on original-owned receivers; no lifetime authority.
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
// FUNCTION: WON 0x007039c0
int MultiListArea::GetColumnAt(int x)
{
    for(int i=0;i<mColumnInfoVector.size();++i)
        if(mColumnInfoVector[i].mx+mColumnInfoVector[i].mWidth>=x) return i;
    return (int)mColumnInfoVector.size()-1;
}
// FUNCTION: WON 0x00703960
int MultiListArea::GetColumnWidth(int column)
{
    if(column<0 || column>=mColumnInfoVector.size()) return 0;
    return mColumnInfoVector[column].mWidth;
}
// FUNCTION: WON 0x00703a60
int MultiListArea::GetColumnPos(int column)
{
    if(column<0 || column>=mColumnInfoVector.size()) return -1;
    return mColumnInfoVector[column].mx;
}
}

namespace WONAPI {

// FUNCTION: WON 0x007037E0
void MultiListArea::SetColumnWidth(int theCol, int theWidth, bool fromSizeChanged)
{
	if(theCol < mColumnInfoVector.size())
	{
		if(theWidth<0)
			theWidth = 0;

		ColumnInfo &aCol = mColumnInfoVector[theCol];
		if(!fromSizeChanged && aCol.mSizeMode==2 && aCol.mPartnerColumn>=0 && aCol.mPartnerColumn<mColumnInfoVector.size())
		{
			ColumnInfo &aCol2 = mColumnInfoVector[aCol.mPartnerColumn];
			int anOldWidth = aCol.mWidth;
			int aDelta = theWidth - anOldWidth;
			if(aDelta>aCol2.mWidth)
				aDelta = aCol2.mWidth;

			aCol.mWidth += aDelta;
			aCol2.mWidth -= aDelta;
		}
		else
		{
			int anOldWidth = aCol.mWidth;

			int aDelta = theWidth - anOldWidth;
			if(mMaxTotalColumnWidth>0 && aDelta>0 && mTotalColumnWidth+aDelta>mMaxTotalColumnWidth)
			{
				aDelta = mMaxTotalColumnWidth - mTotalColumnWidth;
				if(aDelta<0)
					aDelta = 0;
			}

			aCol.mWidth += aDelta;
			mTotalColumnWidth += aDelta;
		}

		int aTotalWidth = 0;
		for(int i=0; i<mColumnInfoVector.size(); i++)
		{
			ScrollAreaViewport *aContainer = mColumnInfoVector[i].mContainer.get();
			if(aContainer!=NULL)
				aContainer->Fn2F4B70(aTotalWidth,0);

			mColumnInfoVector[i].mx = aTotalWidth;
			aTotalWidth += mColumnInfoVector[i].mWidth;
		}
		
		NotifyListHeader();
		VirtualCC(false);
	}
}

// FUNCTION: WON 0x00703AC0
void MultiListArea::SetSortColumn(int theCol, ListSortType theType)
{
	if(theCol<0 || theCol>=mColumnInfoVector.size())
		return;

	mSortColumnPrev = mSortColumn;
	mSortColumn = theCol;
	mSortType = theType;
	mSortColFunc = mColumnInfoVector[theCol].mSortFunc;
	if(mListHeader!=NULL)
		mListHeader->SetSortColumn(theCol,theType);

	Sort();
}

// Necessary support0: SetSortColumnFunc
// FUNCTION: WON 0x00703b50
void MultiListArea::SetSortColumnFunc(int theCol, SortFunc theFunc)
{
	if(theCol<0 || theCol>=mColumnInfoVector.size())
		return;

	mColumnInfoVector[theCol].mSortFunc = theFunc;
	if(mSortColumn==theCol)
		mSortColFunc = theFunc;
}

// Necessary support0: AdjustPosition
// FUNCTION: WON 0x00703bb0
void MultiListArea::VirtualCC(bool becauseOfScrollbar)
{
	mL0A4 = mTotalColumnWidth;
	ListArea::VirtualCC(becauseOfScrollbar);
	NotifyListHeader();
}

// FUNCTION: WON 0x00703BE0
void MultiListArea::Virtual08()
{
	int oldWidth = mScrollArea.width;

	ListArea::Virtual08();
	if (mDynamicSizeColumn >= 0 && mDynamicSizeColumn < (int)mColumnInfoVector.size())
	{
		int aSizeDelta = mScrollArea.width - oldWidth;
		if(mTotalColumnWidth + aSizeDelta > mScrollArea.width)
		{
			aSizeDelta = mScrollArea.width - mTotalColumnWidth;
			if(aSizeDelta<0)
				aSizeDelta = 0;
		}

		if(aSizeDelta!=0)
			SetColumnWidth(mDynamicSizeColumn, mColumnInfoVector[mDynamicSizeColumn].mWidth + aSizeDelta,true);
	}
}

// FUNCTION: WON 0x00703C70
void MultiListArea::VirtualD4(Graphics &g)
{
	int anXPos = 0;
	

	// Give chance to paint over all columns
	mColumnPaintContext = -1;
	mL12C = mTotalColumnWidth;
	ListArea::VirtualD4(g);

	// Now paint each column
	for(int i=0; i<mColumnInfoVector.size(); i++)
	{		
		ColumnInfo &aCol = mColumnInfoVector[i];
		mColumnPaintContext = i;
		mL12C = aCol.mWidth-mColumnBorderWidth;

		g.PushClipRect(anXPos,mVertOffset,mL12C,mScrollArea.height);
		g.Translate(anXPos,0);
		ListArea::VirtualD4(g);
		g.Translate(-anXPos,0);
		g.PopClipRect();


	
		anXPos += aCol.mWidth;
	}
}

// FUNCTION: WON 0x00703E50
MultiListItem* MultiListArea::UpdateItem(MultiListItem *theItem)
{
	ListArea::Fn2FC730(theItem);
	if(mAutoGrowColumns)
	{
		for(int i=0; i<mColumnInfoVector.size(); i++)
		{
			ListItem *anItem = theItem->GetItem(i);
			if(anItem!=NULL && anItem->mL10>0)
			{
				int aWidth = mColumnInfoVector[i].mWidth;
				if(anItem->mL10 + mColumnBorderWidth > aWidth)
					SetColumnWidth(i,anItem->mL10 + mColumnBorderWidth,false);
			}
		}
	}
	return theItem;
}

// FUNCTION: WON 0x00703EE0
MultiListItem* MultiListArea::SetCell(int theRow, int theCol, ListItem *theCellItem)
{
	if(theCol >= (int)mColumnInfoVector.size())
		return NULL;

	MultiListItem* aRow = (MultiListItem*)ListArea::GetItem(theRow);
	if(aRow==NULL)
		return NULL;

	aRow->SetItem(theCellItem, theCol);
	UpdateItem(aRow);
	return aRow;
}

}

namespace WONAPI {
// FUNCTION: WON 0x00703600
void MultiListArea::SetNumColumns(int numColumns)
{
    mColumnInfoVector.resize(numColumns);
    mTotalColumnWidth=0;
    for(int i=0;i<mColumnInfoVector.size();++i) {
        ScrollAreaViewport* container=mColumnInfoVector[i].mContainer.get();
        if(container==NULL) {
            container=new ScrollAreaViewport(this);
            mColumnInfoVector[i].mContainer=container;
            GetViewport()->Fn2F6A60(container);
        }
        mTotalColumnWidth+=mColumnInfoVector[i].mWidth;
    }
    NotifyListHeader();
    VirtualCC(false);
}
}

namespace WONAPI {
// FUNCTION: WON 0x00703f60
MultiListItem* MultiListArea::SetString(int row,int column,const GUIString& text,Image* image)
{
    SmartPtr<TextListItem> item=new TextListItem;
    item->mText=text;
    item->mL1C=mTextColor;
    item->mFont=mFont;
    item->mImage=image;
    return SetCell(row,column,item.get());
}
// Original3035f0/16B conditional notification support0.
// FUNCTION: WON 0x007035f0
void MultiListArea::NotifyListHeader()
{
    if(mListHeader!=NULL) mListHeader->NotifyColumnChange();
}
// Original303e30/22B base insertion/original argument result support0.
// FUNCTION: WON 0x00703e30
MultiListItem* MultiListArea::InsertItem(MultiListItem* item,int position)
{
    ListArea::InsertItem(item,position);
    return item;
}
// Original303790/79B guarded one-byte field update support0.
// FUNCTION: WON 0x00703790
void MultiListArea::SetColumnHAlign(int column,HorzAlign align)
{
    if(column<0 || column>=mColumnInfoVector.size()) return;
    mColumnInfoVector[column].mHAlign=static_cast<unsigned char>(align);
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK multicolumn construction/size-mode policy.
void MultiListArea::SetColumnSizeMode(int theCol, unsigned char theMode)
{
	if(theCol<0 || theCol >= mColumnInfoVector.size())
		return;
	
	int anOldMode = mColumnInfoVector[theCol].mSizeMode;
	mColumnInfoVector[theCol].mSizeMode = theMode;	

	if(theMode!=2 && anOldMode!=2)
		return;

	int aPrevPartner = -1;
	for(int i=0; i<mColumnInfoVector.size(); i++)
	{
		ColumnInfo &anInfo = mColumnInfoVector[i];
		if(anInfo.mSizeMode==2)
		{
			if(aPrevPartner>=0)
				mColumnInfoVector[aPrevPartner].mPartnerColumn = i;

			aPrevPartner = i;
		}
	}
}

MultiListItem* MultiListArea::InsertRow(int theRow)
{
	return InsertItem(new MultiListItem, theRow);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00706a40
// (public: virtual void * __thiscall WONAPI::ScrollAreaViewport::`scalar deleting destructor'(unsigned int)    [??_GScrollAreaViewport@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00706a60
// (public: virtual __thiscall WONAPI::ScrollAreaViewport::~ScrollAreaViewport(void)    [??1ScrollAreaViewport@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x00724b10
// (void __cdecl std::fill(class std::_Ptrit<struct WONAPI::MultiListArea::ColumnInfo,int,struct WONAPI::MultiListArea::ColumnInfo *,struct WONAPI::MultiL)
// TEMPLATE: WON 0x00724ba0
// (class std::_Ptrit<struct WONAPI::MultiListArea::ColumnInfo,int,struct WONAPI::MultiListArea::ColumnInfo *,struct WONAPI::MultiListArea::ColumnInfo &,s)
// TEMPLATE: WON 0x00724c40
// (struct WONAPI::MultiListArea::ColumnInfo * __cdecl std::copy(class std::_Ptrit<struct WONAPI::MultiListArea::ColumnInfo,int,struct WONAPI::MultiListAr)
// TEMPLATE: WON 0x00724f10
// (void __cdecl std::_Construct(struct WONAPI::MultiListArea::ColumnInfo *,struct WONAPI::MultiListArea::ColumnInfo const &)    [?_Construct@std@@YAXPAUC)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00703d60
// (public: virtual bool __thiscall WONAPI::ListItem::Virtual1C(int,int)    [?Virtual1C@ListItem@WONAPI@@UAE_NHH@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00703d90
// (public: virtual void * __thiscall WONAPI::MultiListItem::`scalar deleting destructor'(unsigned int)    [??_GMultiListItem@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00724ad0
// (protected: struct WONAPI::MultiListArea::ColumnInfo * __thiscall std::vector<struct WONAPI::MultiListArea::ColumnInfo,class std::allocator<struct WONA)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00703db0
// (public: virtual __thiscall WONAPI::MultiListItem::~MultiListItem(void))
// TEMPLATE: WON 0x0071d060
// (public: void __thiscall std::vector<struct WONAPI::MultiListArea::ColumnInfo,class std::allocator<struct WONAPI::MultiListArea::ColumnInfo> >::resize()
// TEMPLATE: WON 0x007212e0
// (public: void __thiscall std::vector<struct WONAPI::MultiListArea::ColumnInfo,class std::allocator<struct WONAPI::MultiListArea::ColumnInfo> >::insert()
// TEMPLATE: WON 0x007217a0
// (public: class std::_Ptrit<struct WONAPI::MultiListArea::ColumnInfo,int,struct WONAPI::MultiListArea::ColumnInfo *,struct WONAPI::MultiListArea::Column)
// TEMPLATE: WON 0x007238e0
// (public: class std::_Ptrit<struct WONAPI::MultiListArea::ColumnInfo,int,struct WONAPI::MultiListArea::ColumnInfo *,struct WONAPI::MultiListArea::Column)
// TEMPLATE: WON 0x00725160
// (public: class std::_Ptrit<struct WONAPI::MultiListArea::ColumnInfo,int,struct WONAPI::MultiListArea::ColumnInfo *,struct WONAPI::MultiListArea::Column)
