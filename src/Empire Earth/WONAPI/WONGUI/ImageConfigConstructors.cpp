// Genuine explicit SDK image configuration constructors; support0 only.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyConfigImages.h"
namespace WONAPI {
// FUNCTION: WON 0x0078b2b0
ImageConfig::ImageConfig()
{
    mHaveInitedComponents=true;
}
// FUNCTION: WON 0x0078c260
ImageFilterConfig::ImageFilterConfig()
{
    mImageIsRawImage=false;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0078b2e0
// (public: virtual void * __thiscall WONAPI::ImageConfig::`scalar deleting destructor'(unsigned int)    [??_GImageConfig@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0078c290
// (public: virtual void * __thiscall WONAPI::ImageFilterConfig::`scalar deleting destructor'(unsigned int)    [??_GImageFilterConfig@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0078c2b0
// (public: virtual __thiscall WONAPI::ImageFilterConfig::~ImageFilterConfig(void)    [??1ImageFilterConfig@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0078c280
// (public: virtual class WONAPI::RawImage * __thiscall WONAPI::ImageFilterConfig::GetRawImage(void))
