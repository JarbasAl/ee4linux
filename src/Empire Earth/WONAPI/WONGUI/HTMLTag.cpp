// Source-only reconstructed WONGUI HTML tags; no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <cstdio>
#include <cctype>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyHTMLTag.h"



using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLTag::HTMLTag(HTMLTagType theType, unsigned short theFlags)
{
	mType = theType;
	mFlags = theFlags;
}

// FUNCTION: WON 0x007449e0
// (??0HTMLTag@WONAPI@@QAE@W4HTMLTagType@1@G@Z)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00744a10
HTMLBodyTag::HTMLBodyTag() : HTMLTag(HTMLTagType_Body)
{
	mBGColor = -1;
	mTextColor = -1;
	mLinkColor = -1;
	mALinkColor = -1;

	mLeftMargin = mRightMargin = 10;
	mTopMargin = mBottomMargin = 15;
}

// FUNCTION: WON 0x00744ab0
// (??_GHTMLBodyTag@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00744ad0
// (??1HTMLBodyTag@WONAPI@@UAE@XZ)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00744b60
void HTMLBodyTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	mBGColor = theAttributes.GetColor("BGCOLOR");
	mTextColor = theAttributes.GetColor("TEXT");
	mLinkColor = theAttributes.GetColor("LINK");
	mALinkColor = theAttributes.GetColor("ALINK");
	theAttributes.GetAttribute("BACKGROUND",mBGImageSrc); 

	if(theAttributes.GetIntParam("MARGINWIDTH",mLeftMargin))
		mRightMargin = mLeftMargin;
	if(theAttributes.GetIntParam("MARGINHEIGHT",mTopMargin))
		mBottomMargin = mTopMargin;

	theAttributes.GetIntParam("LEFTMARGIN",mLeftMargin);
	theAttributes.GetIntParam("RIGHTMARGIN",mRightMargin);
	theAttributes.GetIntParam("TOPMARGIN",mTopMargin);
	theAttributes.GetIntParam("BOTTOMMARGIN",mBottomMargin);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00744fa0
void HTMLLineBreakTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	GUIString *aParam = theAttributes.GetAttribute("CLEAR");
	if(aParam!=NULL)
	{
		if(aParam->CompareNoCase("LEFT")==0)
			mClearLeft = true;
		else if(aParam->CompareNoCase("RIGHT")==0)
			mClearRight = true;
		else if(aParam->CompareNoCase("ALL")==0)
			mClearLeft = mClearRight = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007450e0
void HTMLHorizontalRuleTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	mSize = 0;
	theAttributes.GetIntParam("SIZE",mSize);
	if(mSize<=0)
		mSize = 2;

	mWidthSpec = theAttributes.GetAbsOrPercentParam("WIDTH");
	mNoShade = theAttributes.GetAttribute("NOSHADE")!=NULL;

	mAlign = HTMLAlign_Center;
	GUIString *aParam = theAttributes.GetAttribute("ALIGN");
	if(aParam!=NULL)
	{
		if(StringCompareNoCase(*aParam,"RIGHT")==0)
			mAlign = HTMLAlign_Right;
		else if(StringCompareNoCase(*aParam,"LEFT")==0)
			mAlign = HTMLAlign_Left;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007453f0
void HTMLAnchorTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	theAttributes.GetAttribute("HREF",mHRef);
	theAttributes.GetAttribute("NAME",mName);

	GUIString *aTarget = theAttributes.GetAttribute("TARGET");
	mLaunchBrowser = aTarget!=NULL && aTarget->CompareNoCase("_browser")==0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007455d0
void HTMLHeadingTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	mAlign = theAttributes.GetBlockAlign("ALIGN");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00745690
void HTMLFontTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	theAttributes.GetAttribute("FACE",mFace);
	int aCommaPos = mFace.find(',',0);
	if(aCommaPos>=0)
		mFace.resize(aCommaPos);

	mSize = 0;
	GUIString *aParam = theAttributes.GetAttribute("SIZE");
	if(aParam!=NULL && !(aParam->length()==0))
	{
		char aStartChar = (*aParam)[0];
		if(aStartChar=='+' || aStartChar=='-')
		{
			int aVal = aParam->mData->ToInt(1);
			if(aStartChar=='+')
			{
				mSize = 3 + aVal;
				if(mSize>7)
					mSize = 7;
			}
			else
			{
				mSize = 3 - aVal;
				if(mSize < 1)
					mSize = 1;
			}
		}
		else
			mSize = aParam->mData->ToInt(0);
	}

	mColor = theAttributes.GetColor("COLOR");
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007458c0
void HTMLBlockTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	mAlign = theAttributes.GetBlockAlign("ALIGN");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00745980
void HTMLImageMapTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	theAttributes.GetAttribute("NAME",mName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00745a40
void HTMLImageTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	theAttributes.GetAttribute("SRC",mSrc);

	mAlign = HTMLAlign_None;
	mHSpace = mVSpace = 0;
	mWidth = mHeight = 0;
	mBorder = -1;

	GUIString *aParam = theAttributes.GetAttribute("ALIGN");
	if(aParam!=NULL)
	{
		if(aParam->CompareNoCase("LEFT")==0)
		{
			mAlign = HTMLAlign_Left;
			mHSpace = 3;
		}
		else if(aParam->CompareNoCase("RIGHT")==0)
		{
			mAlign = HTMLAlign_Right;
			mHSpace = 3;
		}
		else if(aParam->CompareNoCase("MIDDLE")==0)
			mAlign = HTMLAlign_Middle;
		else if(aParam->CompareNoCase("TOP")==0)
			mAlign = HTMLAlign_Top;
	}

	theAttributes.GetIntParam("HSPACE",mHSpace);
	theAttributes.GetIntParam("VSPACE",mVSpace);

	theAttributes.GetIntParam("WIDTH",mWidth);
	theAttributes.GetIntParam("HEIGHT",mHeight);
	
	aParam  = theAttributes.GetAttribute("USEMAP");
	if(aParam!=NULL && aParam->length()>1 && (*aParam)[0]=='#')
		mUseMap = aParam->substr(1);

	theAttributes.GetIntParam("BORDER",mBorder);
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00745e60
void HTMLAreaTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	mIsValid = false;
	theAttributes.GetAttribute("HREF",mHRef);


	GUIString *aParamStr = theAttributes.GetAttribute("COORDS");
	if(aParamStr==NULL)
		return;

	string aParam = *aParamStr;
	mLeft = mTop = mRight = mBottom = -1;
	sscanf(aParam.c_str(),"%d,%d,%d,%d",&mLeft,&mTop,&mRight,&mBottom);
	if(mLeft==-1 || mTop==-1 || mRight==-1 || mBottom==-1)
		return;

	mIsValid = true;
	GUIString *aTarget= theAttributes.GetAttribute("TARGET");
	mLaunchBrowser = aTarget!=NULL && aTarget->CompareNoCase("_browser")==0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00746100
void HTMLListTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	if(GetType()==HTMLTagType_OrderedList)
	{
		mListType = HTMLListType_Digit;
		GUIString *aParam = theAttributes.GetAttribute("TYPE");
		if(aParam!=NULL)
		{
			if(aParam->compare("1") == 0)
				mListType = HTMLListType_Digit;
			else if(aParam->compare("a") == 0)
				mListType = HTMLListType_LowerLetter;
			else if(aParam->compare("A") == 0)
				mListType = HTMLListType_UpperLetter;
			else if(aParam->compare("i") == 0)
				mListType = HTMLListType_LowerRoman;
			else if(aParam->compare("I") == 0)
				mListType = HTMLListType_UpperRoman;
		}
	}
	else
	{
		mListType = HTMLListType_None;
		GUIString *aParam = theAttributes.GetAttribute("TYPE");
		if(aParam!=NULL)
		{
			if(aParam->CompareNoCase("disc")==0)
				mListType = HTMLListType_Disc;
			else if(aParam->CompareNoCase("circle")==0)
				mListType = HTMLListType_Circle;
			else if(aParam->CompareNoCase("square")==0)
				mListType = HTMLListType_Square;
		}
	}

	mStart = 1;
	theAttributes.GetIntParam("START",mStart);
	if(mStart<1)
		mStart = 1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007463c0
void HTMLListItemTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	mListType = HTMLListType_Disc;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007463d0
void HTMLTableTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	mCellSpacing = 2;
	mCellPadding = 1;
	mBorder = 0;

	theAttributes.GetIntParam("CELLSPACING",mCellSpacing);
	theAttributes.GetIntParam("CELLPADDING",mCellPadding);
	theAttributes.GetIntParam("BORDER",mBorder);

	GUIString *aParam = theAttributes.GetAttribute("BORDER"); 
	if(aParam!=NULL)
	{
		mBorder = aParam->mData->ToInt(0);
		if(mBorder==0 && (*aParam)[0]!='0') // border may be a boolean
			mBorder = 1;
	}

	mBGColor = theAttributes.GetColor("BGCOLOR");
	mWidthSpec = theAttributes.GetAbsOrPercentParam("WIDTH");

	mAlign = theAttributes.GetStandardHAlign("ALIGN");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007466f0
void HTMLTableRowTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	mBGColor = theAttributes.GetColor("BGCOLOR");
	mHAlign = theAttributes.GetTableDataHAlign("ALIGN",HTMLAlign_Left);
	mVAlign = theAttributes.GetTableDataVAlign("VALIGN",HTMLAlign_Middle);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007468c0
void HTMLTableDataTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	mBGColor = theAttributes.GetColor("BGCOLOR");
	mHAlign = theAttributes.GetTableDataHAlign("ALIGN",GetType()==HTMLTagType_TableData?HTMLAlign_Left:HTMLAlign_Center);
	mVAlign = theAttributes.GetTableDataVAlign("VALIGN");

	mNoWrap = theAttributes.GetAttribute("NOWRAP")!=NULL;
	mRowSpan = mColSpan = 0;
	theAttributes.GetIntParam("ROWSPAN",mRowSpan);
	theAttributes.GetIntParam("COLSPAN",mColSpan);
	
	if(mRowSpan<=0) mRowSpan = 1;
	if(mColSpan<=0) mColSpan = 1;

	mWidthSpec = mHeight = 0;
	mWidthSpec = theAttributes.GetAbsOrPercentParam("WIDTH");
	theAttributes.GetIntParam("HEIGHT",mHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00746c40
void HTMLInputTag::ParseAttributes(HTMLTagAttributes &theAttributes)
{
	if(mInputType==HTMLInputType_None)
	{
		GUIString aType;
		theAttributes.GetAttribute("TYPE",aType);

		if(aType.CompareNoCase("Button")==0) mInputType = HTMLInputType_Button;
		else if(aType.CompareNoCase("Submit")==0) mInputType = HTMLInputType_Submit;
		else if(aType.CompareNoCase("Reset")==0) mInputType = HTMLInputType_Reset;
		else if(aType.CompareNoCase("Radio")==0) mInputType = HTMLInputType_Radio;
		else if(aType.CompareNoCase("Checkbox")==0) mInputType = HTMLInputType_Checkbox;
		else if(aType.CompareNoCase("Hidden")==0) mInputType = HTMLInputType_Hidden;
		else if(aType.CompareNoCase("Image")==0) mInputType = HTMLInputType_Image;
		else if(aType.CompareNoCase("Select")==0) mInputType = HTMLInputType_Select;
		else if(aType.CompareNoCase("Password")==0) mInputType = HTMLInputType_Password;
		else mInputType = HTMLInputType_Text;
	}

	theAttributes.GetAttribute("VALUE",mValue);

	switch(mInputType)
	{
		case HTMLInputType_Submit: if((mValue.length()==0)) mValue = "Submit";	break;
		case HTMLInputType_Reset: if((mValue.length()==0)) mValue = "Reset";		break;
		case HTMLInputType_Image: theAttributes.GetAttribute("SRC",mImageSrc);	break;
		
		case HTMLInputType_Password:
		case HTMLInputType_Text:		
			mSize = 20;
			mMaxLength = 0;
			theAttributes.GetIntParam("SIZE",mSize);
			theAttributes.GetIntParam("MAXLENGTH",mMaxLength);
			mReadOnly = theAttributes.GetAttribute("READONLY")!=NULL;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// FUNCTION: WON 0x00747140
void HTMLTagAttributes::Clear()
{	
	mAttributeMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString* HTMLTagAttributes::GetAttribute(const GUIString &theKey)
{
	AttributeMap::iterator anItr = mAttributeMap.find(theKey);
	if(anItr==mAttributeMap.end())
		return NULL;
	else
		return &(anItr->second);
}

// FUNCTION: WON 0x007471f0
// (?GetAttribute@HTMLTagAttributes@WONAPI@@QAEPAVGUIString@2@ABV32@@Z)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00747220
void HTMLTagAttributes::GetAttribute(const GUIString &theKey, GUIString &theValue)
{
	AttributeMap::iterator anItr = mAttributeMap.find(theKey);
	if(anItr==mAttributeMap.end())
		theValue.erase();
	else
		theValue = anItr->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007472d0
void HTMLTagAttributes::AddAttribute(const GUIString &theKey, const GUIString &theValue)
{
	mAttributeMap[theKey] = theValue;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int GetHexDigit(char theChar)
{
	if(theChar>='0' && theChar<='9')
		return theChar - '0';
	
	theChar = toupper(theChar);
	if(theChar>='A' && theChar<='F')
		return theChar - 'A' + 10;

	return -1;
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00747330
bool HTMLTagAttributes::GetIntParam(const GUIString &theKey, int &theInt)
{
	GUIString *aParam = GetAttribute(theKey);
	if(aParam==NULL)
		return false; 

	string aStr = *aParam;
	if(sscanf(aStr.c_str(),"%d",&theInt)==1)
		return true;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00747480
int HTMLTagAttributes::GetAbsOrPercentParam(const GUIString &theKey)
{
	GUIString *aParam = GetAttribute(theKey);
	if(aParam==NULL || (aParam->length()==0))
		return 0;

	if((*aParam)[aParam->length()-1]=='%')
		return -aParam->mData->ToInt(0);
	else
		return aParam->mData->ToInt(0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007474d0
HTMLAlign HTMLTagAttributes::GetBlockAlign(const GUIString &theKey, HTMLAlign theDefault)
{
	GUIString *anAlignStr = GetAttribute(theKey);
	if(anAlignStr==NULL)
		return theDefault;

	if(anAlignStr->CompareNoCase("CENTER")==0)
		return HTMLAlign_Center;
	else if(anAlignStr->CompareNoCase("RIGHT")==0)
		return HTMLAlign_Right;
	else if(anAlignStr->CompareNoCase("LEFT")==0)
		return HTMLAlign_Left;
	else if(anAlignStr->CompareNoCase("JUSTIFY")==0)
		return HTMLAlign_Left;
	else
		return theDefault;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00747560
HTMLAlign HTMLTagAttributes::GetStandardHAlign(const GUIString &theKey, HTMLAlign theDefault)
{
	GUIString *anAlignStr = GetAttribute(theKey);
	if(anAlignStr==NULL)
		return theDefault;

	if(anAlignStr->CompareNoCase("CENTER")==0)
		return HTMLAlign_Center;
	else if(anAlignStr->CompareNoCase("RIGHT")==0)
		return HTMLAlign_Right;
	else if(anAlignStr->CompareNoCase("LEFT")==0)
		return HTMLAlign_Left;
	else
		return theDefault;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007475d0
HTMLAlign HTMLTagAttributes::GetTableDataHAlign(const GUIString &theKey, HTMLAlign theDefault)
{
	GUIString *anAlign = GetAttribute(theKey);
	if(anAlign==NULL)
		return theDefault;

	if(anAlign->CompareNoCase("LEFT")==0)
		return HTMLAlign_Left;
	else if(anAlign->CompareNoCase("CENTER")==0)
		return HTMLAlign_Center;
	else if(anAlign->CompareNoCase("MIDDLE")==0)
		return HTMLAlign_Center;
	else if(anAlign->CompareNoCase("RIGHT")==0)
		return HTMLAlign_Right;
	else
		return theDefault;

}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00747650
HTMLAlign HTMLTagAttributes::GetTableDataVAlign(const GUIString &theKey, HTMLAlign theDefault)
{
	GUIString *anAlign = GetAttribute(theKey);
	if(anAlign==NULL)
		return theDefault;

	if(anAlign->CompareNoCase("TOP")==0)
		return HTMLAlign_Top;
	else if(anAlign->CompareNoCase("MIDDLE")==0)
		return HTMLAlign_Middle;
	else if(anAlign->CompareNoCase("CENTER")==0)
		return HTMLAlign_Middle;
	else if(anAlign->CompareNoCase("BOTTOM")==0)
		return HTMLAlign_Bottom;
	else
		return theDefault;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007476e0
int HTMLTagAttributes::GetColor(const GUIString &theKey)
{
	GUIString *aStr = GetAttribute(theKey);
	if(aStr==NULL)
		return -1;

	if((aStr->length()==0))
		return -1;

	int anOffset = 0;
	if((*aStr)[0]!='#')
	{
		struct ColorDefine { const char* mName; int mColor; };
		static ColorDefine aColorTable[] = {
			{"Black",			0x000000},
			{"Silver",			0xC0C0C0},
			{"Gray",			0x808080},
			{"White",			0xFFFFFF},
			{"Maroon",			0x800000},
			{"Red",				0xFF0000},
			{"Purple",			0x800080},
			{"Fuchsia",			0xFF00FF},
			{"Green",			0x008000},
			{"Lime",			0x00FF00},
			{"Olive",			0x808000},
			{"Yellow",			0xFFFF00},
			{"Navy",			0x000080},
			{"Blue",			0x0000FF},
			{"Teal",			0x008080},
			{"Aqua",			0x00FFFF},
			{"Snow",					0xFFFAFA},
			{"GhostWhite",				0xF8F8FF },
			{"WhiteSmoke",				0xF5F5F5 },
			{"Gainsboro",				0xDCDCDC },
			{"FloralWhite",				0xFFFAF0 },
			{"OldLace",					0xFDF5E6 },
			{"Linen",					0xFAF0E6 },
			{"AntiqueWhite",			0xFAEBD7 },
			{"PapayaWhip",				0xFFEFD5 },
			{"BlanchedAlmond",			0xFFEBCD },
			{"Bisque",					0xFFE4C4 },
			{"PeachPuff",				0xFFDAB9 },
			{"NavajoWhite",				0xFFDEAD },
			{"Moccasin",				0xFFE4B5 },
			{"Cornsilk",				0xFFF8DC },
			{"Ivory",					0xFFFFF0 },
			{"LemonChiffon",			0xFFFACD },
			{"Seashell",				0xFFF5EE },
			{"Honeydew",				0xF0FFF0 },
			{"MintCream",				0xF5FFFA },
			{"Azure",					0xF0FFFF },
			{"AliceBlue",				0xF0F8FF },
			{"lavender",				0xE6E6FA },
			{"LavenderBlush",			0xFFF0F5 },
			{"MistyRose",				0xFFE4E1 },
			{"White",					0xFFFFFF },
			{"Black",					0x000000 },
			{"DarkSlateGray",			0x2F4F4F },
			{"DimGrey",					0x696969 },
			{"SlateGrey",				0x708090 },
			{"LightSlateGray",			0x778899 },
			{"Grey",					0xBEBEBE },
			{"LightGrey",				0xD3D3D3 },
			{"MidnightBlue",			0x191970 },
			{"NavyBlue",				0x000080 },
			{"CornflowerBlue",			0x6495ED },
			{"DarkSlateBlue",			0x483D8B },
			{"SlateBlue",				0x6A5ACD },
			{"MediumSlateBlue",			0x7B68EE },
			{"LightSlateBlue",			0x8470FF },
			{"MediumBlue",				0x0000CD },
			{"RoyalBlue",				0x4169E1 },
			{"Blue",					0x0000FF },
			{"DodgerBlue",				0x1E90FF },
			{"DeepSkyBlue",				0x00BFFF },
			{"SkyBlue",					0x87CEEB },
			{"LightSkyBlue",			0x87CEFA },
			{"SteelBlue",				0x4682B4 },
			{"LightSteelBlue",			0xB0C4DE },
			{"LightBlue",				0xADD8E6 },
			{"PowderBlue",				0xB0E0E6 },
			{"PaleTurquoise",			0xAFEEEE },
			{"DarkTurquoise",			0x00CED1 },
			{"MediumTurquoise",			0x48D1CC },
			{"Turquoise",				0x40E0D0 },
			{"Cyan",					0x00FFFF },
			{"LightCyan",				0xE0FFFF },
			{"CadetBlue",				0x5F9EA0 },
			{"MediumAquamarine",		0x66CDAA },
			{"Aquamarine",				0x7FFFD4 },
			{"DarkGreen",				0x006400 },
			{"DarkOliveGreen",			0x556B2F },
			{"DarkSeaGreen",			0x8FBC8F },
			{"SeaGreen",				0x2E8B57 },
			{"MediumSeaGreen",			0x3CB371 },
			{"LightSeaGreen",			0x20B2AA },
			{"PaleGreen",				0x98FB98 },
			{"SpringGreen",				0x00FF7F },
			{"LawnGreen",				0x7CFC00 },
			{"Green",					0x00FF00 },
			{"Chartreuse",				0x7FFF00 },
			{"MedSpringGreen",			0x00FA9A },
			{"GreenYellow",				0xADFF2F },
			{"LimeGreen",				0x32CD32 },
			{"YellowGreen",				0x9ACD32 },
			{"ForestGreen",				0x228B22 },
			{"OliveDrab",				0x6B8E23 },
			{"DarkKhaki",				0xBDB76B },
			{"PaleGoldenrod",			0xEEE8AA },
			{"LtGoldenrodYello",		0xFAFAD2 },
			{"LightYellow",				0xFFFFE0 },
			{"Yellow",					0xFFFF00 },
			{"Gold",					0xFFD700 },
			{"LightGoldenrod",			0xEEDD82 },
			{"goldenrod",				0xDAA520 },
			{"DarkGoldenrod",			0xB8860B },
			{"RosyBrown",				0xBC8F8F },
			{"IndianRed",				0xCD5C5C },
			{"SaddleBrown",				0x8B4513 },
			{"Sienna",					0xA0522D },
			{"Peru",					0xCD853F },
			{"Burlywood",				0xDEB887 },
			{"Beige",					0xF5F5DC },
			{"Wheat",					0xF5DEB3 },
			{"SandyBrown",				0xF4A460 },
			{"Tan",						0xD2B48C },
			{"Chocolate",				0xD2691E },
			{"Firebrick",				0xB22222 },
			{"Brown",					0xA52A2A },
			{"DarkSalmon",				0xE9967A },
			{"Salmon",					0xFA8072 },
			{"LightSalmon",				0xFFA07A },
			{"Orange",					0xFFA500 },
			{"DarkOrange",				0xFF8C00 },
			{"Coral",					0xFF7F50 },
			{"LightCoral",				0xF08080 },
			{"Tomato",					0xFF6347 },
			{"OrangeRed",				0xFF4500 },
			{"Red",						0xFF0000 },
			{"HotPink",					0xFF69B4 },
			{"DeepPink",				0xFF1493 },
			{"Pink",					0xFFC0CB },
			{"LightPink",				0xFFB6C1 },
			{"PaleVioletRed",			0xDB7093 },
			{"Maroon",					0xB03060 },
			{"MediumVioletRed",			0xC71585 },
			{"VioletRed",				0xD02090 },
			{"Magenta",					0xFF00FF },
			{"Violet",					0xEE82EE },
			{"Plum",					0xDDA0DD },
			{"Orchid",					0xDA70D6 },
			{"MediumOrchid",			0xBA55D3 },
			{"DarkOrchid",				0x9932CC },
			{"DarkViolet",				0x9400D3 },
			{"BlueViolet",				0x8A2BE2 },
			{"Purple",					0xA020F0 },
			{"MediumPurple",			0x9370DB },
			{"Thistle",					0xD8BFD8 },
			{"DarkGrey",				0xA9A9A9 },
			{"DarkBlue",				0x00008B },
			{"DarkCyan",				0x008B8B },
			{"DarkMagenta",				0x8B008B },
			{"DarkRed",					0x8B0000 },
			{"LightGreen",				0x90EE90 }
		};
		typedef map<GUIString,int,StringLessNoCase> ColorMap; 
		static ColorMap aColorMap;
		if(aColorMap.empty())
		{
			int aNumColors = sizeof(aColorTable)/sizeof(ColorDefine);
			for(int i=0; i<aNumColors; i++)
				aColorMap[aColorTable[i].mName] = aColorTable[i].mColor;
		}
		ColorMap::iterator anItr = aColorMap.find(*aStr);
		if(anItr!=aColorMap.end())
			return anItr->second;	
//		else
//			return -1;
	}
	else
		anOffset = 1;

//	if(aStr->length()!=7)
//		return -1;

	int aColor = 0, i;
	for(i=0; i<6; i++)
	{
		aColor<<=4;
		int aDigit = GetHexDigit((*aStr)[i+anOffset]);
		if(aDigit<0)
			return -1;

		aColor |= aDigit;
	
		if((*aStr)[i+2]=='\0')
			break;
	}

	if(i<6)
		aColor<<=(4*(5-i));
	
	return aColor;

}


// FUNCTION: WON 0x007478e0
// (_$E72)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0074b1f0
// (public: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::GUIString>,struct std::_Mapk<class WONA)
// TEMPLATE: WON 0x0074bbf0
// (public: struct std::pair<class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::GUIString>,struct std:)
// TEMPLATE: WON 0x0074e420
// (protected: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::GUIString>,struct std::_Mapk<class W)
// TEMPLATE: WON 0x0074e720
// (protected: struct std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::GUIString>,struct std::_Mapk<class )
// TEMPLATE: WON 0x0074b060
// (public: class WONAPI::GUIString & __thiscall std::map<class WONAPI::GUIString,class WONAPI::GUIString,struct WONAPI::StringLessNoCase,class std::alloc)
