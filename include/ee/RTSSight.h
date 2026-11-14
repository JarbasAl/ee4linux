// A tile's sight count for one player, in the 16-bit entry of RTSTile::mVisibility.
//
// The game's meaning: 0 unexplored, 1 explored, 2 + n while n of the player's units see the tile. Every reader takes
// only the entry's low byte, and the original add carries from 255 into the high byte, so the 256th unit made the low
// byte 0: the tile read as unexplored, and everything on it dropped out of the draw list. The count here is kept
// whole: the low byte runs 2..255 and the high byte counts the 254-unit blocks above that, so every reader of the low
// byte sees 0, 1 or 2..255 as before, and a count below 256 is stored exactly as the game stores it.
//
// From Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/engine/sight.c and
// dll/game/api/map.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef EE_RTSSIGHT_H
#define EE_RTSSIGHT_H

const unsigned long kRTSSightMax = 65025;   // 2 + 255 * 254 + 253: the entry 0xFFFF

// The count an entry holds
inline unsigned long RTSSightCount(unsigned long inEntry)
{
    unsigned long theLow = inEntry & 0xff;
    return theLow < 2 ? theLow : 2 + ((inEntry >> 8) & 0xff) * 254 + (theLow - 2);
}

// The entry that holds a count
inline unsigned long RTSSightEntry(unsigned long inCount)
{
    if (inCount < 2)
        return inCount;
    unsigned long theRest = inCount - 2;
    return ((theRest / 254) << 8) | (2 + theRest % 254);
}

// The count after one more unit sees the tile: an unexplored or explored tile becomes seen by one; the top stays
inline unsigned long RTSSightAdded(unsigned long inCount)
{
    return inCount < 2 ? 2 : inCount < kRTSSightMax ? inCount + 1 : inCount;
}

// The count after one unit stops seeing it: the last one leaves the tile explored
inline unsigned long RTSSightRemoved(unsigned long inCount)
{
    return inCount > 1 ? inCount - 1 : inCount;
}

#endif
