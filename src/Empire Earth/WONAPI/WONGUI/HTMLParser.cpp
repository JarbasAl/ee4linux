// Source-only reconstructed WONGUI HTML parser; no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyHTMLParser.h"
#include "ee/wonapi/WONLobbyAnimation.h"


using namespace WONAPI;

// Genuine SDK same-TU HTML tag registry and allocation callback closure; support0.
// FUNCTION: WON 0x00741c10
static HTMLTag* TagFactory(HTMLTagType theTagType) { return new HTMLTag(theTagType); }
// FUNCTION: WON 0x00741c70
static HTMLTag* TagBodyFactory(HTMLTagType theTagType) { return new HTMLBodyTag; }
// FUNCTION: WON 0x00741cd0
static HTMLTag* TagBlockElemFactory(HTMLTagType theTagType) { return new HTMLTag(theTagType,true); }
// FUNCTION: WON 0x00741d30
static HTMLTag* TagLineBreakFactory(HTMLTagType theTagType) { return new HTMLLineBreakTag; }
// FUNCTION: WON 0x00741da0
static HTMLTag* TagHorizontalRuleFactory(HTMLTagType theTagType) { return new HTMLHorizontalRuleTag; }
// FUNCTION: WON 0x00741e00
static HTMLTag* TagHeadingFactory(HTMLTagType theTagType) { return new HTMLHeadingTag(theTagType); }
// FUNCTION: WON 0x00741e60
static HTMLTag* TagAnchorFactory(HTMLTagType theTagType) { return new HTMLAnchorTag; }
// FUNCTION: WON 0x00741f10
static HTMLTag* TagFontFactory(HTMLTagType theTagType) { return new HTMLFontTag(theTagType); }
// FUNCTION: WON 0x00741fc0
// (??_GHTMLFontTag@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00741fe0
// (??1HTMLFontTag@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x00742040
static HTMLTag* TagBlockFactory(HTMLTagType theTagType) { return new HTMLBlockTag(theTagType); }
// FUNCTION: WON 0x007420a0
static HTMLTag* TagImageMapFactory(HTMLTagType theTagType) { return new HTMLImageMapTag; }
// FUNCTION: WON 0x007421d0
static HTMLTag* TagAreaFactory(HTMLTagType theTagType) { return new HTMLAreaTag; }
// FUNCTION: WON 0x007422e0
static HTMLTag* TagImageFactory(HTMLTagType theTagType) { return new HTMLImageTag; }
// FUNCTION: WON 0x00742150
// (??_GHTMLImageMapTag@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00742170
// (??1HTMLImageMapTag@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x00742260
// (??_GHTMLAreaTag@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00742280
// (??1HTMLAreaTag@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x007423a0
// (??_GHTMLImageTag@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x007423c0
// (??1HTMLImageTag@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x007424b0
static HTMLTag* TagListFactory(HTMLTagType theTagType) { return new HTMLListTag(theTagType); }
// FUNCTION: WON 0x00742510
static HTMLTag* TagListItemFactory(HTMLTagType theTagType) { return new HTMLListItemTag; }
// FUNCTION: WON 0x007425c0
// (??_GHTMLListItemTag@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x007425e0
// (??1HTMLListItemTag@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x00742640
static HTMLTag* TagTableFactory(HTMLTagType theTagType) { return new HTMLTableTag; }
// FUNCTION: WON 0x007426a0
static HTMLTag* TagTableRowFactory(HTMLTagType theTagType) { return new HTMLTableRowTag; }
// FUNCTION: WON 0x00742700
static HTMLTag* TagTableDataFactory(HTMLTagType theTagType) { return new HTMLTableDataTag(theTagType); }
// FUNCTION: WON 0x00742760
static HTMLTag* TagInputFactory(HTMLTagType theTagType) { return new HTMLInputTag; }
// FUNCTION: WON 0x007428f0
static HTMLTag* TagSelectFactory(HTMLTagType theTagType) { return new HTMLInputTag(HTMLInputType_Select); }
// FUNCTION: WON 0x00742820
// (??_GHTMLInputTag@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00742840
// (??1HTMLInputTag@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x007429b0
static HTMLTag* TagSkipFactory(HTMLTagType theTagType) { return new HTMLTag(theTagType,HTMLTagFlag_SkipToEndTag); }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00740740
HTMLParser::TagNameMap& HTMLParser::GetTagNameMap()
{
	static TagNameMap mTagNameMap;
	if(mTagNameMap.empty())
	{
		mTagNameMap["BODY"]		=		TagLookupInfo(TagBodyFactory,HTMLTagType_Body);

		mTagNameMap["BR"]		=		TagLookupInfo(TagLineBreakFactory,HTMLTagType_LineBreak);
		mTagNameMap["HR"]		=		TagLookupInfo(TagHorizontalRuleFactory,HTMLTagType_HorizontalRule);

		mTagNameMap["P"]		=		TagLookupInfo(TagBlockFactory,HTMLTagType_Paragraph);
		mTagNameMap["DIV"]		=		TagLookupInfo(TagBlockFactory,HTMLTagType_Div);

		mTagNameMap["TABLE"]	=		TagLookupInfo(TagTableFactory,HTMLTagType_Table);
		mTagNameMap["TR"]		=		TagLookupInfo(TagTableRowFactory,HTMLTagType_TableRow);
		mTagNameMap["TD"]		=		TagLookupInfo(TagTableDataFactory,HTMLTagType_TableData);
		mTagNameMap["TH"]		=		TagLookupInfo(TagTableDataFactory,HTMLTagType_TableHeader);

		mTagNameMap["H1"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H1);
		mTagNameMap["H2"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H2);
		mTagNameMap["H3"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H3);
		mTagNameMap["H4"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H4);
		mTagNameMap["H5"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H5);
		mTagNameMap["H6"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H6);

		mTagNameMap["UL"]		=		TagLookupInfo(TagListFactory,HTMLTagType_UnorderedList);
		mTagNameMap["OL"]		=		TagLookupInfo(TagListFactory,HTMLTagType_OrderedList);
		mTagNameMap["LI"]		=		TagLookupInfo(TagListItemFactory,HTMLTagType_ListItem);

		mTagNameMap["PRE"]		=		TagLookupInfo(TagBlockElemFactory,HTMLTagType_Pre);
		mTagNameMap["CENTER"]	=		TagLookupInfo(TagBlockElemFactory,HTMLTagType_Center);

		mTagNameMap["A"]		=		TagLookupInfo(TagAnchorFactory,HTMLTagType_Anchor);

		mTagNameMap["B"]		=		TagLookupInfo(TagFactory,HTMLTagType_Bold);
		mTagNameMap["STRONG"]	=		TagLookupInfo(TagFactory,HTMLTagType_Bold);
		mTagNameMap["I"]		=		TagLookupInfo(TagFactory,HTMLTagType_Italic);
		mTagNameMap["EM"]		=		TagLookupInfo(TagFactory,HTMLTagType_Italic);
		mTagNameMap["U"]		=		TagLookupInfo(TagFactory,HTMLTagType_Underline);
		mTagNameMap["CODE"]		=		TagLookupInfo(TagFactory,HTMLTagType_Code);
		mTagNameMap["TT"]		=		TagLookupInfo(TagFactory,HTMLTagType_Code);
		mTagNameMap["KBD"]		=		TagLookupInfo(TagFactory,HTMLTagType_Code);
		mTagNameMap["SAMP"]		=		TagLookupInfo(TagFactory,HTMLTagType_Code);
		mTagNameMap["SMALL"]	=		TagLookupInfo(TagFactory,HTMLTagType_Small);
		mTagNameMap["BIG"]		=		TagLookupInfo(TagFactory,HTMLTagType_Big);

		mTagNameMap["IMG"]		=		TagLookupInfo(TagImageFactory,HTMLTagType_Image);
		mTagNameMap["MAP"]		=		TagLookupInfo(TagImageMapFactory,HTMLTagType_ImageMap);
		mTagNameMap["AREA"]		=		TagLookupInfo(TagAreaFactory,HTMLTagType_Area);
		
		mTagNameMap["FONT"]		=		TagLookupInfo(TagFontFactory,HTMLTagType_Font);
		mTagNameMap["BASEFONT"]	=		TagLookupInfo(TagFontFactory,HTMLTagType_BaseFont);

		mTagNameMap["NOBR"]		=		TagLookupInfo(TagFactory,HTMLTagType_NoBreak);
		
		mTagNameMap["INPUT"]	=		TagLookupInfo(TagInputFactory,HTMLTagType_Input);
		mTagNameMap["SELECT"]	=		TagLookupInfo(TagSelectFactory,HTMLTagType_Select);
		mTagNameMap["OPTION"]	=		TagLookupInfo(TagFactory,HTMLTagType_Option);
		
		mTagNameMap["SCRIPT"]	=		TagLookupInfo(TagSkipFactory,HTMLTagType_Script);
		mTagNameMap["STYLE"]	=		TagLookupInfo(TagSkipFactory,HTMLTagType_Style);
	}

	return mTagNameMap;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


// FUNCTION: WON 0x00742a10
// (_$E72)

// FUNCTION: WON 0x00742bb0
void HTMLParser::CloseFile()
{
	mBackStr.erase();
	if(mFile!=NULL) { fclose(mFile); mFile=NULL; }

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00743ff0
// (?EndOfFile@HTMLParser@WONAPI@@IAE_NXZ)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00742fc0
static int GetWesternChar(int theChar)
{
	switch(theChar)
	{
		case 130:   return 8218;  //     Single Low-9 Quotation Mark
		case 131:   return 402;  //      Latin Small Letter F With Hook
		case 132:   return 8222;  //     Double Low-9 Quotation Mark
		case 133:   return 8230;  //     Horizontal Ellipsis
		case 134:   return 8224;  //     Dagger
		case 135:   return 8225;  //     Double Dagger
		case 136:   return 710;  //      Modifier Letter Circumflex Accent
		case 137:   return 8240;  //     Per Mille Sign
		case 138:   return 352;  //      Latin Capital Letter S With Caron
		case 139:   return 8249;  //     Single Left-Pointing Angle Quotation Mark
		case 140:   return 338;  //      Latin Capital Ligature OE
		case 145:   return 8216;  //     Left Single Quotation Mark
		case 146:   return 8217;  //     Right Single Quotation Mark
		case 147:   return 8220;  //     Left Double Quotation Mark
		case 148:   return 8221;  //     Right Double Quotation Mark
		case 149:   return 8226;  //     Bullet
		case 150:   return 8211;  //     En Dash
		case 151:   return 8212;  //     Em Dash
		case 152:   return 732;  //      Small Tilde
		case 153:   return 8482;  //     Trade Mark Sign
		case 154:   return 353;  //      Latin Small Letter S With Caron
		case 155:   return 8250;  //     Single Right-Pointing Angle Quotation Mark
		case 156:   return 339;  //      Latin Small Ligature OE
		case 159:   return 376;  //      Latin Capital Letter Y With Diaeresis
	}

	return theChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007434d0
inline int HTMLParser::GetChar()
{
	int aChar = 0;
	if((mBackStr.length()==0))
	{
		if(mFile!=NULL) aChar = fgetc(mFile); else aChar = EOF;

		if(aChar>=130 && aChar<=159)
			aChar = GetWesternChar(aChar);
	}
	else
	{
		aChar = mBackStr[mBackStr.length()-1];
		mBackStr.resize(mBackStr.length()-1);
	}

	if(aChar=='\n')
		mLineNum++;

	return aChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00744010
inline int HTMLParser::PeekChar()
{
	if(EndOfFile())
		return -1;


	int aChar = GetChar();
	UngetChar(aChar);
	return aChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007430f0
// (?UngetChar@HTMLParser@WONAPI@@IAEXH@Z)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline void HTMLParser::UngetString(const GUIString &theStr)
{
	for(int i=theStr.length()-1; i>=0; i--)
	{
		UngetChar(theStr[i]);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline void HTMLParser::UngetString(const GUIString &theStr, int theStartPos)
{
	for(int i=theStr.length()-1; i>=theStartPos; i--)
	{
		UngetChar(theStr[i]);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00744200
bool HTMLParser::SkipWhitespace()
{
	bool hadSpace = false;
	while(!EndOfFile())
	{
		int aChar = GetChar();
		if(!((unsigned int)aChar<256 && isspace(aChar)))
		{
			UngetChar(aChar);
			break;
		}
		else
			hadSpace = true;
	}

	return hadSpace;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00742c10
inline int HTMLParser::ReadEntity()
{
	GUIString anEntity;
	int i;
	for(i=0; i<20 && !EndOfFile(); i++)
	{
		int aChar = GetChar();
		if(aChar==';')
			break;

		anEntity.append(aChar);
	}

	if(i==20)
	{
		UngetString(anEntity);
		return -1;
	}

	if(anEntity[0]=='#')
	{
		int anAsciiValue = anEntity.mData->ToInt(1);

		if(anAsciiValue>=130 && anAsciiValue<=159)
			anAsciiValue = GetWesternChar(anAsciiValue);

		if(anAsciiValue==0)
			anAsciiValue = -1;

		return anAsciiValue;
	}

	struct EntityDefine { const char* mName; int mEntity; };
	static EntityDefine anEntityTable[] = {
		{"nbsp", 160},
		{"iexcl", 161},
		{"cent", 162},
		{"pound", 163},
		{"curren", 164},
		{"yen", 165},
		{"brvbar", 166},
		{"sect", 167},
		{"uml", 168},
		{"copy", 169},
		{"ordf", 170},
		{"laquo", 171},
		{"not", 172},
		{"shy", 173},
		{"reg", 174},
		{"macr", 175},
		{"deg", 176},
		{"plusmn", 177},
		{"sup2", 178},
		{"sup3", 179},
		{"acute", 180},
		{"micro", 181},
		{"para", 182},
		{"middot", 183},
		{"cedil", 184},
		{"sup1", 185},
		{"ordm", 186},
		{"raquo", 187},
		{"frac14", 188},
		{"frac12", 189},
		{"frac34", 190},
		{"iquest", 191},
		{"Agrave", 192},
		{"Aacute", 193},
		{"Acirc", 194},
		{"Atilde", 195},
		{"Auml", 196},
		{"Aring", 197},
		{"AElig", 198},
		{"Ccedil", 199},
		{"Egrave", 200},
		{"Eacute", 201},
		{"Ecirc", 202},
		{"Euml", 203},
		{"Igrave", 204},
		{"Iacute", 205},
		{"Icirc", 206},
		{"Iuml", 207},
		{"ETH", 208},
		{"Ntilde", 209},
		{"Ograve", 210},
		{"Oacute", 211},
		{"Ocirc", 212},
		{"Otilde", 213},
		{"Ouml", 214},
		{"times", 215},
		{"Oslash", 216},
		{"Ugrave", 217},
		{"Uacute", 218},
		{"Ucirc", 219},
		{"Uuml", 220},
		{"Yacute", 221},
		{"THORN", 222},
		{"szlig", 223},
		{"agrave", 224},
		{"aacute", 225},
		{"acirc", 226},
		{"atilde", 227},
		{"auml", 228},
		{"aring", 229},
		{"aelig", 230},
		{"ccedil", 231},
		{"egrave", 232},
		{"eacute", 233},
		{"ecirc", 234},
		{"euml", 235},
		{"igrave", 236},
		{"iacute", 237},
		{"icirc", 238},
		{"iuml", 239},
		{"eth", 240},
		{"ntilde", 241},
		{"ograve", 242},
		{"oacute", 243},
		{"ocirc", 244},
		{"otilde", 245},
		{"ouml", 246},
		{"divide", 247},
		{"oslash", 248},
		{"ugrave", 249},
		{"uacute", 250},
		{"ucirc", 251},
		{"uuml", 252},
		{"yacute", 253},
		{"thorn", 254},
		{"yuml", 255},

		{"quot", 34},
		{"amp", 38},
		{"lt", 60},
		{"gt", 62},
		{"OElig", 338},
		{"oelig", 339},
		{"Scaron", 352},
		{"scaron", 353},
		{"Yuml", 376},
		{"circ", 710},
		{"tilde", 732},
		{"ensp", 8194},
		{"emsp", 8195},
		{"thinsp", 8201},
		{"zwnj", 8204},
		{"zwj", 8205},
		{"lrm", 8206},
		{"rlm", 8207},
		{"ndash", 8211},
		{"mdash", 8212},
		{"lsquo", 8216},
		{"rsquo", 8217},
		{"sbquo", 8218},
		{"ldquo", 8220},
		{"rdquo", 8221},
		{"bdquo", 8222},
		{"dagger", 8224},
		{"Dagger", 8225},
		{"permil", 8240},
		{"lsaquo", 8249},
		{"rsaquo", 8250},
		{"euro", 8364},

		{"fnof", 402},
		{"Alpha", 913},
		{"Beta", 914},
		{"Gamma", 915},
		{"Delta", 916},
		{"Epsilon", 917},
		{"Zeta", 918},
		{"Eta", 919},
		{"Theta", 920},
		{"Iota", 921},
		{"Kappa", 922},
		{"Lambda", 923},
		{"Mu", 924},
		{"Nu", 925},
		{"Xi", 926},
		{"Omicron", 927},
		{"Pi", 928},
		{"Rho", 929},
		{"Sigma", 931},
		{"Tau", 932},
		{"Upsilon", 933},
		{"Phi", 934},
		{"Chi", 935},
		{"Psi", 936},
		{"Omega", 937},
		{"alpha", 945},
		{"beta", 946},
		{"gamma", 947},
		{"delta", 948},
		{"epsilon", 949},
		{"zeta", 950},
		{"eta", 951},
		{"theta", 952},
		{"iota", 953},
		{"kappa", 954},
		{"lambda", 955},
		{"mu", 956},
		{"nu", 957},
		{"xi", 958},
		{"omicron", 959},
		{"pi", 960},
		{"rho", 961},
		{"sigmaf", 962},
		{"sigma", 963},
		{"tau", 964},
		{"upsilon", 965},
		{"phi", 966},
		{"chi", 967},
		{"psi", 968},
		{"omega", 969},
		{"thetasym", 977},
		{"upsih", 978},
		{"piv", 982},
		{"bull", 8226},
		{"hellip", 8230},
		{"prime", 8242},
		{"Prime", 8243},
		{"oline", 8254},
		{"frasl", 8260},
		{"weierp", 8472},
		{"image", 8465},
		{"real", 8476},
		{"trade", 8482},
		{"alefsym", 8501},
		{"larr", 8592},
		{"uarr", 8593},
		{"rarr", 8594},
		{"darr", 8595},
		{"harr", 8596},
		{"crarr", 8629},
		{"lArr", 8656},
		{"uArr", 8657},
		{"rArr", 8658},
		{"dArr", 8659},
		{"hArr", 8660},
		{"forall", 8704},
		{"part", 8706},
		{"exist", 8707},
		{"empty", 8709},
		{"nabla", 8711},
		{"isin", 8712},
		{"notin", 8713},
		{"ni", 8715},
		{"prod", 8719},
		{"sum", 8721},
		{"minus", 8722},
		{"lowast", 8727},
		{"radic", 8730},
		{"prop", 8733},
		{"infin", 8734},
		{"ang", 8736},
		{"and", 8743},
		{"or", 8744},
		{"cap", 8745},
		{"cup", 8746},
		{"int", 8747},
		{"there4", 8756},
		{"sim", 8764},
		{"cong", 8773},
		{"asymp", 8776},
		{"ne", 8800},
		{"equiv", 8801},
		{"le", 8804},
		{"ge", 8805},
		{"sub", 8834},
		{"sup", 8835},
		{"nsub", 8836},
		{"sube", 8838},
		{"supe", 8839},
		{"oplus", 8853},
		{"otimes", 8855},
		{"perp", 8869},
		{"sdot", 8901},
		{"lceil", 8968},
		{"rceil", 8969},
		{"lfloor", 8970},
		{"rfloor", 8971},
		{"lang", 9001},
		{"rang", 9002},
		{"loz", 9674},
		{"spades", 9824},
		{"clubs", 9827},
		{"hearts", 9829},
		{"diams", 9830}
	};
	typedef std::map<GUIString,int,StringLessNoCase> EntityMap; 
	static EntityMap anEntityMap;
	if(anEntityMap.empty())
	{
		int aNumEntities = sizeof(anEntityTable)/sizeof(EntityDefine);
		for(int i=0; i<aNumEntities; i++)
			anEntityMap[anEntityTable[i].mName] = anEntityTable[i].mEntity;
	}
	EntityMap::iterator anItr = anEntityMap.find(anEntity);
	if(anItr!=anEntityMap.end())
		return anItr->second;	
	else
	{
		UngetChar(';');
		UngetString(anEntity);
		return -1;
	}


	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00743280
bool HTMLParser::ReadWord(GUIString &theWord,  const char *theStopChars, bool stopAtSpace, bool skip, bool erase)
{
	if(erase)
	{
		theWord.resize(0);
		theWord.reserve(32);
	}

	if(stopAtSpace)
		SkipWhitespace();

	while(true)
	{
		int aChar = GetChar();
		if(aChar==-1)
			break;

		if(aChar<256 && ((stopAtSpace && ((unsigned int)aChar<256 && isspace(aChar)))  || (theStopChars!=NULL && strchr(theStopChars,aChar))))
		{
			if(aChar!=0)
			{
				UngetChar(aChar);
				break;
			}
		}

		if(aChar=='&')
		{
			aChar = ReadEntity();
			if(!skip)
			{
				if(aChar!=-1)
					theWord.append(aChar);
				else
					theWord.append('&');
			}
		}
		else
		{
			if(!skip)
				theWord.append(aChar);
		}
	}

	return !(theWord.length()==0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007435e0
void HTMLParser::SkipTag(const GUIString &theName)
{
	const int BUFFER_SIZE = 512;
	GUIString aSearch = "</";
	aSearch.append(theName);
	aSearch.append('>');
	aSearch.toUpper();

	GUIString aBuf(new GUIStringData((unsigned short)(BUFFER_SIZE+1)));
	GUIString aCapBuf(new GUIStringData((unsigned short)(BUFFER_SIZE+1)));

	while(!EndOfFile())
	{
		aBuf.resize(0);
		aCapBuf.resize(0);
		for(int i=0; i<BUFFER_SIZE; i++)
		{
			int aChar = GetChar();
			if(aChar<0)
				break;
			
			aCapBuf.append(toupper(aChar));
			aBuf.append(aChar);
		}
		
		int anEndPos = aCapBuf.find(aSearch,0);
		if(anEndPos!=-1)
		{
			UngetString(aBuf,anEndPos+aSearch.length());
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007438a0
bool HTMLParser::GetActualTag()
{
	mTagAttributes.Clear();
	bool isEndTag = false;
	GUIString aTagName;

	if(PeekChar()=='/')
	{
		GetChar(); // skip '/'
		isEndTag = true;
	}

	ReadWord(aTagName,">");
	if(aTagName.length()>=3 && aTagName[0]=='!' && aTagName[1]=='-' && aTagName[2]=='-') // comment
	{
		// Skip comment
		if(aTagName.length()>2 && aTagName[aTagName.length()-2]=='-' && aTagName[aTagName.length()-1]=='-' && PeekChar()=='>')
		{
			GetChar(); // skip >
			return true;
		}

		while(!EndOfFile())
		{
			int char1 = GetChar();
			int char2 = GetChar();
			int char3 = GetChar();
			if(char1=='-' && char2=='-' && char3=='>')
				return true;

			UngetChar(char3);
			UngetChar(char2);
		}

		return false;
	}

	TagNameMap::iterator anItr = mTagNameMap.find(aTagName);
	if(anItr==mTagNameMap.end()) // unrecognized tag... just skip it
	{
		GUIString aSkip;
		ReadWord(aSkip,">",false,true);
		GetChar(); // skip '>'
		return true;
	}

	TagLookupInfo &anInfo = anItr->second;	
	HTMLTagPtr aTag = anInfo.mFactoryFunc(anInfo.mType);
	aTag->SetLineNum(mLineNum);

	if(isEndTag) // end tag has no parameters
	{
		aTag->SetFlags(HTMLTagFlag_IsEndTag,true);
		GUIString aSkip;
		ReadWord(aSkip,">",false,true);
		GetChar(); // skip '>'
		mDocument->AddTag(aTag.get());
		return true;
	}

	while(!EndOfFile())
	{
		if(PeekChar()=='>')
		{
			GetChar(); // skip '>'
			break;
		}

		GUIString anAttribute;
		GUIString aValue;

		ReadWord(anAttribute,">=");
		SkipWhitespace();
		if(PeekChar()=='=') // read attribute value
		{
			GetChar(); // skip '=' sign
			SkipWhitespace();
			if(PeekChar()=='"')
			{
				GetChar(); // Skip '"' sign
				ReadWord(aValue,"\"",false);
				GetChar(); // Skip '"' sign
			}
			else if(PeekChar()=='\'')
			{
				GetChar(); // Skip ' sign
				ReadWord(aValue,"'",false);
				GetChar(); // Skip ' sign
			}
			else
				ReadWord(aValue,">");
		}
		mTagAttributes.AddAttribute(anAttribute,aValue);

	}

	if(aTag->FlagSet(HTMLTagFlag_SkipToEndTag))
		SkipTag(aTagName);
	else
		aTag->ParseAttributes(mTagAttributes);

	mDocument->AddTag(aTag.get());
	return true;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007443f0
bool HTMLParser::GetNextTag()
{
	GUIString aWord;
	ReadWord(aWord,"<",false);
	if(!(aWord.length()==0))
	{
		HTMLTagPtr aTag = new HTMLTextTag(aWord);
		aTag->SetLineNum(mLineNum);
		mDocument->AddTag(aTag.get());
	}
	if(PeekChar()!='<')
		return false;

	GetChar();
	bool aVal = GetActualTag();
	if(aVal)
	{
	// in HTML supposedly a carriage return directly after a tag is supposed to be ignored
	// (as well as one directly before a tag)
	//		int aChar = PeekChar();
	//		if(aChar=='\n')
	//			GetChar();
	}
	return aVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007446e0
bool HTMLParser::ParseFromFile(const GUIString &theFileName)
{
	CloseFile();

	if((theFileName.empty()))
		return false;

	std::string aFileName = theFileName;
	mFile=fopen(aFileName.c_str(),"r");
	if(mFile==NULL)
		return false;

	return Parse();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00744850
bool HTMLParser::ParseFromText(const GUIString &theText)
{
	CloseFile();

	mBackStr.resize(0);
	mBackStr.CopyOnWrite();
	mBackStr.mData->Reserve(theText.length(),false);
	for(int i = theText.length()-1; i>=0; i--)
		mBackStr.append(theText[i]);

	return Parse();
}

// FUNCTION: WON 0x007445f0
bool HTMLParser::Parse()
{
    mLineNum=1;
    if(mDocument.get()==NULL) mDocument=new HTMLDocument(gEmptyGUIString);
    else mDocument->Clear();
    while(!EndOfFile())
        if(!GetNextTag()) break;
    CloseFile();
    return true;
}

namespace WONAPI {
// Genuine original FILE-based parser constructor; shared registry is initialized
// through the existing actualSDK provider, with natural member construction.
// FUNCTION: WON 0x00742ae0
HTMLParser::HTMLParser() :mTagNameMap(GetTagNameMap())
{
    mLineNum=0;
    mFile=NULL;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00742a20
// (public: __thiscall std::map<class WONAPI::GUIString,struct WONAPI::HTMLParser::TagLookupInfo,struct WONAPI::GUIStringLessNoCase,class std::allocator<s)
// TEMPLATE: WON 0x00742f10
// (public: void __thiscall WONAPI::GUIString::append(int))
// TEMPLATE: WON 0x0074b300
// (public: struct WONAPI::HTMLParser::TagLookupInfo & __thiscall std::map<class WONAPI::GUIString,struct WONAPI::HTMLParser::TagLookupInfo,struct WONAPI:)
// TEMPLATE: WON 0x0074b470
// (public: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,int>,struct std::_Mapk<class WONAPI::GUIString,int,st)
// TEMPLATE: WON 0x0074cc40
// (public: struct std::pair<class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::HTMLParser::TagLookup)
// TEMPLATE: WON 0x0074cd40
// (public: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::HTMLParser::TagLookupInfo>,struct std:)
// TEMPLATE: WON 0x0074cd90
// (public: __thiscall std::pair<class WONAPI::GUIString const ,struct WONAPI::HTMLParser::TagLookupInfo>::pair<class WONAPI::GUIString const ,struct WONA)
// TEMPLATE: WON 0x0074cdd0
// (public: struct std::pair<class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,int>,struct std::_Mapk<class WONAPI:)
// TEMPLATE: WON 0x00750e00
// (public: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::HTMLParser::TagLookupInfo>,struct std:)
// TEMPLATE: WON 0x007512f0
// (protected: void __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::HTMLParser::TagLookupInfo)
// TEMPLATE: WON 0x00751330
// (protected: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::HTMLParser::TagLookupInfo>,struct s)
// TEMPLATE: WON 0x00751630
// (public: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,int>,struct std::_Mapk<class WONAPI::GUIString,int,st)
// TEMPLATE: WON 0x00751b20
// (protected: void __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,int>,struct std::_Mapk<class WONAPI::GUI)
// TEMPLATE: WON 0x00751e60
// (protected: struct std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,int>,struct std::_Mapk<class WONAPI::GUIString,in)
// TEMPLATE: WON 0x00754dd0
// (void __cdecl std::_Construct(struct std::pair<class WONAPI::GUIString const ,struct WONAPI::HTMLParser::TagLookupInfo> *,struct std::pair<class WONAPI)
// TEMPLATE: WON 0x0074cbf0
// (public: __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::HTMLParser::TagLookupInfo>,struct)
// TEMPLATE: WON 0x00751b60
// (protected: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,int>,struct std::_Mapk<class WONAPI::GUIString,int)
// TEMPLATE: WON 0x007f1b60
// (public: __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,int>,struct std::_Mapk<class WONAPI::GUIString,i)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007022d0
// (public: void __thiscall WONAPI::GUIStringData::append(int))
// TEMPLATE: WON 0x0070a1d0
// (public: void __thiscall WONAPI::GUIStringData::resize(int,bool))
