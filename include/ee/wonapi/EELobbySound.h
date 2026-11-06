// The lobby's sounds: WONAPI's Sound / SoundDecoder bases (RTTI
// WONAPI::Sound vtable 0x4510fc, WONAPI::SoundDecoder vtable 0x451a88) and the game's implementations, which play
// through the engine's sound manager (EELobbyMusic vtable 0x44c7e8, EELobbySound 0x44c800, EELobbySoundDecoder
// 0x44c6dc). Names PROVISIONAL (by behaviour) unless RTTI gives them. Requires <string>, lle/LowLevelEngine.h,
// ee/EEWonNet.h.
#ifndef EE_WONAPI_EELOBBYSOUND_H
#define EE_WONAPI_EELOBBYSOUND_H

namespace WONAPI {

// what a sound is made from (Sound's ctor copies mL00 to +0x08 and mPath to +0x0c; the decoder loads mPath)
struct SoundDescriptor {
    SoundDescriptor(); //329810 actual default, native flags0 and empty path
    SoundDescriptor(const char*);
    void SetSoundFlags(int,bool); //329990 actual native bit update
    SoundDescriptor(const std::string& inPath, long inType);    // 0x329830

    long mL00;                              // +0x00
    std::string mPath;                      // +0x04
};

class Sound : public RefCount {
public:
    Sound(const SoundDescriptor& inDescriptor);     // 0x3299c0 (WONAPI, /GX)
    // no user dtor: EELobbyMusic's ??_G 0x29ce1e destroys mPath and stores RefCount's vptr only
    virtual void PlayHook() {}              // +0x04 genuine SDK empty hook; native326b60
    virtual void StopHook() {}              // +0x08, genuine SDK empty hook; native326b60
    virtual void ResumeHook() {}            // +0x0c, genuine SDK empty hook; native326b60
    virtual void RewindHook() {}            // +0x10 genuine SDK empty hook; native326b60
    void Fn329BD0();                        // 0x329bd0, actual public Play wrapper (call-only)
    void Stop();                           // genuine SDK public CPP API, native3a6db0
    void Resume();                         // genuine SDK public CPP API, native3a6dc0
    void Rewind();
    void SetLooping(bool inLooping) { mIsLooping = inLooping; }
    void Fn329BF0(bool inMuted);            // 0x329bf0: toggles descriptor flag4; call-only provisional name
    bool GetMuted();                       //genuine SDK CPP, native329c20/9

    long mL08;                              // +0x08
    std::string mPath;                      // +0x0c
    bool mIsLooping;                        // +0x1c; natural padding through1f, actual ctor byte store
};

typedef SmartPtr<Sound> SoundPtr;

class SoundDecoder : public RefCount {
public:
    SoundDecoder();                         // 0x34a530
    virtual ~SoundDecoder();                // 0x328590 (ICF: ~LobbyPlayer; ??_GEELobbySoundDecoder 0x297dc5 calls it)
    SoundPtr DecodeDescriptor(const SoundDescriptor& inDescriptor); // 0x34a540, forwards to Decode
    virtual SoundPtr Decode(const SoundDescriptor& inDescriptor) = 0;   // +0x04
};

// the lobby music: "playing" it turns the game's music volume up, stopping it turns it down
class EELobbyMusic : public Sound {
public:
    EELobbyMusic(const SoundDescriptor& inDescriptor);    // 0x29ce06

    virtual void PlayHook();                // +0x04 0x29ce46
    virtual void StopHook();                // +0x08 0x29ce61
    virtual void ResumeHook();              // +0x0c 0x29ce78 (plays again)
};

// a lobby sound effect: a sample of the engine's sound manager
class EELobbySound : public Sound {
public:
    EELobbySound(const SoundDescriptor& inDescriptor, unsigned long inSampleID);    // 0x29ce7d
    virtual ~EELobbySound();                // 0x29cebc (unloads the sample)

    virtual void PlayHook();                // +0x04 0x29ceeb
    virtual void StopHook();                // +0x08 0x29cf1c

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
