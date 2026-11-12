// The native Linux build's DirectDraw 7 and Direct3D 7 on OpenGL 3.3: what DirectDraw.cpp and Direct3D.cpp share.
//
// Every surface keeps its pixels in CPU memory, in the exact layout of its pixel format, and may keep an RGBA8
// OpenGL texture with the same rows, row 0 first. Either copy can be the newer one: Direct3D draws write the texture,
// and Lock, GetDC and CPU blits write the memory. A surface converts one copy into the other when a caller needs it.
//
// One recursive lock serialises every call, as DDSCL_MULTITHREADED does. The OpenGL context is current on no thread
// between calls: a call that needs OpenGL makes it current on its own thread for the call (GLScope), so that the
// game's five threads that use DirectDraw can all reach it. Direct3D therefore records its draws and runs them as a
// batch at EndScene, and before anything reads or writes a surface that a recorded draw uses.
#ifndef EE_PORT_LINUX_DIRECTDRAWINTERNAL_H
#define EE_PORT_LINUX_DIRECTDRAWINTERNAL_H

#include "Win32Internal.h"

#include <ddraw.h>
#include <d3d.h>

#include <vector>

struct SDL_Window;

namespace eeport {
namespace dx {

// ---- OpenGL 3.3 core: the names the layer calls, loaded through SDL_GL_GetProcAddress ----
// Declared here with the platform's C calling convention: SDL_opengl.h would take Wine's APIENTRY, which is stdcall.
using GLenum = unsigned int;
using GLuint = unsigned int;
using GLint = int;
using GLsizei = int;
using GLboolean = unsigned char;
using GLbitfield = unsigned int;
using GLfloat = float;
using GLdouble = double;
using GLchar = char;
using GLsizeiptr = intptr_t;
using GLintptr = intptr_t;

enum : GLenum {
    GL_ZERO = 0, GL_ONE = 1, GL_POINTS = 0, GL_LINES = 1, GL_LINE_STRIP = 3, GL_TRIANGLES = 4, GL_TRIANGLE_STRIP = 5,
    GL_TRIANGLE_FAN = 6, GL_NEVER = 0x0200, GL_LESS = 0x0201, GL_EQUAL = 0x0202, GL_LEQUAL = 0x0203,
    GL_GREATER = 0x0204, GL_NOTEQUAL = 0x0205, GL_GEQUAL = 0x0206, GL_ALWAYS = 0x0207, GL_SRC_COLOR = 0x0300,
    GL_ONE_MINUS_SRC_COLOR = 0x0301, GL_SRC_ALPHA = 0x0302, GL_ONE_MINUS_SRC_ALPHA = 0x0303, GL_DST_ALPHA = 0x0304,
    GL_ONE_MINUS_DST_ALPHA = 0x0305, GL_DST_COLOR = 0x0306, GL_ONE_MINUS_DST_COLOR = 0x0307,
    GL_SRC_ALPHA_SATURATE = 0x0308, GL_FRONT = 0x0404, GL_BACK = 0x0405, GL_FRONT_AND_BACK = 0x0408, GL_CW = 0x0900,
    GL_CCW = 0x0901, GL_CULL_FACE = 0x0B44, GL_DEPTH_TEST = 0x0B71, GL_BLEND = 0x0BE2, GL_SCISSOR_TEST = 0x0C11,
    GL_UNPACK_ROW_LENGTH = 0x0CF2, GL_UNPACK_ALIGNMENT = 0x0CF5, GL_PACK_ROW_LENGTH = 0x0D02,
    GL_PACK_ALIGNMENT = 0x0D05, GL_TEXTURE_2D = 0x0DE1, GL_UNSIGNED_BYTE = 0x1401, GL_UNSIGNED_SHORT = 0x1403,
    GL_FLOAT = 0x1406, GL_RGBA = 0x1908, GL_POINT = 0x1B00, GL_LINE = 0x1B01, GL_FILL = 0x1B02, GL_VENDOR = 0x1F00,
    GL_RENDERER = 0x1F01, GL_VERSION = 0x1F02, GL_NEAREST = 0x2600, GL_LINEAR = 0x2601,
    GL_NEAREST_MIPMAP_NEAREST = 0x2700, GL_LINEAR_MIPMAP_NEAREST = 0x2701, GL_NEAREST_MIPMAP_LINEAR = 0x2702,
    GL_LINEAR_MIPMAP_LINEAR = 0x2703, GL_TEXTURE_MAG_FILTER = 0x2800, GL_TEXTURE_MIN_FILTER = 0x2801,
    GL_TEXTURE_WRAP_S = 0x2802, GL_TEXTURE_WRAP_T = 0x2803, GL_REPEAT = 0x2901, GL_DEPTH_BUFFER_BIT = 0x0100,
    GL_COLOR_BUFFER_BIT = 0x4000, GL_RGBA8 = 0x8058, GL_BGRA = 0x80E1, GL_CLAMP_TO_EDGE = 0x812F,
    GL_TEXTURE_BASE_LEVEL = 0x813C, GL_TEXTURE_MAX_LEVEL = 0x813D, GL_DEPTH_COMPONENT16 = 0x81A5,
    GL_DEPTH_COMPONENT24 = 0x81A6, GL_MIRRORED_REPEAT = 0x8370, GL_TEXTURE0 = 0x84C0, GL_TEXTURE_LOD_BIAS = 0x8501,
    GL_ARRAY_BUFFER = 0x8892, GL_ELEMENT_ARRAY_BUFFER = 0x8893, GL_STREAM_DRAW = 0x88E0,
    GL_FRAGMENT_SHADER = 0x8B30, GL_VERTEX_SHADER = 0x8B31, GL_COMPILE_STATUS = 0x8B81, GL_LINK_STATUS = 0x8B82,
    GL_INFO_LOG_LENGTH = 0x8B84, GL_READ_FRAMEBUFFER = 0x8CA8, GL_DRAW_FRAMEBUFFER = 0x8CA9,
    GL_FRAMEBUFFER_COMPLETE = 0x8CD5, GL_COLOR_ATTACHMENT0 = 0x8CE0, GL_DEPTH_ATTACHMENT = 0x8D00,
    GL_FRAMEBUFFER = 0x8D40, GL_RENDERBUFFER = 0x8D41, GL_FIRST_VERTEX_CONVENTION = 0x8E4D,
};

#define EE_GL_FUNCTIONS(X)                                                                                         \
    X(const unsigned char*, GetString, GLenum)                                                                     \
    X(GLenum, GetError, void)                                                                                      \
    X(void, Enable, GLenum)                                                                                        \
    X(void, Disable, GLenum)                                                                                       \
    X(void, Viewport, GLint, GLint, GLsizei, GLsizei)                                                              \
    X(void, Scissor, GLint, GLint, GLsizei, GLsizei)                                                               \
    X(void, ClearColor, GLfloat, GLfloat, GLfloat, GLfloat)                                                        \
    X(void, ClearDepth, GLdouble)                                                                                  \
    X(void, Clear, GLbitfield)                                                                                     \
    X(void, ColorMask, GLboolean, GLboolean, GLboolean, GLboolean)                                                 \
    X(void, DepthMask, GLboolean)                                                                                  \
    X(void, DepthFunc, GLenum)                                                                                     \
    X(void, DepthRange, GLdouble, GLdouble)                                                                        \
    X(void, BlendFunc, GLenum, GLenum)                                                                             \
    X(void, CullFace, GLenum)                                                                                      \
    X(void, FrontFace, GLenum)                                                                                     \
    X(void, PolygonMode, GLenum, GLenum)                                                                           \
    X(void, PixelStorei, GLenum, GLint)                                                                            \
    X(void, ReadPixels, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void*)                                     \
    X(void, GenTextures, GLsizei, GLuint*)                                                                         \
    X(void, DeleteTextures, GLsizei, const GLuint*)                                                                \
    X(void, BindTexture, GLenum, GLuint)                                                                           \
    X(void, TexImage2D, GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*)                \
    X(void, TexSubImage2D, GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void*)             \
    X(void, TexParameteri, GLenum, GLenum, GLint)                                                                  \
    X(void, TexParameterf, GLenum, GLenum, GLfloat)                                                                \
    X(void, ActiveTexture, GLenum)                                                                                 \
    X(void, DrawArrays, GLenum, GLint, GLsizei)                                                                    \
    X(void, DrawElementsBaseVertex, GLenum, GLsizei, GLenum, const void*, GLint)                                   \
    X(void, GenFramebuffers, GLsizei, GLuint*)                                                                     \
    X(void, DeleteFramebuffers, GLsizei, const GLuint*)                                                            \
    X(void, BindFramebuffer, GLenum, GLuint)                                                                       \
    X(void, FramebufferTexture2D, GLenum, GLenum, GLenum, GLuint, GLint)                                           \
    X(void, FramebufferRenderbuffer, GLenum, GLenum, GLenum, GLuint)                                               \
    X(GLenum, CheckFramebufferStatus, GLenum)                                                                      \
    X(void, GenRenderbuffers, GLsizei, GLuint*)                                                                    \
    X(void, DeleteRenderbuffers, GLsizei, const GLuint*)                                                           \
    X(void, BindRenderbuffer, GLenum, GLuint)                                                                      \
    X(void, RenderbufferStorage, GLenum, GLenum, GLsizei, GLsizei)                                                 \
    X(void, GenBuffers, GLsizei, GLuint*)                                                                          \
    X(void, BindBuffer, GLenum, GLuint)                                                                            \
    X(void, BufferData, GLenum, GLsizeiptr, const void*, GLenum)                                                   \
    X(void, GenVertexArrays, GLsizei, GLuint*)                                                                     \
    X(void, BindVertexArray, GLuint)                                                                               \
    X(void, VertexAttribPointer, GLuint, GLint, GLenum, GLboolean, GLsizei, const void*)                           \
    X(void, EnableVertexAttribArray, GLuint)                                                                       \
    X(GLuint, CreateShader, GLenum)                                                                                \
    X(void, ShaderSource, GLuint, GLsizei, const GLchar* const*, const GLint*)                                     \
    X(void, CompileShader, GLuint)                                                                                 \
    X(void, GetShaderiv, GLuint, GLenum, GLint*)                                                                   \
    X(void, GetShaderInfoLog, GLuint, GLsizei, GLsizei*, GLchar*)                                                  \
    X(void, DeleteShader, GLuint)                                                                                  \
    X(GLuint, CreateProgram, void)                                                                                 \
    X(void, AttachShader, GLuint, GLuint)                                                                          \
    X(void, BindAttribLocation, GLuint, GLuint, const GLchar*)                                                     \
    X(void, LinkProgram, GLuint)                                                                                   \
    X(void, GetProgramiv, GLuint, GLenum, GLint*)                                                                  \
    X(void, GetProgramInfoLog, GLuint, GLsizei, GLsizei*, GLchar*)                                                 \
    X(void, UseProgram, GLuint)                                                                                    \
    X(GLint, GetUniformLocation, GLuint, const GLchar*)                                                            \
    X(void, Uniform1i, GLint, GLint)                                                                               \
    X(void, Uniform1f, GLint, GLfloat)                                                                             \
    X(void, Uniform2f, GLint, GLfloat, GLfloat)                                                                    \
    X(void, Uniform4f, GLint, GLfloat, GLfloat, GLfloat, GLfloat)                                                  \
    X(void, Uniform2i, GLint, GLint, GLint)                                                                        \
    X(void, Uniform3i, GLint, GLint, GLint, GLint)                                                                 \
    X(void, Uniform4i, GLint, GLint, GLint, GLint, GLint)                                                          \
    X(void, Uniform1iv, GLint, GLsizei, const GLint*)                                                              \
    X(void, Uniform3fv, GLint, GLsizei, const GLfloat*)                                                            \
    X(void, Uniform4fv, GLint, GLsizei, const GLfloat*)                                                            \
    X(void, UniformMatrix3fv, GLint, GLsizei, GLboolean, const GLfloat*)                                           \
    X(void, UniformMatrix4fv, GLint, GLsizei, GLboolean, const GLfloat*)                                           \
    X(void, ProvokingVertex, GLenum)                                                                               \
    X(void, GenSamplers, GLsizei, GLuint*)                                                                         \
    X(void, BindSampler, GLuint, GLuint)                                                                           \
    X(void, SamplerParameteri, GLuint, GLenum, GLint)                                                              \
    X(void, SamplerParameterf, GLuint, GLenum, GLfloat)                                                            \
    X(void, Flush, void)

struct GL {
#define EE_GL_MEMBER(result, name, ...) result (*name)(__VA_ARGS__);
    EE_GL_FUNCTIONS(EE_GL_MEMBER)
#undef EE_GL_MEMBER
};
extern GL gl;

// ---- the lock and the OpenGL context ----
extern std::recursive_mutex gLock;

// Makes the context current on this thread until the scope ends, on the window that presents, or on the layer's own
// hidden window. Scopes nest; the outermost one releases the context. Call it under gLock only.
class GLScope {
public:
    explicit GLScope(SDL_Window* inWindow = nullptr);
    ~GLScope();
    GLScope(const GLScope&) = delete;
    GLScope& operator=(const GLScope&) = delete;
    bool Ok() const { return mOk; }

private:
    bool mOk;
};

// Deletes OpenGL names at the next scope: a surface can go on a thread that holds no context
void DeleteTexture(GLuint inTexture);
void DeleteRenderbuffer(GLuint inRenderbuffer);

// ---- pixel formats ----
// A format the layer stores: RGB at 16 or 32 bits with optional alpha, or a Z buffer
struct Format {
    bool mZ = false;
    int mBytes = 4;
    int mZBits = 0;
    DWORD mMask[4] = {};        // red, green, blue, alpha
    int mShift[4] = {};
    DWORD mMax[4] = {};         // the channel's largest value; 0 for no channel

