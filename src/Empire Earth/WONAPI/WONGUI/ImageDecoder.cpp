// Source-only logical image decoders, no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyAnimation.h"
#include "ee/wonapi/WONLobbyImageDecoder.h"
namespace WONAPI {
ImageAttributes::ImageAttributes()
{
    mWidth=mHeight=0;
}
ImageDecoder::ImageDecoder()
{
    mFileReader=NULL;
}
ImageDecoder::~ImageDecoder() {}
SmartPtr<Animation> ImageDecoder::GetAnimation() { return NULL; }
// FUNCTION: WON 0x00747910
// (??_GImageDecoder@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00747930
// (??1ImageDecoder@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x00747940
SmartPtr<NativeImage> ImageDecoder::DoDecode(DisplayContext* context)
{
    SmartPtr<RawImage> image=DoDecodeRaw();
    if(image.get()!=NULL) return image->GetNative(context);
    return NULL;
}
SmartPtr<NativeImage> ImageDecoder::Decode(DisplayContext* context,const char* path)
{
    FileReader reader(path);
    return Decode(context,reader);
}
SmartPtr<NativeImage> ImageDecoder::Decode(DisplayContext* context,FileReader& reader)
{
    if(reader.mFile==NULL && reader.mBuffer==NULL) return NULL;
    mFileReader=&reader;
    return DoDecode(context);
}
SmartPtr<RawImage> ImageDecoder::DecodeRaw(const char* path)
{
    FileReader reader(path);
    return DecodeRaw(reader);
}
SmartPtr<RawImage> ImageDecoder::DecodeRaw(FileReader& reader)
{
    if(reader.mFile==NULL && reader.mBuffer==NULL) return NULL;
    mFileReader=&reader;
    return DoDecodeRaw(); //target has no FileReaderException catch in this overload
}
bool ImageDecoder::GetImageAttributes(const char* path,ImageAttributes& attributes)
{
    FileReader reader(path);
    return GetImageAttributes(reader,attributes);
}
// FUNCTION: WON 0x007479f0
// (?Decode@ImageDecoder@WONAPI@@QAE?AV?$SmartPtr@VNativeImage@WONAPI@@@2@PAVDisplayContext@2@PBD@Z)

// FUNCTION: WON 0x00747a70
// (?Decode@ImageDecoder@WONAPI@@QAE?AV?$SmartPtr@VNativeImage@WONAPI@@@2@PAVDisplayContext@2@AAVFileReader@2@@Z)

// FUNCTION: WON 0x00747ab0
// (?DecodeRaw@ImageDecoder@WONAPI@@QAE?AV?$SmartPtr@VRawImage@WONAPI@@@2@PBD@Z)

// FUNCTION: WON 0x00747b30
// (?DecodeRaw@ImageDecoder@WONAPI@@QAE?AV?$SmartPtr@VRawImage@WONAPI@@@2@AAVFileReader@2@@Z)

// FUNCTION: WON 0x00747b70
// (?GetImageAttributes@ImageDecoder@WONAPI@@QAE_NPBDAAVImageAttributes@2@@Z)

// FUNCTION: WON 0x00747be0
bool ImageDecoder::GetImageAttributes(FileReader& reader,ImageAttributes& attributes)
{
    if(reader.mFile==NULL && reader.mBuffer==NULL) return false;
    mFileReader=&reader;
    try { return DoGetAttrib(attributes); }
    catch(FileReaderException&) {}
    return false;
}
MultiImageDecoder::MultiImageDecoder() {}
// FUNCTION: WON 0x00747c50
// (??0MultiImageDecoder@WONAPI@@QAE@XZ)

// FUNCTION: WON 0x00747d20
// (??_GMultiImageDecoder@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00747d40
// (??1?$set@PAVImageDecoder@WONAPI@@U?$less@PAVImageDecoder@WONAPI@@@std@@V?$allocator@PAVImageDecoder@WONAPI@@@4@@std@@QAE@XZ)

// FUNCTION: WON 0x00747eb0
MultiImageDecoder::~MultiImageDecoder()
{
    DecoderSet::iterator i=mDecoderSet.begin();
    while(i!=mDecoderSet.end())
    {
        delete *i;
        ++i;
    }
}
// FUNCTION: WON 0x00748080
void MultiImageDecoder::AddDecoder(ImageDecoder* decoder,const char* extension)
{
    // Target retains all distinct pointers, then replaces this extension's map value.
    mDecoderSet.insert(decoder);
    mDecoderMap[extension]=decoder;
}
// FUNCTION: WON 0x00748270
void MultiImageDecoder::SetFileName(const char* path)
{
    WONFile file(path);
    SetCurExtension(file.GetFileExtension().c_str());
}
// FUNCTION: WON 0x00748450
SmartPtr<NativeImage> MultiImageDecoder::Decode(DisplayContext* context,const char* path)
{
    SetFileName(path);
    return ImageDecoder::Decode(context,path);
}
// FUNCTION: WON 0x00748490
SmartPtr<RawImage> MultiImageDecoder::DecodeRaw(const char* path)
{
    SetFileName(path);
    return ImageDecoder::DecodeRaw(path);
}
// FUNCTION: WON 0x007484c0
bool MultiImageDecoder::GetImageAttributes(const char* path,ImageAttributes& attributes)
{
    SetFileName(path);
    return ImageDecoder::GetImageAttributes(path,attributes);
}
// FUNCTION: WON 0x007484e0
SmartPtr<NativeImage> MultiImageDecoder::DoDecode(DisplayContext* context)
{
    mAnimation=NULL;
    DecoderMap::iterator i=mDecoderMap.find(mCurExtension);
    if(i==mDecoderMap.end()) return NULL;
    ImageDecoder* decoder=i->second;
    SmartPtr<NativeImage> image=decoder->Decode(context,*mFileReader);
    if(image.get()!=NULL) mAnimation=decoder->GetAnimation();
    return image;
}
// FUNCTION: WON 0x00748670
SmartPtr<RawImage> MultiImageDecoder::DoDecodeRaw()
{
    mAnimation=NULL;
    DecoderMap::iterator i=mDecoderMap.find(mCurExtension);
    if(i==mDecoderMap.end()) return NULL;
    ImageDecoder* decoder=i->second;
    SmartPtr<RawImage> image=decoder->DecodeRaw(*mFileReader);
    if(image.get()!=NULL) mAnimation=decoder->GetAnimation();
    return image;
}
// FUNCTION: WON 0x007487f0
bool MultiImageDecoder::DoGetAttrib(ImageAttributes& attributes)
{
    DecoderMap::iterator i=mDecoderMap.find(mCurExtension);
    if(i==mDecoderMap.end()) return false;
    ImageDecoder* aDecoder=i->second;
    return aDecoder->GetImageAttributes(*mFileReader,attributes);
}
// FUNCTION: WON 0x00748870
SmartPtr<Animation> MultiImageDecoder::GetAnimation(bool evenIfOnlyOneFrame)
{
    if(evenIfOnlyOneFrame) return mAnimation;
    else if(mAnimation.get()==NULL || mAnimation->GetNumFrames()<=1) return NULL;
    else return mAnimation;
}
// FUNCTION: WON 0x007488e0
ImageDecoder* MultiImageDecoder::Duplicate()
{
    MultiImageDecoder* decoder=new MultiImageDecoder;
    decoder->mDecoderMap=mDecoderMap;
    DecoderMap::iterator i=decoder->mDecoderMap.begin();
    while(i!=decoder->mDecoderMap.end())
    {
        i->second=i->second->Duplicate();
        decoder->mDecoderSet.insert(i->second);
        ++i;
    }
    return decoder;
}
}


