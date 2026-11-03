// Miles Sound System 6.x API as used by the Miles Sound System Mixer plug-in. PROVISIONAL reconstruction:
// the real mss.h is not available. Names and argument counts are proven by the plug-in's import table
// (mss32.dll exports the decorated __stdcall names, e.g. _AIL_startup@0); argument roles from the RAD
// documentation; preference numbers are the values the plug-in passes (names recalled from mss.h, unverified).
#ifndef MSS_H
#define MSS_H

#define AILCALL __stdcall
#define DXDEC __declspec(dllimport)

typedef long S32;
typedef unsigned long U32;

typedef struct _DIG_DRIVER* HDIGDRIVER;
typedef struct _SAMPLE* HSAMPLE;
typedef void* AILLPDIRECTSOUND;
typedef void* AILLPDIRECTSOUNDBUFFER;

// AIL_set_preference / AIL_get_preference
#define DIG_MIXER_CHANNELS 1
#define DIG_USE_WAVEOUT 15
#define DIG_DS_FRAGMENT_SIZE 34
#define DIG_DS_FRAGMENT_CNT 35
#define DIG_DS_MIX_FRAGMENT_CNT 42

// AIL_sample_status
#define SMP_PLAYING 4

#ifdef __cplusplus
extern "C" {
#endif

DXDEC S32 AILCALL AIL_startup(void);
DXDEC void AILCALL AIL_shutdown(void);
DXDEC char* AILCALL AIL_set_redist_directory(const char* dir);
DXDEC char* AILCALL AIL_last_error(void);
DXDEC S32 AILCALL AIL_set_preference(U32 number, S32 value);
DXDEC S32 AILCALL AIL_get_preference(U32 number);
DXDEC void AILCALL AIL_serve(void);
DXDEC S32 AILCALL AIL_get_timer_highest_delay(void);

DXDEC void* AILCALL AIL_mem_alloc_lock(U32 size);
DXDEC void AILCALL AIL_mem_free_lock(void* ptr);
DXDEC void* AILCALL AIL_file_read(const char* filename, void* dest);
DXDEC S32 AILCALL AIL_file_size(const char* filename);

DXDEC HDIGDRIVER AILCALL AIL_open_digital_driver(U32 frequency, S32 bits, S32 channel, U32 flags);
DXDEC void AILCALL AIL_close_digital_driver(HDIGDRIVER dig);
DXDEC S32 AILCALL AIL_set_DirectSound_HWND(HDIGDRIVER dig, HWND wnd);
DXDEC void AILCALL AIL_get_DirectSound_info(HSAMPLE S, AILLPDIRECTSOUND* lplpDS, AILLPDIRECTSOUNDBUFFER* lplpDSB);
DXDEC S32 AILCALL AIL_digital_CPU_percent(HDIGDRIVER dig);
DXDEC void AILCALL AIL_set_digital_master_volume(HDIGDRIVER dig, S32 master_volume);

DXDEC HSAMPLE AILCALL AIL_allocate_sample_handle(HDIGDRIVER dig);
DXDEC void AILCALL AIL_release_sample_handle(HSAMPLE S);
DXDEC void AILCALL AIL_init_sample(HSAMPLE S);
DXDEC S32 AILCALL AIL_set_named_sample_file(HSAMPLE S, const char* file_type_suffix, const void* file_image,
                                            S32 file_size, S32 block);
DXDEC void AILCALL AIL_start_sample(HSAMPLE S);
DXDEC void AILCALL AIL_end_sample(HSAMPLE S);
DXDEC U32 AILCALL AIL_sample_status(HSAMPLE S);
DXDEC S32 AILCALL AIL_sample_granularity(HSAMPLE S);
DXDEC S32 AILCALL AIL_sample_playback_rate(HSAMPLE S);
DXDEC U32 AILCALL AIL_sample_position(HSAMPLE S);
DXDEC void AILCALL AIL_sample_ms_position(HSAMPLE S, S32* total_milliseconds, S32* current_milliseconds);
DXDEC void AILCALL AIL_set_sample_position(HSAMPLE S, U32 pos);
DXDEC void AILCALL AIL_set_sample_loop_count(HSAMPLE S, S32 loop_count);
DXDEC void AILCALL AIL_set_sample_pan(HSAMPLE S, S32 pan);
DXDEC void AILCALL AIL_set_sample_volume(HSAMPLE S, S32 volume);
DXDEC void AILCALL AIL_set_sample_playback_rate(HSAMPLE S, S32 playback_rate);

// mss32.lib static member (built from src/ThirdParty/mss/mssauto.c): registers AIL_shutdown with atexit. mss.h
// routes AIL_startup through it.
S32 __cdecl MSS_auto_cleanup(void);
#define AIL_startup() (MSS_auto_cleanup(), AIL_startup())

#ifdef __cplusplus
}
#endif

#endif