    static bool From(const DDPIXELFORMAT& inFormat, Format* outFormat);
    bool HasAlpha() const { return mMax[3] != 0; }
    // A pixel's RGBA bytes: each channel scaled to 0..255 with rounding, as OpenGL converts normalised values; alpha
    // 255 when the format has none
    void Decode(const uint8_t* inPixel, uint8_t* outRGBA) const;
    // The pixel of RGBA bytes, each channel rounded to the nearest value the format holds
    void Encode(const uint8_t* inRGBA, uint8_t* outPixel) const;
    DWORD Read(const uint8_t* inPixel) const;
    void Write(uint8_t* outPixel, DWORD inValue) const;
};

class DirectDraw;
class Surface;

// ---- Direct3D (Direct3D.cpp) ----
// Runs every device's recorded draws; with a surface, only when a recorded draw writes or reads it
void SubmitDraws(Surface* inSurface = nullptr);
// The IDirect3D7 of a DirectDraw object, with a reference
HRESULT NewDirect3D(IDirectDraw7* inOwner, void** outObject);

// ---- surfaces (DirectDraw.cpp) ----
class Surface final : public IDirectDrawSurface7 {
public:
    enum Kind { kPrimary, kBack, kOffscreen, kTexture, kZ };

    // ---- the layer's side ----
    Kind mKind = kOffscreen;
    DirectDraw* mOwner = nullptr;
    DDSURFACEDESC2 mDesc = {};      // as created, with lPitch and the full pixel format
    Format mFormat;
    int mWidth = 0, mHeight = 0, mPitch = 0;

