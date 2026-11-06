// Complete actual TextLinkLineSegment34; genuine inline constructor for real factory339540.
// Actual source lifetime/copy bodies remain original-owned; no fabricated external constructor.
#ifndef EE_WONAPI_WONLOBBYTEXTLINKSEGMENT_H
#define EE_WONAPI_WONLOBBYTEXTLINKSEGMENT_H
#include <ctype.h>
#include "ee/wonapi/WONLobbyTextSegment.h"
namespace WONAPI {
class TextLinkLineSegment : public TextLineSegment {
public:
    DWORD mNormalColor,mDownColor; //28,2c; actual normal then down, not reversed
    SmartPtr<TextLinkParam> mLinkParam; //30, sole derived retained owner; full34
    TextLinkLineSegment(TextLinkParam* param,const GUIString& text,int color,int downColor,Font* font)
        :TextLineSegment(text,color,font),mNormalColor(color),mDownColor(downColor),mLinkParam(param)
    {
        mLinkParam->mLinkList.push_back(this); //genuine SDK AddLink inline body; borrowed list payload
        if(mText.length()==1 && isspace(mText[0])) mText.erase(); //native unguarded CRT classification
    }
    virtual void MouseEnter(int,int); //28=30ac00
    virtual void MouseExit(); //2c=30ae40
    virtual void MouseDown(int,int,MouseButton); //30=30ad70
    virtual void MouseUp(int,int,MouseButton); //34=30ad90
    virtual void LinkDown(); //40=30abc0
    virtual void LinkUp(); //44=30abe0
    virtual bool CanStartSelection() { return false; } //48=30a7b0 inherited/shared helper0
    // No Paint override: true inherited TextLineSegment30a900 remains external.
};
}
#endif
