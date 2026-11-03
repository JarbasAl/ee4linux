// ee/EEEffectKindIDs.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_EEEFFECTKINDIDS_H
#define EE_EEEFFECTKINDIDS_H

// Effect class kinds (EEEffect::mKind, the ctor argument; EEGraphicEffect::mKind = the kind a graphic-effect record
// creates): EEEffectManager.cpp creates each class by kind (load, create, copy switches); names from the RTTI class
// created (EEE prefix dropped).
enum EEEffectKind {
    kEffect3DIcon = 0x2,                  // RTTI EEE3DIcon: EEEffectManager.cpp case 0x2 creates it
    kEffect2DExhaustTrail = 0x3,          // RTTI EEE2DExhaustTrail: EEEffectManager.cpp case 0x3 creates it
    kEffect2DExplosion = 0x4,             // RTTI EEE2DExplosion: EEEffectManager.cpp case 0x4 creates it
    kEffect2DFire = 0x5,                  // RTTI EEE2DFire: EEEffectManager.cpp case 0x5 creates it
    kEffect2DImpact = 0x6,                // RTTI EEE2DImpact: EEEffectManager.cpp case 0x6 creates it
    kEffect2DImpactBlood = 0x7,           // RTTI EEE2DImpactBlood: EEEffectManager.cpp case 0x7 creates it
    kEffect2DLightning = 0x8,             // RTTI EEE2DLightning: EEEffectManager.cpp case 0x8 creates it
    kEffect2DMuzzleFlash = 0x9,           // RTTI EEE2DMuzzleFlash: EEEffectManager.cpp case 0x9 creates it
    kEffect2DPlasmaPulse = 0xa,           // RTTI EEE2DPlasmaPulse: EEEffectManager.cpp case 0xa creates it
    kEffect2DSmokePlume = 0xb,            // RTTI EEE2DSmokePlume: EEEffectManager.cpp case 0xb creates it
    kEffect2DVelocityParticle = 0xc,      // RTTI EEE2DVelocityParticle: EEEffectManager.cpp case 0xc creates it
    kEffect2DLightGlow = 0xd,             // RTTI EEE2DLightGlow: EEEffectManager.cpp case 0xd creates it
    kEffect2DFireTrail = 0xe,             // RTTI EEE2DFireTrail: EEEffectManager.cpp case 0xe creates it
    kEffect2DSquareMarchingIcons = 0xf,   // RTTI EEE2DSquareMarchingIcons: EEEffectManager.cpp case 0xf creates it
    kEffect2DCircularMarchingIcons = 0x10,  // RTTI EEE2DCircularMarchingIcons: EEEffectManager.cpp case 0x10 creates it
    kEffect2DShockwave = 0x12,            // RTTI EEE2DShockwave: EEEffectManager.cpp case 0x12 creates it
    kEffect2DSmokeMuzzleBlast = 0x14,     // RTTI EEE2DSmokeMuzzleBlast: EEEffectManager.cpp case 0x14 creates it
    kEffect2DWinterBreath = 0x16,         // RTTI EEE2DWinterBreath: EEEffectManager.cpp case 0x16 creates it
    kEffect3DBlastWave = 0x18,            // RTTI EEE3DBlastWave: EEEffectManager.cpp case 0x18 creates it
    kEffect3DEarthquake = 0x1a,           // RTTI EEE3DEarthquake: EEEffectManager.cpp case 0x1a creates it; its ctor passes the kind
    kEffect3DWaterSplash = 0x1b,          // RTTI EEE3DWaterSplash: EEEffectManager.cpp case 0x1b creates it; its ctor passes the kind
    kEffect3DDestructionSmoke = 0x1e,     // RTTI EEE3DDestructionSmoke: EEEffectManager.cpp case 0x1e creates it
    kEffect3DVolcano = 0x20,              // RTTI EEE3DVolcano: EEEffectManager.cpp case 0x20 creates it; its ctor passes the kind
    kEffect3DTracers = 0x21,              // RTTI EEE3DTracers: EEEffectManager.cpp case 0x21 creates it
    kEffect2DWaterRipple = 0x22,          // RTTI EEE2DWaterRipple: EEEffectManager.cpp case 0x22 creates it
    kEffect3DDisease = 0x23,              // RTTI EEE3DDisease: EEEffectManager.cpp case 0x23 creates it; its ctor passes the kind
    kEffect3DHurricane = 0x24,            // RTTI EEE3DHurricane: EEEffectManager.cpp case 0x24 creates it; its ctor passes the kind
    kEffect3DLinearProjectile = 0x25,     // RTTI EEE3DLinearProjectile: EEEffectManager.cpp case 0x25 creates it; its ctor passes the kind
    kEffect3DShells = 0x26,               // RTTI EEE3DShells: EEEffectManager.cpp case 0x26 creates it
    kEffect3DAdditivePass = 0x27,         // RTTI EEE3DAdditivePass: EEEffectManager.cpp case 0x27 creates it; its ctor passes the kind
    kEffect3DNuclearExplosion = 0x28,     // RTTI EEE3DNuclearExplosion: EEEffectManager.cpp case 0x28 creates it; its ctor passes the kind
    kEffect3DFarm = 0x29,                 // RTTI EEE3DFarm: EEEffectManager.cpp case 0x29 creates it; its ctor passes the kind
    kEffect3DRain = 0x2a,                 // RTTI EEE3DRain: EEEffectManager.cpp case 0x2a creates it
    kEffect3DSmokeGenerator = 0x2b,       // RTTI EEE3DSmokeGenerator: EEEffectManager.cpp case 0x2b creates it
    kEffect3DDamageSmoke = 0x2c,          // RTTI EEE3DDamageSmoke: EEEffectManager.cpp case 0x2c creates it
    kEffect3DMarchingLine = 0x2d,         // RTTI EEE3DMarchingLine: EEEffectManager.cpp case 0x2d creates it; its ctor passes the kind
    kEffect2DParticle = 0x2e,             // RTTI EEE2DParticle: EEEffectManager.cpp case 0x2e creates it
    kEffect2DPulseWave = 0x2f,            // RTTI EEE2DPulseWave: EEEffectManager.cpp case 0x2f creates it
    kEffect2DLinearProjectile = 0x30,     // RTTI EEE2DLinearProjectile: EEEffectManager.cpp case 0x30 creates it
    kEffect2DVortex = 0x31,               // RTTI EEE2DVortex: EEEffectManager.cpp case 0x31 creates it
    kEffect2DBattlefieldAmbient = 0x32,   // RTTI EEE2DBattlefieldAmbient: EEEffectManager.cpp case 0x32 creates it
    kEffect3DCitizenBeam = 0x33,          // RTTI EEE3DCitizenBeam: EEEffectManager.cpp case 0x33 creates it
    kEffect3DEnergyWall = 0x34,           // RTTI EEE3DEnergyWall: EEEffectManager.cpp case 0x34 creates it
    kEffect3DDamageVent = 0x35,           // RTTI EEE3DDamageVent: EEEffectManager.cpp case 0x35 creates it; its ctor passes the kind
    kEffect3DParticle = 0x39,             // RTTI EEE3DParticle: EEEffectManager.cpp case 0x39 creates it
    kEffect3DParticleEmitter = 0x3a,      // RTTI EEE3DParticleEmitter: EEEffectManager.cpp case 0x3a creates it
    kEffect3DMultiEffect = 0x3b,          // RTTI EEE3DMultiEffect: EEEffectManager.cpp case 0x3b creates it
    kEffect3DGoal = 0x3c,                 // RTTI EEE3DGoal: EEEffectManager.cpp case 0x3c creates it
    kEffect3DSnow = 0x3d,                 // RTTI EEE3DSnow: EEEffectManager.cpp case 0x3d creates it
    kEffectDamaged = 0x3e,                // RTTI EEEDamagedEffect: EEEffectManager.cpp case 0x3e creates it
    kEffect3DTeleport = 0x3f,             // RTTI EEE3DTeleport: EEEffectManager.cpp case 0x3f creates it
    kEffect3DTracerBeam = 0x40,           // RTTI EEE3DTracerBeam: EEEffectManager.cpp case 0x40 creates it
    kEffect3DCommandConfirmation = 0x41,  // RTTI EEE3DCommandConfirmation: EEEffectManager.cpp case 0x41 creates it
    kEffect3DVirus = 0x42,                // RTTI EEE3DVirus: EEEffectManager.cpp case 0x42 creates it; its ctor passes the kind
    kEffect3DPulseCannonBall = 0x43,      // RTTI EEE3DPulseCannonBall: EEEffectManager.cpp case 0x43 creates it
    kEffect3DTimeWarp = 0x44,             // RTTI EEE3DTimeWarp: EEEffectManager.cpp case 0x44 creates it
    kEffect3DShield = 0x45,               // RTTI EEE3DShield: EEEffectManager.cpp case 0x45 creates it; its ctor passes the kind
    kEffect3DScreenFlash = 0x46,          // RTTI EEE3DScreenFlash: EEEffectManager.cpp case 0x46 creates it; its ctor passes the kind
    kEffect3DEnergyWave = 0x47,           // RTTI EEE3DEnergyWave: EEEffectManager.cpp case 0x47 creates it; its ctor passes the kind
};

#endif // EE_EEEFFECTKINDIDS_H