    // The surface's storage. Flip exchanges it between the surfaces of a flip chain.
    struct Store {
        std::vector<uint8_t> mBits;
        GLuint mTexture = 0;        // RGBA8 for a colour surface, a renderbuffer for a Z buffer
        bool mCpuValid = true;      // the memory holds the current image
        bool mGpuValid = false;     // the texture holds the current image
        int mKeyed = -1;            // for a texture: whether its upload turned colour-keyed texels transparent
    } mStore;

    bool mHasKey = false;
    DDCOLORKEY mKey = {};
    Surface* mRoot = this;          // the surface that owns a flip chain or a mip chain
    std::vector<Surface*> mChain;   // the root's: back buffers or mip levels, in order
    Surface* mZ = nullptr;          // the attached Z buffer
    int mLocks = 0;
    bool mLockedReadOnly = false;
    HDC mDC = NULL;
    std::vector<uint8_t> mDCBase;   // the image when GetDC gave the DC, to find what GDI drew
    int mRecordedUses = 0;          // draws recorded and not yet run that use this surface
    int mDeviceUses = 0;            // Direct3D devices that render into it, each with a reference
    bool mOrphan = false;           // a chain member whose root went while the game still held it

    Surface(DirectDraw* inOwner, Kind inKind, const DDSURFACEDESC2& inDesc, const Format& inFormat, int inWidth,
            int inHeight);
    ~Surface();

