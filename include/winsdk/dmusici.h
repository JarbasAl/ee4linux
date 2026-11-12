// DirectX SDK <dmusici.h> / <dmusicc.h> STAND-IN: only the DirectMusic declarations this project uses, in the
// documented COM vtable order (slot offsets are what the engine's codegen proves). The VC6 package has no DirectX
// SDK. Replace with the real DirectX 7/8 SDK headers if they are obtained. GUIDs are declared extern (dxguid.lib).
#ifndef EE_WINSDK_DMUSICI_H
#define EE_WINSDK_DMUSICI_H
#include <windows.h>
#include <objbase.h>

typedef long MUSIC_TIME;
typedef LONGLONG REFERENCE_TIME;

struct IDirectMusicTrack;
struct IDirectMusicSegmentState;
struct IDirectMusicPerformance;
struct IDirectMusicGraph;
struct IDirectMusicBand;
struct IDirectMusicChordMap;

// ---- IDirectMusicSegment (vtable 0x00..)
struct IDirectMusicSegment : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetLength(MUSIC_TIME* pmtLength) = 0;                      // +0x0c
    virtual HRESULT STDMETHODCALLTYPE SetLength(MUSIC_TIME mtLength) = 0;                        // +0x10
    virtual HRESULT STDMETHODCALLTYPE GetRepeats(DWORD* pdwRepeats) = 0;                         // +0x14
    virtual HRESULT STDMETHODCALLTYPE SetRepeats(DWORD dwRepeats) = 0;                           // +0x18
    virtual HRESULT STDMETHODCALLTYPE GetDefaultResolution(DWORD* pdwResolution) = 0;            // +0x1c
    virtual HRESULT STDMETHODCALLTYPE SetDefaultResolution(DWORD dwResolution) = 0;              // +0x20
    virtual HRESULT STDMETHODCALLTYPE GetTrack(REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex,
                                               IDirectMusicTrack** ppTrack) = 0;                // +0x24
    virtual HRESULT STDMETHODCALLTYPE GetTrackGroup(IDirectMusicTrack* pTrack, DWORD* pdwGroupBits) = 0;
    virtual HRESULT STDMETHODCALLTYPE InsertTrack(IDirectMusicTrack* pTrack, DWORD dwGroupBits) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveTrack(IDirectMusicTrack* pTrack) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitPlay(IDirectMusicSegmentState** ppSegState,
                                               IDirectMusicPerformance* pPerformance, DWORD dwFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetGraph(IDirectMusicGraph** ppGraph) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetGraph(IDirectMusicGraph* pGraph) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddNotificationType(REFGUID rguidNotificationType) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveNotificationType(REFGUID rguidNotificationType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetParam(REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime,
                                               MUSIC_TIME* pmtNext, void* pParam) = 0;           // +0x48
    virtual HRESULT STDMETHODCALLTYPE SetParam(REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime,
                                               void* pParam) = 0;                                // +0x4c
    virtual HRESULT STDMETHODCALLTYPE Clone(MUSIC_TIME mtStart, MUSIC_TIME mtEnd, IDirectMusicSegment** ppSegment) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetStartPoint(MUSIC_TIME mtStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStartPoint(MUSIC_TIME* pmtStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetLoopPoints(MUSIC_TIME mtStart, MUSIC_TIME mtEnd) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLoopPoints(MUSIC_TIME* pmtStart, MUSIC_TIME* pmtEnd) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPChannelsUsed(DWORD dwNumPChannels, DWORD* paPChannels) = 0;
};

// ---- IDirectMusicStyle
struct IDirectMusicStyle : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetBand(WCHAR* pwszName, IDirectMusicBand** ppBand) = 0;   // +0x0c
    virtual HRESULT STDMETHODCALLTYPE EnumBand(DWORD dwIndex, WCHAR* pwszName) = 0;               // +0x10
    virtual HRESULT STDMETHODCALLTYPE GetDefaultBand(IDirectMusicBand** ppBand) = 0;              // +0x14
    virtual HRESULT STDMETHODCALLTYPE EnumMotif(DWORD dwIndex, WCHAR* pwszName) = 0;              // +0x18
    virtual HRESULT STDMETHODCALLTYPE GetMotif(WCHAR* pwszName, IDirectMusicSegment** ppSegment) = 0;   // +0x1c
    virtual HRESULT STDMETHODCALLTYPE GetDefaultChordMap(IDirectMusicChordMap** ppChordMap) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumChordMap(DWORD dwIndex, WCHAR* pwszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChordMap(WCHAR* pwszName, IDirectMusicChordMap** ppChordMap) = 0;
};

#define DMUS_MAX_NAME 64
#define DMUS_MAX_CATEGORY 64
#define DMUS_MAX_FILENAME MAX_PATH
#define DMUS_MAX_DESCRIPTION 128

// ---- messages (dmusici.h; DMUS_PMSG_PART, natural packing: rtTime at +0x08)
struct IDirectMusicTool;
#define DMUS_PMSG_PART                                                                                              \
    DWORD dwSize;                                                                                                   \
    REFERENCE_TIME rtTime;                                                                                          \
    MUSIC_TIME mtTime;                                                                                              \
    DWORD dwFlags;                                                                                                  \
    DWORD dwPChannel;                                                                                               \
    DWORD dwVirtualTrackID;                                                                                         \
    IDirectMusicTool* pTool;                                                                                        \
    IDirectMusicGraph* pGraph;                                                                                      \
    DWORD dwType;                                                                                                   \
    DWORD dwVoiceID;                                                                                                \
    DWORD dwGroupID;                                                                                                \
    IUnknown* punkUser;
typedef struct _DMUS_PMSG { DMUS_PMSG_PART } DMUS_PMSG;
typedef struct _DMUS_NOTIFICATION_PMSG {    // sizeof 0x58
    DMUS_PMSG_PART
    GUID guidNotificationType;              // +0x38
    DWORD dwNotificationOption;             // +0x48
    DWORD dwField1;
    DWORD dwField2;
} DMUS_NOTIFICATION_PMSG;
#define DMUS_NOTIFICATION_SEGSTART 0
#define DMUS_NOTIFICATION_SEGEND 1
#define DMUS_NOTIFICATION_SEGALMOSTEND 2
#define DMUS_NOTIFICATION_SEGLOOP 3
#define DMUS_NOTIFICATION_SEGABORT 4
#define DMUS_NOTIFICATION_MUSICSTARTED 0
#define DMUS_NOTIFICATION_MUSICSTOPPED 1

// ---- object descriptor (sizeof 0x350)
typedef struct _DMUS_VERSION { DWORD dwVersionMS; DWORD dwVersionLS; } DMUS_VERSION;
typedef struct _DMUS_OBJECTDESC {
    DWORD dwSize;
    DWORD dwValidData;
    GUID guidObject;
    GUID guidClass;                         // +0x18
    FILETIME ftDate;
    DMUS_VERSION vVersion;
    WCHAR wszName[DMUS_MAX_NAME];
    WCHAR wszCategory[DMUS_MAX_CATEGORY];
    WCHAR wszFileName[DMUS_MAX_FILENAME];   // +0x138
    LONGLONG llMemLength;
    LPBYTE pbMemData;
    IStream* pStream;
} DMUS_OBJECTDESC;
#define DMUS_OBJ_OBJECT (1 << 0)
#define DMUS_OBJ_CLASS (1 << 1)
#define DMUS_OBJ_NAME (1 << 2)
#define DMUS_OBJ_CATEGORY (1 << 3)
#define DMUS_OBJ_FILENAME (1 << 4)
#define DMUS_S_PARTIALLOAD MAKE_HRESULT(0, 0x878, 0x1091)

// ---- ports (dmusicc.h; DX8 DMUS_PORTPARAMS8 is 0x24 bytes)
typedef struct _DMUS_PORTCAPS {             // sizeof 0x134
    DWORD dwSize;
    DWORD dwFlags;                          // +0x04
    GUID guidPort;                          // +0x08
    DWORD dwClass;                          // +0x18
    DWORD dwType;
    DWORD dwMemorySize;
    DWORD dwMaxChannelGroups;               // +0x24
    DWORD dwMaxVoices;
    DWORD dwMaxAudioChannels;
    DWORD dwEffectFlags;
    WCHAR wszDescription[DMUS_MAX_DESCRIPTION];
} DMUS_PORTCAPS;
typedef struct _DMUS_PORTPARAMS {           // sizeof 0x24
    DWORD dwSize;
    DWORD dwValidParams;
    DWORD dwVoices;
    DWORD dwChannelGroups;                  // +0x0c
    DWORD dwAudioChannels;
    DWORD dwSampleRate;
    DWORD dwEffectFlags;
    BOOL fShare;
    DWORD dwFeatures;
} DMUS_PORTPARAMS;
#define DMUS_PC_DLS (0x00000001)
#define DMUS_PC_OUTPUTCLASS 1
#define DMUS_PORTPARAMS_CHANNELGROUPS 0x00000002
#define DMUS_EFFECT_REVERB 0x00000001
typedef struct _DMUS_WAVES_REVERB_PARAMS {
    float fInGain;
    float fReverbMix;
    float fReverbTime;
    float fHighFreqRTRatio;
} DMUS_WAVES_REVERB_PARAMS;

// segment/performance flags
#define DMUS_SEGF_QUEUE (1 << 8)
#define DMUS_SEGF_SECONDARY (1 << 7)
#define DMUS_SEGF_DEFAULT (1 << 14)
#define DMUS_COMMANDT_GROOVE 0
#define DMUS_COMMANDT_FILL 1
#define DMUS_COMMANDT_INTRO 2
#define DMUS_COMMANDT_BREAK 3
#define DMUS_COMMANDT_END 4
#define DMUS_COMMANDT_ENDANDINTRO 5
#define DMUS_COMPOSEF_MEASURE 0x20
#define DMUS_COMPOSEF_MODULATE 0x1000
#define DMUS_COMPOSEF_LONG 0x2000

struct IDirectMusicPort;
struct IDirectMusicBuffer;
struct IReferenceClock;
struct IDirectSound;
struct IDirectMusicDownload;
struct IDirectMusicInstrument;
struct IDirectMusicDownloadedInstrument;
struct IDirectMusicObject;
struct IDirectMusicTemplate;
struct _DMUS_CLOCKINFO;
struct _DMUS_SYNTHSTATS;
struct _DMUS_PORT_FORMAT;

// ---- IDirectMusic
struct IDirectMusic : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE EnumPort(DWORD dwIndex, DMUS_PORTCAPS* pPortCaps) = 0;        // +0x0c
    virtual HRESULT STDMETHODCALLTYPE CreateMusicBuffer(void* pBufferDesc, IDirectMusicBuffer** ppBuffer,
                                                        IUnknown* pUnkOuter) = 0;                    // +0x10
    virtual HRESULT STDMETHODCALLTYPE CreatePort(REFCLSID rclsidPort, DMUS_PORTPARAMS* pPortParams,
                                                 IDirectMusicPort** ppPort, IUnknown* pUnkOuter) = 0;   // +0x14
    virtual HRESULT STDMETHODCALLTYPE EnumMasterClock(DWORD dwIndex, _DMUS_CLOCKINFO* lpClockInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMasterClock(GUID* pguidClock, IReferenceClock** ppReferenceClock) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMasterClock(REFGUID rguidClock) = 0;
    virtual HRESULT STDMETHODCALLTYPE Activate(BOOL fEnable) = 0;                                  // +0x24
    virtual HRESULT STDMETHODCALLTYPE GetDefaultPort(GUID* pguidPort) = 0;                         // +0x28
    virtual HRESULT STDMETHODCALLTYPE SetDirectSound(IDirectSound* pDirectSound, HWND hWnd) = 0;
};

// ---- IDirectMusicPort
struct IDirectMusicPort : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE PlayBuffer(IDirectMusicBuffer* pBuffer) = 0;                 // +0x0c
    virtual HRESULT STDMETHODCALLTYPE SetReadNotificationHandle(HANDLE hEvent) = 0;
    virtual HRESULT STDMETHODCALLTYPE Read(IDirectMusicBuffer* pBuffer) = 0;
    virtual HRESULT STDMETHODCALLTYPE DownloadInstrument(IDirectMusicInstrument* pInstrument,
                                                         IDirectMusicDownloadedInstrument** ppDownloadedInstrument,
                                                         void* pNoteRanges, DWORD dwNumNoteRanges) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnloadInstrument(IDirectMusicDownloadedInstrument* pDownloadedInstrument) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLatencyClock(IReferenceClock** ppClock) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRunningStats(_DMUS_SYNTHSTATS* pStats) = 0;
    virtual HRESULT STDMETHODCALLTYPE Compact() = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCaps(DMUS_PORTCAPS* pPortCaps) = 0;                       // +0x2c
    virtual HRESULT STDMETHODCALLTYPE DeviceIoControl(DWORD dwIoControlCode, void* lpInBuffer, DWORD nInBufferSize,
                                                      void* lpOutBuffer, DWORD nOutBufferSize,
                                                      DWORD* lpBytesReturned, void* lpOverlapped) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetNumChannelGroups(DWORD dwChannelGroups) = 0;              // +0x34
    virtual HRESULT STDMETHODCALLTYPE GetNumChannelGroups(DWORD* pdwChannelGroups) = 0;
    virtual HRESULT STDMETHODCALLTYPE Activate(BOOL fActive) = 0;                                  // +0x3c
    virtual HRESULT STDMETHODCALLTYPE SetChannelPriority(DWORD dwChannelGroup, DWORD dwChannel, DWORD dwPriority) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelPriority(DWORD dwChannelGroup, DWORD dwChannel, DWORD* pdwPriority) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDirectSound(IDirectSound* pDirectSound, void* pDirectSoundBuffer) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFormat(void* pWaveFormatEx, DWORD* pdwWaveFormatExSize,
                                                DWORD* pdwBufferSize) = 0;
};

// ---- IDirectMusicLoader
struct IDirectMusicLoader : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetObject(DMUS_OBJECTDESC* pDesc, REFIID riid, LPVOID* ppv) = 0;   // +0x0c
    virtual HRESULT STDMETHODCALLTYPE SetObject(DMUS_OBJECTDESC* pDesc) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSearchDirectory(REFGUID rguidClass, WCHAR* pwzPath, BOOL fClear) = 0;   // +0x14
    virtual HRESULT STDMETHODCALLTYPE ScanDirectory(REFGUID rguidClass, WCHAR* pwzFileExtension,
                                                    WCHAR* pwzScanFileName) = 0;
    virtual HRESULT STDMETHODCALLTYPE CacheObject(IDirectMusicObject* pObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReleaseObject(IDirectMusicObject* pObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE ClearCache(REFGUID rguidClass) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnableCache(REFGUID rguidClass, BOOL fEnable) = 0;           // +0x28
    virtual HRESULT STDMETHODCALLTYPE EnumObject(REFGUID rguidClass, DWORD dwIndex, DMUS_OBJECTDESC* pDesc) = 0;
};

// ---- IDirectMusicSegmentState
struct IDirectMusicSegmentState : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetRepeats(DWORD* pdwRepeats) = 0;                            // +0x0c
    virtual HRESULT STDMETHODCALLTYPE GetSegment(IDirectMusicSegment** ppSegment) = 0;             // +0x10
    virtual HRESULT STDMETHODCALLTYPE GetStartTime(MUSIC_TIME* pmtStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSeek(MUSIC_TIME* pmtSeek) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStartPoint(MUSIC_TIME* pmtStart) = 0;
};

// ---- IDirectMusicPerformance
struct IDirectMusicPerformance : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Init(IDirectMusic** ppDirectMusic, IDirectSound* pDirectSound, HWND hWnd) = 0;   // +0x0c
    virtual HRESULT STDMETHODCALLTYPE PlaySegment(IDirectMusicSegment* pSegment, DWORD dwFlags, __int64 i64StartTime,
                                                  IDirectMusicSegmentState** ppSegmentState) = 0;   // +0x10
    virtual HRESULT STDMETHODCALLTYPE Stop(IDirectMusicSegment* pSegment, IDirectMusicSegmentState* pSegmentState,
                                           MUSIC_TIME mtTime, DWORD dwFlags) = 0;                  // +0x14
    virtual HRESULT STDMETHODCALLTYPE GetSegmentState(IDirectMusicSegmentState** ppSegmentState,
                                                      MUSIC_TIME mtTime) = 0;                      // +0x18
    virtual HRESULT STDMETHODCALLTYPE SetPrepareTime(DWORD dwMilliSeconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPrepareTime(DWORD* pdwMilliSeconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetBumperLength(DWORD dwMilliSeconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBumperLength(DWORD* pdwMilliSeconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE SendPMsg(DMUS_PMSG* pPMSG) = 0;
    virtual HRESULT STDMETHODCALLTYPE MusicToReferenceTime(MUSIC_TIME mtTime, REFERENCE_TIME* prtTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReferenceToMusicTime(REFERENCE_TIME rtTime, MUSIC_TIME* pmtTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsPlaying(IDirectMusicSegment* pSegment, IDirectMusicSegmentState* pSegState) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTime(REFERENCE_TIME* prtNow, MUSIC_TIME* pmtNow) = 0;     // +0x3c
    virtual HRESULT STDMETHODCALLTYPE AllocPMsg(ULONG cb, DMUS_PMSG** ppPMSG) = 0;
    virtual HRESULT STDMETHODCALLTYPE FreePMsg(DMUS_PMSG* pPMSG) = 0;                              // +0x44
    virtual HRESULT STDMETHODCALLTYPE GetGraph(IDirectMusicGraph** ppGraph) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetGraph(IDirectMusicGraph* pGraph) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetNotificationHandle(HANDLE hNotification, REFERENCE_TIME rtMinimum) = 0;   // +0x50
    virtual HRESULT STDMETHODCALLTYPE GetNotificationPMsg(DMUS_NOTIFICATION_PMSG** ppNotificationPMsg) = 0;   // +0x54
    virtual HRESULT STDMETHODCALLTYPE AddNotificationType(REFGUID rguidNotificationType) = 0;      // +0x58
    virtual HRESULT STDMETHODCALLTYPE RemoveNotificationType(REFGUID rguidNotificationType) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddPort(IDirectMusicPort* pPort) = 0;                        // +0x60
    virtual HRESULT STDMETHODCALLTYPE RemovePort(IDirectMusicPort* pPort) = 0;
    virtual HRESULT STDMETHODCALLTYPE AssignPChannelBlock(DWORD dwBlockNum, IDirectMusicPort* pPort,
                                                          DWORD dwGroup) = 0;                       // +0x68
    virtual HRESULT STDMETHODCALLTYPE AssignPChannel(DWORD dwPChannel, IDirectMusicPort* pPort, DWORD dwGroup,
                                                     DWORD dwMChannel) = 0;
    virtual HRESULT STDMETHODCALLTYPE PChannelInfo(DWORD dwPChannel, IDirectMusicPort** ppPort, DWORD* pdwGroup,
                                                   DWORD* pdwMChannel) = 0;
    virtual HRESULT STDMETHODCALLTYPE DownloadInstrument(IDirectMusicInstrument* pInst, DWORD dwPChannel,
                                                         IDirectMusicDownloadedInstrument** ppDownInst,
                                                         void* pNoteRanges, DWORD dwNumNoteRanges,
                                                         IDirectMusicPort** ppPort, DWORD* pdwGroup,
                                                         DWORD* pdwMChannel) = 0;
    virtual HRESULT STDMETHODCALLTYPE Invalidate(MUSIC_TIME mtTime, DWORD dwFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetParam(REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime,
                                               MUSIC_TIME* pmtNext, void* pParam) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetParam(REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime,
                                               void* pParam) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetGlobalParam(REFGUID rguidType, void* pParam, DWORD dwSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetGlobalParam(REFGUID rguidType, void* pParam, DWORD dwSize) = 0;   // +0x88
    virtual HRESULT STDMETHODCALLTYPE GetLatencyTime(REFERENCE_TIME* prtTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetQueueTime(REFERENCE_TIME* prtTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE AdjustTime(REFERENCE_TIME rtAmount) = 0;
    virtual HRESULT STDMETHODCALLTYPE CloseDown() = 0;                                              // +0x98
};

// ---- IDirectMusicComposer
struct IDirectMusicComposer : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE ComposeSegmentFromTemplate(IDirectMusicStyle* pStyle,
                                                                 IDirectMusicSegment* pTemplate, WORD wActivity,
                                                                 IDirectMusicChordMap* pChordMap,
                                                                 IDirectMusicSegment** ppSegment) = 0;   // +0x0c
    virtual HRESULT STDMETHODCALLTYPE ComposeSegmentFromShape(IDirectMusicStyle* pStyle, WORD wNumMeasures,
                                                              WORD wShape, WORD wActivity, BOOL fIntro, BOOL fEnd,
                                                              IDirectMusicChordMap* pChordMap,
                                                              IDirectMusicSegment** ppSegment) = 0;
    virtual HRESULT STDMETHODCALLTYPE ComposeTransition(IDirectMusicSegment* pFromSeg, IDirectMusicSegment* pToSeg,
                                                        MUSIC_TIME mtTime, WORD wCommand, DWORD dwFlags,
                                                        IDirectMusicChordMap* pChordMap,
                                                        IDirectMusicSegment** ppTransSeg) = 0;
    virtual HRESULT STDMETHODCALLTYPE AutoTransition(IDirectMusicPerformance* pPerformance,
                                                     IDirectMusicSegment* pToSeg, WORD wCommand, DWORD dwFlags,
                                                     IDirectMusicChordMap* pChordMap,
                                                     IDirectMusicSegment** ppTransSeg,
                                                     IDirectMusicSegmentState** ppToSegState,
                                                     IDirectMusicSegmentState** ppTransSegState) = 0;   // +0x18
    virtual HRESULT STDMETHODCALLTYPE ComposeTemplateFromShape(WORD wNumMeasures, WORD wShape, BOOL fIntro,
                                                               BOOL fEnd, WORD wEndLength,
                                                               IDirectMusicSegment** ppTemplate) = 0;
    virtual HRESULT STDMETHODCALLTYPE ChangeChordMap(IDirectMusicSegment* pSegment, BOOL fTrackScale,
                                                     IDirectMusicChordMap* pChordMap) = 0;
};

// ---- ks.h: IKsControl (dsound.h's KSPROPERTY)
typedef struct {
    GUID Set;
    ULONG Id;
    ULONG Flags;
} KSPROPERTY;
#define KSPROPERTY_TYPE_GET 0x00000001
#define KSPROPERTY_TYPE_SET 0x00000002
struct IKsControl : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE KsProperty(KSPROPERTY* Property, ULONG PropertyLength, LPVOID PropertyData,
                                                 ULONG DataLength, ULONG* BytesReturned) = 0;       // +0x0c
    virtual HRESULT STDMETHODCALLTYPE KsMethod(KSPROPERTY* Method, ULONG MethodLength, LPVOID MethodData,
                                               ULONG DataLength, ULONG* BytesReturned) = 0;
    virtual HRESULT STDMETHODCALLTYPE KsEvent(KSPROPERTY* Event, ULONG EventLength, LPVOID EventData,
                                              ULONG DataLength, ULONG* BytesReturned) = 0;
};

// dxguid.lib (SEConductor.cpp)
EXTERN_C const GUID CLSID_DirectMusicPerformance;     // d2ac2881-b39b-11d1-8704-00600893b1bd
EXTERN_C const GUID CLSID_DirectMusicSegment;         // d2ac2882-...
EXTERN_C const GUID CLSID_DirectMusicComposer;        // d2ac2890-...
EXTERN_C const GUID CLSID_DirectMusicLoader;          // d2ac2892-...
EXTERN_C const GUID GUID_DirectMusicAllTypes;         // d2ac2893-...
EXTERN_C const GUID GUID_NOTIFICATION_SEGMENT;        // d2ac2899-...
EXTERN_C const GUID GUID_Download;                    // d2ac28a7-...
EXTERN_C const GUID GUID_PerfMasterGrooveLevel;       // d2ac28b2-...
EXTERN_C const GUID IID_IDirectMusicComposer;         // d2ac28bf-...
EXTERN_C const GUID GUID_NOTIFICATION_PERFORMANCE;    // 81f75bc5-4e5d-11d2-bcc7-00a0c922e6eb
EXTERN_C const GUID IID_IDirectMusicLoader;           // 2ffaaca2-5dca-11d2-afa6-00aa0024d8b6
EXTERN_C const GUID IID_IDirectMusicPerformance2;     // 6fc2cae0-bc78-11d2-afa6-00aa0024d8b6
EXTERN_C const GUID IID_IDirectMusicSegment2;         // d38894d1-c052-11d2-872f-00600893b1bd
EXTERN_C const GUID IID_IDirectMusicSegmentState;     // a3afdcc7-d3ee-11d1-bc8d-00a0c922e6eb
EXTERN_C const GUID GUID_PerfAutoDownload;            // fb09565b-3631-11d2-bcb8-00a0c922e6eb
EXTERN_C const GUID IID_IKsControl;                   // 28f54685-06fd-11d2-b27a-00a0c9223196
EXTERN_C const GUID GUID_DMUS_PROP_Volume;            // fedfae25-e46e-11d1-aace-0000f875ac12
EXTERN_C const GUID GUID_DMUS_PROP_Effects;           // cda8d611-684a-11d2-871e-00600893b1bd
EXTERN_C const GUID GUID_DMUS_PROP_WavesReverb;       // 04cb5622-32e5-11d2-afa6-00aa0024d8b6

// dxguid.lib
EXTERN_C const GUID GUID_IDirectMusicStyle;     // d2ac28a1-b39b-11d1-8704-00600893b1bd
EXTERN_C const GUID GUID_Unload;                // d2ac28a8-b39b-11d1-8704-00600893b1bd

#endif
