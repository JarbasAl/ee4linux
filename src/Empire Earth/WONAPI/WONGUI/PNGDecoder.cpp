// Native image codec adapters; source-only, no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <setjmp.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyAnimation.h"
#include "ee/wonapi/WONLobbyImageDecoder.h"
#include "ee/wonapi/codec/png.h"
using namespace WONAPI;

// FUNCTION: WON 0x007d2630
static void png_filereader_read_data(png_structp png,png_bytep data,png_size_t length)
{
    int count=((FileReader*)png->io_ptr)->ReadMaxBytes(data,length);
    if(count!=length)
        png_error(png,"Read Error");
}
namespace WONAPI {
// Native registration107 at3d24e0 andDuplicate80 support0, complete fieldless8 owners.
// FUNCTION: WON 0x007d24e0
void PNGDecoder::Add(MultiImageDecoder* decoder)
{
    PNGDecoder* png=new PNGDecoder;
    decoder->AddDecoder(png,"png");
}
// FUNCTION: WON 0x007d29d0
ImageDecoder* PNGDecoder::Duplicate() { return new PNGDecoder; }

// FUNCTION: WON 0x007d2550
bool PNGDecoder::DoGetAttrib(ImageAttributes& attributes)
{
    png_structp png=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
    if(png==NULL) return false;
    png_infop info=png_create_info_struct(png);
    if(info==NULL) {
        png_destroy_read_struct(&png,NULL,NULL);
        return false;
    }
    if(setjmp(png->jmpbuf)) {
        png_destroy_read_struct(&png,&info,NULL);
        return false;
    }
    png_set_read_fn(png,mFileReader,png_filereader_read_data);
    png_read_info(png,info);
    png_uint_32 width,height;
    int bitDepth,colorType,interlaceType;
    png_get_IHDR(png,info,&width,&height,&bitDepth,&colorType,&interlaceType,NULL,NULL);
    attributes.mWidth=width;
    attributes.mHeight=height;
    png_destroy_read_struct(&png,&info,NULL);
    return true;
}

// FUNCTION: WON 0x007d2660
SmartPtr<RawImage> PNGDecoder::DoDecodeRaw()
{
    png_structp png=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
    if(png==NULL) return NULL;
    png_infop info=png_create_info_struct(png);
    if(info==NULL) {
        png_destroy_read_struct(&png,NULL,NULL);
        return NULL;
    }
    if(setjmp(png->jmpbuf)) {
        png_destroy_read_struct(&png,&info,NULL);
        return NULL;
    }
    png_set_read_fn(png,mFileReader,png_filereader_read_data);
    png_read_info(png,info);
    png_uint_32 width,height;
    int bitDepth,colorType,interlaceType;
    png_get_IHDR(png,info,&width,&height,&bitDepth,&colorType,&interlaceType,NULL,NULL);
    png_set_strip_16(png);
    png_set_packing(png);
    if(colorType==PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png);
    if(colorType==PNG_COLOR_TYPE_GRAY && bitDepth<8)
        png_set_expand(png);
    if(png_get_valid(png,info,PNG_INFO_tRNS))
        png_set_expand(png);
    // Target1.0.12 converts gray/gray-alpha and always reads RGBA rows.
    if(colorType==PNG_COLOR_TYPE_GRAY || colorType==PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);
    png_set_filler(png,0xff,PNG_FILLER_AFTER);
    png_bytepp rows=new png_bytep[height];
    // Original target queries rowbytes, then allocates actual width*4 per row.
    (void)png_get_rowbytes(png,info);
    int y;
    for(y=0;y<height;++y)
        rows[y]=new png_byte[width*4];
    png_read_image(png,rows);
    // Target directly allocates the complete concrete24B RawImage32 owner.
    SmartPtr<RawImage32> image=new RawImage32(width,height);
    if((colorType&PNG_COLOR_MASK_ALPHA) || png_get_valid(png,info,PNG_INFO_tRNS))
        image->SetDoTransparency(true);
    image->StartRowTraversal(0);
    for(y=0;y<height;++y) {
        png_bytep pixel=rows[y];
        for(int x=0;x<width;++x,pixel+=4) {
            DWORD color=(pixel[0]<<16)|(pixel[1]<<8)|pixel[2]|(pixel[3]<<24);
            image->SetRowPixel(color);
            image->NextRowPixel();
        }
        image->NextRow();
    }
    for(y=0;y<height;++y)
        delete [] rows[y];
    delete [] rows;
    png_destroy_read_struct(&png,&info,NULL);
    return image.get();
}
}