    // The pixels at (x, y)
    uint8_t* Pixel(int inX, int inY) { return mStore.mBits.data() + (size_t)inY * mPitch + (size_t)inX * mFormat.mBytes; }
    bool IsRenderable() const { return mKind == kPrimary || mKind == kBack || (mDesc.ddsCaps.dwCaps & DDSCAPS_3DDEVICE); }
    // The surface whose texture a draw samples: a mip level's root
    Surface* TextureRoot() { return mDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP ? mRoot : this; }
    // Under a GLScope: makes the texture hold the image, uploading the memory when it is newer. inKeyed asks for the
    // upload that Direct3D's colour keying samples: texels inside the source colour key get alpha 0.
    bool EnsureGpu(bool inKeyed = false);
    // Under gLock: makes the memory hold the image, running recorded draws and reading the texture back as needed
    void EnsureCpu();
    // The memory was written: the texture is out of date, for a mip level the root's too
    void CpuWritten();
    // Under gLock, while GDI holds the surface's DC: runs the draws recorded so far, then lays what GDI wrote since
    // the last call over them, so that draws and GDI reach the image in the order of the calls
    void TakeGdi();
    // Under a GLScope: the RGBA8 texture's name, made at the surface size when missing
    GLuint ColourTexture();
    // Under a GLScope: the Z buffer's renderbuffer, made when missing
    GLuint DepthBuffer();

