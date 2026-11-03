// The lobby's sounds (RECONSTRUCTION; r12-misc-h2, round 12): WONAPI's Sound / SoundDecoder bases (RTTI
// WONAPI::Sound vtable 0x4510fc, WONAPI::SoundDecoder vtable 0x451a88) and the game's implementations, which play
// through the engine's sound manager (EELobbyMusic vtable 0x44c7e8, EELobbySound 0x44c800, EELobbySoundDecoder
// 0x44c6dc). Names PROVISIONAL (by behaviour) unless RTTI gives them. Requires <string>, lle/LowLevelEngine.h,
// ee/EEWonNet.h.
#ifndef EE_WONAPI_EELOBBYSOUND_H
#define EE_WONAPI_EELOBBYSOUND_H

namespace WONAPI {

// what a sound is made from (Sound's ctor copies mL00 to +0x08 and mPath to +0x0c; the decoder loads mPath)
struct SoundDescriptor {
    SoundDescriptor(const std::string& inPath, long inType);    // 0x329830 (WONAPI; r12-misc-h5)

    long mL00;                              // +0x00
    std::string mPath;                      // +0x04
};

class Sound : public RefCount {
public:
    Sound(const SoundDescriptor& inDescriptor);     // 0x3299c0 (WONAPI, /GX)
    // no user dtor: EELobbyMusic's ??_G 0x29ce1e destroys mPath and stores RefCount's vptr only
    virtual void Play();                    // +0x04 (0x326b60: empty)
    virtual void Stop();                    // +0x08 (0x326b60)
    virtual void Virtual0C();               // +0x0c (empty)
    virtual void Virtual10();               // +0x10 (empty)

    long mL08;                              // +0x08
    std::string mPath;                      // +0x0c
    long mL1C;                              // +0x1c
};

typedef SmartPtr<Sound> SoundPtr;

class SoundDecoder : public RefCount {
public:
    SoundDecoder();                         // 0x34a530 (EELobbySoundDecoder's inline ctor calls it; r24pre-won)
    virtual ~SoundDecoder();                // 0x328590 (ICF: ~LobbyPlayer; ??_GEELobbySoundDecoder 0x297dc5 calls it)
    virtual SoundPtr Decode(const SoundDescriptor& inDescriptor) = 0;   // +0x04
};

// the lobby music: "playing" it turns the game's music volume up, stopping it turns it down
class EELobbyMusic : public Sound {
public:
    EELobbyMusic(const SoundDescriptor& inDescriptor);    // 0x29ce06

    virtual void Play();                    // +0x04 0x29ce46
    virtual void Stop();                    // +0x08 0x29ce61
    virtual void Virtual0C();               // +0x0c 0x29ce78 (plays again)
};

// a lobby sound effect: a sample of the engine's sound manager
class EELobbySound : public Sound {
public:
    EELobbySound(const SoundDescriptor& inDescriptor, unsigned long inSampleID);    // 0x29ce7d
    virtual ~EELobbySound();                // 0x29cebc (unloads the sample)

    virtual void Play();                    // +0x04 0x29ceeb
    virtual void Stop();                    // +0x08 0x29cf1c

    unsigned long mSampleID;                // +0x20
    unsigned long mInstanceID;              // +0x24 (-1: not playing)
};

// the lobby's sound decoder (ctor and ??_G in the lobby init unit, 0x2972c6 / 0x297dc5)
class EELobbySoundDecoder : public SoundDecoder {
public:
    virtual SoundPtr Decode(const SoundDescriptor& inDescriptor);       // +0x04 0x29cf40
};

}

#endif
