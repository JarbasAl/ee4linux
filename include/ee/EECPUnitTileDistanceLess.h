// The units by the distance of their tiles from mCenter, the nearer first (0x2cf1e, EEComputerPlayer.cpp; built without
// /Og). The computer player's sort COMDATs 0x44005c.., EECPBuildManager's gate sort 0x46c2cc.. and EECPResourceManager's
// 0x4920ec.. call it (not EETileDistanceLess's farther-first 0x21e104). Not an EETileDistanceLess: the sort COMDATs copy
// it field by field (no user copy ctor). DistanceSquared ICF-folds into 0x8a3f (body in EEComputerPlayer.cpp).
// Name is provisional. Requires EEUnit / EETileCoordinate. Only the AI Thread EECP* units include it.
#ifndef EE_EECPUNITTILEDISTANCELESS_H
#define EE_EECPUNITTILEDISTANCELESS_H

struct EECPUnitTileDistanceLess {
    EECPUnitTileDistanceLess(const EETileCoordinate& inCenter) : mCenter(inCenter) {}
    // 0x8a3f (ICF). Inline: VC6's /Os post-check undoes its expansion at the no-/Og call sites (0x2cf1e), so the calls
    // stay out of line while its two locals keep their caller frame slots
    long DistanceSquared(const EETileCoordinate& inTile) const
    {
        long theDY = inTile.GetY();
        theDY = this->mCenter.mY - theDY;
        long theDX = inTile.GetX();
        theDX = this->mCenter.mX - theDX;
        theDX *= theDX;
        theDY *= theDY;
        return theDX + theDY;
    }
    bool operator()(EEUnit* inFirst, EEUnit* inSecond) const;

    EETileCoordinate mCenter;
};

#endif