    // ---- IDirectDrawSurface7 ----
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override;
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef() override;
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release() override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE AddAttachedSurface(IDirectDrawSurface7* inSurface) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE AddOverlayDirtyRect(LPRECT) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Blt(RECT* inDestination, IDirectDrawSurface7* inSource,
                                                      RECT* inSourceRect, DWORD inFlags, DDBLTFX* inFX) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE BltBatch(DDBLTBATCH*, DWORD, DWORD) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE BltFast(DWORD inX, DWORD inY, IDirectDrawSurface7* inSource,
                                                          RECT* inSourceRect, DWORD inFlags) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DeleteAttachedSurface(DWORD, IDirectDrawSurface7* inSurface) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumAttachedSurfaces(void*, LPDDENUMSURFACESCALLBACK7) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumOverlayZOrders(DWORD, void*, LPDDENUMSURFACESCALLBACK7) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Flip(IDirectDrawSurface7* inTarget, DWORD inFlags) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetAttachedSurface(DDSCAPS2* inCaps,
                                                                     IDirectDrawSurface7** outSurface) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetBltStatus(DWORD) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetCaps(DDSCAPS2* outCaps) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetClipper(IDirectDrawClipper**) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetColorKey(DWORD inFlags, DDCOLORKEY* outKey) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDC(HDC* outDC) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetFlipStatus(DWORD) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetOverlayPosition(LPLONG, LPLONG) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPalette(IDirectDrawPalette**) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPixelFormat(DDPIXELFORMAT* outFormat) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetSurfaceDesc(DDSURFACEDESC2* outDesc) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Initialize(IDirectDraw*, DDSURFACEDESC2*) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IsLost() override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Lock(RECT* inRect, DDSURFACEDESC2* outDesc, DWORD inFlags,
                                                       HANDLE) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ReleaseDC(HDC inDC) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Restore() override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetClipper(IDirectDrawClipper*) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetColorKey(DWORD inFlags, DDCOLORKEY* inKey) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetOverlayPosition(LONG, LONG) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetPalette(IDirectDrawPalette*) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Unlock(LPRECT) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE UpdateOverlay(RECT*, IDirectDrawSurface7*, RECT*, DWORD,
                                                                DDOVERLAYFX*) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE UpdateOverlayDisplay(DWORD) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE UpdateOverlayZOrder(DWORD, IDirectDrawSurface7*) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDDInterface(void** outDirectDraw) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE PageLock(DWORD) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE PageUnlock(DWORD) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetSurfaceDesc(DDSURFACEDESC2*, DWORD) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID, void*, DWORD, DWORD) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID, void*, DWORD*) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetUniquenessValue(LPDWORD) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ChangeUniquenessValue() override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetPriority(DWORD) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPriority(LPDWORD outPriority) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetLOD(DWORD) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetLOD(LPDWORD outLOD) override;

    std::atomic<long> mReferences{1};
};

// The layer's surface behind a game pointer, or NULL
Surface* SurfaceOf(IDirectDrawSurface7* inSurface);

// Under a GLScope: draws a source rectangle of an RGBA8 texture into a rectangle of a target texture, nearest
// sampled, skipping texels whose alpha is below one half when inKeyed. Rectangles are in rows from the top; the
// source's is left, top, right and bottom in texels.
void DrawTexture(GLuint inTarget, int inTargetWidth, int inTargetHeight, const RECT& inTargetRect, GLuint inSource,
                 int inSourceWidth, int inSourceHeight, const float inSourceRect[4], bool inKeyed);
// Under a GLScope: a framebuffer with the colour texture and, when given, the depth renderbuffer attached
GLuint BindTarget(GLuint inColour, GLuint inDepth);

} // namespace dx
} // namespace eeport

#endif
