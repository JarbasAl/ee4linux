#ifndef EE_WONAPI_WONLOBBYTEXTAREACONFIG_H
#define EE_WONAPI_WONLOBBYTEXTAREACONFIG_H
#include "ee/wonapi/WONLobbyResourceConfig.h"
namespace WONAPI {
// TextBox records use actual target fields/slots. TextSegmentBase2c and
// TextAlignInstruction30 are complete finite owners for TextArea instructions;
// original external base constructor/destructor retain actual lifetime behavior.
class TextArea;
class TextLine;
class TextSegmentBase : public RefCount {
public:
    TextSegmentBase(); //310a30 originalexternal, complete actual2c owner
    virtual int GetLength(); //04
    virtual void CalcDimensions(); //08
    virtual void PerformInstructions(TextArea*); //0c
    virtual void GetSelText(TextArea*,GUIString&); //10
    virtual void Paint(Graphics&,TextArea*); //14
    virtual int CharAt(int); //18
    virtual void Wrap(TextArea*,TextLine*); //1c
    virtual bool IsLink(); //20
    virtual void Pressed(); //24
    virtual bool Released(); //28
    virtual const GUIString& GetLinkParam(); //2c
    virtual TextSegmentBase* GetActual(); //30
    TextLine* mLine; //08 borrowed
    int mAbsStartChar,mAbsEndChar; //0c/10
    int mWidth,mHeight,mAscent,mx,my; //14/18/1c/20/24
    bool mLineBreak; //28, natural29..2b
};
// Complete actual30 instruction: caller312756 new30, base2c plus scalar align2c.
// Genuine implicit constructor calls original TextSegmentBase310a30; natural
// implicit cleanup has no additional owner and shares base normal338c70.
class TextAlignInstruction : public TextSegmentBase {
public:
    virtual void PerformInstructions(TextArea*); //311680 actual slot0c, originalexternal
    HorzAlign mAlign; //2c, complete30
};
// Genuine SDK Source-only complete derived30 width instruction; no target Native identity claimed.
class TextWidthInstruction : public TextSegmentBase {
public:
    TextWidthInstruction(int width=0) :mLineShouldBeAtLeastThisWidth(width) {}
    virtual void PerformInstructions(TextArea*);
    int mLineShouldBeAtLeastThisWidth;
};
// Genuine SDK Source-only complete30 anchor instruction; no target Native identity.
class TextAnchorInstruction : public TextSegmentBase 
{
public:
	GUIString mAnchorName;
	TextAnchorInstruction(const GUIString &theAnchorName) : mAnchorName(theAnchorName) { }
	virtual void PerformInstructions(TextArea *theTextArea);	
};
// Complete native TextSegment38: caller313c9e new38, ctor310c40, normaldtor310ce0.
class TextSegment : public TextSegmentBase {
public:
    TextSegment(); //310c40 originalexternal
    virtual int GetLength() { return mText.length(); } //genuine SDK inline, native310cb0..310cbc12B, existing slot04
    virtual void CalcDimensions(); //310d70
    virtual void GetSelText(TextArea*,GUIString&); //310db0
    void Paint(Graphics&,TextArea*,int,int,int,int,int); //311200
    virtual void Paint(Graphics&,TextArea*); //311330
    int CharAt(int,int,int); //311360
    virtual int CharAt(int); //3113c0
    virtual void Wrap(TextArea*,TextLine*); //310fb0
    GUIString mText; //2c realowner
    int mColor; //30
    SmartPtr<Font> mFont; //34 realowner
};
// Genuine SDK TextLink48: TextSegment38 + ownedGUIString38 + bool3c + DWORD40/44.
// Native3114f0 andVT450354 agree; destructor remains implicit, native311590.
class TextLink : public TextSegment {
public:
    TextLink();
    virtual bool IsLink();
    virtual void Pressed();
    virtual bool Released();
    virtual const GUIString& GetLinkParam();
    GUIString mLinkParam;
    bool mPressed;
    unsigned long mDownColor,mNormalColor;
};
// Complete native TextSegmentPiece38: Wrap3110b9/311191 new38, ctor3113f0.
// All added fields borrowed/scalar; implicit destructor is the true shared base cleanup338c70.
class TextSegmentPiece : public TextSegmentBase {
public:
    TextSegmentPiece(TextSegment*,int,int,int); //3113f0 originalexternal actual4args
    virtual void Paint(Graphics&,TextArea*); //3114a0
    virtual int CharAt(int); //3114d0
    virtual bool IsLink(); //311430 trueexternal forwarded slots
    virtual void Pressed(); //311440
    virtual bool Released(); //311450
    virtual const GUIString& GetLinkParam(); //311460
    virtual TextSegmentBase* GetActual(); //311470
    TextSegment* mSeg; //2c borrowed, no retention
    int GetStartChar() { return mStartChar; } //actualSDKinline, support0
    int GetEndChar() { return mEndChar; } //actualSDKinline, support0
    int mStartChar,mEndChar; //30/34
};
class TextLine : public RefCount {
public:
    TextLine(); //311690 external actual2c ctor; complete owner fields/list already declared
    virtual ~TextLine(); //311730 originalexternal, native45038c one-slot vtable
    int mx,my,mWidth,mHeight,mBaseline,mDescent; //08..1c
    typedef std::list<SmartPtr<TextSegmentBase> > SegmentList;
    SegmentList mSegmentList; //20..2b originalexternal lifetime
};
// Complete actual24 event owner: native313753 new24, RTTI4504ac two slots.
// Retains originalactual segment throughSP20; realComponentEvent20 sender ownership.
class TextLinkActivatedEvent : public ComponentEvent {
public:
    TextLinkActivatedEvent(Component* component,TextSegmentBase* link)
      :ComponentEvent(component,ComponentEvent_TextLinkActivated),mLink(link) {}
    SmartPtr<TextSegmentBase> mLink; //20
};
enum TextFlags {
    TextFlag_WordWrap=0x01,TextFlag_AllowSelection=0x02,TextFlag_ScrollOnBottom=0x04,
    TextFlag_ShowPartialLines=0x08,TextFlag_ScrollFullLine=0x10,TextFlag_DelayWordWrap=0x20
};
// Complete actual TextArea198 fields and55 virtual slots. Native3117c0
// constructor may use natural base/member construction; original311be0 normal
// destructor remains external. No private fields or alternate owner introduced.
class TextArea : public ScrollArea {
public:
    TextArea(); //3117c0 actual complete198 initialization, support0
    virtual ~TextArea(); //311be0 originalexternal
    virtual void Virtual08(); //312030 SizeChanged
    virtual void Virtual24(int,int); //3139f0 MouseEnter
    virtual void Virtual28(); //3138f0 MouseExit
    virtual void Virtual2C(int,int,int); //3135e0 MouseDown
    virtual void Virtual30(int,int,int); //313680 MouseUp originalexternal
    virtual void Virtual34(int,int); //313930 MouseMove
    virtual void Virtual38(int,int); //313a40 MouseDrag
    virtual bool Virtual44(int); //313480 TimerEvent
    virtual bool Virtual50(int); //313c20 KeyChar
    virtual bool Virtual54(int); //313a90 KeyDown
    virtual void Virtual84(bool); //313080 NotifyRootVisibility
    virtual void VirtualCC(bool); //3120f0 AdjustPosition
    virtual void VirtualD4(Graphics&); //3130d0 ScrollPaint
    virtual void VirtualD8(int,int,bool); //3133d0 CheckSel, oneactual appendedslot
    void Reset(bool=false); //311df0
    void RecalcWordWrap(bool); //311fc0
    void AddSegmentPrv(TextSegmentBase*); //3127d0
    TextLine* AddLine(int=0,int=0); //3124e0 actualret8 originalexternal
    void AddSeg(TextSegmentBase*); //312260
    void AlignLine(TextLine*,HorzAlign); //3124a0
    void AddSegment(TextSegmentBase*); //312980
    void EnforceInvariants(); //312b60
    void RemoveSegmentsUpTo(TextSegmentBase*); //312cd0
    bool ShouldScrollOnBottom(); //3128c0
    bool IsLineVisible(TextLine*); //313030
    void FindTopLine(); //313250
    TextSegmentBase* FindSegment(int,int,bool); //313290
    void CopySelection(); //313b60
    void EnforceMaxChars(); //312f70 originalexternal
    void EnforceMaxLines(); //312fd0 originalexternal
    void AddImage(Image*,const GUIString&,bool=false); //genuine SDK retained inline-image segment
    void AddSegment(const GUIString&,bool=false); //313c60 actual ret8 originalexternal
    void AddSegment(const GUIString&,int,bool=false); //313c80 genuine SDK overload, actualRET0c
    void AddSegment(const GUIString&,int,Font*,bool=false); //313da0 actual SDK overload, nativeRET10 external
    void LineBreak(int=0); //312690 actual SDK/RET4, originalexternal
    void AddLink(const GUIString&,const GUIString&,bool=false);
    void AddAnchor(const GUIString&,int);
    bool GotoAnchor(const GUIString&);
    void Clear() { Reset(); }
    void SetFont(Font* font) { mFont=font; }
    void SetWrapIndentSize(int size) { mWrapIndentSize=size; }
    int GetNumLines() { return mLineVector.size(); }
    void AddFormatedText(const GUIString&,bool=false); //313ec0 actual SDK spelling/ret8
    void SetTextFlags(int,bool); //313ff0 actual ret8
    void SetMaxChars(int); //314090 actual ret4
    void SetMaxLines(int); //3140c0 actual ret4
    void SetLineAlignment(HorzAlign); //312730 actual ret4 originalexternal
    typedef std::list<SmartPtr<TextSegmentBase> > SegmentList;
    typedef std::deque<SmartPtr<TextLine> > LineVector;
    LineVector::iterator FindLineAt(int); //3131b0
    SegmentList mSegmentList; //110..11b originalowned list12
    LineVector mLineVector; //11c..12f originalowned deque20
    int mTopLineVectorPos,mMaxChars,mMaxLines,mNumLines; //130/134/138/13c
    bool mNeedRecalcWordWrap; //140, natural141..143
    int mDelayWordWrapTimeAccum; //144
    TextSegmentBase *mCurLink,*mLastFindSeg; //148/14c borrowed
    GUIString mLinkParam; //150 originalowner
    bool mSelecting,mBackwardsSel,mHasBeenVisible; //154/155/156, natural157
    int mSelStartChar,mSelEndChar; //158/15c
    SmartPtr<TextLine> mSearchLine; //160 originalowned searchitem
    int mTextFlags; //164 actual flags method313ff0
    bool TextFlagSet(int theFlag) { return mTextFlags&theFlag?true:false; }
    SmartPtr<Font> mFont,mLinkFont; //168/16c retained originalowned fields
    int mTextColor,mLinkColor; //170/174
    SelectionColor mSelColor; //178..183 actual12-byte value
    int mWrapIndentSize; //184
    HorzAlign mLineAlign; //188
    typedef std::map<GUIString,int> AnchorMap;
    AnchorMap mAnchorMap; //18c..197 originalowned actualmap12
};
class TextAreaConfig : public ScrollAreaConfig {
public:
    static ComponentConfig* CfgFactory(); //39b4d0 genuine SDK static API, original external provider only
    static Component* CompFactory() { return new TextArea; } //genuine SDK inline component factory, native39b470/90
    virtual bool Virtual24(const std::string&); //395990
    virtual void Virtual34(Component*); //38f860 original external override
    TextArea* mTextArea; //9c, borrowed prefixa0 only
};
}
#endif
