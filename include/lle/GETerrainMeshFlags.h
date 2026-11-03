// lle/GETerrainMeshFlags.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_GETERRAINMESHFLAGS_H
#define EE_GETERRAINMESHFLAGS_H

// GETMTile::mWaterInfo (+0x03): bit 0 water, bits 1..4 the shoreline directions (ComputeShorelines 0x1002795f ORs sNeighbours[i].mShoreline), bit 7 space
enum GETMWaterInfoFlag {
    kWaterInfoWater = 1,           // SetIsWater sets it; ComputeShorelines starts a water tile at 1 (provisional name)
    kWaterInfoShorelineMask = 0x1e,  // the shoreline bits (UpdateVisibleTerrain: code = (info >> 1) & 0xf) (provisional name)
    kWaterInfoWaterMask = 0x7f,    // every bit but kWaterInfoSpace (GetIsWaterTile, SetIsSpace) (provisional name)
    kWaterInfoSpace = 0x80,        // SetIsSpace sets it and counts mSpaceTileCount; GetIsSpaceTile tests it (provisional name)
};

// GETerrainRenderVertex::mClipFlags (+0x20): DX7Rasterizer::TransformToCamera sets one bit per view-frustum plane the camera-space vertex is outside of
enum GETerrainClipFlag {
    kVertexClipNear = 1,     // TransformToCamera: camera-space y <= the near distance (provisional name)
    kVertexClipFar = 2,      // TransformToCamera: y >= the far distance (provisional name)
    kVertexClipLeft = 4,     // TransformToCamera: on or below the viewport's left plane (provisional name)
    kVertexClipRight = 8,    // TransformToCamera: on or below the right plane (provisional name)
    kVertexClipTop = 0x10,   // TransformToCamera: on or below the top plane (provisional name)
    kVertexClipBottom = 0x20,  // TransformToCamera: on or below the bottom plane (provisional name)
};

#endif // EE_GETERRAINMESHFLAGS_H
