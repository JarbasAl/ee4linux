// DirectDraw 7 (ddraw.dll) for the DX7 display plug-in, on OpenGL 3.3 through SDL. DirectDrawInternal.h describes
// how surfaces keep their memory and their texture in step, and how the threads share the context.
//
// There is one device, the primary display, with the desktop's size. A display mode is a size and a depth that the
// layer pretends to set: in full screen the game window covers the desktop and shows the primary scaled to fit, with
// its aspect kept. The primary of a windowed game is the window's client area, and its rectangles are in screen
// coordinates, as on the desktop surface of Windows. Flip shows the primary at once; other writes to it show within
// 16 ms (PresentQueue).
//
// EE_PORT_FRAME_DUMP names a folder: every 5 seconds a shown frame is written there as frame-NNN.ppm, at most 200 of
// them, so a run on SDL's offscreen driver can be inspected.
#include "DirectDrawInternal.h"

#include <SDL.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <set>
#include <thread>

namespace eeport {
namespace dx {

GL gl;
std::recursive_mutex gLock;

namespace {

SDL_GLContext gContext;
SDL_Window* gHiddenWindow;      // the window the context is current on outside a present
std::string gRenderer;
int gScopeDepth;
SDL_Window* gScopeWindow;
int gSwapInterval = -1;
std::vector<GLuint> gDeadTextures, gDeadRenderbuffers;

GLuint gBlitProgram, gBlitVertexArray, gBlitBuffer, gFramebuffer, gScratch, gNearest, gLinear;
GLint gBlitKeyed;

const char kBlitVertex[] = R"(#version 330 core
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexel;
out vec2 vTexel;
void main() { vTexel = aTexel; gl_Position = vec4(aPosition, 0.0, 1.0); }
)";

const char kBlitFragment[] = R"(#version 330 core
uniform sampler2D uSource;
uniform int uKeyed;
in vec2 vTexel;
out vec4 oColour;
void main()
{
    vec4 theColour = texture(uSource, vTexel);
    if (uKeyed != 0 && theColour.a < 0.5)
        discard;
    oColour = theColour;
}
)";

GLuint Compile(GLenum inKind, const char* inSource)
{
    GLuint theShader = gl.CreateShader(inKind);
    gl.ShaderSource(theShader, 1, &inSource, nullptr);
    gl.CompileShader(theShader);
    GLint theOk = 0;
    gl.GetShaderiv(theShader, GL_COMPILE_STATUS, &theOk);
    if (!theOk) {
        char theLog[2048] = {};
        gl.GetShaderInfoLog(theShader, sizeof theLog, nullptr, theLog);
        Log("OpenGL shader does not compile: %s", theLog);
    }
    return theShader;
}

bool StartGL()
{
    std::lock_guard<std::recursive_mutex> theSdlLock(gSdlLock);
    if (gContext)
        return true;
    if (!StartVideo())
        return false;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    gHiddenWindow = SDL_CreateWindow("ee-port DirectDraw", 0, 0, 16, 16, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!gHiddenWindow) {
        Log("DirectDraw has no OpenGL window: %s", SDL_GetError());
        return false;
    }
    gContext = SDL_GL_CreateContext(gHiddenWindow);
    if (!gContext) {
        Log("DirectDraw has no OpenGL 3.3 core context: %s", SDL_GetError());
        SDL_DestroyWindow(gHiddenWindow);
        gHiddenWindow = nullptr;
        return false;
    }
    bool theLoaded = true;
#define EE_GL_LOAD(result, name, ...)                                                                              \
    gl.name = (result(*)(__VA_ARGS__))SDL_GL_GetProcAddress("gl" #name);                                           \
    if (!gl.name) {                                                                                                \
        Log("OpenGL has no gl" #name);                                                                             \
        theLoaded = false;                                                                                         \
    }
    EE_GL_FUNCTIONS(EE_GL_LOAD)
#undef EE_GL_LOAD
    if (!theLoaded) {
        SDL_GL_DeleteContext(gContext);
        gContext = nullptr;
        return false;
    }
    gRenderer = (const char*)gl.GetString(GL_RENDERER);
    Log("DirectDraw draws with OpenGL %s on %s", (const char*)gl.GetString(GL_VERSION), gRenderer.c_str());

    gBlitProgram = gl.CreateProgram();
    GLuint theVertex = Compile(GL_VERTEX_SHADER, kBlitVertex), theFragment = Compile(GL_FRAGMENT_SHADER, kBlitFragment);
    gl.AttachShader(gBlitProgram, theVertex);
    gl.AttachShader(gBlitProgram, theFragment);
    gl.LinkProgram(gBlitProgram);
    gl.DeleteShader(theVertex);
    gl.DeleteShader(theFragment);
    gl.UseProgram(gBlitProgram);
    gl.Uniform1i(gl.GetUniformLocation(gBlitProgram, "uSource"), 0);
    gBlitKeyed = gl.GetUniformLocation(gBlitProgram, "uKeyed");
    gl.GenVertexArrays(1, &gBlitVertexArray);
    gl.BindVertexArray(gBlitVertexArray);
    gl.GenBuffers(1, &gBlitBuffer);
    gl.BindBuffer(GL_ARRAY_BUFFER, gBlitBuffer);
    gl.VertexAttribPointer(0, 2, GL_FLOAT, 0, 16, (const void*)0);
    gl.VertexAttribPointer(1, 2, GL_FLOAT, 0, 16, (const void*)8);
    gl.EnableVertexAttribArray(0);
    gl.EnableVertexAttribArray(1);
    gl.GenFramebuffers(1, &gFramebuffer);
    gl.GenTextures(1, &gScratch);
    SDL_GL_MakeCurrent(gHiddenWindow, nullptr);
    return true;
}

// The OpenGL wrap mode and filters of a blit's source
void SetBlitTexture(GLuint inTexture, bool inLinear)
{
    gl.ActiveTexture(GL_TEXTURE0);
    gl.BindTexture(GL_TEXTURE_2D, inTexture);
    gl.BindSampler(0, 0);
    GLint theFilter = inLinear ? GL_LINEAR : GL_NEAREST;
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, theFilter);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, theFilter);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// Draws a quad of the bound blit source: positions in normalised device coordinates, texels in 0..1
void DrawQuad(const float inPosition[4], const float inTexel[4], bool inKeyed)
{
    const float theLeft = inPosition[0], theTop = inPosition[1], theRight = inPosition[2], theBottom = inPosition[3];
    const float theU0 = inTexel[0], theV0 = inTexel[1], theU1 = inTexel[2], theV1 = inTexel[3];
    const float theQuad[] = {theLeft, theTop, theU0, theV0, theRight, theTop, theU1, theV0,
                             theLeft, theBottom, theU0, theV1, theRight, theBottom, theU1, theV1};
    gl.UseProgram(gBlitProgram);
    gl.Uniform1i(gBlitKeyed, inKeyed);
    gl.BindVertexArray(gBlitVertexArray);
    gl.BindBuffer(GL_ARRAY_BUFFER, gBlitBuffer);
    gl.BufferData(GL_ARRAY_BUFFER, sizeof theQuad, theQuad, GL_STREAM_DRAW);
    gl.Disable(GL_BLEND);
    gl.Disable(GL_DEPTH_TEST);
    gl.Disable(GL_CULL_FACE);
    gl.Disable(GL_SCISSOR_TEST);
    gl.ColorMask(1, 1, 1, 1);
    gl.PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    gl.DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// Under a GLScope: the RGBA bytes of a rectangle of a texture, rows from the top
void ReadTexture(GLuint inTexture, const RECT& inRect, uint8_t* outRGBA)
{
    gl.BindFramebuffer(GL_READ_FRAMEBUFFER, gFramebuffer);
    gl.FramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inTexture, 0);
    gl.FramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    gl.PixelStorei(GL_PACK_ALIGNMENT, 1);
    gl.PixelStorei(GL_PACK_ROW_LENGTH, 0);
    gl.ReadPixels(inRect.left, inRect.top, inRect.right - inRect.left, inRect.bottom - inRect.top, GL_RGBA,
                  GL_UNSIGNED_BYTE, outRGBA);
    gl.BindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

// Under a GLScope: fills the scratch texture with RGBA rows
void LoadScratch(const uint8_t* inRGBA, int inWidth, int inHeight)
{
    gl.BindTexture(GL_TEXTURE_2D, gScratch);
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);
    gl.PixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, inWidth, inHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, inRGBA);
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
}

int Width(const RECT& inRect) { return inRect.right - inRect.left; }
int Height(const RECT& inRect) { return inRect.bottom - inRect.top; }

} // namespace

// ---- the context ----
GLScope::GLScope(SDL_Window* inWindow)
{
    std::lock_guard<std::recursive_mutex> theSdlLock(gSdlLock);
    SDL_Window* theWindow = inWindow ? inWindow : gScopeDepth ? gScopeWindow : gHiddenWindow;
    mOk = gContext != nullptr;
    if (!mOk)
        return;
    if (gScopeDepth == 0 || theWindow != gScopeWindow) {
        if (SDL_GL_MakeCurrent(theWindow, gContext) != 0) {
            Log("DirectDraw cannot use OpenGL on this thread: %s", SDL_GetError());
            mOk = false;
            return;
        }
    }
    // a nested scope that changes the window leaves the outer scope on the new window, which draws the same way
    gScopeWindow = theWindow;
    if (gScopeDepth++ == 0) {
        if (!gDeadTextures.empty())
            gl.DeleteTextures((GLsizei)gDeadTextures.size(), gDeadTextures.data());
        if (!gDeadRenderbuffers.empty())
            gl.DeleteRenderbuffers((GLsizei)gDeadRenderbuffers.size(), gDeadRenderbuffers.data());
        gDeadTextures.clear();
        gDeadRenderbuffers.clear();
    }
}

