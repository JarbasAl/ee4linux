// HTML text whitespace normalization and cross-tag glue-word handling.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cctype>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
namespace WONAPI {
// Genuine pinned SDK StringUtil inline predicate, no standalone primary quota.
inline int safe_isspace(unsigned long ch) { return ch<256?isspace(ch):0; }
// Genuine pinned SDK inline method, expanded in both original algorithms; no separate body quota.
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
}
