// Land unit collision (land_collide): circles as wide as the game's size for a unit, pushed apart gently instead of slowed and
// jumped back, a unit walking into another veers off and slides past, and a standing unit of the same player or an ally steps
// aside. Units at work are passed. Units at rest drift apart too.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/engine/collide.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. The land part only (ship_collide is not ported). The game applies the
// push itself right after the collision step, so the mover's push is not applied here; a contact with a gate or a building
// still goes through the game's own collision step.
#include <windows.h>
#include <unistd.h>
#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GESurface.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "GEWindow.h"
#include "lle/GEScreen.h"
#include "lle/GEViewport.h"
#include "lle/GEVertexArray.h"
#include "lle/GERasterizer.h"
#include "lle/GERhombus.h"
#include "lle/GEBitmap.h"
#include "lle/FSFileSpec.h"
#include "lle/FSArchive.h"
#include "lle/FSPath.h"
#include "lle/TMPoll.h"
#include "lle/TSCriticalSection.h"
#include "lle/TSScheduler.h"
#include "lle/SESample.h"
#include "lle/SESMInstanceNode.h"
#include "lle/SESoundManager.h"
#include "lle/Input System.h"
#include "lle/ISKeyboard.h"
#include "lle/ISMouse.h"
#include "lle/UIControl.h"
#include "lle/UIEvent.h"
#include "lle/UIEntityGroup.h"
#include "lle/UIBackground.h"
#include "lle/UIFontTable.h"
#include "lle/UIHotKey.h"
#include "lle/UIIMECandidate.h"
#include "lle/UIForm.h"
#include "lle/UIManager.h"
#include "lle/UIButton.h"
#include "lle/NEMessage.h"
#include "lle/NESynchronousCommunications.h"
#include "lle/GELine.h"
#include "lle/UITextLabel.h"
#include "lle/UIListBox.h"
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "lle/UILBString.h"
#include "lle/UIFormManager.h"
#include "lle/UResourceFile.h"
#include "lle/UIEditBox.h"
#include "lle/UICheckBox.h"
#include "lle/UIRadioButton.h"
#include "lle/UIScrollBar.h"
#include "lle/UIFileListBox.h"
#include "lle/UIComboBox.h"
#include "lle/UIBlinkingButton.h"
#include "lle/UIBitmap.h"
#include "lle/NESynchronousServer.h"
#include "lle/FSRestorable.h"
#include "ee/EmpireEarth.h"
#include "AgeOfAI/AgeOfAI.h"
#include "ee/EEAction.h"
#include "ee/EEARemovePlayer.h"
#include "ee/EEMessage.h"
#include "ee/EECommunications.h"
#include "ee/EEUserInterface.h"
#include "ee/EEUIManager.h"
#include "ee/EEServer.h"
#include "ee/EEWonNet.h"
#include "ee/EEGameOptions.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEGamePreferences.h"
#include "ee/EESMMediaStrings.h"
#include "ee/EEPSaveGameHeader.h"
#include "ee/EESaveGameFiles.h"
#include "ee/EEGameVariant.h"
#include "ee/EEDatabaseTables.h"
#include "ee/EERandomMapGenerators.h"
#include "ee/EEUICPing.h"
#include "ee/EEUIELobbyEvents.h"
#include "ee/EEUIEPostGameSummaryEvents.h"
#include "ee/EEStartingResources.h"
#include "ee/EEStringIDs.h"
#include "ee/EETechTreeTechID.h"
#include "ee/EEPlayer.h"
#include "ee/EEPhysics.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GEVector.h"
#include "lle/GE3DPlane.h"
#include "lle/GE3DLine.h"
#include "lle/GETransformation.h"
#include "lle/GEPhysicalModel.h"
#include "lle/GETerrainMesh.h"
#include "lle/FSFile.h"
#include "lle/U2DSparseArray.h"
#include "lle/U2DPolygon.h"
#include "lle/USingleton.h"
#include "ee/EEComputerPlayer.h"
#include "ee/EEUnit.h"
#include "platform/PlatSim.h"
#include "ee/EEMap.h"
#include "ee/EEUnit.h"
#include "ee/EEUnitClassIDs.h"
#include "ee/EECPBuildManager.h"
#include "ee/EECPResourceManager.h"
#include "ee/EECPCivilizationManager.h"
#include "ee/EEUGGather.h"
#include "ee/EEUGBuildRepair.h"
#include "ee/EEUGLocation.h"
#include "ee/EEUGAttackUnit.h"
#include "ee/EEUGGather.h"
#include "ee/EEUGBuildRepair.h"
#include "ee/EEPhysicsModel.h"
#include "AgeOfAI.h"
#include "AgeOfAISettings.h"

