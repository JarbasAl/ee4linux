// Original Helicopter target predicate: construction 0x45a82..0x45aa3,
// operator() 0x45655 and Accept 0x45674. Sixteen-byte record, nominal spelling
// and original header placement provisional. Moved unchanged from AirTransport.
#ifndef EE_EECPAAIRTARGETFILTERS_H
#define EE_EECPAAIRTARGETFILTERS_H
#include "ee/EETileCoordinate.h"
class EECPASGAir;
class EECPASGSea;
class EECPASGSpaceship;
class EECPASGLand;
class EECPASGHelicopter;
class EEPlayer;
class EEComplexUnit;

class EECPAHelicopterTargetFilter {
public:
    EECPASGHelicopter* mGroup;
    EEPlayer* mPlayer;
    long mPlayerID;
    bool mIncludeEnemies;
    bool mReserved;
    bool operator()(EEComplexUnit* inUnit);
    bool Accept(EEComplexUnit* inUnit);
};
// Original24-byte Land predicate, constructor476ad; moved unchanged from AirTransport.
class EECPALandTargetFilter {
public:
    EETileLocation mCenter;
    EECPASGLand* mGroup;
    EEPlayer* mPlayer;
    long mPlayerID;
    bool mIncludeEnemies;
    bool mAllowWalls;
    bool mAllowGates;
    bool mTileReady;
    bool operator()(EEComplexUnit* inUnit);
    bool Accept(EEComplexUnit* inUnit);
};
// Original 16-byte predicate; moved unchanged from AirTransport.
class EECPAAirTargetFilter {
public:
    EECPASGAir* mGroup;
    EEPlayer* mPlayer;
    long mPlayerID;
    bool mIncludeEnemies;

    bool operator()(EEComplexUnit* inUnit);
    bool Accept(EEComplexUnit* inUnit);
};
// Original 16-byte predicate; moved unchanged from AirTransport.
class EECPASeaTargetFilter {
public:
    EECPASGSea* mGroup;
    EEPlayer* mPlayer;
    long mPlayerID;
    bool mIncludeEnemies;
    bool mAlternate;
    bool mAllowNaval;
    bool mAllowShore;
    bool operator()(EEComplexUnit* inUnit);
    bool Accept(EEComplexUnit* inUnit);
    bool AcceptAlternate(EEComplexUnit* inUnit);
};
// Original 16-byte predicate; moved unchanged from AirTransport.
class EECPASpaceshipTargetFilter {
public:
    EECPASGSpaceship* mGroup;
    EEPlayer* mPlayer;
    long mPlayerID;
    bool mIncludeEnemies;
    bool mAllowShore;
    bool operator()(EEComplexUnit* inUnit);
    bool Accept(EEComplexUnit* inUnit);
};
#endif
