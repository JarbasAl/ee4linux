// Spreading missiles ([general] spread_missiles, on in the shipped file). Every launcher picks its target on its own,
// so many anti-air towers fire at the same aircraft and most of their missiles hit a wreck. Each launched missile is
// counted with the damage its launcher will do (the game's own shot formula). Before a launcher fires, if the missiles
// already on the way cover the target's hit points, the shot is held or sent at the nearest other enemy of the same
// family in range that its owner can see and that is not covered yet. A missile frees its share again when it is gone.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/engine/aim.c
// (hook_missile_launch, missile_damage, hook_fire, spread_target) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07,
// GPL-3.0. Upstream's is_vanilla (a player of the classic AI in a test game) maps to our engine check; unlike lead
// targets, this setting reads every player's [general] rows, so a multiplayer game needs the same value on each side.
#include <windows.h>
#include <algorithm>
#include <map>
#include <math.h>

#include "lle/LowLevelEngine.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEPlayer.h"
#include "ee/EEUnit.h"
#include "ee/EEUCMissile.h"
#include "ee/EEUGAttackUnit.h"
#include "ee/EEPhysics.h"
#include "AgeOfAI.h"
#include "AgeOfAISettings.h"
#include "AgeOfAI/AgeOfAIStats.h"

namespace AgeOfAI {
namespace {

struct MissileLoad {
    EEUnit* mTarget;
    int mDamage;
    unsigned long mFirst, mLast;
};

std::map<unsigned long, MissileLoad> sLoad;   // by missile unit id; not saved: a loaded missile is counted again
unsigned long sLaunched, sHeld, sSwitched;

bool On(unsigned long inPlayerID)
{
    return GeneralOf(inPlayerID).spread_missiles != 0 && EngineOf(inPlayerID) == kEngineAgeOfAI;
}

// The unit an attack goal points at, or NULL. Upstream's goal_get_state == 2 is an attack-unit goal here.
EEUnit* GoalTarget(EEComplexUnit* inUnit)
{
    EEUGAttackUnit* theGoal = dynamic_cast<EEUGAttackUnit*>(inUnit->GetLoadGoal());
    return theGoal != NULL ? theGoal->GetTarget() : NULL;
}

// The damage the missile's launcher will do to the target, from the game's own shot formula. Upstream takes the
// attack from the launcher's type and its owner's upgraded stats (the missile's own class carries no attack).
int Damage(EEComplexUnit* inMissile, EEUnit* inTarget)
{
    EEUnitClass* theLauncher = inMissile->mLauncherClass;
    EEPlayer* theOwner = inMissile->mPlayer;
    if (theLauncher == NULL || theOwner == NULL || inTarget == NULL)
        return 0;
    Stats::Group theAttacker = {};
    theAttacker.mRep = inMissile;
    theAttacker.mClass = theLauncher;
    theAttacker.mStats = Stats::Record(theOwner, (long)theLauncher->GetUnitType());
    theAttacker.mOwner = (long)theOwner->GetPlayerID();
    Stats::Group theVictim = {};
    theVictim.mRep = inTarget;
    theVictim.mClass = inTarget->mUnitClass;
    theVictim.mStats = inTarget->mStats;
    theVictim.mOwner = inTarget->mPlayer != NULL ? (long)inTarget->mPlayer->GetPlayerID() : 0;
    return Stats::Shot(theAttacker, theVictim);
}

// A missile is gone when it has not been steered for three steps, with a floor of 100 ms (upstream's missile_tick).
unsigned long GoneMilliseconds()
{
    double theStep = gPhysics != NULL ? (double)gPhysics->mTickSeconds * 3000.0 : 0.0;
    return (unsigned long)std::max(100.0, theStep);
}

// Drops missiles that stopped steering, then adds the damage of those still heading for inTarget.
int Incoming(EEUnit* inTarget)
{
    unsigned long theNow = gPhysics->mGameTime;
    unsigned long theGone = GoneMilliseconds();
    for (std::map<unsigned long, MissileLoad>::iterator it = sLoad.begin(); it != sLoad.end();) {
        if (theNow > it->second.mLast && theNow - it->second.mLast > theGone)
            it = sLoad.erase(it);
        else
            ++it;
    }
    int theSum = 0;
    for (std::map<unsigned long, MissileLoad>::const_iterator it = sLoad.begin(); it != sLoad.end(); ++it)
        if (it->second.mTarget == inTarget)
            theSum += it->second.mDamage;
    return theSum;
}

bool Doomed(EEUnit* inTarget)
{
    return inTarget->mHitPoints > 0 && Incoming(inTarget) >= inTarget->mHitPoints;
}

// The nearest living enemy of the target's family in the shooter's range, seen by its owner and not covered yet.
EEUnit* SpreadTarget(EEComplexUnit* inShooter, EEUnit* inTarget)
{
    EEPlayer* theOwner = inShooter->mPlayer;
    EEUnitClass* theTargetClass = inTarget->mUnitClass;
    if (theOwner == NULL || theTargetClass == NULL)
        return NULL;
    unsigned long theFamily = theTargetClass->GetFamily();
    GE3DPoint thePosition;
    inShooter->GetPosition(thePosition);
    float theRange = inShooter->mStats != NULL ? inShooter->mStats->mRange : inShooter->mUnitClass->mRange;
    if (!(theRange > 0.0f))
        return NULL;
    float theBest = theRange * theRange;
    EEUnit* theFound = NULL;
    for (unsigned long i = 1; i < ::gWorld.GetNumPlayers(); i++) {
        EEPlayer* thePlayer = ::gWorld.GetPlayer(i);
        if (thePlayer == NULL || !IsEnemyOf(theOwner, thePlayer))
            continue;
        for (EEUnit* theUnit = thePlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit == inTarget || theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL
                || theUnit->mUnitClass->GetFamily() != theFamily || !SeenBy(theUnit, theOwner) || Doomed(theUnit))
                continue;
            GE3DPoint theOther;
            theUnit->GetPosition(theOther);
            float theX = theOther.GetX() - thePosition.GetX(), theY = theOther.GetY() - thePosition.GetY();
            float theDistance = theX * theX + theY * theY;
            if (theDistance <= theBest) {
                theBest = theDistance;
                theFound = theUnit;
            }
        }
    }
    return theFound;
}

}   // namespace

