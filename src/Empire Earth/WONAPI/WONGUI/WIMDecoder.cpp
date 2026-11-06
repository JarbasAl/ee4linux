// Source-only WIM image reader; no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <cstring>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyImageDecoder.h"
namespace WONAPI {
// FUNCTION: WON 0x0074a560
bool WIMDecoder::DoGetAttrib(ImageAttributes& attributes)
{
    char signature[4];
    mFileReader->ReadBytes(signature,3);
    signature[3]=0;
    if(strcmp(signature,"WIM")!=0) return false;
    if((unsigned long)mFileReader->ReadLong()!=32) return false;
    mFileReader->ReadByte();
    unsigned long width=mFileReader->ReadLong();
    unsigned long height=mFileReader->ReadLong();
    attributes.mWidth=width;
    attributes.mHeight=height;
    return true;
}
// FUNCTION: WON 0x0074a600
SmartPtr<RawImage> WIMDecoder::DoDecodeRaw()
{
    try
    {
        char signature[4];
        mFileReader->ReadBytes(signature,3);
        signature[3]=0;
        if(strcmp(signature,"WIM")!=0) return NULL;
        if((unsigned long)mFileReader->ReadLong()!=32) return NULL;
        bool transparency=mFileReader->ReadByte()!=0;
        unsigned long width=mFileReader->ReadLong();
        unsigned long height=mFileReader->ReadLong();
        unsigned long numBytes=width*height*4;
        if(mFileReader->Available()<numBytes) return NULL;

        SmartPtr<RawImage32> image=new RawImage32(width,height);
        image->mDoTransparency=transparency;
        mFileReader->ReadBytes(image->mImageData,numBytes);
        return image.get();
    }
    catch(FileReaderException&) {}
    return NULL;
}
// FUNCTION: WON 0x0072ba40
ImageDecoder* WIMDecoder::Duplicate() { return new WIMDecoder; } //ordinary support allocator0
}