GLScope::~GLScope()
{
    std::lock_guard<std::recursive_mutex> theSdlLock(gSdlLock);
    if (mOk && --gScopeDepth == 0) {
        gl.Flush();
        SDL_GL_MakeCurrent(gScopeWindow, nullptr);
        gScopeWindow = nullptr;
    }
}

void DeleteTexture(GLuint inTexture)
{
    if (inTexture)
        gDeadTextures.push_back(inTexture);
}

void DeleteRenderbuffer(GLuint inRenderbuffer)
{
    if (inRenderbuffer)
        gDeadRenderbuffers.push_back(inRenderbuffer);
}

GLuint BindTarget(GLuint inColour, GLuint inDepth)
{
    gl.BindFramebuffer(GL_FRAMEBUFFER, gFramebuffer);
    gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inColour, 0);
    gl.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, inDepth);
    return gFramebuffer;
}

void DrawTexture(GLuint inTarget, int inTargetWidth, int inTargetHeight, const RECT& inTargetRect, GLuint inSource,
                 int inSourceWidth, int inSourceHeight, const float inSourceRect[4], bool inKeyed)
{
    BindTarget(inTarget, 0);
    gl.Viewport(0, 0, inTargetWidth, inTargetHeight);
    SetBlitTexture(inSource, false);
    // rows from the top are window rows from the bottom of the framebuffer, as the texture holds row 0 first
    const float thePosition[4] = {2.0f * inTargetRect.left / inTargetWidth - 1, 2.0f * inTargetRect.top / inTargetHeight - 1,
                                  2.0f * inTargetRect.right / inTargetWidth - 1,
                                  2.0f * inTargetRect.bottom / inTargetHeight - 1};
    const float theTexel[4] = {inSourceRect[0] / inSourceWidth, inSourceRect[1] / inSourceHeight,
                               inSourceRect[2] / inSourceWidth, inSourceRect[3] / inSourceHeight};
    DrawQuad(thePosition, theTexel, inKeyed);
    gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
}

// ---- pixel formats ----
bool Format::From(const DDPIXELFORMAT& inFormat, Format* outFormat)
{
    Format theFormat;
    if (inFormat.dwFlags & DDPF_ZBUFFER) {
        theFormat.mZ = true;
        theFormat.mZBits = (int)inFormat.dwZBufferBitDepth;
        if (theFormat.mZBits != 16 && theFormat.mZBits != 24 && theFormat.mZBits != 32)
            return false;
        theFormat.mBytes = theFormat.mZBits == 16 ? 2 : 4;
        *outFormat = theFormat;
        return true;
    }
    if (!(inFormat.dwFlags & DDPF_RGB) || (inFormat.dwRGBBitCount != 16 && inFormat.dwRGBBitCount != 32))
        return false;
    theFormat.mBytes = (int)inFormat.dwRGBBitCount / 8;
    const DWORD theMasks[4] = {inFormat.dwRBitMask, inFormat.dwGBitMask, inFormat.dwBBitMask,
                               inFormat.dwFlags & DDPF_ALPHAPIXELS ? inFormat.dwRGBAlphaBitMask : 0u};
    for (int i = 0; i < 4; ++i) {
        DWORD theMask = theMasks[i];
        if (theFormat.mBytes == 2)
            theMask &= 0xffff;
        if (!theMask) {
            if (i < 3)
                return false;
            continue;
        }
        int theShift = __builtin_ctz(theMask);
        DWORD theMax = theMask >> theShift;
        if (theMax & (theMax + 1))      // not one run of bits
            return false;
        theFormat.mMask[i] = theMask;
        theFormat.mShift[i] = theShift;
        theFormat.mMax[i] = theMax;
    }
    *outFormat = theFormat;
    return true;
}

DWORD Format::Read(const uint8_t* inPixel) const
{
    if (mBytes == 2)
        return (DWORD)inPixel[0] | (DWORD)inPixel[1] << 8;
    DWORD theValue;
    memcpy(&theValue, inPixel, 4);
    return theValue;
}

void Format::Write(uint8_t* outPixel, DWORD inValue) const
{
    if (mBytes == 2) {
        outPixel[0] = (uint8_t)inValue;
        outPixel[1] = (uint8_t)(inValue >> 8);
    } else
        memcpy(outPixel, &inValue, 4);
}

void Format::Decode(const uint8_t* inPixel, uint8_t* outRGBA) const
{
    DWORD theValue = Read(inPixel);
    for (int i = 0; i < 4; ++i) {
        if (!mMax[i]) {
            outRGBA[i] = 255;
            continue;
        }
        DWORD theChannel = (theValue & mMask[i]) >> mShift[i];
        outRGBA[i] = (uint8_t)((theChannel * 255 + mMax[i] / 2) / mMax[i]);
    }
}

void Format::Encode(const uint8_t* inRGBA, uint8_t* outPixel) const
{
    DWORD theValue = 0;
    for (int i = 0; i < 4; ++i)
        if (mMax[i])
            theValue |= ((inRGBA[i] * mMax[i] + 127) / 255) << mShift[i];
    Write(outPixel, theValue);
}

// ---- DirectDraw ----
namespace {

struct Mode {
    int mWidth, mHeight, mBits, mRefresh;
};

DDPIXELFORMAT RGBFormat(int inBits)
{
    DDPIXELFORMAT theFormat = {};
    theFormat.dwSize = sizeof theFormat;
    theFormat.dwFlags = DDPF_RGB;
    theFormat.dwRGBBitCount = (DWORD)inBits;
    if (inBits == 16) {
        theFormat.dwRBitMask = 0xf800;
        theFormat.dwGBitMask = 0x07e0;
        theFormat.dwBBitMask = 0x001f;
    } else {
        theFormat.dwRBitMask = 0xff0000;
        theFormat.dwGBitMask = 0x00ff00;
        theFormat.dwBBitMask = 0x0000ff;
    }
    return theFormat;
}

// The desktop: its size and refresh rate. The game runs in the desktop's 32-bit format when windowed.
Mode DesktopMode()
{
    SDL_Rect theBounds = {0, 0, 1024, 768};
    SDL_GetDisplayBounds(0, &theBounds);
    SDL_DisplayMode theMode = {};
    int theRefresh = SDL_GetDesktopDisplayMode(0, &theMode) == 0 && theMode.refresh_rate ? theMode.refresh_rate : 60;
    return {theBounds.w, theBounds.h, 32, theRefresh};
}

// The modes the game may choose: common sizes up to the desktop's, the desktop's own, and 640x480 and 800x600, which
// the game needs whatever the desktop, each at 16 and 32 bits
std::vector<Mode> DisplayModes()
{
    static const int kSizes[][2] = {{640, 480},   {800, 600},   {1024, 768},  {1152, 864},  {1280, 720},
                                    {1280, 800},  {1280, 960},  {1280, 1024}, {1366, 768},  {1440, 900},
                                    {1600, 900},  {1600, 1200}, {1680, 1050}, {1920, 1080}, {1920, 1200},
                                    {2560, 1440}, {2560, 1600}, {3840, 2160}};
    Mode theDesktop = DesktopMode();
    std::set<std::pair<int, int>> theSizes = {{theDesktop.mWidth, theDesktop.mHeight}};
    for (const auto& theSize : kSizes)
        if ((theSize[0] <= theDesktop.mWidth && theSize[1] <= theDesktop.mHeight) || theSize[0] <= 800)
            theSizes.insert({theSize[0], theSize[1]});
    std::vector<Mode> theModes;
    for (int theBits : {16, 32})
        for (const auto& theSize : theSizes)
            theModes.push_back({theSize.first, theSize.second, theBits, theDesktop.mRefresh});
    return theModes;
}

const DWORD kVideoMemory = 512u << 20;

} // namespace

class DirectDraw;

// Writes to the primary outside Flip (the cursor thread's restore, save and draw, a GDI DC, a windowed present) show
// once they pause for kPresentQuiet, and at the latest kPresentLongest after the first: the steps of one cursor move
// show as one image, as they do on Windows' front buffer, where each lasts microseconds. The presenter thread shows
// them; Flip shows at once and takes the pending present with it. The state lives for the whole process, because the
// presenter thread does.
struct PresentQueue {
    std::mutex mLock;                       // after gLock
    std::condition_variable mWake;
    DirectDraw* mOwner = nullptr;
    bool mPending = false, mStarted = false;
    std::chrono::steady_clock::time_point mFirst, mLast;
};
PresentQueue& gPresents = *new PresentQueue;
const std::chrono::milliseconds kPresentQuiet(4), kPresentLongest(16);
void PresentPending();

class DirectDraw final : public IDirectDraw7 {
public:
    ~DirectDraw()
    {
        std::lock_guard<std::mutex> theLock(gPresents.mLock);
        if (gPresents.mOwner == this) {
            gPresents.mOwner = nullptr;
            gPresents.mPending = false;
        }
    }

    // Under gLock: shows the primary soon, with the writes to it that follow at once
    void PresentSoon()
    {
        {
            std::lock_guard<std::mutex> theLock(gPresents.mLock);
            auto theNow = std::chrono::steady_clock::now();
            if (!gPresents.mPending || gPresents.mOwner != this)
                gPresents.mFirst = theNow;
            gPresents.mLast = theNow;
            gPresents.mOwner = this;
            gPresents.mPending = true;
            if (!gPresents.mStarted) {
                std::thread(PresentPending).detach();
                gPresents.mStarted = true;
            }
        }
        gPresents.mWake.notify_one();
    }

    // Under gLock: Flip shows the primary now, with any write that waited
    void PresentNow(bool inVSync)
    {
        {
            std::lock_guard<std::mutex> theLock(gPresents.mLock);
            if (gPresents.mOwner == this)
                gPresents.mPending = false;
        }
        Present(inVSync);
    }

    std::atomic<long> mReferences{1};
    int mSurfaces = 0;              // surfaces alive: the object stays until they go too
    HWND mWindow = NULL;
    SDL_Window* mNative = nullptr;
    bool mExclusive = false;
    bool mModeSet = false;
    bool mFullscreen = false;
    Mode mMode = {};
    Surface* mPrimary = nullptr;

