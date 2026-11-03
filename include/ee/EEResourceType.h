// ee/EEResourceType.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_EERESOURCETYPE_H
#define EE_EERESOURCETYPE_H

// EEUnit/EEUnitClass mResourceType (also the player's resource index, EEPlayer::AddResource).
enum EEResourceType {
    kResourceNone = 0,  // EEUnit::Init/ResetEEUnit store 0; `!= 0` tests 'is a resource' (EEUCResource::SimpleUnitCreated sets the amount only then) (provisional name)
    kResourceAny = 1,  // EEComplexUnit::CanTake 0x5f6a1a: a storage of type 1 takes every resource (mResourceType == 1 || == inValue); CanGather/status box exclude 0 and 1 (provisional name)
    kResourceFood = 2,  // the player's resource index 2..6 (mResourceCaps[7]; EEWorld::RecordStatistics pairs [2]/[3]/[4]/[5]/[6] with the starting food/wood/stone/gold/iron amounts); same ids as EEUnit::mResourceType (provisional name)
    kResourceWood = 3,  // the player's resource index 2..6 (mResourceCaps[7]; EEWorld::RecordStatistics pairs [2]/[3]/[4]/[5]/[6] with the starting food/wood/stone/gold/iron amounts); same ids as EEUnit::mResourceType (provisional name)
    kResourceStone = 4,  // the player's resource index 2..6 (mResourceCaps[7]; EEWorld::RecordStatistics pairs [2]/[3]/[4]/[5]/[6] with the starting food/wood/stone/gold/iron amounts); same ids as EEUnit::mResourceType (provisional name)
    kResourceGold = 5,  // the player's resource index 2..6 (mResourceCaps[7]; EEWorld::RecordStatistics pairs [2]/[3]/[4]/[5]/[6] with the starting food/wood/stone/gold/iron amounts); same ids as EEUnit::mResourceType (provisional name)
    kResourceIron = 6,  // the player's resource index 2..6 (mResourceCaps[7]; EEWorld::RecordStatistics pairs [2]/[3]/[4]/[5]/[6] with the starting food/wood/stone/gold/iron amounts); same ids as EEUnit::mResourceType (provisional name)
};

#endif // EE_EERESOURCETYPE_H
