// The compact list of unit ids that a selection message carries (EEMSelectUnits::mBuffer, 0x100 bytes, its length one
// byte). Byte 0 is the owner (bits 24..31 of every id). Then runs of units whose ids share bits 8..23: a header byte
// with the unit count n in its low 6 bits (n <= 63) and in its top 2 bits how many bytes of the shared bits follow
// (0: they are zero; 1: one byte, bits 8..15; 2: two bytes, bits 8..23), then n bytes of each id's bits 0..7. An empty
// list is the single byte 0xFF.
//
// The game packed at most 63 units. SelectionPack packs as many as fit into the 255 bytes the length byte allows (about
// 240, fewer with scattered ids), from a position in the list, so the rest can go in later messages of the kind that
// adds to the selection. The format is the game's own, so the game's unpacker reads it (multiplayer).
//
// Idea and format from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI),
// dll/engine/select.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef EE_EESELECTIONPACK_H
#define EE_EESELECTIONPACK_H

#include <stddef.h>
#include <map>
#include <vector>

const size_t kSelectionPackMax = 255;

// The ids in the order they are packed: by bits 8..23 ascending, each group in the order given
inline void SelectionOrder(const std::vector<unsigned long>& inIDs, std::vector<unsigned long>& outOrdered)
{
    std::map<unsigned long, std::vector<unsigned long> > theGroups;
    for (size_t i = 0; i < inIDs.size(); i++)
        theGroups[(inIDs[i] >> 8) & 0xffff].push_back(inIDs[i]);
    outOrdered.clear();
    for (std::map<unsigned long, std::vector<unsigned long> >::iterator it = theGroups.begin(); it != theGroups.end(); ++it)
        outOrdered.insert(outOrdered.end(), it->second.begin(), it->second.end());
}

// Packs the ordered ids from inFirst on into outBuffer (kSelectionPackMax bytes at most); ioPacked is the number of ids
// packed. Returns the length. An id whose group header and one id byte do not fit ends the list.
inline unsigned SelectionPack(const std::vector<unsigned long>& inOrdered, size_t inFirst, unsigned long inOwner,
                              unsigned char* outBuffer, size_t& outPacked)
{
    size_t thePos = 0;
    outBuffer[thePos++] = (unsigned char)inOwner;
    size_t i = inFirst;
    while (i < inOrdered.size()) {
        unsigned long theGroup = (inOrdered[i] >> 8) & 0xffff;
        size_t theRun = 0;
        while (i + theRun < inOrdered.size() && theRun < 63 && ((inOrdered[i + theRun] >> 8) & 0xffff) == theGroup)
            theRun++;
        size_t theHead = theGroup == 0 ? 1 : theGroup <= 0xff ? 2 : 3;
        if (thePos + theHead + 1 > kSelectionPackMax)
            break;
        if (thePos + theHead + theRun > kSelectionPackMax)
            theRun = kSelectionPackMax - thePos - theHead;
        if (theGroup == 0)
            outBuffer[thePos++] = (unsigned char)theRun;
        else if (theGroup <= 0xff) {
            outBuffer[thePos++] = (unsigned char)(0x40 | theRun);
            outBuffer[thePos++] = (unsigned char)theGroup;
        } else {
            outBuffer[thePos++] = (unsigned char)(0x80 | theRun);
            outBuffer[thePos++] = (unsigned char)(theGroup >> 8);
            outBuffer[thePos++] = (unsigned char)theGroup;
        }
        for (size_t k = 0; k < theRun; k++)
            outBuffer[thePos++] = (unsigned char)(inOrdered[i + k] & 0xff);
        i += theRun;
    }
    if (thePos == 1)
        outBuffer[thePos++] = 0xff;
    outPacked = i - inFirst;
    return (unsigned)thePos;
}

#endif