bool SpreadMissilesOn(unsigned long inPlayerID)
{
    return On(inPlayerID);
}

// Called from EEUCMissile::LaunchProjectile: records the missile's target and its share of the damage. Upstream starts
// the gone clock at the end of the climb (the missile's flight time); steering in EEPMMissile::Home extends it.
void NoteMissileLaunch(EEComplexUnit* inMissile)
{
    if (inMissile == NULL || inMissile->mPlayer == NULL || !On(inMissile->mPlayer->GetPlayerID()))
        return;
    EEUnit* theTarget = GoalTarget(inMissile);
    ++sLaunched;
    if (theTarget == NULL)
        return;
    if (sLoad.size() > 8192)
        sLoad.clear();
    unsigned long theNow = gPhysics->mGameTime;
    MissileLoad& theLoad = sLoad[inMissile->mUnitID];
    theLoad.mTarget = theTarget;
    theLoad.mDamage = Damage(inMissile, theTarget);
    theLoad.mFirst = theNow;
    theLoad.mLast = theNow + (inMissile->mFlightTime > 0 ? (unsigned long)inMissile->mFlightTime : 0);
}

// Called from EEPMMissile::Home each step the missile steers: refreshes the gone clock, and re-reads the damage when
// the missile switches target.
void NoteMissileStep(EEComplexUnit* inMissile, EEUnit* inTarget)
{
    if (inMissile == NULL || inMissile->mPlayer == NULL || !On(inMissile->mPlayer->GetPlayerID()) || inTarget == NULL)
        return;
    std::map<unsigned long, MissileLoad>::iterator it = sLoad.find(inMissile->mUnitID);
    if (it == sLoad.end())
        return;
    if (it->second.mTarget != inTarget) {
        it->second.mTarget = inTarget;
        it->second.mDamage = Damage(inMissile, inTarget);
    }
    it->second.mLast = gPhysics->mGameTime;
}

// Called from EEUnitClass::FireShot before the drop location is found. When the target is already covered, returns
// another target to fire at, or sets outHold and returns NULL to keep this shot. NULL without outHold is the normal
// case: the launcher fires at its own target.
EEUnit* SpreadBeforeFire(EEComplexUnit* inShooter, bool& outHold)
{
    outHold = false;
    if (inShooter == NULL || inShooter->mPlayer == NULL || !On(inShooter->mPlayer->GetPlayerID()))
        return NULL;
    EEUnitClass* theClass = inShooter->mUnitClass;
    if (theClass == NULL || dynamic_cast<EEUCMissile*>(theClass->mProjectileClass) == NULL)
        return NULL;
    EEUnit* theTarget = GoalTarget(inShooter);
    if (theTarget == NULL || !Doomed(theTarget))
        return NULL;
    EEUnit* theAlternate = SpreadTarget(inShooter, theTarget);
    if (theAlternate == NULL) {
        ++sHeld;
        outHold = true;
        return NULL;
    }
    ++sSwitched;
    return theAlternate;
}

SpreadCounts MissileSpreadCounts()
{
    SpreadCounts theCounts = {};
    theCounts.mLaunched = sLaunched;
    theCounts.mHeld = sHeld;
    theCounts.mSwitched = sSwitched;
    return theCounts;
}

}   // namespace AgeOfAI