    void SurfaceGone()
    {
        if (--mSurfaces == 0 && mReferences.load() == 0)
            delete this;
    }

    Mode CurrentMode() const { return mExclusive && mModeSet ? mMode : DesktopMode(); }

    // The primary's size: the mode in full screen, else the window's client area
    void PrimarySize(int* outWidth, int* outHeight) const
    {
        RECT theClient;
        if (!mExclusive && mWindow && GetClientRect(mWindow, &theClient) && theClient.right > 0 && theClient.bottom > 0) {
            *outWidth = theClient.right;
            *outHeight = theClient.bottom;
            return;
        }
        Mode theMode = CurrentMode();
        *outWidth = theMode.mWidth;
        *outHeight = theMode.mHeight;
    }

    // Where the primary's rectangles start on the screen: the client origin of a windowed game
    POINT PrimaryOrigin() const
    {
        POINT theOrigin = {0, 0};
        if (!mExclusive && mWindow)
            ClientToScreen(mWindow, &theOrigin);
        return theOrigin;
    }

    void SetFullscreen(bool inOn)
    {
        if (mFullscreen == inOn || !mNative || getenv("EE_PORT_WINDOWED"))
            return;
        // the window's own thread is the one that made its native window, and the one that changes it
        if (NativeWindow(mWindow, true) != mNative)
            return;
        {
            std::lock_guard<std::recursive_mutex> theSdlLock(gSdlLock);
            SDL_SetWindowFullscreen(mNative, inOn ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
        }
        mFullscreen = inOn;
    }

    // Shows the primary in the window: scaled to fit in full screen, at the top left when windowed
    void Present(bool inVSync)
    {
        if (!mPrimary || !mNative || !IsWindow(mWindow))
            return;
        std::lock_guard<std::recursive_mutex> theSdlLock(gSdlLock);
        GLScope theScope(mNative);
        if (!theScope.Ok() || !mPrimary->EnsureGpu())
            return;
        int theWidth = 0, theHeight = 0;
        SDL_GL_GetDrawableSize(mNative, &theWidth, &theHeight);
        if (theWidth <= 0 || theHeight <= 0)
            return;
        int theSwap = inVSync ? 1 : 0;
        if (theSwap != gSwapInterval) {
            SDL_GL_SetSwapInterval(theSwap);
            gSwapInterval = theSwap;
        }
        gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
        gl.Viewport(0, 0, theWidth, theHeight);
        gl.Disable(GL_SCISSOR_TEST);
        gl.ClearColor(0, 0, 0, 1);
        gl.Clear(GL_COLOR_BUFFER_BIT);
        float theScale = 1.0f;
        if (mExclusive)
            theScale = std::min(float(theWidth) / mPrimary->mWidth, float(theHeight) / mPrimary->mHeight);
        float theShownWidth = mPrimary->mWidth * theScale, theShownHeight = mPrimary->mHeight * theScale;
        float theLeft = mExclusive ? (theWidth - theShownWidth) / 2 : 0;
        float theTop = mExclusive ? (theHeight - theShownHeight) / 2 : 0;
        // the default framebuffer shows window row 0 at the bottom
        const float thePosition[4] = {2 * theLeft / theWidth - 1, 1 - 2 * theTop / theHeight,
                                      2 * (theLeft + theShownWidth) / theWidth - 1,
                                      1 - 2 * (theTop + theShownHeight) / theHeight};
        const float theTexel[4] = {0, 0, 1, 1};
        SetBlitTexture(mPrimary->mStore.mTexture, theScale != 1.0f);
        DrawQuad(thePosition, theTexel, false);
        DumpFrame();
        SDL_GL_SwapWindow(mNative);
    }

    // Under a GLScope, in Present: writes the primary to EE_PORT_FRAME_DUMP when the last frame there is 5 s old
    void DumpFrame()
    {
        static const char* const sFolder = getenv("EE_PORT_FRAME_DUMP");
        static int sCount = 0;
        static Uint32 sLast = 0;
        if (!sFolder || sCount >= 200 || (sCount > 0 && SDL_GetTicks() - sLast < 5000))
            return;
        sLast = SDL_GetTicks();
        int theWidth = mPrimary->mWidth, theHeight = mPrimary->mHeight;
        std::vector<uint8_t> theRGBA((size_t)theWidth * theHeight * 4);
        ReadTexture(mPrimary->mStore.mTexture, {0, 0, theWidth, theHeight}, theRGBA.data());
        char thePath[4096];
        snprintf(thePath, sizeof(thePath), "%s/frame-%03d.ppm", sFolder, sCount++);
        FILE* theFile = fopen(thePath, "wb");
        if (!theFile)
            return;
        fprintf(theFile, "P6\n%d %d\n255\n", theWidth, theHeight);
        for (size_t i = 0; i < theRGBA.size(); i += 4)
            fwrite(&theRGBA[i], 1, 3, theFile);
        fclose(theFile);
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        if (!outObject)
            return E_POINTER;
        *outObject = nullptr;
        if (IsEqualGUID(inID, IID_IDirect3D7))
            return NewDirect3D(this, outObject);
        if (!IsEqualGUID(inID, IID_IUnknown) && !IsEqualGUID(inID, IID_IDirectDraw7))
            return E_NOINTERFACE;
        AddRef();
        *outObject = this;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef() override { return ULONG(++mReferences); }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release() override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        long theCount = --mReferences;
        if (theCount == 0) {
            SetFullscreen(false);
            if (mSurfaces == 0)
                delete this;
        }
        return ULONG(theCount);
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Compact() override { return DD_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateClipper(DWORD, IDirectDrawClipper**, IUnknown*) override
    {
        EE_PORT_UNSUPPORTED("IDirectDraw7::CreateClipper");
        return DDERR_UNSUPPORTED;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreatePalette(DWORD, PALETTEENTRY*, IDirectDrawPalette**,
                                                                IUnknown*) override
    {
        EE_PORT_UNSUPPORTED("IDirectDraw7::CreatePalette");
        return DDERR_UNSUPPORTED;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateSurface(DDSURFACEDESC2* inDesc, IDirectDrawSurface7** outSurface,
                                                                IUnknown* inOuter) override
    {
        if (!inDesc || !outSurface || inDesc->dwSize != sizeof(DDSURFACEDESC2) || inOuter)
            return DDERR_INVALIDPARAMS;
        *outSurface = nullptr;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        DDSURFACEDESC2 theDesc = *inDesc;
        DWORD theCaps = theDesc.dwFlags & DDSD_CAPS ? theDesc.ddsCaps.dwCaps : 0;
        if (!(theCaps & DDSCAPS_SYSTEMMEMORY))
            theCaps |= DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
        Surface::Kind theKind = theCaps & DDSCAPS_PRIMARYSURFACE ? Surface::kPrimary
                              : theCaps & DDSCAPS_ZBUFFER        ? Surface::kZ
                              : theCaps & DDSCAPS_TEXTURE        ? Surface::kTexture
                                                                 : Surface::kOffscreen;
        int theWidth, theHeight;
        DDPIXELFORMAT thePixels;
        if (theKind == Surface::kPrimary) {
            if (mPrimary)
                return DDERR_PRIMARYSURFACEALREADYEXISTS;
            PrimarySize(&theWidth, &theHeight);
            thePixels = RGBFormat(CurrentMode().mBits);
        } else {
            if ((theDesc.dwFlags & (DDSD_WIDTH | DDSD_HEIGHT)) != (DDSD_WIDTH | DDSD_HEIGHT) || !theDesc.dwWidth ||
                !theDesc.dwHeight || theDesc.dwWidth > 16384 || theDesc.dwHeight > 16384)
                return DDERR_INVALIDPARAMS;
            theWidth = (int)theDesc.dwWidth;
            theHeight = (int)theDesc.dwHeight;
            if (theDesc.dwFlags & DDSD_PIXELFORMAT)
                thePixels = theDesc.ddpfPixelFormat;
            else if (theKind == Surface::kZ)
                return DDERR_INVALIDPIXELFORMAT;
            else
                thePixels = RGBFormat(CurrentMode().mBits);
        }
        Format theFormat;
        if (!Format::From(thePixels, &theFormat) || theFormat.mZ != (theKind == Surface::kZ))
            return DDERR_INVALIDPIXELFORMAT;
        theDesc.ddsCaps.dwCaps = theCaps;
        theDesc.ddpfPixelFormat = thePixels;
        theDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

        Surface* theSurface = new Surface(this, theKind, theDesc, theFormat, theWidth, theHeight);
        if (theKind == Surface::kPrimary) {
            mPrimary = theSurface;
            if (theCaps & DDSCAPS_FLIP) {
                int theCount = theDesc.dwFlags & DDSD_BACKBUFFERCOUNT ? (int)theDesc.dwBackBufferCount : 1;
                if (theCount < 1)
                    theCount = 1;
                theSurface->mDesc.ddsCaps.dwCaps |= DDSCAPS_FRONTBUFFER;
                DDSURFACEDESC2 theBack = theSurface->mDesc;
                theBack.ddsCaps.dwCaps = (theCaps & ~(DDSCAPS_PRIMARYSURFACE | DDSCAPS_FRONTBUFFER)) |
                                         DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE;
                for (int i = 0; i < theCount; ++i) {
                    Surface* theMember = new Surface(this, Surface::kBack, theBack, theFormat, theWidth, theHeight);
                    theMember->mRoot = theSurface;
                    theMember->mReferences = 0;
                    theSurface->mChain.push_back(theMember);
                    // only the first back buffer has DDSCAPS_BACKBUFFER
                    theBack.ddsCaps.dwCaps &= ~DDSCAPS_BACKBUFFER;
                }
            }
        } else if (theCaps & DDSCAPS_MIPMAP) {
            int theLevels = 1;
            if (theDesc.dwFlags & DDSD_MIPMAPCOUNT)
                theLevels = std::max(1, (int)theDesc.dwMipMapCount);
            else
                while ((theWidth >> theLevels) || (theHeight >> theLevels))
                    ++theLevels;
            theSurface->mDesc.dwFlags |= DDSD_MIPMAPCOUNT;
            theSurface->mDesc.dwMipMapCount = (DWORD)theLevels;
            DDSURFACEDESC2 theLevel = theSurface->mDesc;
            theLevel.ddsCaps.dwCaps2 |= DDSCAPS2_MIPMAPSUBLEVEL;
            for (int i = 1; i < theLevels; ++i) {
                theLevel.dwMipMapCount = (DWORD)(theLevels - i);
                Surface* theMember = new Surface(this, Surface::kTexture, theLevel, theFormat,
                                                 std::max(1, theWidth >> i), std::max(1, theHeight >> i));
                theMember->mRoot = theSurface;
                theMember->mReferences = 0;
                theMember->mHasKey = theSurface->mHasKey;
                theMember->mKey = theSurface->mKey;
                theSurface->mChain.push_back(theMember);
            }
        }
        *outSurface = theSurface;
        return DD_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DuplicateSurface(IDirectDrawSurface7*, IDirectDrawSurface7**) override
    {
        EE_PORT_UNSUPPORTED("IDirectDraw7::DuplicateSurface");
        return DDERR_UNSUPPORTED;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumDisplayModes(DWORD, DDSURFACEDESC2* inFilter, void* inContext,
                                                                   LPDDENUMMODESCALLBACK2 inCallback) override
    {
        if (!inCallback)
            return DDERR_INVALIDPARAMS;
        for (const Mode& theMode : DisplayModes()) {
            if (inFilter) {
                if ((inFilter->dwFlags & DDSD_WIDTH) && inFilter->dwWidth != (DWORD)theMode.mWidth)
                    continue;
                if ((inFilter->dwFlags & DDSD_HEIGHT) && inFilter->dwHeight != (DWORD)theMode.mHeight)
                    continue;
                if ((inFilter->dwFlags & DDSD_PIXELFORMAT) &&
                    inFilter->ddpfPixelFormat.dwRGBBitCount != (DWORD)theMode.mBits)
                    continue;
            }
            DDSURFACEDESC2 theDesc = {sizeof theDesc};
            theDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_REFRESHRATE;
            theDesc.dwWidth = (DWORD)theMode.mWidth;
            theDesc.dwHeight = (DWORD)theMode.mHeight;
            theDesc.lPitch = theMode.mWidth * theMode.mBits / 8;
            theDesc.dwRefreshRate = (DWORD)theMode.mRefresh;
            theDesc.ddpfPixelFormat = RGBFormat(theMode.mBits);
            if (inCallback(&theDesc, inContext) == DDENUMRET_CANCEL)
                break;
        }
        return DD_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumSurfaces(DWORD, DDSURFACEDESC2*, void*,
                                                               LPDDENUMSURFACESCALLBACK7) override
    {
        EE_PORT_UNSUPPORTED("IDirectDraw7::EnumSurfaces");
        return DDERR_UNSUPPORTED;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE FlipToGDISurface() override { return DD_OK; }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetCaps(DDCAPS* outDriver, DDCAPS* outEmulation) override
    {
        if (!outDriver && !outEmulation)
            return DDERR_INVALIDPARAMS;
        DDCAPS theCaps = {};
        theCaps.dwCaps = DDCAPS_3D | DDCAPS_BLT | DDCAPS_BLTCOLORFILL | DDCAPS_BLTSTRETCH | DDCAPS_COLORKEY |
                         DDCAPS_CANBLTSYSMEM | DDCAPS_BLTDEPTHFILL;
        theCaps.dwCaps2 = DDCAPS2_WIDESURFACES | DDCAPS2_CANRENDERWINDOWED | DDCAPS2_FLIPNOVSYNC;
        theCaps.dwCKeyCaps = DDCKEYCAPS_SRCBLT;
        theCaps.dwFXCaps = DDFXCAPS_BLTARITHSTRETCHY | DDFXCAPS_BLTSHRINKX | DDFXCAPS_BLTSHRINKY |
                           DDFXCAPS_BLTSTRETCHX | DDFXCAPS_BLTSTRETCHY;
        theCaps.dwZBufferBitDepths = DDBD_16 | DDBD_24 | DDBD_32;
        theCaps.dwVidMemTotal = kVideoMemory;
        theCaps.dwVidMemFree = kVideoMemory - kVideoMemory / 8;
        theCaps.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_BACKBUFFER | DDSCAPS_COMPLEX | DDSCAPS_FLIP |
                                 DDSCAPS_FRONTBUFFER | DDSCAPS_MIPMAP | DDSCAPS_OFFSCREENPLAIN |
                                 DDSCAPS_PRIMARYSURFACE | DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE |
                                 DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM | DDSCAPS_ZBUFFER;
        theCaps.ddsOldCaps.dwCaps = theCaps.ddsCaps.dwCaps;
        for (DDCAPS* theOut : {outDriver, outEmulation}) {
            if (!theOut)
                continue;
            DWORD theSize = theOut->dwSize;
            if (theSize < sizeof(DWORD) || theSize > sizeof(DDCAPS))
                return DDERR_INVALIDPARAMS;
            memcpy(theOut, &theCaps, theSize);
            theOut->dwSize = theSize;
            // the emulation layer has no 3D
            if (theOut == outEmulation)
                theOut->dwCaps &= ~DDCAPS_3D;
        }
        return DD_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDisplayMode(DDSURFACEDESC2* outDesc) override
    {
        if (!outDesc || outDesc->dwSize != sizeof(DDSURFACEDESC2))
            return DDERR_INVALIDPARAMS;
        Mode theMode = CurrentMode();
        memset(outDesc, 0, sizeof *outDesc);
        outDesc->dwSize = sizeof *outDesc;
        outDesc->dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_REFRESHRATE;
        outDesc->dwWidth = (DWORD)theMode.mWidth;
        outDesc->dwHeight = (DWORD)theMode.mHeight;
        outDesc->lPitch = theMode.mWidth * theMode.mBits / 8;
        outDesc->dwRefreshRate = (DWORD)theMode.mRefresh;
        outDesc->ddpfPixelFormat = RGBFormat(theMode.mBits);
        return DD_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetFourCCCodes(LPDWORD outCount, LPDWORD) override
    {
        if (!outCount)
            return DDERR_INVALIDPARAMS;
        *outCount = 0;
        return DD_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetGDISurface(IDirectDrawSurface7** outSurface) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        if (!outSurface)
            return DDERR_INVALIDPARAMS;
        *outSurface = mPrimary;
        if (!mPrimary)
            return DDERR_NOTFOUND;
        mPrimary->AddRef();
        return DD_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetMonitorFrequency(LPDWORD outFrequency) override
    {
        if (!outFrequency)
            return DDERR_INVALIDPARAMS;
        *outFrequency = (DWORD)CurrentMode().mRefresh;
        return DD_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetScanLine(LPDWORD) override { return DDERR_UNSUPPORTED; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetVerticalBlankStatus(BOOL* outInBlank) override
    {
        if (!outInBlank)
            return DDERR_INVALIDPARAMS;
        *outInBlank = FALSE;
        return DD_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Initialize(GUID*) override { return DDERR_ALREADYINITIALIZED; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE RestoreDisplayMode() override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        mModeSet = false;
        SetFullscreen(false);
        return DD_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetCooperativeLevel(HWND inWindow, DWORD inFlags) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        if ((inFlags & DDSCL_SETFOCUSWINDOW) && !(inFlags & (DDSCL_EXCLUSIVE | DDSCL_NORMAL)))
            return DD_OK;
        if (((inFlags & DDSCL_EXCLUSIVE) != 0) != ((inFlags & DDSCL_FULLSCREEN) != 0) ||
            ((inFlags & DDSCL_EXCLUSIVE) && (inFlags & DDSCL_NORMAL)) ||
            !(inFlags & (DDSCL_EXCLUSIVE | DDSCL_NORMAL)))
            return DDERR_INVALIDPARAMS;
        if ((inFlags & DDSCL_EXCLUSIVE) && !inWindow)
            return DDERR_INVALIDPARAMS;
        bool theExclusive = (inFlags & DDSCL_EXCLUSIVE) != 0;
        if (!theExclusive && mExclusive) {
            mModeSet = false;
            SetFullscreen(false);
        }
        if (inWindow != mWindow) {
            SetFullscreen(false);
            mWindow = inWindow;
            mNative = nullptr;
        }
        mExclusive = theExclusive;
        if (mWindow)
            if (SDL_Window* theNative = NativeWindow(mWindow, true))
                mNative = theNative;
        if (mExclusive && mModeSet)
            SetFullscreen(true);
        return DD_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetDisplayMode(DWORD inWidth, DWORD inHeight, DWORD inBits,
                                                                 DWORD inRefresh, DWORD) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        if (!mExclusive)
            return DDERR_NOEXCLUSIVEMODE;
        for (const Mode& theMode : DisplayModes())
            if ((DWORD)theMode.mWidth == inWidth && (DWORD)theMode.mHeight == inHeight && (DWORD)theMode.mBits == inBits) {
                mMode = theMode;
                if (inRefresh)
                    mMode.mRefresh = (int)inRefresh;
                mModeSet = true;
                SetFullscreen(true);
                return DD_OK;
            }
        return DDERR_INVALIDMODE;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE WaitForVerticalBlank(DWORD, HANDLE) override { return DD_OK; }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetAvailableVidMem(DDSCAPS2*, DWORD* outTotal, DWORD* outFree) override
    {
        if (!outTotal && !outFree)
            return DDERR_INVALIDPARAMS;
        if (outTotal)
            *outTotal = kVideoMemory;
        if (outFree)
            *outFree = kVideoMemory - kVideoMemory / 8;
        return DD_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetSurfaceFromDC(HDC inDC, IDirectDrawSurface7** outSurface) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE RestoreAllSurfaces() override { return DD_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE TestCooperativeLevel() override { return DD_OK; }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDeviceIdentifier(DDDEVICEIDENTIFIER2* outIdentifier, DWORD) override
    {
        if (!outIdentifier)
            return DDERR_INVALIDPARAMS;
        memset(outIdentifier, 0, sizeof *outIdentifier);
        snprintf(outIdentifier->szDriver, sizeof outIdentifier->szDriver, "ee-port OpenGL");
        snprintf(outIdentifier->szDescription, sizeof outIdentifier->szDescription, "OpenGL 3.3 on %s",
                 gRenderer.c_str());
        return DD_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE StartModeTest(LPSIZE, DWORD, DWORD) override { return DDERR_UNSUPPORTED; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EvaluateMode(DWORD, DWORD*) override { return DDERR_UNSUPPORTED; }
};

namespace {
std::set<Surface*> gSurfaces;
}

Surface* SurfaceOf(IDirectDrawSurface7* inSurface)
{
    Surface* theSurface = static_cast<Surface*>(inSurface);
    return theSurface && gSurfaces.count(theSurface) ? theSurface : nullptr;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DirectDraw::GetSurfaceFromDC(HDC inDC, IDirectDrawSurface7** outSurface)
{
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    if (!outSurface)
        return DDERR_INVALIDPARAMS;
    *outSurface = nullptr;
    for (Surface* theSurface : gSurfaces)
        if (theSurface->mDC == inDC && inDC) {
            theSurface->AddRef();
            *outSurface = theSurface;
            return DD_OK;
        }
    return DDERR_NOTFOUND;
}

// ---- surfaces ----
Surface::Surface(DirectDraw* inOwner, Kind inKind, const DDSURFACEDESC2& inDesc, const Format& inFormat, int inWidth,
                 int inHeight)
    : mKind(inKind), mOwner(inOwner), mDesc(inDesc), mFormat(inFormat), mWidth(inWidth), mHeight(inHeight)
{
    mPitch = (mWidth * mFormat.mBytes + 3) & ~3;
    mStore.mBits.assign((size_t)mPitch * mHeight, 0);
    mDesc.dwSize = sizeof mDesc;
    mDesc.dwFlags |= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT;
    mDesc.dwWidth = (DWORD)mWidth;
    mDesc.dwHeight = (DWORD)mHeight;
    mDesc.lPitch = mPitch;
    mDesc.lpSurface = nullptr;
    if (mDesc.dwFlags & DDSD_CKSRCBLT) {
        mHasKey = true;
        mKey = mDesc.ddckCKSrcBlt;
    }
    ++mOwner->mSurfaces;
    gSurfaces.insert(this);
}

Surface::~Surface()
{
    gSurfaces.erase(this);
    if (mDC)
        DeleteDC(mDC);
    if (mKind == kZ)
        DeleteRenderbuffer(mStore.mTexture);
    else
        DeleteTexture(mStore.mTexture);
    if (mZ)
        mZ->Release();
    if (mOwner->mPrimary == this)
        mOwner->mPrimary = nullptr;
    mOwner->SurfaceGone();
}

GLuint Surface::ColourTexture()
{
    if (!mStore.mTexture) {
        gl.GenTextures(1, &mStore.mTexture);
        gl.BindTexture(GL_TEXTURE_2D, mStore.mTexture);
        gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    }
    return mStore.mTexture;
}

GLuint Surface::DepthBuffer()
{
    if (!mStore.mTexture) {
        gl.GenRenderbuffers(1, &mStore.mTexture);
        gl.BindRenderbuffer(GL_RENDERBUFFER, mStore.mTexture);
        gl.RenderbufferStorage(GL_RENDERBUFFER, mFormat.mZBits == 16 ? GL_DEPTH_COMPONENT16 : GL_DEPTH_COMPONENT24,
                               mWidth, mHeight);
        gl.BindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    return mStore.mTexture;
}

namespace {

// Whether a pixel of a format is inside a colour-key range. Only the bits the format holds count: a 32-bit surface
// without an alpha channel still carries a byte there, which the game fills with 0xff and its key does not name, so a
// keyed comparison of the whole value would never match the game's own pixels.
bool InKey(const Format& inFormat, DWORD inValue, const DDCOLORKEY& inKey)
{
    DWORD theBits = inFormat.mMask[0] | inFormat.mMask[1] | inFormat.mMask[2] | inFormat.mMask[3];
    return (inValue & theBits) >= (inKey.dwColorSpaceLowValue & theBits) &&
           (inValue & theBits) <= (inKey.dwColorSpaceHighValue & theBits);
}

// The RGBA rows of a surface's memory. With inKeyed, texels inside the source colour key get alpha 0, and all
// others alpha 255 when the format has no alpha.
void DecodeSurface(Surface& inSurface, bool inKeyed, std::vector<uint8_t>& outRGBA)
{
    outRGBA.resize((size_t)inSurface.mWidth * inSurface.mHeight * 4);
    uint8_t* theOut = outRGBA.data();
    for (int y = 0; y < inSurface.mHeight; ++y)
        for (int x = 0; x < inSurface.mWidth; ++x, theOut += 4) {
            const uint8_t* thePixel = inSurface.Pixel(x, y);
            inSurface.mFormat.Decode(thePixel, theOut);
            if (inKeyed && InKey(inSurface.mFormat, inSurface.mFormat.Read(thePixel), inSurface.mKey))
                theOut[3] = 0;
        }
}

} // namespace

bool Surface::EnsureGpu(bool inKeyed)
{
    if (mKind == kZ)
        return false;
    Surface* theRoot = TextureRoot();
    if (theRoot != this)
        return theRoot->EnsureGpu(inKeyed);
    inKeyed = inKeyed && mHasKey;
    if (mStore.mGpuValid && (mStore.mKeyed == (int)inKeyed || (!inKeyed && mStore.mKeyed < 0)))
        return true;
    // the texture of a render target can hold the newer image; a keyed upload starts from the memory
    if (mStore.mGpuValid && !mStore.mCpuValid)
        EnsureCpu();
    std::vector<uint8_t> theRGBA;
    ColourTexture();
    gl.BindTexture(GL_TEXTURE_2D, mStore.mTexture);
    gl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);
    gl.PixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    int theLevel = 0;
    for (Surface* theSurface = this;; theSurface = mChain[theLevel - 1]) {
        DecodeSurface(*theSurface, inKeyed, theRGBA);
        gl.TexImage2D(GL_TEXTURE_2D, theLevel, GL_RGBA8, theSurface->mWidth, theSurface->mHeight, 0, GL_RGBA,
                      GL_UNSIGNED_BYTE, theRGBA.data());
        ++theLevel;
        if (!(mDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP) || theLevel > (int)mChain.size())
            break;
    }
    gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, theLevel - 1);
    mStore.mGpuValid = true;
    mStore.mKeyed = inKeyed;
    return true;
}

void Surface::EnsureCpu()
{
    if (mKind == kZ)
        return;
    SubmitDraws(this);
    if (mStore.mCpuValid || mDC)
        return;
    GLScope theScope;
    if (!theScope.Ok())
        return;
    std::vector<uint8_t> theRGBA((size_t)mWidth * mHeight * 4);
    ReadTexture(mStore.mTexture, {0, 0, mWidth, mHeight}, theRGBA.data());
    const uint8_t* theIn = theRGBA.data();
    for (int y = 0; y < mHeight; ++y)
        for (int x = 0; x < mWidth; ++x, theIn += 4)
            mFormat.Encode(theIn, Pixel(x, y));
    mStore.mCpuValid = true;
}

void Surface::CpuWritten()
{
    mStore.mCpuValid = true;
    mStore.mGpuValid = false;
    if (mRoot != this && (mDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP))
        mRoot->mStore.mGpuValid = false;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::QueryInterface(REFIID inID, void** outObject)
{
    if (!outObject)
        return E_POINTER;
    *outObject = nullptr;
    if (!IsEqualGUID(inID, IID_IUnknown) && !IsEqualGUID(inID, IID_IDirectDrawSurface7))
        return E_NOINTERFACE;
    AddRef();
    *outObject = static_cast<IDirectDrawSurface7*>(this);
    return S_OK;
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Surface::AddRef()
{
    return ULONG(++mReferences);
}

COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Surface::Release()
{
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    long theCount = --mReferences;
    if (theCount > 0)
        return ULONG(theCount);
    if (mRoot != this) {
        // a chain member goes with its root
        mReferences = 0;
        return 0;
    }
    if (mOrphan) {
        // a chain member that outlived its root
        delete this;
        return 0;
    }
    SubmitDraws(this);
    for (Surface* theMember : mChain) {
        // a flip chain goes whole, as the game never releases its back buffers, unless a device still renders into
        // one; a mip level the game still holds goes with its last reference
        if (mKind == kPrimary)
            theMember->mReferences = theMember->mDeviceUses;
        if (theMember->mReferences.load() <= 0)
            delete theMember;
        else {
            theMember->mRoot = theMember;
            theMember->mOrphan = true;
        }
    }
    mChain.clear();
    delete this;
    return 0;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::AddAttachedSurface(IDirectDrawSurface7* inSurface)
{
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    Surface* theZ = SurfaceOf(inSurface);
    if (!theZ || theZ->mKind != kZ || theZ == this)
        return DDERR_CANNOTATTACHSURFACE;
    if (mZ)
        return DDERR_SURFACEALREADYATTACHED;
    if (theZ->mWidth != mWidth || theZ->mHeight != mHeight)
        return DDERR_CANNOTATTACHSURFACE;
    theZ->AddRef();
    mZ = theZ;
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::DeleteAttachedSurface(DWORD, IDirectDrawSurface7* inSurface)
{
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    Surface* theZ = SurfaceOf(inSurface);
    if (!mZ || (theZ && theZ != mZ))
        return DDERR_SURFACENOTATTACHED;
    SubmitDraws(this);
    mZ->Release();
    mZ = nullptr;
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetAttachedSurface(DDSCAPS2* inCaps, IDirectDrawSurface7** outSurface)
{
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    if (!inCaps || !outSurface)
        return DDERR_INVALIDPARAMS;
    *outSurface = nullptr;
    Surface* theFound = nullptr;
    if (inCaps->dwCaps & DDSCAPS_ZBUFFER)
        theFound = mZ;
    else if (inCaps->dwCaps & (DDSCAPS_FLIP | DDSCAPS_BACKBUFFER | DDSCAPS_MIPMAP)) {
        std::vector<Surface*>& theChain = mRoot->mChain;
        if (mRoot == this)
            theFound = theChain.empty() ? nullptr : theChain.front();
        else {
            auto it = std::find(theChain.begin(), theChain.end(), this);
            if (it != theChain.end() && std::next(it) != theChain.end())
                theFound = *std::next(it);
            else if (mKind == kBack)
                theFound = mRoot;       // a flip chain is a ring
        }
        if (theFound && (inCaps->dwCaps & DDSCAPS_MIPMAP) && !(theFound->mDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP))
            theFound = nullptr;
    }
    if (!theFound)
        return DDERR_NOTFOUND;
    theFound->AddRef();
    *outSurface = theFound;
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::AddOverlayDirtyRect(LPRECT) { return DDERR_UNSUPPORTED; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::BltBatch(DDBLTBATCH*, DWORD, DWORD) { return DDERR_UNSUPPORTED; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::EnumAttachedSurfaces(void*, LPDDENUMSURFACESCALLBACK7)
{
    EE_PORT_UNSUPPORTED("IDirectDrawSurface7::EnumAttachedSurfaces");
    return DDERR_UNSUPPORTED;
}
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::EnumOverlayZOrders(DWORD, void*, LPDDENUMSURFACESCALLBACK7) { return DDERR_UNSUPPORTED; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetBltStatus(DWORD) { return DD_OK; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetFlipStatus(DWORD) { return DD_OK; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetClipper(IDirectDrawClipper**) { return DDERR_NOCLIPPERATTACHED; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetOverlayPosition(LPLONG, LPLONG) { return DDERR_NOTAOVERLAYSURFACE; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetPalette(IDirectDrawPalette**) { return DDERR_NOPALETTEATTACHED; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::Initialize(IDirectDraw*, DDSURFACEDESC2*) { return DDERR_ALREADYINITIALIZED; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::IsLost() { return DD_OK; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::Restore() { return DD_OK; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::SetClipper(IDirectDrawClipper*)
{
    EE_PORT_UNSUPPORTED("IDirectDrawSurface7::SetClipper");
    return DDERR_UNSUPPORTED;
}
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::SetOverlayPosition(LONG, LONG) { return DDERR_NOTAOVERLAYSURFACE; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::SetPalette(IDirectDrawPalette*) { return DDERR_UNSUPPORTED; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::UpdateOverlay(RECT*, IDirectDrawSurface7*, RECT*, DWORD, DDOVERLAYFX*)
{
    return DDERR_NOTAOVERLAYSURFACE;
}
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::UpdateOverlayDisplay(DWORD) { return DDERR_NOTAOVERLAYSURFACE; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::UpdateOverlayZOrder(DWORD, IDirectDrawSurface7*) { return DDERR_NOTAOVERLAYSURFACE; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::PageLock(DWORD) { return DD_OK; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::PageUnlock(DWORD) { return DD_OK; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::SetSurfaceDesc(DDSURFACEDESC2*, DWORD) { return DDERR_UNSUPPORTED; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::SetPrivateData(REFGUID, void*, DWORD, DWORD) { return DDERR_UNSUPPORTED; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetPrivateData(REFGUID, void*, DWORD*) { return DDERR_NOTFOUND; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::FreePrivateData(REFGUID) { return DDERR_NOTFOUND; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetUniquenessValue(LPDWORD outValue)
{
    if (!outValue)
        return DDERR_INVALIDPARAMS;
    *outValue = 0;
    return DD_OK;
}
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::ChangeUniquenessValue() { return DD_OK; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::SetPriority(DWORD) { return DD_OK; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetPriority(LPDWORD outPriority)
{
    if (!outPriority)
        return DDERR_INVALIDPARAMS;
    *outPriority = 0;
    return DD_OK;
}
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::SetLOD(DWORD) { return DD_OK; }
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetLOD(LPDWORD outLOD)
{
    if (!outLOD)
        return DDERR_INVALIDPARAMS;
    *outLOD = 0;
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetDDInterface(void** outDirectDraw)
{
    if (!outDirectDraw)
        return DDERR_INVALIDPARAMS;
    mOwner->AddRef();
    *outDirectDraw = static_cast<IDirectDraw7*>(mOwner);
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetCaps(DDSCAPS2* outCaps)
{
    if (!outCaps)
        return DDERR_INVALIDPARAMS;
    *outCaps = mDesc.ddsCaps;
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetPixelFormat(DDPIXELFORMAT* outFormat)
{
    if (!outFormat)
        return DDERR_INVALIDPARAMS;
    *outFormat = mDesc.ddpfPixelFormat;
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetSurfaceDesc(DDSURFACEDESC2* outDesc)
{
    if (!outDesc || outDesc->dwSize != sizeof(DDSURFACEDESC2))
        return DDERR_INVALIDPARAMS;
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    *outDesc = mDesc;
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetColorKey(DWORD inFlags, DDCOLORKEY* outKey)
{
    if (!outKey || inFlags != DDCKEY_SRCBLT)
        return DDERR_INVALIDPARAMS;
    if (!mHasKey)
        return DDERR_NOCOLORKEY;
    *outKey = mKey;
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::SetColorKey(DWORD inFlags, DDCOLORKEY* inKey)
{
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    if ((inFlags & ~DDCKEY_COLORSPACE) != DDCKEY_SRCBLT) {
        EE_PORT_UNSUPPORTED("a colour key other than DDCKEY_SRCBLT");
        return DDERR_UNSUPPORTED;
    }
    SubmitDraws(this);
    mHasKey = inKey != nullptr;
    if (inKey) {
        mKey = *inKey;
        if (!(inFlags & DDCKEY_COLORSPACE))
            mKey.dwColorSpaceHighValue = mKey.dwColorSpaceLowValue;
        mDesc.dwFlags |= DDSD_CKSRCBLT;
        mDesc.ddckCKSrcBlt = mKey;
    } else
        mDesc.dwFlags &= ~DDSD_CKSRCBLT;
    if (mRoot == this && (mDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP))
        for (Surface* theLevel : mChain) {
            theLevel->mHasKey = mHasKey;
            theLevel->mKey = mKey;
        }
    // a keyed upload of the texture no longer matches
    TextureRoot()->mStore.mKeyed = -1;
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::Lock(RECT* inRect, DDSURFACEDESC2* outDesc, DWORD inFlags, HANDLE)
{
    if (!outDesc || outDesc->dwSize != sizeof(DDSURFACEDESC2))
        return DDERR_INVALIDPARAMS;
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    if (inRect && (inRect->left < 0 || inRect->top < 0 || inRect->right > mWidth || inRect->bottom > mHeight ||
                   inRect->left >= inRect->right || inRect->top >= inRect->bottom))
        return DDERR_INVALIDRECT;
    EnsureCpu();
    *outDesc = mDesc;
    outDesc->dwFlags |= DDSD_LPSURFACE;
    outDesc->lpSurface = inRect ? Pixel(inRect->left, inRect->top) : mStore.mBits.data();
    mLockedReadOnly = mLocks == 0 ? (inFlags & DDLOCK_READONLY) != 0 : mLockedReadOnly && (inFlags & DDLOCK_READONLY);
    ++mLocks;
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::Unlock(LPRECT)
{
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    if (mLocks == 0)
        return DDERR_NOTLOCKED;
    if (--mLocks == 0 && !mLockedReadOnly)
        CpuWritten();
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::GetDC(HDC* outDC)
{
    if (!outDC)
        return DDERR_INVALIDPARAMS;
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    if (mFormat.mZ)
        return DDERR_CANTCREATEDC;
    if (mDC)
        return DDERR_DCALREADYCREATED;
    EnsureCpu();
    mDCBase = mStore.mBits;
    // before GDI draws text, the memory under it takes what Direct3D drew since, so the text blends with that
    auto theBeforeText = [](void* inSurface, const RECT& inArea) {
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        static_cast<Surface*>(inSurface)->BeforeGdi(inArea);
    };
    mDC = CreateBufferDC(mStore.mBits.data(), mWidth, mHeight, mPitch, mFormat.mBytes * 8, mFormat.mMask[0],
                         mFormat.mMask[1], mFormat.mMask[2], theBeforeText, this);
    if (!mDC)
        return DDERR_CANTCREATEDC;
    *outDC = mDC;
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::ReleaseDC(HDC inDC)
{
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    if (!mDC || inDC != mDC)
        return DDERR_NODC;
    TakeGdi();
    DeleteDC(mDC);
    mDC = NULL;
    mDCBase.clear();
    mDCBase.shrink_to_fit();
    if (mKind == kPrimary)
        mOwner->PresentSoon();
    return DD_OK;
}

void Surface::BeforeGdi(const RECT& inArea)
{
    if (!mDC)
        return;
    TakeGdi();
    SubmitDraws(this);
    if (mStore.mCpuValid || !mStore.mGpuValid)
        return;
    GLScope theScope;
    if (!theScope.Ok())
        return;
    std::vector<uint8_t> theRGBA((size_t)Width(inArea) * Height(inArea) * 4);
    ReadTexture(mStore.mTexture, inArea, theRGBA.data());
    const uint8_t* theIn = theRGBA.data();
    for (int y = inArea.top; y < inArea.bottom; ++y)
        for (int x = inArea.left; x < inArea.right; ++x, theIn += 4) {
            uint8_t* thePixel = Pixel(x, y);
            mFormat.Encode(theIn, thePixel);
            memcpy(mDCBase.data() + (thePixel - mStore.mBits.data()), thePixel, mFormat.mBytes);
        }
}

void Surface::TakeGdi()
{
    RECT theWritten;
    if (!mDC || !TakeBufferDrawn(mDC, &theWritten))
        return;
    SubmitDraws(this);
    // what GDI drew: the pixels that differ from the image as GDI last left it
    RECT theChanged = {mWidth, mHeight, 0, 0};
    const size_t theRowBytes = (size_t)Width(theWritten) * mFormat.mBytes;
    for (int y = theWritten.top; y < theWritten.bottom; ++y) {
        const uint8_t* theNow = Pixel(theWritten.left, y);
        const uint8_t* theBefore = mDCBase.data() + (theNow - mStore.mBits.data());
        if (!memcmp(theNow, theBefore, theRowBytes))
            continue;
        for (int x = theWritten.left; x < theWritten.right; ++x) {
            size_t theOffset = (size_t)(x - theWritten.left) * mFormat.mBytes;
            if (memcmp(theNow + theOffset, theBefore + theOffset, mFormat.mBytes)) {
                theChanged.left = std::min<LONG>(theChanged.left, x);
                theChanged.right = std::max<LONG>(theChanged.right, x + 1);
            }
        }
        theChanged.top = std::min<LONG>(theChanged.top, y);
        theChanged.bottom = y + 1;
    }
    if (theChanged.left < theChanged.right) {
        if (mStore.mGpuValid && !mStore.mCpuValid) {
            // Direct3D drew into the texture while GDI drew into the memory: the texture takes GDI's pixels over
            // whatever it holds there
            int theWidth = Width(theChanged), theHeight = Height(theChanged);
            std::vector<uint8_t> theRGBA((size_t)theWidth * theHeight * 4);
            uint8_t* theOut = theRGBA.data();
            for (int y = theChanged.top; y < theChanged.bottom; ++y)
                for (int x = theChanged.left; x < theChanged.right; ++x, theOut += 4) {
                    size_t theOffset = (size_t)y * mPitch + (size_t)x * mFormat.mBytes;
                    bool theDrawn = memcmp(mStore.mBits.data() + theOffset, mDCBase.data() + theOffset, mFormat.mBytes);
                    mFormat.Decode(mStore.mBits.data() + theOffset, theOut);
                    theOut[3] = theDrawn ? 255 : 0;
                }
            GLScope theScope;
            if (theScope.Ok()) {
                LoadScratch(theRGBA.data(), theWidth, theHeight);
                const float theSource[4] = {0, 0, float(theWidth), float(theHeight)};
                DrawTexture(ColourTexture(), mWidth, mHeight, theChanged, gScratch, theWidth, theHeight, theSource, true);
            }
        } else
            CpuWritten();
        for (int y = theChanged.top; y < theChanged.bottom; ++y) {
            size_t theOffset = (size_t)y * mPitch + (size_t)theChanged.left * mFormat.mBytes;
            memcpy(mDCBase.data() + theOffset, mStore.mBits.data() + theOffset,
                   (size_t)Width(theChanged) * mFormat.mBytes);
        }
    }
}

namespace {

// Clips a destination rectangle to a surface and moves the source rectangle with it, in the source's scale
bool Clip(RECT& ioTarget, float ioSource[4], int inWidth, int inHeight)
{
    float theScaleX = (ioSource[2] - ioSource[0]) / Width(ioTarget);
    float theScaleY = (ioSource[3] - ioSource[1]) / Height(ioTarget);
    RECT theClipped = {std::max<LONG>(ioTarget.left, 0), std::max<LONG>(ioTarget.top, 0),
                       std::min<LONG>(ioTarget.right, inWidth), std::min<LONG>(ioTarget.bottom, inHeight)};
    if (theClipped.left >= theClipped.right || theClipped.top >= theClipped.bottom)
        return false;
    ioSource[0] += (theClipped.left - ioTarget.left) * theScaleX;
    ioSource[1] += (theClipped.top - ioTarget.top) * theScaleY;
    ioSource[2] -= (ioTarget.right - theClipped.right) * theScaleX;
    ioSource[3] -= (ioTarget.bottom - theClipped.bottom) * theScaleY;
    ioTarget = theClipped;
    return true;
}

} // namespace

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::Blt(RECT* inDestination, IDirectDrawSurface7* inSource, RECT* inSourceRect,
                                       DWORD inFlags, DDBLTFX* inFX)
{
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    Surface* theSource = SurfaceOf(inSource);
    if (inSource && !theSource)
        return DDERR_INVALIDPARAMS;
    if (mKind == kPrimary && !mOwner->mExclusive) {
        // a windowed primary follows the window's client area
        int theWidth, theHeight;
        mOwner->PrimarySize(&theWidth, &theHeight);
        if (theWidth != mWidth || theHeight != mHeight) {
            SubmitDraws(this);
            DeleteTexture(mStore.mTexture);
            mStore = Store();
            mWidth = theWidth;
            mHeight = theHeight;
            mPitch = (mWidth * mFormat.mBytes + 3) & ~3;
            mStore.mBits.assign((size_t)mPitch * mHeight, 0);
            mDesc.dwWidth = (DWORD)mWidth;
            mDesc.dwHeight = (DWORD)mHeight;
            mDesc.lPitch = mPitch;
        }
    }
    RECT theTarget = inDestination ? *inDestination : RECT{0, 0, mWidth, mHeight};
    if (mKind == kPrimary) {
        POINT theOrigin = mOwner->PrimaryOrigin();
        OffsetRect(&theTarget, -theOrigin.x, -theOrigin.y);
    }
    if (theTarget.left >= theTarget.right || theTarget.top >= theTarget.bottom)
        return DDERR_INVALIDRECT;
    // without a clipper DirectDraw refuses a rectangle that leaves the surface. A windowed primary stands for the
    // whole screen, of which the window shows a part, so its rectangles are clipped to the window instead
    bool theWindowed = mKind == kPrimary && !mOwner->mExclusive;
    if (!theWindowed &&
        (theTarget.left < 0 || theTarget.top < 0 || theTarget.right > mWidth || theTarget.bottom > mHeight))
        return DDERR_INVALIDRECT;

    if (inFlags & (DDBLT_COLORFILL | DDBLT_DEPTHFILL)) {
        if (!inFX)
            return DDERR_INVALIDPARAMS;
        float theNone[4] = {0, 0, 1, 1};
        if (!Clip(theTarget, theNone, mWidth, mHeight))
            return DD_OK;
        SubmitDraws(this);
        if (mKind == kZ) {
            EE_PORT_UNSUPPORTED("DDBLT_DEPTHFILL");
            return DD_OK;
        }
        DWORD theFill = inFX->dwFillColor;
        if (mStore.mGpuValid && !mStore.mCpuValid && !mDC) {
            uint8_t thePixel[4], theRGBA[4];
            mFormat.Write(thePixel, theFill);
            mFormat.Decode(thePixel, theRGBA);
            GLScope theScope;
            if (theScope.Ok()) {
                BindTarget(ColourTexture(), 0);
                gl.Viewport(0, 0, mWidth, mHeight);
                gl.Enable(GL_SCISSOR_TEST);
                gl.Scissor(theTarget.left, theTarget.top, Width(theTarget), Height(theTarget));
                gl.ColorMask(1, 1, 1, 1);
                gl.ClearColor(theRGBA[0] / 255.0f, theRGBA[1] / 255.0f, theRGBA[2] / 255.0f, theRGBA[3] / 255.0f);
                gl.Clear(GL_COLOR_BUFFER_BIT);
                gl.Disable(GL_SCISSOR_TEST);
                gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        } else {
            EnsureCpu();
            for (int y = theTarget.top; y < theTarget.bottom; ++y)
                for (int x = theTarget.left; x < theTarget.right; ++x)
                    mFormat.Write(Pixel(x, y), theFill);
            if (!mDC)
                CpuWritten();
        }
        if (mKind == kPrimary)
            mOwner->PresentSoon();
        return DD_OK;
    }

    if (!theSource)
        return DDERR_INVALIDPARAMS;
    if (theSource->mFormat.mZ || mFormat.mZ)
        return DDERR_UNSUPPORTED;
    RECT theSourceRect = inSourceRect ? *inSourceRect : RECT{0, 0, theSource->mWidth, theSource->mHeight};
    if (inSourceRect && theSource->mKind == kPrimary && !theSource->mOwner->mExclusive) {
        // a windowed primary's rectangles are in screen coordinates, as the destination's above
        POINT theOrigin = theSource->mOwner->PrimaryOrigin();
        OffsetRect(&theSourceRect, -theOrigin.x, -theOrigin.y);
    }
    if (theSourceRect.left < 0 || theSourceRect.top < 0 || theSourceRect.right > theSource->mWidth ||
        theSourceRect.bottom > theSource->mHeight || theSourceRect.left >= theSourceRect.right ||
        theSourceRect.top >= theSourceRect.bottom)
        return DDERR_INVALIDRECT;
    bool theKeyed = false;
    DDCOLORKEY theKey = {};
    if (inFlags & DDBLT_KEYSRCOVERRIDE) {
        if (!inFX)
            return DDERR_INVALIDPARAMS;
        theKeyed = true;
        theKey = inFX->ddckSrcColorkey;
    } else if ((inFlags & DDBLT_KEYSRC) && theSource->mHasKey) {
        theKeyed = true;
        theKey = theSource->mKey;
    }
    float theFrom[4] = {float(theSourceRect.left), float(theSourceRect.top), float(theSourceRect.right),
                        float(theSourceRect.bottom)};
    // every destination pixel takes the source pixel under its centre, from the rectangles as the game gave them
    const RECT theWhole = theTarget;
    const float theScaleX = float(Width(theSourceRect)) / Width(theWhole);
    const float theScaleY = float(Height(theSourceRect)) / Height(theWhole);
    if (!Clip(theTarget, theFrom, mWidth, mHeight))
        return DD_OK;
    SubmitDraws(this);
    SubmitDraws(theSource);

    bool theSourceNewerOnGpu = theSource->mStore.mGpuValid && !theSource->mStore.mCpuValid && !theSource->mDC;
    bool theTargetNewerOnGpu = mStore.mGpuValid && !mStore.mCpuValid;
    if (!mDC && IsRenderable() && (theTargetNewerOnGpu || (theSourceNewerOnGpu && theSource != this))) {
        // the blit draws into the destination's texture, which holds or will hold the newer image
        GLScope theScope;
        if (!theScope.Ok() || !EnsureGpu())
            return DD_OK;
        mStore.mCpuValid = false;
        bool theSourceOnGpu = theSource->mStore.mGpuValid && !theSource->mStore.mCpuValid && !theKeyed &&
                              theSource != this && !theSource->mDC;
        if (theSourceOnGpu)
            DrawTexture(ColourTexture(), mWidth, mHeight, theTarget, theSource->mStore.mTexture, theSource->mWidth,
                        theSource->mHeight, theFrom, false);
        else {
            theSource->EnsureCpu();
            int theWidth = Width(theSourceRect), theHeight = Height(theSourceRect);
            std::vector<uint8_t> theRGBA((size_t)theWidth * theHeight * 4);
            uint8_t* theOut = theRGBA.data();
            for (int y = theSourceRect.top; y < theSourceRect.bottom; ++y)
                for (int x = theSourceRect.left; x < theSourceRect.right; ++x, theOut += 4) {
                    const uint8_t* thePixel = theSource->Pixel(x, y);
                    theSource->mFormat.Decode(thePixel, theOut);
                    if (theKeyed)
                        theOut[3] = InKey(theSource->mFormat, theSource->mFormat.Read(thePixel), theKey) ? 0 : 255;
                }
            LoadScratch(theRGBA.data(), theWidth, theHeight);
            const float theScratch[4] = {theFrom[0] - theSourceRect.left, theFrom[1] - theSourceRect.top,
                                         theFrom[2] - theSourceRect.left, theFrom[3] - theSourceRect.top};
            DrawTexture(ColourTexture(), mWidth, mHeight, theTarget, gScratch, theWidth, theHeight, theScratch,
                        theKeyed);
        }
    } else {
        EnsureCpu();
        // the source's pixels in its own format: from its memory, or read back from its texture when that is newer
        std::vector<uint8_t> theCopy;
        const uint8_t* theRows;
        int theRowBytes;
        int theOriginX = 0, theOriginY = 0;
        if (theSource->mStore.mCpuValid || theSource->mDC || theSource == this) {
            if (theSource == this) {
                theCopy = mStore.mBits;     // an overlapping blit reads the image before it
                theRows = theCopy.data();
            } else
                theRows = theSource->mStore.mBits.data();
            theRowBytes = theSource->mPitch;
        } else {
            GLScope theScope;
            if (!theScope.Ok())
                return DD_OK;
            int theWidth = Width(theSourceRect), theHeight = Height(theSourceRect);
            std::vector<uint8_t> theRGBA((size_t)theWidth * theHeight * 4);
            ReadTexture(theSource->mStore.mTexture, theSourceRect, theRGBA.data());
            theCopy.resize((size_t)theWidth * theHeight * theSource->mFormat.mBytes);
            for (size_t i = 0; i < (size_t)theWidth * theHeight; ++i)
                theSource->mFormat.Encode(&theRGBA[i * 4], &theCopy[i * theSource->mFormat.mBytes]);
            theRows = theCopy.data();
            theRowBytes = theWidth * theSource->mFormat.mBytes;
            theOriginX = theSourceRect.left;
            theOriginY = theSourceRect.top;
        }
        const Format& theIn = theSource->mFormat;
        bool theSame = theIn.mBytes == mFormat.mBytes && !memcmp(theIn.mMask, mFormat.mMask, sizeof mFormat.mMask);
        for (int y = theTarget.top; y < theTarget.bottom; ++y) {
            int theSourceY = std::min<int>(theSourceRect.bottom - 1,
                                      (int)(theSourceRect.top + (y - theWhole.top + 0.5f) * theScaleY));
            for (int x = theTarget.left; x < theTarget.right; ++x) {
                int theSourceX = std::min<int>(theSourceRect.right - 1,
                                          (int)(theSourceRect.left + (x - theWhole.left + 0.5f) * theScaleX));
                const uint8_t* thePixel = theRows + (size_t)(theSourceY - theOriginY) * theRowBytes +
                                          (size_t)(theSourceX - theOriginX) * theIn.mBytes;
                if (theKeyed && InKey(theIn, theIn.Read(thePixel), theKey))
                    continue;
                if (theSame)
                    memcpy(Pixel(x, y), thePixel, mFormat.mBytes);
                else {
                    uint8_t theRGBA[4];
                    theIn.Decode(thePixel, theRGBA);
                    mFormat.Encode(theRGBA, Pixel(x, y));
                }
            }
        }
        if (!mDC)
            CpuWritten();
    }
    if (mKind == kPrimary)
        mOwner->PresentSoon();
    return DD_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::BltFast(DWORD inX, DWORD inY, IDirectDrawSurface7* inSource, RECT* inSourceRect,
                                           DWORD inFlags)
{
    Surface* theSource = SurfaceOf(inSource);
    if (!theSource)
        return DDERR_INVALIDPARAMS;
    RECT theSourceRect = inSourceRect ? *inSourceRect : RECT{0, 0, theSource->mWidth, theSource->mHeight};
    RECT theTarget = {(LONG)inX, (LONG)inY, (LONG)inX + Width(theSourceRect), (LONG)inY + Height(theSourceRect)};
    if (mKind == kPrimary) {
        POINT theOrigin = mOwner->PrimaryOrigin();
        OffsetRect(&theTarget, theOrigin.x, theOrigin.y);
    }
    return Blt(&theTarget, inSource, &theSourceRect, inFlags & DDBLTFAST_SRCCOLORKEY ? DDBLT_KEYSRC : 0, nullptr);
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Surface::Flip(IDirectDrawSurface7*, DWORD inFlags)
{
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    if (mKind != kPrimary || mChain.empty())
        return DDERR_NOTFLIPPABLE;
    for (Surface* theMember : mChain)
        if (theMember->mDC || theMember->mLocks)
            return DDERR_SURFACEBUSY;
    if (mDC || mLocks)
        return DDERR_SURFACEBUSY;
    SubmitDraws(nullptr);
    // the first back buffer's image shows; each surface takes the next one's storage, the last the front's
    Store theFront = std::move(mStore);
    mStore = std::move(mChain.front()->mStore);
    for (size_t i = 0; i + 1 < mChain.size(); ++i)
        mChain[i]->mStore = std::move(mChain[i + 1]->mStore);
    mChain.back()->mStore = std::move(theFront);
    mOwner->PresentNow(!(inFlags & DDFLIP_NOVSYNC));
    return DD_OK;
}

void PresentPending()
{
    for (;;) {
        std::unique_lock<std::mutex> theLock(gPresents.mLock);
        gPresents.mWake.wait(theLock, [] { return gPresents.mPending; });
        while (gPresents.mPending) {
            auto theDue = std::min(gPresents.mLast + kPresentQuiet, gPresents.mFirst + kPresentLongest);
            if (std::chrono::steady_clock::now() >= theDue)
                break;
            gPresents.mWake.wait_until(theLock, theDue);
        }
        theLock.unlock();
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        theLock.lock();
        DirectDraw* theOwner = gPresents.mPending ? gPresents.mOwner : nullptr;
        gPresents.mPending = false;
        theLock.unlock();
        if (theOwner)
            theOwner->Present(false);
    }
}

} // namespace dx
} // namespace eeport

using eeport::dx::DirectDraw;
using eeport::dx::gLock;

HRESULT WINAPI DirectDrawCreateEx(GUID* inDriver, void** outDirectDraw, REFIID inInterface, IUnknown* inOuter)
{
    if (!outDirectDraw)
        return DDERR_INVALIDPARAMS;
    *outDirectDraw = nullptr;
    if (inOuter)
        return CLASS_E_NOAGGREGATION;
    if (!IsEqualGUID(inInterface, IID_IDirectDraw7))
        return DDERR_INVALIDPARAMS;
    // NULL, DDCREATE_HARDWAREONLY (1) and DDCREATE_EMULATIONONLY (2) name the primary display, as GUID_NULL does
    if ((uintptr_t)inDriver > 2 && !IsEqualGUID(*inDriver, GUID_NULL))
        return DDERR_INVALIDDIRECTDRAWGUID;
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    if (!eeport::dx::StartGL())
        return DDERR_NODIRECTDRAWHW;
    *outDirectDraw = static_cast<IDirectDraw7*>(new DirectDraw);
    return DD_OK;
}

HRESULT WINAPI DirectDrawEnumerateExA(LPDDENUMCALLBACKEXA inCallback, void* inContext, DWORD)
{
    if (!inCallback)
        return DDERR_INVALIDPARAMS;
    char theDescription[] = "Primary Display Driver", theName[] = "display";
    inCallback(nullptr, theDescription, theName, inContext, NULL);
    return DD_OK;
}

HRESULT WINAPI DirectDrawEnumerateA(LPDDENUMCALLBACKA inCallback, void* inContext)
{
    if (!inCallback)
        return DDERR_INVALIDPARAMS;
    char theDescription[] = "Primary Display Driver", theName[] = "display";
    inCallback(nullptr, theDescription, theName, inContext);
    return DD_OK;
}
