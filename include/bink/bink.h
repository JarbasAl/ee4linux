// RAD Game Tools Bink (binkw32.dll) API as used by the DX7 renderer plug-ins. PROVISIONAL stand-in: the real bink.h is
// not available. Names and argument counts are proven by DX7HRDisplay.dll's import table (binkw32.dll exports the
// decorated __stdcall names, e.g. _BinkOpen@8); argument roles from the RAD documentation. Only the BINK fields the
// plug-in reads are declared (Width +0, Height +4, Frames +8, FrameNum +0xc).
#ifndef BINK_H
#define BINK_H

typedef unsigned long U32;
typedef long S32;

typedef struct BINK {
    U32 Width;
    U32 Height;
    U32 Frames;
    U32 FrameNum;
} BINK, *HBINK;

typedef void* (__stdcall* BINKSNDOPEN)(void* inBink, U32 inTrack);
typedef BINKSNDOPEN (__stdcall* BINKSNDSYSOPEN)(U32 inParameter);

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) HBINK __stdcall BinkOpen(const char* inName, U32 inFlags);
__declspec(dllimport) void __stdcall BinkClose(HBINK inBink);
__declspec(dllimport) S32 __stdcall BinkDoFrame(HBINK inBink);
__declspec(dllimport) void __stdcall BinkNextFrame(HBINK inBink);
__declspec(dllimport) S32 __stdcall BinkWait(HBINK inBink);
__declspec(dllimport) S32 __stdcall BinkCopyToBuffer(HBINK inBink, void* outDestination, S32 inPitch, U32 inHeight,
                                                     U32 inX, U32 inY, U32 inFlags);
__declspec(dllimport) S32 __stdcall BinkDDSurfaceType(void* inSurface);
__declspec(dllimport) S32 __stdcall BinkSetSoundSystem(BINKSNDSYSOPEN inOpen, U32 inParameter);
__declspec(dllimport) BINKSNDOPEN __stdcall BinkOpenDirectSound(U32 inParameter);
#ifdef __cplusplus
}
#endif

#ifdef EE_PORT_LONG32
#define BinkSoundUseDirectSound(lpDS) BinkSetSoundSystem(BinkOpenDirectSound, (U32)(UINT_PTR)(lpDS))
#else
#define BinkSoundUseDirectSound(lpDS) BinkSetSoundSystem(BinkOpenDirectSound, (U32)(lpDS))
#endif

#endif
