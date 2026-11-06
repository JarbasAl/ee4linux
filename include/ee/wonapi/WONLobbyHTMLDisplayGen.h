// Complete native nonvirtual HTMLDisplayGencc value owner; original constructor337f50.
// Member types/order and native implicit cleanup proven; no fabricated lifetime provider.
#ifndef EE_WONAPI_WONLOBBYHTMLDISPLAYGEN_H
#define EE_WONAPI_WONLOBBYHTMLDISPLAYGEN_H
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyLineLayout.h"
#include "ee/wonapi/WONLobbyHTMLTag.h"
#include "ee/wonapi/WONLobbyTableElement.h"
namespace WONAPI {
class HTMLDisplayGen {
protected:
    SmartPtr<HTMLDocument> mDocument; //00
    const HTMLTagList* mTagList; //04 borrowed
    HTMLTagList::const_iterator mTagListItr,mTagListEnd; //08,0c
    HTMLTextTag* mNextTextTag; //10 borrowed
    SmartPtr<Table> mDisplayContainer; //14
    SmartPtr<MultiLineElement> mDisplay; //18
    Window* mWindow; //1c borrowed
    LineBuilder mCurLine; //20..33 actual14, retained line30
    SmartPtr<LineSegment> mPrevSegment; //34
    bool mCurLineEmpty,mPrevLineEmpty,mCanBreak,mLastWasSpace; //38..3b
    bool mInPre; //3c; natural3d..3f
    int mNoBreak; //40
    bool mInParagraph; //44; natural45..47
    SmartPtr<ComboBox> mCurCombo; //48
    bool mNoBackground; //4c; natural4d..4f
    HTMLFontSize mFontSize; //50, genuine shared SDK enum
    SmartPtr<TableElement> mCurTable; //54
    HTMLAlign mCurTableAlign; //58
    bool mDone,mInTable,mParsingTableData,mInAnchor; //5c..5f
    SmartPtr<TextLinkParam> mCurLinkParam; //60
    struct FontInfo {
        std::string mFace; //00..0f
        int mSize,mColor; //10,14
        bool mIsMonoSpace; //18; natural19..1b, full1c
        FontInfo() {}
        FontInfo(const std::string& face,int size,int color,bool mono)
            :mFace(face),mSize(size),mColor(color),mIsMonoSpace(mono) {}
    };
    typedef std::vector<FontInfo> FontStack;
    FontStack mFontStack; //64..73, real allocator64/data68/end6c/cap70, stride1c
    SmartPtr<Font> mCurFont; //74
    int mCurColor; //78
    int mBoldCount,mItalicCount,mUnderlineCount; //7c,80,84
    int mDefaultLinkColor,mDefaultALinkColor,mDefaultTextColor; //88,8c,90
    std::string mDefaultFontFace; //94..a3
    HTMLAlign mDefaultAlign; //a4
    typedef std::vector<HTMLAlign> AlignStack;
    AlignStack mAlignStack; //a8..b7, real allocatora8/dataac/endb0/capb4, stride4
    int mLeftMargin,mRightMargin,mTopMargin,mBottomMargin; //b8,bc,c0,c4
    int mListDepth; //c8, fullcc
    HTMLTag* GetNextTag(); //338270 support0
    void CalcLineEmpty(int,bool); //338290
    void AddDisplaySegment(LineSegment*,int=0); //3382d0
    void AddNonDisplaySegment(LineSegment*,int=0); //338430 support0
    void AddEmptySegment(LineSegment*,int=0); //338460 support0
    void ClearAccumulatedSegment(int=0); //3384a0
    int GetFontStyle(); //3386f0
    bool GetIsFontMonospace(); //338720
    void CalcFont(); //338750
    void PushFont(const char*,int,int,bool=false); //338960
    void PopFont(); //338b30
    HTMLAlign GetCurAlign(); //338ba0
    void GenerateTableData(HTMLDisplayGen&,MultiLineElement*,bool,HTMLAlign); //33c250
    int HandleBeginGlueWord(const GUIString&); //339290/687
    inline void HandlePreSpace(int,GUIString&);
    void HandleTextTag(HTMLTextTag*); //3398c0/951
    TextLineSegment* GetTextLineSegment(const GUIString&,int,Font*); //339540 externalrawproviderRET0c
    HTMLTextTag* GetNextTextTag(); //339d20 externalboundedlookahead
    void GenerateBase(); //actual82B33c5b0
    void HandleTag(HTMLTag*); //actualoriginal33c020 dispatchprovider, external atthisstage
    void HandleTableData(HTMLTableDataTag*); //33b1d0/709
    void HandleEndTableData(); //33b660/14 support0
    void PushAlign(HTMLAlign,bool=false); //338bd0/160 actualexternalSDKproviderRET8; no newAlignLineSegment constructionauthority
    void HandleHeading(HTMLHeadingTag*); //339e80/121 RET4
    void HandlePre(HTMLTag*); //33a220/87 RET4
    void HandleCenter(HTMLTag*); //33a280/51 RET4 support0
    void HandleModifyFontStyle(HTMLTag*,int); //33a2c0/64 RET8 support0
    void HandleModifyFont(HTMLTag*,const char*,int,int,bool); //33a300/53 RET14 support0
    void HandleFont(HTMLFontTag*); //33a340/344 RET4
    void EndParagraph(); //33a4a0/21 RET0 support0
    void HandleBlock(HTMLBlockTag*); //33a4c0/121 RET4
    void HandleInput(HTMLInputTag*); //33b8b0/1473 actual RET4
    void HandleOption(HTMLTag*); //33b6e0/449 actual RET4
    void StartFreshLine(); //338560/70 actual external RET0; no new LineBreak18 construction authority
    void StartFreshParagraph(); //3385c0/137 actual external RET0; no new LineBreak18 construction authority
    void PopAlign(); //338c80/174 actual external RET0; no new AlignLineSegment construction authority
    void AddLineBreak(int=0); //338650/160 support0, genuine SDK default
    void HandleList(HTMLListTag*); //33ac40/247
    bool FindBody(); //3391c0/196 support0
    void HandleBody(HTMLBodyTag*); //338d30/962
    void HandleLineBreak(HTMLLineBreakTag*); //339d50/38 support0
    void HandleTable(HTMLTableTag*); //33afc0/414
    void HandleTableRow(HTMLTableRowTag*); //33b160/112 support0
    void HandleImage(HTMLImageTag*); //33a540/925 code,948 physical
    void HandleAnchor(HTMLAnchorTag*); //339f00/437 RET4
    void HandleHorizontalRule(HTMLHorizontalRuleTag*); //339d80/203 support0 RET4
    void HandleListItem(HTMLListItemTag*); //33ad40/379 RET4
    void HandleGUIComponent(HTMLGUIComponentTag*); //33b670/106 support0 RET4
public:
    HTMLDisplayGen(); //337f50 genuine155B constructor, authored as support0 in flow family
    // Keep implicit natural destructor: native33b4a0 has no own-vptr store and only real member cleanup.
    void SetNoBackground(bool value) { mNoBackground=value; } //actualSDKinline/native Browser3fc644
    void SetFontSize(HTMLFontSize size) { mFontSize=size; } //actualSDKinline/native Browser3fc63a
    void Generate(HTMLDocument*,MultiLineElement*,Table*); //33c430 originalexternal379B/RET0c
    static void SetGlobalDefaultFontFace(const std::string&); //3380e0 existing external static API
};
}
#endif
