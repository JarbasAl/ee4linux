// Native HTML segment accumulation and line-state transitions on original-owned receivers.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <ctype.h>
#include <cctype>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
#include "ee/wonapi/WONLobbyHTMLAnchorSegment.h"
#include "ee/wonapi/WONLobbyHTMLSegments.h"
#include "ee/wonapi/WONLobbyHTMLImageSegments.h"
#include "ee/wonapi/WONLobbyHTMLElementSegments.h"
#include "ee/wonapi/WONLobbyAnimationComponent.h"
#include "ee/wonapi/WONLobbyImageButton.h"
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
#include "ee/wonapi/WONLobbyMSBorderPainters.h"
#include "ee/wonapi/WONLobbyTextLinkSegment.h"
#include "LostDebug.h"
namespace WONAPI {
inline int safe_isspace(unsigned long ch) { return ch<256?isspace(ch):0; }
inline void HTMLDisplayGen::HandlePreSpace(int ch,GUIString& text)
{
    if(ch=='\n')
    {
        AddDisplaySegment(GetTextLineSegment(text,mCurColor,mCurFont.get()),8);
        ClearAccumulatedSegment(8);
        text.erase();
    }
    else text.append(L' ');
}
// FUNCTION: WON 0x00737f50
// (??0HTMLDisplayGen@WONAPI@@QAE@XZ)
// FUNCTION: WON 0x00737ff0
// (_$E74)
// FUNCTION: WON 0x00738080
// (_$E73)
// FUNCTION: WON 0x00738090
// (_$E72)
static std::string gGlobalDefaultFontFace("Times New Roman");
// FUNCTION: WON 0x007380e0
void HTMLDisplayGen::SetGlobalDefaultFontFace(const std::string& face)
{
    gGlobalDefaultFontFace=face;
}
// FUNCTION: WON 0x00738270
HTMLTag* HTMLDisplayGen::GetNextTag()
{
    if(mTagListItr==mTagListEnd) return NULL;
    return (mTagListItr++)->get();
}
// FUNCTION: WON 0x00738290
void HTMLDisplayGen::CalcLineEmpty(int flags,bool hasContent)
{
    if(flags&4)
    {
        mPrevLineEmpty=mCurLineEmpty;
        mCurLineEmpty=true;
        mLastWasSpace=true;
    }
    if(hasContent) mCurLineEmpty=false;
    if(flags&8)
    {
        mPrevLineEmpty=mCurLineEmpty;
        mCurLineEmpty=true;
        mLastWasSpace=true;
    }
}
// FUNCTION: WON 0x007382d0
void HTMLDisplayGen::AddDisplaySegment(LineSegment* segment,int flags)
{
    if(mCanBreak)
    {
        ClearAccumulatedSegment();
        if(flags!=0) mDisplay->AddSegment(segment,(unsigned short)flags);
        else mPrevSegment=segment;
        CalcLineEmpty(flags,true);
    }
    else
    {
        if(mCurLine.mLine.get()==NULL)
        {
            mCurLine.StartBuild(new LineElement);
            mCurLine.mLine->mIsGlueLine=true;
        }
        if(mPrevSegment.get()!=NULL) mCurLine.AddSegment(mPrevSegment.get());
        mPrevSegment=segment;
        mCurLineEmpty=false;
    }
}
// FUNCTION: WON 0x00738430
void HTMLDisplayGen::AddNonDisplaySegment(LineSegment* segment,int flags)
{
    ClearAccumulatedSegment();
    mDisplay->AddSegment(segment,(unsigned short)flags);
    CalcLineEmpty(flags,false);
}
// FUNCTION: WON 0x00738460
void HTMLDisplayGen::AddEmptySegment(LineSegment* segment,int flags)
{
    ClearAccumulatedSegment();
    mDisplay->AddSegment(segment,(unsigned short)flags);
    if(!mCurLineEmpty) CalcLineEmpty(flags,false);
}
// FUNCTION: WON 0x007384a0
void HTMLDisplayGen::ClearAccumulatedSegment(int flags)
{
    mCanBreak=true;
    if(mPrevSegment.get()!=NULL)
    {
        if(!mCurLine.IsEmpty())

        {
            mCurLine.AddSegment(mPrevSegment.get());
            mCurLine.FinishBuild();
            mDisplay->AddSegment(mCurLine.mLine.get(),(unsigned short)flags);
            mCurLine.mLine=NULL;
        }
        else mDisplay->AddSegment(mPrevSegment.get(),(unsigned short)flags);
        CalcLineEmpty(flags,true);
        mPrevSegment=NULL;
    }
}

// Native337f50/155B: actual default member initialization plus two scalar defaults; support0.
HTMLDisplayGen::HTMLDisplayGen()
    :mTagListItr(NULL),mTagListEnd(NULL)
{
    mNoBackground=false;
    mFontSize=HTMLFontSize_Medium;
}
// FUNCTION: WON 0x00738560
void HTMLDisplayGen::StartFreshLine()
{
    if(!mCurLineEmpty)
        AddNonDisplaySegment(new LineBreakSegment(0),LineSegmentFlag_PostLineBreak);
}
// FUNCTION: WON 0x007385c0
void HTMLDisplayGen::StartFreshParagraph()
{
    if(!mCurLineEmpty)
        AddNonDisplaySegment(new LineBreakSegment(20),LineSegmentFlag_PreLineBreak|LineSegmentFlag_PostLineBreak);
    else if(!mPrevLineEmpty)
        AddNonDisplaySegment(new LineBreakSegment(20),LineSegmentFlag_PostLineBreak);
}
// FUNCTION: WON 0x00738650
void HTMLDisplayGen::AddLineBreak(int extraFlags)
{
    if(!mCurLineEmpty)
        AddNonDisplaySegment(new LineBreakSegment(0),extraFlags|LineSegmentFlag_PostLineBreak);
    else
        AddNonDisplaySegment(new LineBreakSegment(20),extraFlags|LineSegmentFlag_PostLineBreak);
}
// FUNCTION: WON 0x007386f0
int HTMLDisplayGen::GetFontStyle()
{
    int style=0;
    if(mBoldCount) style|=FontStyle_Bold;
    if(mItalicCount) style|=FontStyle_Italic;
    if(mUnderlineCount) style|=FontStyle_Underline;
    return style;
}
// FUNCTION: WON 0x00738720
bool HTMLDisplayGen::GetIsFontMonospace()
{
    return mFontStack.empty()?false:mFontStack.back().mIsMonoSpace;
}
// FUNCTION: WON 0x00738750
void HTMLDisplayGen::CalcFont()
{
    static int FONT_SIZE_TABLE[5][8]={
        {4,6,8,9,10,12,16,24},
        {5,7,9,10,12,16,20,30},
        {6,8,10,12,14,18,24,36},
        {8,10,12,14,16,20,28,42},
        {10,12,14,16,18,24,32,48}
    };
    if(mFontStack.empty()) return;
    FontInfo info=mFontStack.back();
    int size=info.mSize-1;
    if(size<0) size=0;
    else if(size>6) size=6;
    if(!info.mIsMonoSpace) size++;
    int pointSize=FONT_SIZE_TABLE[mFontSize][size];
    FontDescriptor descriptor(info.mFace,GetFontStyle(),pointSize);
    mCurFont=mWindow->GetFont(descriptor);
    if(info.mColor==-1) mCurColor=mDefaultTextColor; else mCurColor=info.mColor;
}
// FUNCTION: WON 0x00738920
// (??1FontInfo@HTMLDisplayGen@WONAPI@@QAE@XZ)
// FUNCTION: WON 0x00738960
void HTMLDisplayGen::PushFont(const char* face,int size,int color,bool mono)
{
    FontInfo info;
    if(!mFontStack.empty()) info=mFontStack.back();
    else
    {
        info.mColor=-1;
        info.mSize=3;
        info.mFace=mDefaultFontFace;
        info.mIsMonoSpace=false;
    }
    if(face!=NULL) info.mFace=face;
    if(size>=1 && size<=7) info.mSize=size;
    if(color!=-1) info.mColor=color;
    info.mIsMonoSpace=mono;
    mFontStack.push_back(info);
    CalcFont();
}
// FUNCTION: WON 0x00738b30
void HTMLDisplayGen::PopFont()
{
    if(mFontStack.size()>1)
    {
        mFontStack.pop_back();
        CalcFont();
    }
}
// FUNCTION: WON 0x00738ba0
HTMLAlign HTMLDisplayGen::GetCurAlign()
{
    return mAlignStack.empty()?mDefaultAlign:mAlignStack.back();
}

// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x00738bd0
void HTMLDisplayGen::PushAlign(HTMLAlign align,bool force)
{
    HTMLAlign oldAlign=GetCurAlign();
    if(align==HTMLAlign_None) align=oldAlign;
    mAlignStack.push_back(align);
    if(oldAlign!=align || force)
        AddEmptySegment(new AlignLineSegment((HorzAlign)align),LineSegmentFlag_PreLineBreak);
}
// FUNCTION: WON 0x00738c70
// (??1MarginLineSegment@WONAPI@@UAE@XZ)
// FUNCTION: WON 0x00738c80
void HTMLDisplayGen::PopAlign()
{
    if(mAlignStack.empty()) return;
    HTMLAlign oldAlign=GetCurAlign();
    mAlignStack.pop_back();
    HTMLAlign newAlign=GetCurAlign();
    if(newAlign!=oldAlign)
        AddEmptySegment(new AlignLineSegment((HorzAlign)newAlign),LineSegmentFlag_PreLineBreak);
}
// FUNCTION: WON 0x00738d30
void HTMLDisplayGen::HandleBody(HTMLBodyTag* tag)
{
    if(tag->IsEnd())
    {
        if(mBottomMargin>0)
            AddNonDisplaySegment(new LineBreakSegment(mBottomMargin),LineSegmentFlag_PreLineBreak|LineSegmentFlag_PostLineBreak);
        mDone=true;
    }
    else
    {
    if(mDisplayContainer.get()!=NULL)
    {
        if(mNoBackground)
        {
            Background aNone(-1);
            mDisplayContainer->SetBackground(aNone);
        }
        else if(tag->mBGImage.get()!=NULL)
        {
            {
                Background anImage(tag->mBGImage.get());
                mDisplayContainer->SetBackground(anImage);
            }
            if(tag->mTextColor<0) mDefaultTextColor=0;
            if(tag->mLinkColor<0) mDefaultLinkColor=0x0000ff;
            if(tag->mALinkColor<0) mDefaultALinkColor=0xff0000;
        }
        else if(tag->mBGColor<0)
            mDisplayContainer->SetBackground(Background());
        else
        {
            Background aColor(tag->mBGColor);
            mDisplayContainer->SetBackground(aColor);
        }
    }
    if(tag->mTextColor>=0) mDefaultTextColor=tag->mTextColor;
    if(tag->mLinkColor>=0) mDefaultLinkColor=tag->mLinkColor;
    if(tag->mALinkColor>=0) mDefaultALinkColor=tag->mALinkColor;
    mFontStack.clear();
    PushFont(mDefaultFontFace.c_str(),3,-1);
    mLeftMargin=tag->mLeftMargin;
    mRightMargin=tag->mRightMargin;
    mTopMargin=tag->mTopMargin;
    mBottomMargin=tag->mBottomMargin;
    if(mLeftMargin>0 || mRightMargin>0)
        AddNonDisplaySegment(new MarginLineSegment(mLeftMargin,mRightMargin));
    if(mTopMargin>0)
        AddNonDisplaySegment(new LineBreakSegment(mTopMargin),LineSegmentFlag_PostLineBreak);
    }
    // LOST-DEBUG: none: exact only
    EE_LOST_DEBUG(1);
}
// FUNCTION: WON 0x00739100
// (??4Background@WONAPI@@QAEAAV01@ABV01@@Z)
// FUNCTION: WON 0x007391c0
bool HTMLDisplayGen::FindBody()
{
    HTMLTag* tag=GetNextTag();
    if(tag==NULL) return false;
    if(tag->GetType()!=HTMLTagType_Body)
    {
        SmartPtr<HTMLBodyTag> body=new HTMLBodyTag;
        HandleTag(body.get());
    }
    --mTagListItr;
    return true;
}
// FUNCTION: WON 0x00739290
int HTMLDisplayGen::HandleBeginGlueWord(const GUIString& oldText)
{
    int i=0;
    GUIString text;
    if(mLastWasSpace && !mInPre)
    {
        for(;i<oldText.length();i++)
            if(!safe_isspace(oldText[i])) break;
        if(i==oldText.length()) return i;
    }
    bool lastWasSpace=false;
    for(;i<oldText.length();i++)
    {
        int ch=oldText[i];
        if(!safe_isspace(ch)) text.append((wchar_t)ch);
        else
        {
            if(i==0)
            {
                ClearAccumulatedSegment();
                break;
            }
            if(mInPre) HandlePreSpace(ch,text);
            else text.append(L' ');
            lastWasSpace=true;
            i++;
            break;
        }
    }
    if(text.length()!=0)
    {
        AddDisplaySegment(GetTextLineSegment(text,mCurColor,mCurFont.get()));
        if(lastWasSpace) ClearAccumulatedSegment();
    }
    mLastWasSpace=lastWasSpace;
    return i;
}
// FUNCTION: WON 0x00739540
TextLineSegment* HTMLDisplayGen::GetTextLineSegment(const GUIString& text,int color,Font* font)
{
    if(mInAnchor)
        return new TextLinkLineSegment(mCurLinkParam.get(),text,color,mDefaultALinkColor,font);
    return new TextLineSegment(text,color,font);
}
// FUNCTION: WON 0x00739740
// (??_GTextLineSegment@WONAPI@@UAEPAXI@Z)
// FUNCTION: WON 0x00739760
// (??1TextLineSegment@WONAPI@@UAE@XZ)
// FUNCTION: WON 0x007397f0
// (??_GTextLinkLineSegment@WONAPI@@UAEPAXI@Z)
// FUNCTION: WON 0x00739810
// (??1TextLinkLineSegment@WONAPI@@UAE@XZ)
// FUNCTION: WON 0x007398c0
void HTMLDisplayGen::HandleTextTag(HTMLTextTag* tag)
{
    const GUIString& oldText=tag->GetText();
    if(oldText.empty()) return;
    GUIString text((unsigned short)oldText.length());
    int i=0,lastWord=0;
    if(!mCanBreak) i=HandleBeginGlueWord(oldText);
    bool lastWasSpace=mLastWasSpace;
    const int length=oldText.length();
    for(;i<length;i++)
    {
        int ch=oldText[i];
        if(lastWasSpace)
        {
            if(!safe_isspace(ch))
            {
                lastWord=text.length();
                text.append((wchar_t)ch);
                lastWasSpace=false;
            }
            else if(mInPre) HandlePreSpace(ch,text);
        }
        else if(safe_isspace(ch))
        {
            if(mInPre) HandlePreSpace(ch,text);
            else text.append(L' ');
            lastWasSpace=true;
        }
        else text.append((wchar_t)ch);
    }
    mLastWasSpace=lastWasSpace;
    if(text.length()==0)
    {
        mCanBreak=lastWasSpace;
        return;
    }
    if(!lastWasSpace)
    {
        HTMLTextTag* next=GetNextTextTag();
        if(next!=NULL && !safe_isspace(next->GetText()[0]))
        {
            GUIString word=text.substr((unsigned short)lastWord);
            text.resize(lastWord);
            if(text.length()!=0)
            {
                AddDisplaySegment(GetTextLineSegment(text,mCurColor,mCurFont.get()));
                ClearAccumulatedSegment();
            }
            if(word.length()!=0) AddDisplaySegment(GetTextLineSegment(word,mCurColor,mCurFont.get()));
            mCanBreak=false;
            return;
        }
    }
    if(text.length()!=0)
    {
        ClearAccumulatedSegment();
        AddDisplaySegment(GetTextLineSegment(text,mCurColor,mCurFont.get()));
    }
    mCanBreak=lastWasSpace;
}
// FUNCTION: WON 0x00739d20
HTMLTextTag* HTMLDisplayGen::GetNextTextTag()
{
    HTMLTagList::const_iterator itr=mTagListItr;
    while(itr!=mTagList->end())
    {
        HTMLTag* tag=itr->get();
        if(tag->GetType()==HTMLTagType_Text)
            return static_cast<HTMLTextTag*>(tag);
        else if(tag->InducesLineBreak())
            return NULL;
        ++itr;
    }
    return NULL;
}
// FUNCTION: WON 0x00739d50
void HTMLDisplayGen::HandleLineBreak(HTMLLineBreakTag* tag)
{
    int flags=0;
    if(tag->mClearLeft) flags|=LineSegmentFlag_BreakClearLeft;
    if(tag->mClearRight) flags|=LineSegmentFlag_BreakClearRight;
    AddLineBreak(flags);
}

// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x00739d80
void HTMLDisplayGen::HandleHorizontalRule(HTMLHorizontalRuleTag* tag)
{
    SmartPtr<HorizontalRuleSegment> segment=new HorizontalRuleSegment;
    segment->mHeight=tag->mSize;
    segment->mWidthSpec=tag->mWidthSpec;
    segment->mNoShade=tag->mNoShade;
    PushAlign(tag->mAlign);
    StartFreshLine();
    AddDisplaySegment(segment.get(),LineSegmentFlag_PostLineBreak);
    PopAlign();
}
// FUNCTION: WON 0x00739e80
void HTMLDisplayGen::HandleHeading(HTMLHeadingTag* tag)
{
    if(tag->IsStart())
    {
        StartFreshParagraph();
        PushAlign(tag->mAlign);
        ++mBoldCount;
        PushFont(mDefaultFontFace.c_str(),HTMLTagType_H6-tag->GetType()+1,-1);
    }
    else
    {
        --mBoldCount;
        PopFont();
        PopAlign();
        StartFreshParagraph();
    }
}
// FUNCTION: WON 0x00739f00
void HTMLDisplayGen::HandleAnchor(HTMLAnchorTag* tag)
{
    if(tag->IsStart())
    {
        if(!tag->mName.empty())
            mDisplay->AddSegment(new AnchorLineSegment(tag->mName));
        if(tag->mHRef.length()!=0)
        {
            mInAnchor=true;
            ++mUnderlineCount;
            PushFont(NULL,-1,mDefaultLinkColor,GetIsFontMonospace());

            mCurLinkParam=new TextLinkParam(tag->mHRef,tag->mLaunchBrowser);
        }
    }
    else if(mInAnchor)
    {
        mInAnchor=false;
        --mUnderlineCount;
        PopFont();
    }
}
// FUNCTION: WON 0x0073a0c0
// (?GetVertAlign@AnchorLineSegment@WONAPI@@UAEDXZ)
// FUNCTION: WON 0x0073a0d0
// (??_GAnchorLineSegment@WONAPI@@UAEPAXI@Z)
// FUNCTION: WON 0x0073a0f0
// (??1AnchorLineSegment@WONAPI@@UAE@XZ)
// FUNCTION: WON 0x0073a150
// (??_GTextLinkParam@WONAPI@@UAEPAXI@Z)
// FUNCTION: WON 0x0073a170
// (??1TextLinkParam@WONAPI@@UAE@XZ)
// FUNCTION: WON 0x0073a220
void HTMLDisplayGen::HandlePre(HTMLTag* tag)
{
    if(tag->IsStart())
    {
        StartFreshParagraph();
        mInPre=true;
        ++mNoBreak;
        PushFont("Courier New",-1,-1,true);
    }
    else
    {
        PopFont();
        mInPre=false;
        --mNoBreak;
        StartFreshParagraph();
    }
}
// FUNCTION: WON 0x0073a280
void HTMLDisplayGen::HandleCenter(HTMLTag* tag)
{
    StartFreshLine();
    if(tag->IsStart()) PushAlign(HTMLAlign_Center);
    else PopAlign();
}
// FUNCTION: WON 0x0073a2c0
void HTMLDisplayGen::HandleModifyFontStyle(HTMLTag* tag,int style)
{
    int increment=tag->IsStart()?1:-1;
    if(style&FontStyle_Bold) mBoldCount+=increment;
    if(style&FontStyle_Italic) mItalicCount+=increment;
    if(style&FontStyle_Underline) mUnderlineCount+=increment;
    CalcFont();
}
// FUNCTION: WON 0x0073a300
void HTMLDisplayGen::HandleModifyFont(HTMLTag* tag,const char* face,int size,int color,bool mono)
{
    if(tag->IsStart()) PushFont(face,size,color,mono);
    else PopFont();
}
// FUNCTION: WON 0x0073a340
void HTMLDisplayGen::HandleFont(HTMLFontTag* tag)
{
    if(tag->IsStart())
    {
        if(tag->mFace.empty()) PushFont(NULL,tag->mSize,tag->mColor);
        else
        {
            std::string face=tag->mFace;
            PushFont(face.c_str(),tag->mSize,tag->mColor);
        }
    }
    else PopFont();
}
// FUNCTION: WON 0x0073a4a0
void HTMLDisplayGen::EndParagraph()
{
    PopAlign();
    StartFreshParagraph();
    mInParagraph=false;
}
// FUNCTION: WON 0x0073a4c0
void HTMLDisplayGen::HandleBlock(HTMLBlockTag* tag)
{
    if(tag->IsStart())
    {
        if(tag->GetType()==HTMLTagType_Paragraph)
        {
            StartFreshParagraph();
            mInParagraph=true;
        }
        else StartFreshLine();
        PushAlign(tag->mAlign);
    }
    else
    {
        if(tag->GetType()!=HTMLTagType_Paragraph)
        {
            StartFreshLine();
            PopAlign();
        }
        else if(mInParagraph) EndParagraph();
    }
}

// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x0073a540
void HTMLDisplayGen::HandleImage(HTMLImageTag* tag)
{
    if(tag->IsEnd()) return;
    if(tag->mAnimation.get()!=NULL)
    {
        SmartPtr<AnimationComponent> animation=new AnimationComponent(tag->mAnimation.get());
        animation->Start();
        AddDisplaySegment(new ComponentLineSegment(animation.get()));
        mCanBreak=false;
        return;
    }
    else if(tag->mImage.get()==NULL) return;
    Image* image=tag->mImage.get();
    SmartPtr<ImageLineSegment> segment;
    ImageMap* map=tag->mImageMap.get();
    int border=mInAnchor?2:0;
    int borderColor=mCurColor;
    if(mInAnchor)
        segment=new ImageLinkLineSegment(mCurLinkParam.get(),mCurColor,mDefaultALinkColor,image);
    else if(map!=NULL)
    {
        border=2;
        borderColor=mDefaultLinkColor;
        segment=new ImageLinkLineSegment(NULL,mCurColor,mDefaultALinkColor,image);
        static_cast<ImageLinkLineSegment*>(segment.get())->mImageMap=map;
    }
    else segment=new ImageLineSegment(image);
    if(tag->mBorder>=0) border=tag->mBorder;
    int flags=0;
    int align=0;
    switch(tag->mAlign)
    {
    case HTMLAlign_Left: flags|=LineSegmentFlag_FloatLeft; break;
    case HTMLAlign_Right: flags|=LineSegmentFlag_FloatRight; break;
    case HTMLAlign_Middle: align=1; break;
    case HTMLAlign_Top: align=2; break;
    }
    segment->mHSpace=tag->mHSpace;
    segment->mVSpace=tag->mVSpace;
    segment->mAlign=(char)align;
    segment->mBorder=border;
    segment->mBorderColor=borderColor;
    ClearAccumulatedSegment();
    AddDisplaySegment(segment.get(),flags);
    if(flags==0)
    {
        mCanBreak=false;
    }
}
// FUNCTION: WON 0x0073a9b0
// (??_GComponentLineSegment@WONAPI@@UAEPAXI@Z)
// FUNCTION: WON 0x0073a9d0
// (??1ComponentLineSegment@WONAPI@@UAE@XZ)
// FUNCTION: WON 0x0073aaf0
// (??1ImageLineSegment@WONAPI@@UAE@XZ)
// FUNCTION: WON 0x0073ab50
// (??_GImageLinkLineSegment@WONAPI@@UAEPAXI@Z)
// FUNCTION: WON 0x0073ab70
// (??1ImageLinkLineSegment@WONAPI@@UAE@XZ)
// FUNCTION: WON 0x0073ac40
void HTMLDisplayGen::HandleList(HTMLListTag* tag)
{
    if(tag->IsEnd())
    {
        if(mListDepth<=0) return;
        --mListDepth;
        AddEmptySegment(new MarginLineSegment(mListDepth*40+mLeftMargin,mRightMargin),LineSegmentFlag_PreLineBreak);
        if(mListDepth==0) StartFreshParagraph();
    }
    else
    {
        ++mListDepth;
        if(mListDepth==1) StartFreshParagraph();
        AddEmptySegment(new MarginLineSegment(mListDepth*40+mLeftMargin,mRightMargin),LineSegmentFlag_PreLineBreak);
    }
}

// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x0073ad40
void HTMLDisplayGen::HandleListItem(HTMLListItemTag* tag)
{
    if(tag->IsEnd()) return;
    StartFreshLine();
    if(mListDepth<1)
        AddDisplaySegment(new BulletElement(mCurColor));
    else if(!tag->mBulletText.empty())
        AddDisplaySegment(new OrderedListElement(mCurColor,mCurFont.get(),tag->mBulletText));
    else
        AddDisplaySegment(new BulletElement(mCurColor,tag->mListType-HTMLListType_Disc+1));
    mCanBreak=false;
    mLastWasSpace=true;
    mCurLineEmpty=mPrevLineEmpty=true;
}
// FUNCTION: WON 0x0073af00
// (?GetAscent@OrderedListElement@WONAPI@@UAEHXZ)
// FUNCTION: WON 0x0073af10
// (??_GOrderedListElement@WONAPI@@UAEPAXI@Z)
// FUNCTION: WON 0x0073af30
// (??1OrderedListElement@WONAPI@@UAE@XZ)
// FUNCTION: WON 0x0073afc0
void HTMLDisplayGen::HandleTable(HTMLTableTag* tag)
{
    if(tag->IsEnd())
    {
        if(mInTable)
        {
            mInTable=false;
            int flags=LineSegmentFlag_PreLineBreak;
            if(mCurTableAlign==HTMLAlign_Left) flags|=LineSegmentFlag_FloatLeft;
            else if(mCurTableAlign==HTMLAlign_Right) flags|=LineSegmentFlag_FloatRight;
            else if(mCurTableAlign==HTMLAlign_Center) PushAlign(HTMLAlign_Center);
            AddDisplaySegment(mCurTable.get(),flags);
            if(mCurTableAlign==HTMLAlign_Center) PopAlign();
            else if(mCurTableAlign==HTMLAlign_None) StartFreshLine();
        }
        else if(mParsingTableData) HandleEndTableData();
    }
    else
    {
        ClearAccumulatedSegment();
        mInTable=true;
        mCurTable=new TableElement;
        mCurTable->mCellSpacing=tag->mCellSpacing;
        mCurTable->mCellPadding=tag->mCellPadding;
        mCurTable->mBorder=tag->mBorder;
        mCurTable->mBGColor=tag->mBGColor;
        mCurTable->mWidthSpec=tag->mWidthSpec;
        mCurTableAlign=tag->mAlign;
        mCurTable->StartTraversal();
    }
}
// FUNCTION: WON 0x0073b160
void HTMLDisplayGen::HandleTableRow(HTMLTableRowTag* tag)
{
    if(tag->IsEnd())
    {
        if(mParsingTableData && !mInTable) HandleEndTableData();
        return;
    }
    if(mInTable)
    {
        mCurTable->NextRow();
        mCurTable->SetRowBGColor(tag->mBGColor);
        int horizontal=tag->mHAlign-HTMLAlign_Left;
        int vertical=tag->mVAlign-HTMLAlign_Top;
        mCurTable->SetRowAlign(horizontal,vertical);
    }
    else if(mParsingTableData) HandleEndTableData();
}
// FUNCTION: WON 0x0073b1d0
void HTMLDisplayGen::HandleTableData(HTMLTableDataTag* tag)
{
    if(tag->IsEnd())
    {
        if(mParsingTableData && !mInTable) HandleEndTableData();
        return;
    }
    if(mInTable)
    {
        TableElement::CellInfo* cell=mCurTable->NextColumn();
        MultiLineElement* element=cell->mContents.get();
        cell->mBGColor=tag->mBGColor;
        cell->mHAlign=(char)(tag->mHAlign-HTMLAlign_Left);
        cell->mVAlign=(char)(tag->mVAlign-HTMLAlign_Top);
        if(cell->mVAlign<0 || cell->mVAlign>2) cell->mVAlign=-1;
        if(cell->mHAlign<0 || cell->mHAlign>2) cell->mHAlign=-1;
        cell->mNoWrap=tag->mNoWrap;
        if(tag->mWidthSpec>=0) cell->mWidth=tag->mWidthSpec;
        else if(tag->mColSpan<=1) mCurTable->SetColumnWidth(tag->mWidthSpec);
        if(tag->mHeight>0) cell->mHeight=tag->mHeight;
        HTMLAlign align=(HTMLAlign)((mCurTable->GetCurRowAlign()-HorzAlign_Left)+HTMLAlign_Left);
        if(cell->mHAlign==1) align=HTMLAlign_Center;
        else if(cell->mHAlign==2) align=HTMLAlign_Right;
        // SetCellSpan may reallocate the cell vector; no cell pointer access follows.
        if(tag->mRowSpan>1 || tag->mColSpan>1)
            mCurTable->SetCellSpan(tag->mRowSpan,tag->mColSpan);
        HTMLDisplayGen generator;
        generator.GenerateTableData(*this,element,tag->GetType()==HTMLTagType_TableHeader,align);
    }
    else if(mParsingTableData) HandleEndTableData();
}
// FUNCTION: WON 0x0073b4a0
// (??1HTMLDisplayGen@WONAPI@@QAE@XZ)
// FUNCTION: WON 0x0073b660
void HTMLDisplayGen::HandleEndTableData()
{
    --mTagListItr;
    mDone=true;
}
// FUNCTION: WON 0x0073b670
void HTMLDisplayGen::HandleGUIComponent(HTMLGUIComponentTag* tag)
{
    AddDisplaySegment(new ComponentLineSegment(tag->mComponent.get()));
    mCanBreak=false;
    mLastWasSpace=false;
}

// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x0073b6e0
void HTMLDisplayGen::HandleOption(HTMLTag* tag)
{
    if(mCurCombo.get()==NULL || tag->IsEnd())
        return;
    GUIString option;
    bool lastWasSpace=true;
    while(true) {
        HTMLTag* next=GetNextTag();
        if(next==NULL)
            break;
        if(next->GetType()==HTMLTagType_Option) {
            if(next->IsStart())
                --mTagListItr;
            break;
        }
        else if(next->GetType()==HTMLTagType_Text) {
            const GUIString& text=((HTMLTextTag*)next)->GetText();
            for(int i=0;i<text.length();++i) {
                unsigned int c=text[i];
                if(c<256 && isspace(c)) {
                    if(!lastWasSpace) {
                        option.append(' ');
                        lastWasSpace=true;
                    }
                }
                else {
                    option.append(c);
                    lastWasSpace=false;
                }
            }
        }
        else {
            --mTagListItr;
            break;
        }
    }
    mCurCombo->AddString(option,-1);
    int width=mCurCombo->mListArea->GetTotalWidth();
    mCurCombo->Fn2F4B90(width+20,mCurCombo->Height());
}
// FUNCTION: WON 0x0073b8b0
void HTMLDisplayGen::HandleInput(HTMLInputTag* tag)
{
    if(tag->IsEnd()) {
        if(mCurCombo.get()!=NULL) {
            AddDisplaySegment(new ComponentLineSegment(mCurCombo.get()));
            mCanBreak=false;
            mLastWasSpace=false;
            mCurCombo=NULL;
        }
        return;
    }
    SmartPtr<LineSegment> segment;
    SmartPtr<Font> font=mWindow->GetFont(FontDescriptor("Tahoma",FontStyle_Plain,10));
    switch(tag->mInputType) {
    case HTMLInputType_Button:
    case HTMLInputType_Submit:
    case HTMLInputType_Reset:
        {
            SmartPtr<MSButton> button=new MSButton(tag->mValue);
            button->SetFont(font.get());
            button->Fn2F4E90(0,0);
            segment=new ComponentLineSegment(button.get());
            break;
        }
    case HTMLInputType_Radio:
        {
            SmartPtr<MSRadioButton> radio=new MSRadioButton;
            radio->SetTransparent(true);
            radio->Fn2F4B90(15,12);
            radio->Fn2F3370(8,false);
            segment=new ComponentLineSegment(radio.get());
            break;
        }
    case HTMLInputType_Image:
        {
            Image* image=tag->mImage.get();
            if(image==NULL)
                break;
            SmartPtr<ImageButton> button=new ImageButton;
            button->Fn2F4B90(image->mWidth,image->mHeight);
            button->SetImage(ImageButtonType_Normal,image);
            button->SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_Hand));
            segment=new ComponentLineSegment(button.get());
            break;
        }
    case HTMLInputType_Checkbox:
    case HTMLInputType_File:
    case HTMLInputType_Hidden:
        break;
    case HTMLInputType_Select:
        mCurCombo=new MSComboBox;
        mCurCombo->Fn2F4B90(50,22);
        break;
    case HTMLInputType_Text:
    case HTMLInputType_Password:
        {
            SmartPtr<MSInputBox> input=new MSInputBox;
            input->mFont=font;
            if(tag->mInputType==HTMLInputType_Password)
                input->SetPasswordChar('*');
            else if(tag->mReadOnly)
                input->SetComponentFlags(8,false);
            input->Fn2F4B90(tag->mSize*6+24,22);
            input->mMaxChars=tag->mMaxLength;
            input->SetText(tag->mValue,true);
            segment=new ComponentLineSegment(input.get());
            break;
        }
    }
    if(segment.get()!=NULL) {
        AddDisplaySegment(segment.get());
        mCanBreak=false;
        mLastWasSpace=false;
    }
}
// FUNCTION: WON 0x0073bea0
// (??_GMSInputBox@WONAPI@@UAEPAXI@Z)

// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x0073c020
void HTMLDisplayGen::HandleTag(HTMLTag* tag)
{
    switch(tag->GetType())
    {
    case HTMLTagType_Body: HandleBody(static_cast<HTMLBodyTag*>(tag)); break;
    case HTMLTagType_Anchor: HandleAnchor(static_cast<HTMLAnchorTag*>(tag)); break;
    case HTMLTagType_LineBreak: HandleLineBreak(static_cast<HTMLLineBreakTag*>(tag)); break;
    case HTMLTagType_HorizontalRule: HandleHorizontalRule(static_cast<HTMLHorizontalRuleTag*>(tag)); break;
    case HTMLTagType_Text: HandleTextTag(static_cast<HTMLTextTag*>(tag)); break;
    case HTMLTagType_Bold: HandleModifyFontStyle(tag,FontStyle_Bold); break;
    case HTMLTagType_Italic: HandleModifyFontStyle(tag,FontStyle_Italic); break;
    case HTMLTagType_Underline: HandleModifyFontStyle(tag,FontStyle_Underline); break;
    case HTMLTagType_Center: HandleCenter(tag); break;
    case HTMLTagType_Small: HandleModifyFont(tag,NULL,2,-1,GetIsFontMonospace()); break;
    case HTMLTagType_Big: HandleModifyFont(tag,NULL,4,-1,GetIsFontMonospace()); break;
    case HTMLTagType_Code: HandleModifyFont(tag,"Courier New",-1,-1,true); break;
    case HTMLTagType_Pre: HandlePre(tag); break;
    case HTMLTagType_Font:
    case HTMLTagType_BaseFont: HandleFont(static_cast<HTMLFontTag*>(tag)); break;
    case HTMLTagType_Div:
    case HTMLTagType_Paragraph: HandleBlock(static_cast<HTMLBlockTag*>(tag)); break;
    case HTMLTagType_Table: HandleTable(static_cast<HTMLTableTag*>(tag)); break;
    case HTMLTagType_TableRow: HandleTableRow(static_cast<HTMLTableRowTag*>(tag)); break;
    case HTMLTagType_TableData:
    case HTMLTagType_TableHeader: HandleTableData(static_cast<HTMLTableDataTag*>(tag)); break;
    case HTMLTagType_UnorderedList:
    case HTMLTagType_OrderedList: HandleList(static_cast<HTMLListTag*>(tag)); break;
    case HTMLTagType_ListItem: HandleListItem(static_cast<HTMLListItemTag*>(tag)); break;
    case HTMLTagType_Image: HandleImage(static_cast<HTMLImageTag*>(tag)); break;
    case HTMLTagType_Input:
    case HTMLTagType_Select: HandleInput(static_cast<HTMLInputTag*>(tag)); break;
    case HTMLTagType_Option: HandleOption(tag); break;
    case HTMLTagType_H1:
    case HTMLTagType_H2:
    case HTMLTagType_H3:
    case HTMLTagType_H4:
    case HTMLTagType_H5:
    case HTMLTagType_H6: HandleHeading(static_cast<HTMLHeadingTag*>(tag)); break;
    case HTMLTagType_GUIComponent: HandleGUIComponent(static_cast<HTMLGUIComponentTag*>(tag)); break;
    default: break;
    }
}
// FUNCTION: WON 0x0073c250
void HTMLDisplayGen::GenerateTableData(HTMLDisplayGen& parent,MultiLineElement* display,bool header,HTMLAlign align)
{
    mDisplay=display;
    mWindow=parent.mWindow;
    mDefaultLinkColor=parent.mDefaultLinkColor;
    mDefaultALinkColor=parent.mDefaultALinkColor;
    mDefaultTextColor=parent.mDefaultTextColor;
    mDefaultFontFace=parent.mDefaultFontFace;
    mDefaultAlign=align;
    mBoldCount=mItalicCount=mUnderlineCount=0;
    mTagList=parent.mTagList;
    mTagListItr=parent.mTagListItr;
    mTagListEnd=parent.mTagListEnd;
    mFontSize=parent.mFontSize;
    mLeftMargin=mRightMargin=mTopMargin=mBottomMargin=0;
    if(header) mBoldCount++;
    mParsingTableData=true;
    PushFont(mDefaultFontFace.c_str(),3,-1);
    if(mDefaultAlign!=HTMLAlign_Left) PushAlign(mDefaultAlign,true);
    GenerateBase();
    parent.mTagListItr=mTagListItr;
}

// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x0073c430
void HTMLDisplayGen::Generate(HTMLDocument* document,MultiLineElement* display,Table* container)
{
    mTagList=&document->GetTagList();
    mTagListItr=document->GetBodyStart();
    mTagListEnd=document->GetBodyEnd();
    mWindow=WONAPI::WindowManager::GetDefaultWindowManager()->mDefaultWindow;
    mDisplay=display;
    mDisplayContainer=container;
    // LOST-DEBUG: none: exact only
    EE_LOST_DEBUG(2);
    mAlignStack.clear();
    mFontStack.clear();
    mDefaultTextColor=(int)(0x80000000u|StandardColor_Text);
    mDefaultFontFace=gGlobalDefaultFontFace;
    mBoldCount=mItalicCount=mUnderlineCount=0;
    mDefaultLinkColor=(int)(0x80000000u|StandardColor_Link);
    mDefaultALinkColor=(int)(0x80000000u|StandardColor_LinkDown);
    mParsingTableData=false;
    mDefaultAlign=HTMLAlign_Left;
    if(!FindBody()) return;
    GenerateBase();
    LineSegment::mCounter=0;
    mDisplay->SetSelIndex();
}

// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x0073c5b0
void HTMLDisplayGen::GenerateBase()
{
    mListDepth=0;
    mCanBreak=true;
    mLastWasSpace=true;
    mCurLineEmpty=true;
    mPrevLineEmpty=true;
    mInPre=false;
    mNoBreak=0;
    mInAnchor=false;
    mInParagraph=false;
    mDone=false;
    mInTable=false;
    mNextTextTag=NULL;
    while(!mDone)
    {
        HTMLTag* tag=GetNextTag();
        if(tag==NULL) break;
        HandleTag(tag);
    }
    ClearAccumulatedSegment();
}
// FUNCTION: WON 0x0074afc0
// (??0?$SmartPtr@VImageMap@WONAPI@@@WONAPI@@QAE@XZ)
// FUNCTION: WON 0x0074afd0
// (?push_back@?$list@PAVLineSegment@WONAPI@@V?$allocator@PAVLineSegment@WONAPI@@@std@@@std@@QAEXABQAVLineSegment@WONAPI@@@Z)
// TEMPLATE: WON 0x00702370
// (public: __thiscall WONAPI::GUIString::operator class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >(void)const )
// TEMPLATE: WON 0x00711480
// (public: virtual void * __thiscall WONAPI::MarginLineSegment::`scalar deleting destructor'(unsigned int))
// TEMPLATE: WON 0x00738000
// (_$E71)
// TEMPLATE: WON 0x007385b0
// (public: virtual int __thiscall WONAPI::LineBreakSegment::GetHeight(void))
// TEMPLATE: WON 0x00739710
// (public: __thiscall WONAPI::LineSegment::LineSegment(void))
// TEMPLATE: WON 0x0073aef0
// (public: virtual int __thiscall WONAPI::OrderedListElement::GetHeight(void))
// TEMPLATE: WON 0x0073bec0
// (public: virtual __thiscall WONAPI::MSInputBox::~MSInputBox(void))
// TEMPLATE: WON 0x0074c8a0
// (public: class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInf)
// TEMPLATE: WON 0x0074c920
// (public: class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInf)
// TEMPLATE: WON 0x0074c9a0
// (public: class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInf)
// TEMPLATE: WON 0x0074ca20
// (protected: void __thiscall std::vector<struct WONAPI::HTMLDisplayGen::FontInfo,class std::allocator<struct WONAPI::HTMLDisplayGen::FontInfo> >::_Tidy()
// TEMPLATE: WON 0x0074ca70
// (public: class std::_Ptrit<enum WONAPI::HTMLAlign,int,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign &,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign)
// TEMPLATE: WON 0x0074cad0
// (public: class std::_Ptrit<enum WONAPI::HTMLAlign,int,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign &,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign)
// TEMPLATE: WON 0x0074cb40
// (public: class std::_Ptrit<enum WONAPI::HTMLAlign,int,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign &,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign)
// TEMPLATE: WON 0x007500f0
// (public: void __thiscall std::vector<struct WONAPI::HTMLDisplayGen::FontInfo,class std::allocator<struct WONAPI::HTMLDisplayGen::FontInfo> >::insert(cl)
// TEMPLATE: WON 0x00750580
// (protected: void __thiscall std::vector<struct WONAPI::HTMLDisplayGen::FontInfo,class std::allocator<struct WONAPI::HTMLDisplayGen::FontInfo> >::_Destr)
// TEMPLATE: WON 0x007505b0
// (public: void __thiscall std::vector<enum WONAPI::HTMLAlign,class std::allocator<enum WONAPI::HTMLAlign> >::insert(class std::_Ptrit<enum WONAPI::HTMLA)
// TEMPLATE: WON 0x007520e0
// (protected: void __thiscall std::vector<enum WONAPI::HTMLAlign,class std::allocator<enum WONAPI::HTMLAlign> >::_Xlen(void)const )
// TEMPLATE: WON 0x007543c0
// (class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInfo &,stru)
// TEMPLATE: WON 0x007544e0
// (struct WONAPI::HTMLDisplayGen::FontInfo * __cdecl std::copy(class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGe)
// TEMPLATE: WON 0x00754660
// (class std::_Ptrit<enum WONAPI::HTMLAlign,int,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign &,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign &> __cd)
// TEMPLATE: WON 0x007546a0
// (enum WONAPI::HTMLAlign * __cdecl std::copy(class std::_Ptrit<enum WONAPI::HTMLAlign,int,enum WONAPI::HTMLAlign *,enum WONAPI::HTMLAlign &,enum WONAPI:)
// TEMPLATE: WON 0x00754880
// (protected: struct WONAPI::HTMLDisplayGen::FontInfo * __thiscall std::vector<struct WONAPI::HTMLDisplayGen::FontInfo,class std::allocator<struct WONAPI)
// TEMPLATE: WON 0x007548c0
// (void __cdecl std::fill(class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisp)
// TEMPLATE: WON 0x00754a30
// (class std::_Ptrit<struct WONAPI::HTMLDisplayGen::FontInfo,int,struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInfo &,stru)
// TEMPLATE: WON 0x00754e40
// (void __cdecl std::_Construct(struct WONAPI::HTMLDisplayGen::FontInfo *,struct WONAPI::HTMLDisplayGen::FontInfo const &))
// TEMPLATE: WON 0x0073aa30
// TEMPLATE: WON 0x0073aad0
}
