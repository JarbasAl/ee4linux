// Genuine SDK explicit SoundDecoder lifetimes; native/current older virtual Decode API retained.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/EELobbySound.h"
namespace WONAPI {
// SUPPORT-ONLY: native34a530/16, complete RefCount8 constructor.
SoundDecoder::SoundDecoder()
{
}

// SUPPORT-ONLY: native328590/7, explicit SDK empty body then RefCount cleanup.
SoundDecoder::~SoundDecoder()
{
}
}
