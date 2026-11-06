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
// FUNCTION: WON 0x0074a530
SoundDecoder::SoundDecoder()
{
}

// SUPPORT-ONLY: native328590/7, explicit SDK empty body then RefCount cleanup.
SoundDecoder::~SoundDecoder()
{
}

// FUNCTION: WON 0x0074a540
SoundPtr SoundDecoder::DecodeDescriptor(const SoundDescriptor& inDescriptor)
{
	return Decode(inDescriptor);
}
}
