// Native HTML segment accumulation and line-state transitions on original-owned receivers.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
namespace WONAPI {
// FUNCTION: WON 0x00737f50
// (??0HTMLDisplayGen@WONAPI@@QAE@XZ)

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
}
