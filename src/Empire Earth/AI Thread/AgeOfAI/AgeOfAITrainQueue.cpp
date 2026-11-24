// The train_queue rule of Grille's Empire Earth Age of AI (dll/ai/econ.c queue_room and train_call at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0). EECPUnitManager::StartProduction gathers the finished
// producers of the requested type and calls TrainQueueCap with their free slots; citizens are left to the game.
#include <windows.h>

#include "AgeOfAI/AgeOfAI.h"
#include "AgeOfAI/AgeOfAIRoles.h"
#include "AgeOfAI/AgeOfAISettings.h"
#include "AgeOfAI/AgeOfAITrainQueueRules.h"
#include "lle/LowLevelEngine.h"
#include "ee/EEUnit.h"

namespace AgeOfAI {
namespace {

struct TrainQueueState {
    TrainQueueCounts mCounts;
};

TrainQueueState sTrainQueue[16];

}  // namespace

TrainQueueCounts TrainQueueCountsOf(unsigned long inPlayerID)
{
    return sTrainQueue[inPlayerID & 15].mCounts;
}

bool TrainQueueOn(unsigned long inPlayerID, EEUnitClass* inClass)
{
    if (inClass == NULL || EngineOf(inPlayerID) != kEngineAgeOfAI)
        return false;
    if (SettingsAIOf(inPlayerID, kDifficultyMedium).train_queue <= 0)
        return false;
    return (Roles::RolesOf(inClass->mCategory, inClass->mFamily, inClass->mRange) & Roles::kCitizen) == 0;
}

TrainQueueRules::Decision TrainQueueCap(unsigned long inPlayerID, long inCount, long inRoom, bool inHaveProducers,
                                         long& outCount)
{
    TrainQueueRules::Decision theDecision = TrainQueueRules::Decide(inCount, inRoom, inHaveProducers, outCount);
    TrainQueueCounts& theCounts = sTrainQueue[inPlayerID & 15].mCounts;
    theCounts.mCalls++;
    if (theDecision == TrainQueueRules::kCut) {
        theCounts.mCuts++;
        theCounts.mCutUnits += (unsigned long)(inCount - outCount);
    } else if (theDecision == TrainQueueRules::kRefuse) {
        theCounts.mRefusals++;
    }
    return theDecision;
}

}  // namespace AgeOfAI
