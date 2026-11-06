// Original-backed MD5-based random byte state and Windows seed collection.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRandom.h"
namespace WONAPI {
long Random::usageCount = 0;
// Native leaves randomBytesUsed and counter uninitialized; preserve actual source behavior.
// FUNCTION: WON 0x007da510
Random::Random()
{
    setSeed(getSeed(20));
}
// FUNCTION: WON 0x007da5e0
void Random::setSeed(const RawBuffer& seed)
{
    if (state.length() != 0)
        digest.update(state);
    state = digest.digest(seed);
}
// FUNCTION: WON 0x007da770
void Random::nextBytes(RawBuffer& bytes)
{
    int requested = bytes.length();
    int received = 0;
    while (received < requested) {
        if (randomBytes.length() == 0 || randomBytesUsed == randomBytes.length()) {
            digest.update(state);
            randomBytes = digest.digest(longToByteArray(counter++));
            randomBytesUsed = 0;
        }
        bytes[received++] = randomBytes[randomBytesUsed++];
    }
}
// Requested seed size is intentionally ignored by this native method.
// FUNCTION: WON 0x007da950
RawBuffer Random::getSeed(int)
{
    RawBuffer bytes(8, static_cast<unsigned char>(0));
    long usage = ++usageCount;
    unsigned long sectorsPerCluster, bytesPerSector, freeClusters, usedClusters;
    __int64 seed = 0;
    GetDiskFreeSpaceA(NULL, &sectorsPerCluster, &bytesPerSector, &freeClusters, &usedClusters);
    usedClusters -= freeClusters;
    seed = (usedClusters < freeClusters ? freeClusters : usedClusters) * sectorsPerCluster * bytesPerSector;
    unsigned long tick = GetTickCount();
    *reinterpret_cast<unsigned short*>(&seed) = reinterpret_cast<unsigned short*>(&tick)[1];
    ULARGE_INTEGER systemTime;
    GetSystemTimeAsFileTime(reinterpret_cast<FILETIME*>(&systemTime));
    seed ^= systemTime.QuadPart;
    seed += usage;
    return bytes.assign(reinterpret_cast<const unsigned char*>(&seed), 8);
}
// FUNCTION: WON 0x007dab20
RawBuffer Random::longToByteArray(__int64 value)
{
    RawBuffer bytes(8, static_cast<unsigned char>(0));
    for (int i=0; i<8; ++i) {
        bytes[i] = static_cast<unsigned char>(value);
        value >>= 8;
    }
    return bytes;
}
}

namespace WONAPI {

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent overload; no Native credit.
Random::Random(const RawBuffer& seed) 
{
	setSeed(seed);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent overload; no Native credit.
void Random::setSeed(__int64 seed) 
{
	if (seed != 0)
		setSeed(longToByteArray(seed));
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent overload; no Native credit.
void Random::nextBytes(void* data, int theLen)
{
	char *bytes = (char*)data;

	int numRequested = theLen;
	int numGot = 0;

	while (numGot < numRequested) 
	{
		// If no more random bytes, make some more
		if (randomBytes.length() == 0 || randomBytesUsed == randomBytes.length()) 
		{
			digest.update(state);
			randomBytes = digest.digest(longToByteArray(counter++));
			randomBytesUsed = 0;
		}

		bytes[numGot++] = randomBytes[randomBytesUsed++];
	}
	
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent overload; no Native credit.
unsigned int Random::next(int numBits) 
{
	int numBytes = (numBits+7)/8;
	RawBuffer b(numBytes,(unsigned char)0);
	
	unsigned int next = 0;

	nextBytes(b);
	for (int i=0; i<numBytes; i++)
		next = (next << 8) + (b[i] & 0xFF);

		return next >> (numBytes*8 - numBits);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007dade0
// (public: class std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> > & __thiscall std::basic_str)
