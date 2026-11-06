#ifndef EE_WONAPI_WONLOBBYTABLEELEMENT_H
#define EE_WONAPI_WONLOBBYTABLEELEMENT_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Actual complete8c original-owned table record, vtable45014c.
// Genuine default constructor30d7f0 is implemented in TextLayoutConstructors.cpp;
// original external destructor/copy behavior remains, no unrelated allocation authority.
// Actual ColumnInfo18/RowInfo24 member-vector growth30d9c0/30dd20 remains proved;
// CellInfo creation and external TableElement lifetime stay original-owned.
class TableElement : public LineSegment {
public:
    TableElement(); //30d7f0 originalexternal165B complete8c constructor; actual allocation33b0ab
    struct ColumnInfo {
        int mx,mWeight,mWidth,mMinWidth,mMaxWidth,mColumnNum; //18
        ColumnInfo(int number=0) : mWeight(0),mWidth(0),mMinWidth(0),mMaxWidth(0),mColumnNum(number) {} //actual30d9fa..16; mx intentionally untouched
        void SetWidthToMin() { mWidth=mMinWidth; }
        void SetWidthToMax() { mWidth=mMaxWidth; }
    };
    struct CellInfo {
        SmartPtr<MultiLineElement> mContents;
        int mColumnSpan,mRowSpan,mBGColor,mWidth,mHeight;
        char mHAlign,mVAlign; bool mNoWrap; //1c
        CellInfo() : mContents(new MultiLineElement),mColumnSpan(1),mRowSpan(1),mBGColor(-1),mWidth(0),mHeight(0),mHAlign(-1),mVAlign(-1),mNoWrap(false) {} //genuine SDK/native30dc3d..30dca3; complete1c
    };
    typedef std::vector<CellInfo> CellRow;
    struct CellInfoEx {
        CellInfo* mCellInfo;
        int mBorderX,mBorderY,mBorderWidth,mBorderHeight,mCellX,mCellY; //1c POD
    };
    struct RowInfo {
        int my,mHeight,mRowNum,mBGColor;
        char mHAlign,mVAlign;
        CellRow mRow; //14; full24
        RowInfo(int number=0) : mHeight(0),mRowNum(number),mBGColor(-1),mHAlign(0),mVAlign(1) {} //actual30dd9a..c3; my intentionally untouched
    };
    typedef std::vector<ColumnInfo> ColumnVector;
    typedef std::vector<RowInfo> RowVector;
    struct RowSearchPred { bool operator()(const RowInfo& a,const RowInfo& b) { return a.my<b.my; } };
    virtual int GetWidth() { return mWidth; } //30d8a0 support0
    virtual int GetMinWidth(); //30eca0 support0 cached calculation
    virtual int GetMaxWidth(); //30ecc0 support0 cached calculation
    virtual int GetHeight() { return mHeight; } //30d8b0 support0
    virtual int GetEndSelIndex() { return mEndSelIndex; }
    virtual void GetSelText(GUIString&,ElementSelectionInfo&);
    virtual bool FindStr(ElementFindInfo&);
    virtual void SetSelIndex(); //30f500 external, deferred
    virtual LineSegment* GetSegmentAt(int&,int&,bool);
    virtual void Paint(Graphics&,int,int,ElementPaintInfo&);
    virtual void SetMaxWidth(int);
    virtual void SetViewport(ScrollAreaViewport*,int,int);
    CellInfo* GetCellInfoAt(RowInfo&,int);
    CellInfo* GetCellInfoAt(int,int);
    MultiLineElement* GetCellAt(int,int);
    MultiLineElement* GetCellAt(RowInfo&,int);
    int FindRowAt(int);
    bool GetCellInfoEx(CellInfoEx&,int,int,RowInfo&,bool=false,bool=false);
    void PaintCell(Graphics&,int,int,int,int,RowInfo&,ElementPaintInfo&,bool);
    void CalcMinMaxWidths();
    void CalcRowHeights();
    void CalcColumnWidths(int);
    void StartTraversal(); //30dfe0/17 actualexternalRET0
    void SetRowBGColor(int); //30e240/63 actualexternalRET4
    void SetRowAlign(int,int); //30e280/72 actualexternalRET8
    void NextRow(); //30e000 support0
    CellInfo* NextColumn(); //30e020
    void SetSize(int,int); //genuine SDK fixed-grid reset; SOURCE_ONLY_NO_NATIVE_ID
    void Rewind(); //genuine SDK cell cursor reset; SOURCE_ONLY_NO_NATIVE_ID
    void AddSegment(LineSegment*,unsigned short=0); //genuine SDK current-cell delegation; SOURCE_ONLY_NO_NATIVE_ID
    MultiLineElement* NextCell(); //genuine SDK cursor traversal; SOURCE_ONLY_NO_NATIVE_ID
    void SetCellBGColor(int); //genuine SDK; SOURCE_ONLY_NO_NATIVE_ID
    void SetCellAlign(int,int); //genuine SDK; SOURCE_ONLY_NO_NATIVE_ID
    HorzAlign GetCurRowAlign(); //30e070 actual15B rowchar return, external
    void SetColumnWidth(int); //30e2d0 actual70B columnweight update, external
    void SetCellSpan(int,int,bool=false); //30e080
    void EnsureNumCols(RowInfo&,int); //30da90 original cell creation/growth; RET8, no candidate new MultiLineElement
    void EnsureNumCols(int); //30d9c0 actual ColumnInfo growth
    void EnsureNumRows(int); //30dd20 genuine original growth API, external
    ColumnVector mColumnVector; //14..23
    RowVector mRowVector; //24..33
    int mEndSelIndex; //34
    RowInfo mSearchRow; //38..5b
    int mCurRow,mCurCol; MultiLineElement* mCurCell; //5c,60,64
    int mWidth,mHeight,mMinWidth,mMaxWidth; //68..77
    int mCellSpacing,mCellPadding,mBorder,mBGColor,mWidthSpec; //78..8b
};
}
#endif
