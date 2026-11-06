// Source-only reconstructed WONGUI HTML document tag operations; no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <cstdio>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyHTMLTag.h"
#include "ee/wonapi/WONLobbyAnimation.h"
#include "ee/wonapi/WONLobbyImageDecoder.h"
#include "ee/wonapi/WONLobbyImageHelpers.h"
#include "ee/wonapi/WONLobbyResourceConfig.h"
#include "ee/wonapi/WONLobbyConfigImages.h"

namespace WONAPI {
// FUNCTION: WON 0x0073c780
// (??_GHTMLDocument@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0073ccf0
void HTMLDocument::SetBasePath(const GUIString& path)
{
    int pos=(int)path.length()-1;
    while(pos>=0 && path[pos]!='\\') --pos;
    if(pos<0)
    {
        pos=(int)path.length()-1;
        while(pos>=0 && path[pos]!='/') --pos;
    }
    if(pos<0) mBasePath.erase();
    else mBasePath=path.substr(0,(unsigned short)(pos+1));
}
// FUNCTION: WON 0x0073d350
// (??1LoadedImageStruct@HTMLDocument@WONAPI@@QAE@XZ)

// FUNCTION: WON 0x0073d400
// (??1?$pair@$$CBVGUIString@WONAPI@@ULoadedImageStruct@HTMLDocument@2@@std@@QAE@XZ)

// FUNCTION: WON 0x0073d4e0
// (??1?$pair@$$CBVGUIString@WONAPI@@V?$SmartPtr@VHTMLTag@WONAPI@@@2@@std@@QAE@XZ)

// FUNCTION: WON 0x0073d9a0
// (??0ImageFilter@WONAPI@@QAE@XZ)

// FUNCTION: WON 0x0073de80
// (??1?$pair@$$CBVGUIString@WONAPI@@V?$SmartPtr@VImageMap@WONAPI@@@2@@std@@QAE@XZ)

// FUNCTION: WON 0x0073df00
// (??_GImageMap@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0073eb80
void HTMLDocument::AddDocument(HTMLDocument* document)
{
    if(document==NULL) return;
    HTMLTagList::iterator begin=document->GetBodyStart(),end=document->GetBodyEnd();
    for(HTMLTagList::iterator i=begin;i!=end;++i) AddTag(i->get());
}
// FUNCTION: WON 0x0073ebd0
void HTMLDocument::AddTag(HTMLTag* tag)
{
    mTagList.push_back(tag);
    switch(tag->GetType())
    {
    case HTMLTagType_Body: HandleBody((HTMLBodyTag*)tag); break;
    case HTMLTagType_Image: HandleImage((HTMLImageTag*)tag); break;
    case HTMLTagType_Input: HandleInput((HTMLInputTag*)tag); break;
    case HTMLTagType_ImageMap: HandleImageMap((HTMLImageMapTag*)tag); break;
    case HTMLTagType_Area: HandleArea((HTMLAreaTag*)tag); break;
    case HTMLTagType_OrderedList:
    case HTMLTagType_UnorderedList: HandleList((HTMLListTag*)tag); break;
    case HTMLTagType_ListItem: HandleListItem((HTMLListItemTag*)tag); break;
    }
}
HTMLTagList::iterator HTMLDocument::GetBodyStart()
{
    return mBodyStart!=mTagList.end() ? mBodyStart : mTagList.begin();
}
// FUNCTION: WON 0x0073efa0
HTMLTagList::iterator HTMLDocument::GetBodyEnd()
{
    HTMLTagList::iterator end=mBodyEnd;
    if(end!=mTagList.end()) ++end;
    return end;
}
// FUNCTION: WON 0x0073e980
static void GetLetterItem(int theCount, char *theBuf, bool upper)
{
	int aPos = 0;
	while(theCount>0)
	{
		theCount--;
		theBuf[aPos++] = (upper?'A':'a') + theCount%26;
		theCount/=26;
	} 


	theBuf[aPos] = '.';
	theBuf[aPos+1] = 0;

	// Reverse String
	int i = 0;
	aPos--;
	while(i<aPos)
	{
		char aTemp = theBuf[aPos]; theBuf[aPos] = theBuf[i]; theBuf[i] = aTemp;
		i++; aPos--;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0073ea90
static void GetRomanNumeralHelp(char *theBuf, int &thePos, int &theNumber, int hiVal, char hiChar, char midChar, char lowChar)
{
	int lowVal = hiVal/10;
	int midVal = hiVal>>1;

	while(theNumber>=hiVal)
	{
		theBuf[thePos++] = hiChar;
		theNumber-=hiVal;
	}

	if(theNumber>=hiVal-lowVal)
	{
		theBuf[thePos++] = lowChar;
		theBuf[thePos++] = hiChar;
		theNumber-=hiVal-lowVal;
	}

	if(theNumber>=midVal)
	{
		theBuf[thePos++] = midChar;
		theNumber-=midVal;
	}

	if(theNumber>=midVal-lowVal)
	{
		theBuf[thePos++] = lowChar;
		theBuf[thePos++] = midChar;
		theNumber-=midVal-lowVal;
	}

	while(theNumber>=lowVal)
	{
		theBuf[thePos++] = lowChar;
		theNumber-=lowVal;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0073e9f0
static void GetRomanNumeralItem(int theCount, char *theBuf, bool upper)
{
	static char lower_letters[] = { 'm', 'd', 'c', 'l', 'x', 'v', 'i' };
	static char upper_letters[] = { 'M', 'D', 'C', 'L', 'X', 'V', 'I' };
	const char* letters = upper?upper_letters:lower_letters;

	if(theCount>=4000)
	{
		sprintf(theBuf,"%d.",theCount);
		return;
	}

	int aPos = 0;
	int aHiVal = 1000;
	int aLetterPos = 0;
	for(int i=0; i<3; i++)
	{
		GetRomanNumeralHelp(theBuf,aPos,theCount,aHiVal,letters[aLetterPos],letters[aLetterPos+1],letters[aLetterPos+2]);
		aHiVal/=10;
		aLetterPos+=2;
	}
	theBuf[aPos++] = '.';
	theBuf[aPos] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// FUNCTION: WON 0x0073e680
void HTMLDocument::HandleList(HTMLListTag *theTag)
{	
	if(theTag->IsEnd())
	{
		if(!mListStack.empty())
			mListStack.pop_back();

		return;
	}

	mListStack.insert(mListStack.end(),ListInfo());

	ListInfo &anInfo = mListStack.back();
	if(theTag->GetType()==HTMLTagType_UnorderedList)
	{
		anInfo.mOrdered = false;
		if(theTag->mListType==HTMLListType_None)
		{
			anInfo.mType = (HTMLListType)(HTMLListType_Disc + mListStack.size() - 1);

		}
		else
			anInfo.mType = theTag->mListType;

		if(anInfo.mType > HTMLListType_Square)
			anInfo.mType = HTMLListType_Square;
		else if(anInfo.mType < HTMLListType_Disc)
			anInfo.mType = HTMLListType_Disc;
	}
	else
	{
		anInfo.mOrdered = true;
		anInfo.mType = theTag->mListType;
	}

	anInfo.mCount = theTag->mStart;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0073e7b0
void HTMLDocument::HandleListItem(HTMLListItemTag *theTag)
{
	if(theTag->IsEnd())
		return;

	if(mListStack.empty())
	{
		theTag->mListType = HTMLListType_Disc;
		return;
	}

	ListInfo &anInfo = mListStack.back();
	theTag->mListType = anInfo.mType;

	if(anInfo.mOrdered)
	{
		char aBuf[100];
		aBuf[0] = 0;
		switch(anInfo.mType)
		{
			case HTMLListType_LowerLetter: GetLetterItem(anInfo.mCount,aBuf,false); break;
			case HTMLListType_UpperLetter: GetLetterItem(anInfo.mCount,aBuf,true); break;
			case HTMLListType_LowerRoman: GetRomanNumeralItem(anInfo.mCount,aBuf,false); break;
			case HTMLListType_UpperRoman: GetRomanNumeralItem(anInfo.mCount,aBuf,true); break;

			case HTMLListType_Digit: 
			default:
				sprintf(aBuf,"%d.",anInfo.mCount); break;
		}
		theTag->mBulletText = aBuf;
	}

	anInfo.mCount++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

HTMLDocument::LoadedImageStruct::LoadedImageStruct(NativeImage* image,Animation* animation) : mImage(image),mAnimation(animation),mHaveUsedAnimation(false) {} //actual SDK value ctor, support0
// FUNCTION: WON 0x0073ce10
bool HTMLDocument::DoDecodeImage(LoadedImageStruct *theStruct, Window *theWindow, const std::string &theImagePath)
{
	if(theWindow==NULL)
		return false;

	MultiImageDecoder *aDecoder = &theWindow->GetWindowManager()->mImageDecoder;
	SmartPtr<RawImage> aRawImage = aDecoder->DecodeRaw(theImagePath.c_str());
	if(aRawImage.get()==NULL)
		return false;

	SmartPtr<NativeImage> anImage = aRawImage->GetNative(theWindow->mDisplayContext.get());
	
	SmartPtr<Animation> anAnimation = aDecoder->GetAnimation(false);
	if(anAnimation.get()!=NULL)
		anAnimation->RealizeNative(mWindow->mDisplayContext.get());

	theStruct->mImage = anImage;
	theStruct->mRawImage = aRawImage;
	theStruct->mAnimation = anAnimation;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0073d010
HTMLDocument::LoadedImageStruct* HTMLDocument::GetImage(const GUIString &thePath, HTMLTag *theTag)
{
	if((thePath.empty()))
		return NULL;

	std::pair<LoadedImageMap::iterator,bool> aRet;
	aRet = mLoadedImageMap.insert(LoadedImageMap::value_type(thePath,LoadedImageStruct()));
	LoadedImageStruct &aStruct = aRet.first->second;

	if(aStruct.mImage.get()!=NULL)
		return &aStruct;

	if(mDelayImages && !mDecodingImages)
	{
		mImageTagMap.insert(ImageTagMap::value_type(thePath,theTag));
		return NULL;
	}

	if(!aRet.second) // already failed to decode
			return NULL;

	GUIString aPath = mBasePath;
	aPath.append(thePath);

	if(DoDecodeImage(&aStruct,mWindow,aPath))
		return &aStruct;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// FUNCTION: WON 0x0073dfa0
void HTMLDocument::HandleInput(HTMLInputTag *theTag)
{
	if(theTag->IsEnd())
		return;

	if((theTag->mImageSrc.length()==0))
		return;

	LoadedImageStruct *aStruct = GetImage(theTag->mImageSrc,theTag);
	if(aStruct==NULL)
		return;

	theTag->mImage = aStruct->mImage.get();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// FUNCTION: WON 0x0073edb0
void HTMLDocument::DecodeImages(bool continueDelay)
{
	if(!continueDelay)
		mDelayImages = false;

	mDecodingImages = true;
	ImageTagMap::iterator anItr = mImageTagMap.begin();

	while(anItr!=mImageTagMap.end())
	{
		HTMLTag *aTag = anItr->second.get();
		switch(aTag->GetType())
		{
			case HTMLTagType_Body: HandleBody((HTMLBodyTag*)aTag); break;
			case HTMLTagType_Image: HandleImage((HTMLImageTag*)aTag); break;
			case HTMLTagType_Input: HandleInput((HTMLInputTag*)aTag); break;
		}

		++anItr;

	}
	mDecodingImages = false;

	if(!continueDelay)
	{
		mImageTagMap.clear();
		mLoadedImageMap.clear();
		mScaledImageMap.clear();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// FUNCTION: WON 0x0073c610
HTMLDocument::HTMLDocument(const GUIString &theBasePath)
{
	Clear();

	mDecodingImages = false;
	mDelayImages = true;
	mWindow = Window::GetDefaultWindow();

	if(!(theBasePath.length()==0))
		SetBasePath(theBasePath);
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0073cbd0
void HTMLDocument::Clear()
{
	mBodyStart = mBodyEnd = mTagList.end();
	mImageTagMap.clear();
	mLoadedImageMap.clear();
	mScaledImageMap.clear();
	mImageTagList.clear();
	mTagList.clear();
	mListStack.clear();
	mCurImageMap = NULL;
	mImageMapMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0073d560
NativeImage* HTMLDocument::CheckScaleImage(LoadedImageStruct* image,const GUIString& path,int width,int height)
{
    NativeImage* native=image->mImage.get();
    // Target revision defaults nonpositive dimensions independently; no aspect-ratio inference.
    if(width<=0) width=native->mWidth;
    if(height<=0) height=native->mHeight;
    if(width!=native->mWidth || height!=native->mHeight)
    {
        char buffer[50];
        sprintf(buffer,"_%d_%d",width,height);
        GUIString scaleName=path;
        scaleName.append(GUIString(buffer));
        LoadedImageMap::iterator i=mScaledImageMap.insert(LoadedImageMap::value_type(scaleName,LoadedImageStruct())).first;
        LoadedImageStruct& scaled=i->second;
        if(scaled.mImage.get()==NULL)
        {
            if(image->mRawImage.get()==NULL) image->mRawImage=native->GetRaw().get();
            SmartPtr<ScaleImageFilter> filter=new ScaleImageFilter(width,height);
            scaled.mImage=filter->Filter(image->mRawImage.get())->GetNative(mWindow->mDisplayContext.get());
        }
        native=scaled.mImage.get();
    }
    return native;
}
// FUNCTION: WON 0x0073dcd0
static void ImagePaint(Graphics &g, int width, int height)
{
	g.SetColor(0x848484);
	g.DrawRect(0,0,width,height);
	g.SetColor(0xd6d6d6);
	g.DrawLine(width-1,0,width-1,height-1);
	g.DrawLine(0,height-1,width-1,height-1);

	int w = 14, h = 16;
	int ox = 7, oy = 7;

	bool clipped = false;
	if(width<w+ox || height<h+oy)
	{
		if(width<8 || height<8)
			return;

		g.PushClipRect(0,0,width-5,height-5);
		clipped = true;
	}

	g.Translate(ox,oy);
	
	g.SetColor(0xffffff);
	g.FillRect(0,0,w,h);
	g.SetColor(0x848484);
	g.DrawLine(0,0,w-1,0);
	g.DrawLine(0,0,0,h-2);
	g.SetColor(0x000000);
	g.DrawLine(0,h-1,w-1,h-1);
	g.DrawLine(w-1,1,w-1,h-1);
	g.SetColor(0xa0a0a0);
	g.DrawLine(w-2,1,w-2,h-2);
	g.DrawLine(1,h-2,w-2,h-2);

	g.SetColor(0xff0000);


	int offset = 0;
	int hmid = w/2;
	int vmid = h/2;
	int left = hmid-4+offset;
	int right = hmid+3+offset;
	int top = vmid-4+offset;
	int bottom = vmid+3+offset;
	for(int i=0; i<4; i++)
	{
		g.DrawLine(left+i,top+i,left+i+1,top+i);
		g.DrawLine(right-i,top+i,right-1-i,top+i);
		g.DrawLine(left+i,bottom-i,left+i+1,bottom-i);
		g.DrawLine(right-i,bottom-i,right-i-1,bottom-i);
	}

	g.Translate(-ox,-oy);

	if(clipped)
		g.PopClipRect();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0073d9e0
void HTMLDocument::HandleImage(HTMLImageTag *theTag)
{
	LoadedImageStruct *aStruct = GetImage(theTag->mSrc,theTag);
	if(aStruct==NULL)
	{		
		if(mDelayImages && theTag->mWidth>0 && theTag->mHeight>0)
			theTag->mImage = new ProgramImage(theTag->mWidth, theTag->mHeight,ImagePaint);

		return;
	}


	if(aStruct->mAnimation.get()!=NULL)
	{
		if(aStruct->mHaveUsedAnimation)
			theTag->mAnimation = aStruct->mAnimation->Duplicate();
		else
		{
			theTag->mAnimation = aStruct->mAnimation;
			aStruct->mHaveUsedAnimation = true;
		}

		return;
	}
	
	NativeImage *anImage = CheckScaleImage(aStruct,theTag->mSrc,theTag->mWidth,theTag->mHeight);
	theTag->mImage = anImage;	

	if(!(theTag->mUseMap.length()==0))
	{
		ImageMapMap::iterator anItr = mImageMapMap.insert(ImageMapMap::value_type(theTag->mUseMap,NULL)).first;
		if(anItr->second.get()==NULL)
			anItr->second = new ImageMap;

		theTag->mImageMap = anItr->second;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0073e020
void HTMLDocument::HandleImageMap(HTMLImageMapTag* theTag)
{
	if(theTag->IsEnd())
	{
		mCurImageMap = NULL;
		return;
	}

	if((theTag->mName.empty()))
		return;

	ImageMapMap::iterator anItr = mImageMapMap.insert(ImageMapMap::value_type(theTag->mName,SmartPtr<ImageMap>())).first;

	if(anItr->second.get()==NULL)
		anItr->second = new ImageMap;

	mCurImageMap = anItr->second;				
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0073e270
void HTMLDocument::HandleArea(HTMLAreaTag *theTag)
{
	if(theTag->IsEnd())
		return;

	if(mCurImageMap.get()==NULL)
		return;

	if(!theTag->mIsValid)
		return;

	WONRectangle aRect(theTag->mLeft,theTag->mTop,theTag->mRight-theTag->mLeft,theTag->mBottom-theTag->mTop);
	mCurImageMap->AddLink(new TextLinkParam(theTag->mHRef, theTag->mLaunchBrowser), aRect);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0073e410
void HTMLDocument::HandleBody(HTMLBodyTag *theTag)
{
	if(!mDecodingImages)
	{
		if(theTag->IsEnd())
		{
			if(mBodyEnd!=mTagList.end())
			{
				mTagList.erase(mBodyEnd);
				mBodyEnd = mTagList.end();
			}

			--mBodyEnd; // point to last added tag

			return;
		}

		if(mBodyStart==mTagList.end())
			--mBodyStart; // point to last added tag
		else // get rid of this body tag because we already have one.
		{
			if(mBodyStart->get()!=theTag)
				mTagList.pop_back();

			return;
		}
	}

	LoadedImageStruct *aStruct = GetImage(theTag->mBGImageSrc,theTag);
	if(aStruct==NULL)
		return;

	NativeImage *anImage = aStruct->mImage.get();

	// Pre-Tile the image a little if it's too small
	int aWidth = anImage->mWidth;
	int aHeight = anImage->mHeight;
	int anExtraWidth = 0;
	int anExtraHeight = 0;
	if(aWidth < 50)
	{
		if(50%aWidth==0)
			anExtraWidth = 50-aWidth;
		else
			anExtraWidth = (50/aWidth)*aWidth;
	}
	if(aHeight < 50)
	{
		if(50%aHeight==0)
			anExtraHeight = 50-aHeight;
		else
			anExtraHeight = (50/aHeight)*aHeight;
	}

	if(anExtraWidth>0 || anExtraHeight>0)
	{
		aWidth+=anExtraWidth;
		aHeight+=anExtraHeight;
		SmartPtr<NativeImage> aBigImage = mWindow->CreateImage(aWidth,aHeight);
		Graphics &g = aBigImage->GetGraphics();
		for(int x=0; x<aWidth; x+=anImage->mWidth)
			for(int y=0; y<aHeight; y+=anImage->mHeight)
				g.DrawImage(anImage,x,y);

		theTag->mBGImage = aBigImage.get();
	}
	else
		theTag->mBGImage = anImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

}
