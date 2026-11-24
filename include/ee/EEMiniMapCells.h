// Cell indices of the 0x40 by 0x40 mini map battle grid. The map-to-cell transform rounds through the x87 unit,
// so a unit on the map's edge can land one cell outside the grid. The game's fixed arrays are indexed by these
// values, so an out-of-range cell would write before or after them.
#ifndef EE_EEMINIMAPCELLS_H
#define EE_EEMINIMAPCELLS_H

namespace EEMiniMapCells {

const long kGrid = 0x40;

inline long Clamp(long inCell)
{
    if (inCell < 0)
        return 0;
    if (inCell >= kGrid)
        return kGrid - 1;
    return inCell;
}

inline long Flatten(long inX, long inY)
{
    return Clamp(inX) * kGrid + Clamp(inY);
}

}

#endif