extern unsigned long gCollisionPeriod;   // EEPhysicsModel.cpp

namespace AgeOfAI {

namespace {

const float kRelax = 0.5f;     // the share of an overlap undone a step
const float kSidestep = 0.5f;  // heading into another: the push turned this much to the side
const float kRestSlop = 0.05f; // at rest a lighter overlap is left: the positions settle instead of trading shoves
const int kRestEvery = 4;      // a unit at rest looks for overlaps every this many steps
const int kRestLock = 3;       // pushes at rest back against the one before, since the unit last moved: then it stays put
const float kTouch = 0.1f;     // a gap this narrow counts as touching (WeightAgainst)
const float kBehind = 0.5f;    // a neighbour within 60 degrees of the push's direction is in its way
const int kMemory = 16384;

struct RestMemory {
    unsigned long mID;
    int mCount;
    float mLastX, mLastY;
} sMemory[kMemory];

float sBiggest;   // the largest size met: how far to look for an overlap

RestMemory* RestOf(EEComplexUnit* inUnit)
{
    unsigned long theID = inUnit->GetUnitID(), theHash = (unsigned long)(theID * 2654435761u) % kMemory, i = theHash;
    for (int k = 0; k < 8; k++, i = (theHash + (unsigned long)k) % kMemory)
        if (sMemory[i].mID == theID || sMemory[i].mID == 0)
            break;
    if (sMemory[i].mID != theID) {
        sMemory[i].mID = theID;
        sMemory[i].mCount = 0;
        sMemory[i].mLastX = sMemory[i].mLastY = 0;
    }
    return &sMemory[i];
}

bool Friendly(EEComplexUnit* inA, EEComplexUnit* inB)
{
    EEPlayer* theA = inA->mPlayer;
    EEPlayer* theB = inB->mPlayer;
    if (theA == theB)
        return true;
    return theA != NULL && theB != NULL && theA->mPlayerID != 0 && theB->mPlayerID != 0 && theA->IsAlly(theB) && theB->IsAlly(theA);
}

void PositionOf(EEComplexUnit* inUnit, float& outX, float& outY)
{
    outX = inUnit->mPhysicalModel.GetLocation().GetX();
    outY = inUnit->mPhysicalModel.GetLocation().GetY();
}

// the unit's heading: the model's forward vector (0, 1, 0), as the game's own push reads it
void HeadingOf(EEComplexUnit* inUnit, float& outX, float& outY)
{
    GEVector theForward(0.0f, 1.0f, 0.0f), theDirection;
    inUnit->mPhysicalModel.GetLocation().Apply(theForward, theDirection);
    float theLength = sqrtf(theDirection.GetX() * theDirection.GetX() + theDirection.GetY() * theDirection.GetY());
    if (theLength > 0.0001f) {
        outX = theDirection.GetX() / theLength;
        outY = theDirection.GetY() / theLength;
    } else {
        outX = 0;
        outY = 1;
    }
}

// a land unit, whole and on its own feet: not gone, not inside a transport or a building
bool IsLand(EEComplexUnit* inUnit)
{
    if (!inUnit->mIsValid || inUnit->mHitPoints <= 0 || inUnit->mHolder != NULL || inUnit->mUnitClass == NULL
        || inUnit->mUnitClass->mPhysicsModel == NULL)
        return false;
    return dynamic_cast<EEPMLand*>(inUnit->mUnitClass->mPhysicsModel) != NULL;
}

// the unit's work (gathering 6 or building 4), else 0: the citizen flag alone stays set on an idle citizen
unsigned char WorkOf(EEComplexUnit* inUnit)
{
    unsigned char theKind = inUnit->mIsCitizen && inUnit->mGoalState != NULL ? inUnit->mGoalState->mKind : 0;
    return theKind == 6 || theKind == 4 ? theKind : 0;
}

// inOther at work: inUnit passes it, unless inUnit is at the same work
bool Passes(EEComplexUnit* inUnit, EEComplexUnit* inOther)
{
    unsigned char theKind = WorkOf(inOther);
    if (theKind == 0)
        return false;
    if (WorkOf(inUnit) != theKind)
        return true;
    if (theKind == 6)
        return static_cast<EEUGGather*>(inUnit->mGoalState)->mResource != static_cast<EEUGGather*>(inOther->mGoalState)->mResource;
    return static_cast<EEUGBuildRepair*>(inUnit->mGoalState)->mTarget != static_cast<EEUGBuildRepair*>(inOther->mGoalState)->mTarget;
}

bool Moves(EEComplexUnit* inUnit)
{
    return inUnit->GetVelocity() > 0.05f || inUnit->mAngularVelocity.GetYaw() != 0.0f;
}

// a standing unit's push applied in physics action 0: it moves without the pushed walk
void PushQuiet(EEComplexUnit* inUnit)
{
    unsigned long theAction = inUnit->mCurrentPhysicsAction;
    inUnit->mCurrentPhysicsAction = 0;
    inUnit->mUnitClass->mPhysicsModel->Move(inUnit);
    inUnit->mCurrentPhysicsAction = theAction;
}

template <class F>
void ForUnitsNear(EEComplexUnit* inCentre, long inReach, F inVisit)
{
    for (long y = inCentre->mTileY - inReach; y <= inCentre->mTileY + inReach; y++)
        for (long x = inCentre->mTileX - inReach; x <= inCentre->mTileX + inReach; x++) {
            if (x < 0 || y < 0 || gWorld.mMap == NULL)
                continue;
            EEMapTile* theTile = gWorld.mMap->GetTile((unsigned long)x, (unsigned long)y);
            if (theTile == NULL)
                continue;
            for (RTSTileUnitNode* theNode = theTile->GetUnits(); theNode != NULL; theNode = theNode->GetNext())
                if (theNode->GetUnit() != NULL && theNode->GetUnit()->mComplexUnit != NULL)
                    inVisit(theNode->GetUnit()->mComplexUnit);
        }
}

// a standing land unit's weight against a mover pushing it along (inDX, inDY): its size squared x (1 + 8^n) for the n standing
// units it touches on that side - a unit steps aside freely, but not into the others
float WeightAgainst(EEComplexUnit* inUnit, float inSize, float inDX, float inDY)
{
    float theX, theY;
    PositionOf(inUnit, theX, theY);
    int theCount = 0;
    long theReach = (long)((inSize + sBiggest) * 0.5f + kTouch) + 1;
    ForUnitsNear(inUnit, theReach, [&](EEComplexUnit* theOther) {
        if (theOther == inUnit)
            return;
        float theOX, theOY;
        PositionOf(theOther, theOX, theOY);
        float theRX = theOX - theX, theRY = theOY - theY;
        float theLimit = (inSize + theOther->mCollisionRadius) * 0.5f + kTouch, theD2 = theRX * theRX + theRY * theRY;
        if (theD2 >= theLimit * theLimit || theD2 < 1e-8f || theRX * inDX + theRY * inDY < kBehind * sqrtf(theD2))
            return;
        if (!IsLand(theOther) || Moves(theOther))
            return;
        theCount++;
    });
    return inSize * inSize * (theCount ? 1.0f + (float)(1 << (theCount < 4 ? 3 * theCount : 12)) : 1.0f);
}

// the push off every overlapping land unit near inUnit, added to its push velocity; the standing friendly ones get their part
bool Separate(EEComplexUnit* inUnit, bool inResting)
{
    float theDT = gPhysics->GetTickSeconds() * (float)gCollisionPeriod;
    if (theDT <= 0)
        return false;
    float theX, theY, theDirX, theDirY;
    PositionOf(inUnit, theX, theY);
    HeadingOf(inUnit, theDirX, theDirY);
    float theLength = inUnit->mCollisionRadius, theRadius = theLength * 0.5f, theMass = theLength * theLength;
    if (theLength > sBiggest)
        sBiggest = theLength;
    float theCorrX = 0, theCorrY = 0;
    long theReach = (long)((theLength + sBiggest) * 0.5f) + 1;
    ForUnitsNear(inUnit, theReach, [&](EEComplexUnit* theOther) {
        if (theOther == inUnit)
            return;
        float theOX, theOY;
        PositionOf(theOther, theOX, theOY);
        float theOtherLength = theOther->mCollisionRadius, theLimit = (theLength + theOtherLength) * 0.5f;
        if (fabsf(theOX - theX) > theLimit || fabsf(theOY - theY) > theLimit)
            return;
        if (!IsLand(theOther) || Passes(inUnit, theOther))
            return;
        float theDX = theX - theOX, theDY = theY - theOY, theD2 = theDX * theDX + theDY * theDY;
        float theRR = theRadius + theOtherLength * 0.5f;
        bool theFriendly = Friendly(inUnit, theOther);
        if (theFriendly && theD2 < theRR * theRR)
            theRR *= LoadedSettings().friendly_collision;
        if (theD2 >= theRR * theRR)
            return;
        float theDist = sqrtf(theD2);
        if (theDist > 0.0001f) {
            theDX /= theDist;
            theDY /= theDist;
        } else {   // the centres coincide: off sideways from the other's heading, to the side the unit's centre is on
            float theHX, theHY;
            HeadingOf(theOther, theHX, theHY);
            float theSideX = theHY, theSideY = -theHX, theSL = sqrtf(theSideX * theSideX + theSideY * theSideY);
            if (theSL <= 0.0001f) {
                theSideX = 1;
                theSideY = 0;
                theSL = 1;
            }
            float theSign = (theSideX * (theX - theOX) + theSideY * (theY - theOY)) < 0 ? -1.0f : 1.0f;
            theDX = theSideX * theSign / theSL;
            theDY = theSideY * theSign / theSL;
        }
        float theK = -(theDirX * theDX + theDirY * theDY);
        if (theK > 0 && inUnit->GetVelocity() > 0.05f) {   // heading into it: off to a side as well, so it slides past
            float theRightX = theDirY, theRightY = -theDirX;
            float theSide = (theDX * theRightX + theDY * theRightY) < -0.05f ? -1.0f : 1.0f;
            theDX += theRightX * theSide * theK * kSidestep;
            theDY += theRightY * theSide * theK * kSidestep;
            float theN = sqrtf(theDX * theDX + theDY * theDY);
            theDX /= theN;
            theDY /= theN;
        }
        bool theMoves = Moves(theOther), theNudge = !inResting && !theMoves && theFriendly && !theOther->mIsDisabled;
        float theOver = theRR - theDist;
        float theOC = theNudge ? WeightAgainst(theOther, theOtherLength, -theDX, -theDY) : theOtherLength * theOtherLength;
        float theShare = theOC / (theMass + theOC);
        if (inResting && theMoves)
            return;   // a moving one does it in its own step
        if (!inResting && !theMoves && !theNudge)
            theShare = 1.0f;
        if (inResting && theOver < kRestSlop)
            return;
        theOver *= kRelax;
        theCorrX += theDX * theOver * theShare;
        theCorrY += theDY * theOver * theShare;
        if (theNudge) {   // the standing one steps aside by its part, and may part from the others again
            RestOf(theOther)->mCount = 0;
            float theBack = -theOver * (1.0f - theShare) / theDT;
            theOther->mPushVelocityX += theDX * theBack;
            theOther->mPushVelocityY += theDY * theBack;
            PushQuiet(theOther);
            theOther->mPushVelocityX = theOther->mPushVelocityY = 0;   // applied: the step after this one adds nothing twice
        }
    });
    if (theCorrX == 0 && theCorrY == 0)
        return false;
    inUnit->mPushVelocityX += theCorrX / theDT;
    inUnit->mPushVelocityY += theCorrY / theDT;
    return true;
}

// a contact the game's own collision still handles: a gate, a building, anything that is not a land unit
bool HasOtherContact(EEComplexUnit* inUnit)
{
    for (UListNode<EEComplexUnit*>* theNode = inUnit->mContacts.mHead; theNode; theNode = theNode->mNext)
        if (!IsLand(theNode->mData))
            return true;
    return false;
}

}

// The collision step of a land unit (EEPMLand::Collide), after the step and before the game applies the push
bool LandCollide(EEComplexUnit* inUnit)
{
    if (!LoadedSettings().land_collide)
        return false;
    if (!inUnit->mIsCollidable || (inUnit->GetVelocity() == 0 && inUnit->mAngularVelocity.GetYaw() == 0.0f)) {
        // at rest: the game's step, then, every kRestEvery steps, off the other units at rest it overlaps - else units
        // that stop on one spot stay one blob; a unit at work keeps its place
        inUnit->mPushVelocityX = inUnit->mPushVelocityY = 0;
        return false;
    }
    RestOf(inUnit)->mCount = 0;   // it moves: free to be pushed at rest again
    Separate(inUnit, false);
    return !HasOtherContact(inUnit);
}

// A unit at rest, once every kRestEvery steps: off the units at rest it overlaps, applied quietly
void LandRest(EEComplexUnit* inUnit)
{
    RestMemory* theRest;
    if (inUnit->mIsCollidable && (inUnit->GetVelocity() != 0 || inUnit->mAngularVelocity.GetYaw() != 0.0f))
        return;   // it moves: LandCollide handled it
    if (!LoadedSettings().land_collide || WorkOf(inUnit) || inUnit->mIsDisabled || inUnit->mHitPoints <= 0 || inUnit->mHolder != NULL
        || (inUnit->GetUnitID() + gPhysics->mNumTicksDone) % kRestEvery != 0 || (theRest = RestOf(inUnit))->mCount >= kRestLock)
        return;
    if (!Separate(inUnit, true))
        return;
    float thePX = inUnit->mPushVelocityX, thePY = inUnit->mPushVelocityY;
    if (thePX * theRest->mLastX + thePY * theRest->mLastY < 0)
        theRest->mCount++;
    theRest->mLastX = thePX;
    theRest->mLastY = thePY;
    PushQuiet(inUnit);
    inUnit->mPushVelocityX = inUnit->mPushVelocityY = 0;
}

}