// Genuine SDK memory-source decoder overloads; SOURCE_ONLY_NO_NATIVE_ID.
namespace WONAPI {
SmartPtr<NativeImage> ImageDecoder::Decode(DisplayContext *theContext, const void *theData, unsigned long theDataLen)
{
	FileReader aReader(theData,theDataLen);
	return Decode(theContext, aReader);
}

SmartPtr<RawImage> ImageDecoder::DecodeRaw(const void *theData, unsigned long theDataLen)
{
	FileReader aReader(theData,theDataLen);
	return DecodeRaw(aReader);
}

bool ImageDecoder::GetImageAttributes(const void *theData, unsigned long theDataLen, ImageAttributes &theAttrib)
{
	FileReader aReader(theData,theDataLen);
	return GetImageAttributes(aReader,theAttrib);
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: existing genuine SDK helper signatures, proposed explicit merged-target allocation port.
SmartPtr<RawImage32> ImageDecoder::CreateRawImage32(int theWidth,int theHeight)
{
    return new RawImage32(theWidth,theHeight);
}
SmartPtr<RawImage8> ImageDecoder::CreateRawImage8(int theWidth,int theHeight,Palette* thePalette)
{
    return new RawImage8(theWidth,theHeight,thePalette);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00747900
// (public: __thiscall WONAPI::ImageDecoder::ImageDecoder(void))
// TEMPLATE: WON 0x00747df0
// (public: __thiscall std::map<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class WONAPI::ImageDecoder *,struc)
// TEMPLATE: WON 0x007478f0
// (public: __thiscall WONAPI::ImageAttributes::ImageAttributes(void))
// TEMPLATE: WON 0x0074b670
// (protected: class std::_Tree<class WONAPI::ImageDecoder *,class WONAPI::ImageDecoder *,struct std::_Setk<class WONAPI::ImageDecoder *,struct std::less<)
// TEMPLATE: WON 0x0074b970
// (public: struct std::pair<class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<cla)
// TEMPLATE: WON 0x0074ba70
// (public: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_str)
// TEMPLATE: WON 0x0074bb20
// (protected: void __thiscall std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class s)
// TEMPLATE: WON 0x0074cfb0
// (public: class std::_Tree<class WONAPI::ImageDecoder *,class WONAPI::ImageDecoder *,struct std::_Setk<class WONAPI::ImageDecoder *,struct std::less<cla)
// TEMPLATE: WON 0x0074d460
// (protected: void __thiscall std::_Tree<class WONAPI::ImageDecoder *,class WONAPI::ImageDecoder *,struct std::_Setk<class WONAPI::ImageDecoder *,struct )
// TEMPLATE: WON 0x0074d9a0
// (protected: struct std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic)
// TEMPLATE: WON 0x0074da30
// (protected: void __thiscall std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class s)
// TEMPLATE: WON 0x0074da70
// (protected: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_)
// TEMPLATE: WON 0x00751f80
// (protected: struct std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic)
// TEMPLATE: WON 0x0074d4c0
// (public: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_str)
// TEMPLATE: WON 0x00754c60
// (void __cdecl std::_Construct(struct std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WON)
