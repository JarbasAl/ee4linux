// Direct3D 7 for the DX7 display plug-in, on OpenGL 3.3: the HAL and T&L HAL devices, vertex buffers, and the fixed
// function pipeline as one GLSL program.
//
// A device records: each draw copies its vertices and indices, with a snapshot of the state it draws with. The
// recording runs as one batch at EndScene, and earlier when DirectDraw needs a surface that a recorded draw writes or
// samples (SubmitDraws). The game's vertex buffer is therefore free to change after a draw, as DDLOCK_DISCARDCONTENTS
// and DDLOCK_NOOVERWRITE allow.
//
// Direct3D's conventions hold in the shader and the state: clip-space z runs from 0 to w, pixel centres are at whole
// coordinates, a texture's row 0 is its top, and D3DCULL_CW culls triangles that turn clockwise on the screen.
// Surfaces keep row 0 first in their textures, so a render target's window row equals its surface row: the shader
// negates y, and viewports, scissors and readbacks need no flip.
#include "DirectDrawInternal.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>
#include <set>

namespace eeport {
namespace dx {
namespace {

const int kLights = 8;          // lights the shader takes at once: the first enabled ones by index
const int kStages = 8;

// The batch's vertex: FVF 0x152, the game's one vertex format, which every recorded vertex is converted to
struct Vertex {
    float mPosition[3];
    float mNormal[3];
    DWORD mDiffuse;
    float mTexel[2];
};
static_assert(sizeof(Vertex) == 36, "the batch vertex is FVF 0x152");

struct State {
    D3DMATRIX mWorld, mView, mProjection;
    D3DVIEWPORT7 mViewport;
    D3DMATERIAL7 mMaterial;
    D3DLIGHT7 mLights[kLights];
    bool mLightOn[kLights];
    DWORD mRender[256];
    DWORD mStage[kStages][32];
    Surface* mTexture;          // stage 0's, or NULL
};

struct Command {
    enum Kind { kDraw, kClear } mKind;
    int mState;
    GLenum mMode;
    int mFirst, mCount;         // vertices, or indices for an indexed draw
    bool mIndexed;
    int mBase;                  // the vertex that index 0 names
    DWORD mClearFlags;
    D3DCOLOR mClearColour;
    float mClearZ;
    RECT mClearRect;
};

float FloatOf(DWORD inBits)
{
    float theValue;
    memcpy(&theValue, &inBits, 4);
    return theValue;
}

DWORD BitsOf(float inValue)
{
    DWORD theBits;
    memcpy(&theBits, &inValue, 4);
    return theBits;
}

D3DMATRIX Identity()
{
    D3DMATRIX theMatrix = {};
    theMatrix._11 = theMatrix._22 = theMatrix._33 = theMatrix._44 = 1;
    return theMatrix;
}

// The row-vector product a × b, as Direct3D composes transforms
D3DMATRIX Multiply(const D3DMATRIX& inA, const D3DMATRIX& inB)
{
    const float(*a)[4] = reinterpret_cast<const float(*)[4]>(&inA);
    const float(*b)[4] = reinterpret_cast<const float(*)[4]>(&inB);
    D3DMATRIX theResult;
    float(*r)[4] = reinterpret_cast<float(*)[4]>(&theResult);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j] + a[i][3] * b[3][j];
    return theResult;
}

// The matrix that takes normals through a transform: the inverse transpose of its upper 3x3, rows for a row vector
void NormalMatrix(const D3DMATRIX& inMatrix, float out[9])
{
    const float(*m)[4] = reinterpret_cast<const float(*)[4]>(&inMatrix);
    float c[3][3] = {{m[1][1] * m[2][2] - m[1][2] * m[2][1], m[1][2] * m[2][0] - m[1][0] * m[2][2],
                      m[1][0] * m[2][1] - m[1][1] * m[2][0]},
                     {m[0][2] * m[2][1] - m[0][1] * m[2][2], m[0][0] * m[2][2] - m[0][2] * m[2][0],
                      m[0][1] * m[2][0] - m[0][0] * m[2][1]},
                     {m[0][1] * m[1][2] - m[0][2] * m[1][1], m[0][2] * m[1][0] - m[0][0] * m[1][2],
                      m[0][0] * m[1][1] - m[0][1] * m[1][0]}};
    float theDeterminant = m[0][0] * c[0][0] + m[0][1] * c[0][1] + m[0][2] * c[0][2];
    float theScale = std::fabs(theDeterminant) > 1e-20f ? 1.0f / theDeterminant : 0.0f;
    // the inverse is the transposed cofactors over the determinant; its transpose is the cofactors themselves
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            out[i * 3 + j] = c[i][j] * theScale;
}

void Transform(const D3DMATRIX& inMatrix, const D3DVECTOR& inPoint, float inW, float out[3])
{
    const float(*m)[4] = reinterpret_cast<const float(*)[4]>(&inMatrix);
    for (int j = 0; j < 3; ++j)
        out[j] = inPoint.x * m[0][j] + inPoint.y * m[1][j] + inPoint.z * m[2][j] + inW * m[3][j];
}

GLenum CompareOf(DWORD inFunction)
{
    switch (inFunction) {
    case D3DCMP_NEVER: return GL_NEVER;
    case D3DCMP_LESS: return GL_LESS;
    case D3DCMP_EQUAL: return GL_EQUAL;
    case D3DCMP_LESSEQUAL: return GL_LEQUAL;
    case D3DCMP_GREATER: return GL_GREATER;
    case D3DCMP_NOTEQUAL: return GL_NOTEQUAL;
    case D3DCMP_GREATEREQUAL: return GL_GEQUAL;
    default: return GL_ALWAYS;
    }
}

GLenum BlendOf(DWORD inBlend)
{
    switch (inBlend) {
    case D3DBLEND_ZERO: return GL_ZERO;
    case D3DBLEND_SRCCOLOR: return GL_SRC_COLOR;
    case D3DBLEND_INVSRCCOLOR: return GL_ONE_MINUS_SRC_COLOR;
    case D3DBLEND_SRCALPHA: return GL_SRC_ALPHA;
    case D3DBLEND_INVSRCALPHA: return GL_ONE_MINUS_SRC_ALPHA;
    case D3DBLEND_DESTALPHA: return GL_DST_ALPHA;
    case D3DBLEND_INVDESTALPHA: return GL_ONE_MINUS_DST_ALPHA;
    case D3DBLEND_DESTCOLOR: return GL_DST_COLOR;
    case D3DBLEND_INVDESTCOLOR: return GL_ONE_MINUS_DST_COLOR;
    case D3DBLEND_SRCALPHASAT: return GL_SRC_ALPHA_SATURATE;
    default: return GL_ONE;
    }
}

const char kVertexShader[] = R"(#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aDiffuse;
layout(location = 3) in vec2 aTexel;

uniform mat4 uWorld, uView, uProjection;
uniform mat3 uNormal;               // world x view, for normals
uniform vec2 uPixelOffset;          // the half-pixel move to Direct3D's pixel centres, in clip units
uniform int uLighting, uNormalize, uColourVertex;
uniform ivec3 uSource;              // ambient, diffuse, emissive: 0 material, 1 vertex diffuse
uniform vec4 uMaterialAmbient, uMaterialDiffuse, uMaterialEmissive, uAmbient;
uniform int uLightCount;
uniform int uLightKind[8];          // 1 point, 3 directional
uniform vec4 uLightDiffuse[8], uLightAmbient[8], uLightAttenuation[8];
uniform vec3 uLightPosition[8], uLightDirection[8];
uniform int uFogMode;               // 0 none, 1 exp, 2 exp2, 3 linear
uniform vec3 uFog;                  // start, end, density

out vec4 vDiffuse;
flat out vec4 vFlatDiffuse;
out vec2 vTexel;
out float vFog;

vec4 Source(int inSource, vec4 inMaterial)
{
    return uColourVertex != 0 && inSource == 1 ? aDiffuse : inMaterial;
}

void main()
{
    vec4 theEye = uView * (uWorld * vec4(aPosition, 1.0));
    vec4 theColour = aDiffuse;
    if (uLighting != 0) {
        vec4 theAmbient = Source(uSource.x, uMaterialAmbient);
        vec4 theDiffuse = Source(uSource.y, uMaterialDiffuse);
        vec4 theEmissive = Source(uSource.z, uMaterialEmissive);
        vec3 theNormal = uNormal * aNormal;
        if (uNormalize != 0 && dot(theNormal, theNormal) > 0.0)
            theNormal = normalize(theNormal);
        vec3 theSum = theEmissive.rgb + theAmbient.rgb * uAmbient.rgb;
        for (int i = 0; i < uLightCount; ++i) {
            vec3 theToLight;
            float theAttenuation = 1.0;
            if (uLightKind[i] == 3)
                theToLight = -uLightDirection[i];
            else {
                vec3 theOffset = uLightPosition[i] - theEye.xyz;
                float theDistance = length(theOffset);
                if (theDistance > uLightAttenuation[i].w)
                    continue;
                theToLight = theDistance > 0.0 ? theOffset / theDistance : vec3(0.0);
                float theDenominator = uLightAttenuation[i].x + uLightAttenuation[i].y * theDistance +
                                       uLightAttenuation[i].z * theDistance * theDistance;
                theAttenuation = theDenominator > 0.0 ? 1.0 / theDenominator : 1.0;
            }
            float theLambert = max(dot(theNormal, theToLight), 0.0);
            theSum += theAttenuation * (theAmbient.rgb * uLightAmbient[i].rgb +
                                        theDiffuse.rgb * uLightDiffuse[i].rgb * theLambert);
        }
        theColour = vec4(clamp(theSum, 0.0, 1.0), theDiffuse.a);
    }
    vDiffuse = theColour;
    vFlatDiffuse = theColour;
    vTexel = aTexel;
    float theDistance = theEye.z;
    if (uFogMode == 3)
        vFog = clamp((uFog.y - theDistance) / (uFog.y - uFog.x), 0.0, 1.0);
    else if (uFogMode == 1)
        vFog = clamp(exp(-uFog.z * theDistance), 0.0, 1.0);
    else if (uFogMode == 2)
        vFog = clamp(exp(-(uFog.z * theDistance) * (uFog.z * theDistance)), 0.0, 1.0);
    else
        vFog = 1.0;
    vec4 theClip = uProjection * theEye;
    // Direct3D's top row is the first row of the target; its depth runs from 0 to w
    theClip.y = -theClip.y;
    theClip.xy += uPixelOffset * theClip.w;
    theClip.z = 2.0 * theClip.z - theClip.w;
    gl_Position = theClip;
}
)";

const char kFragmentShader[] = R"(#version 330 core
uniform sampler2D uTexture;
uniform int uTextured, uFlat, uKeyed;
uniform ivec2 uOp;                  // colour and alpha D3DTOP of stage 0
uniform ivec4 uArgument;            // colour 1, colour 2, alpha 1, alpha 2: D3DTA values
uniform vec4 uFactor;
uniform int uAlphaTest, uAlphaFunction, uAlphaReference;
uniform int uFogOn;
uniform vec4 uFogColour;

in vec4 vDiffuse;
flat in vec4 vFlatDiffuse;
in vec2 vTexel;
in float vFog;
out vec4 oColour;

vec4 Argument(int inArgument, vec4 inDiffuse, vec4 inTexture)
{
    int theWhich = inArgument & 15;
    vec4 theValue = theWhich == 2 ? inTexture : theWhich == 3 ? uFactor : inDiffuse;
    if ((inArgument & 0x20) != 0)
        theValue = vec4(theValue.a);
    if ((inArgument & 0x10) != 0)
        theValue = vec4(1.0) - theValue;
    return theValue;
}

vec4 Operate(int inOp, vec4 inFirst, vec4 inSecond, vec4 inDiffuse, vec4 inTexture)
{
    switch (inOp) {
    case 2: return inFirst;
    case 3: return inSecond;
    case 4: return inFirst * inSecond;
    case 5: return min(inFirst * inSecond * 2.0, 1.0);
    case 6: return min(inFirst * inSecond * 4.0, 1.0);
    case 7: return min(inFirst + inSecond, 1.0);
    case 8: return clamp(inFirst + inSecond - 0.5, 0.0, 1.0);
    case 10: return max(inFirst - inSecond, 0.0);
    case 12: return mix(inSecond, inFirst, inDiffuse.a);
    case 13: return mix(inSecond, inFirst, inTexture.a);
    default: return inDiffuse;
    }
}

bool Passes(int inFunction, int inValue)
{
    switch (inFunction) {
    case 1: return false;
    case 2: return inValue < uAlphaReference;
    case 3: return inValue == uAlphaReference;
    case 4: return inValue <= uAlphaReference;
    case 5: return inValue > uAlphaReference;
    case 6: return inValue != uAlphaReference;
    case 7: return inValue >= uAlphaReference;
    default: return true;
    }
}

void main()
{
    vec4 theDiffuse = uFlat != 0 ? vFlatDiffuse : vDiffuse;
    vec4 theTexture = uTextured != 0 ? texture(uTexture, vTexel) : vec4(1.0);
    if (uKeyed != 0 && theTexture.a < 0.5)
        discard;
    vec4 theColour = theDiffuse;
    if (uOp.x != 1) {
        theColour.rgb = Operate(uOp.x, Argument(uArgument.x, theDiffuse, theTexture),
                                Argument(uArgument.y, theDiffuse, theTexture), theDiffuse, theTexture).rgb;
        if (uOp.y != 1)
            theColour.a = Operate(uOp.y, Argument(uArgument.z, theDiffuse, theTexture),
                                  Argument(uArgument.w, theDiffuse, theTexture), theDiffuse, theTexture).a;
    }
    if (uAlphaTest != 0 && !Passes(uAlphaFunction, int(floor(theColour.a * 255.0 + 0.5))))
        discard;
    if (uFogOn != 0)
        theColour.rgb = mix(uFogColour.rgb, theColour.rgb, vFog);
    oColour = theColour;
}
)";

struct Program {
    GLuint mName = 0;
    GLuint mVertexArray = 0, mVertices = 0, mIndices = 0;
#define EE_D3D_UNIFORMS(X)                                                                                         \
    X(uWorld) X(uView) X(uProjection) X(uNormal) X(uPixelOffset) X(uLighting) X(uNormalize) X(uColourVertex)       \
    X(uSource) X(uMaterialAmbient) X(uMaterialDiffuse) X(uMaterialEmissive) X(uAmbient) X(uLightCount)             \
    X(uLightKind) X(uLightDiffuse) X(uLightAmbient) X(uLightAttenuation) X(uLightPosition) X(uLightDirection)      \
    X(uFogMode) X(uFog) X(uTexture) X(uTextured) X(uFlat) X(uKeyed) X(uOp) X(uArgument) X(uFactor) X(uAlphaTest)   \
    X(uAlphaFunction) X(uAlphaReference) X(uFogOn) X(uFogColour)
#define EE_D3D_UNIFORM_MEMBER(name) GLint name = -1;
    EE_D3D_UNIFORMS(EE_D3D_UNIFORM_MEMBER)
#undef EE_D3D_UNIFORM_MEMBER
    std::map<uint64_t, GLuint> mSamplers;
};

Program gProgram;

GLuint CompileShader(GLenum inKind, const char* inSource)
{
    GLuint theShader = gl.CreateShader(inKind);
    gl.ShaderSource(theShader, 1, &inSource, nullptr);
    gl.CompileShader(theShader);
    GLint theOk = 0;
    gl.GetShaderiv(theShader, GL_COMPILE_STATUS, &theOk);
    if (!theOk) {
        char theLog[4096] = {};
        gl.GetShaderInfoLog(theShader, sizeof theLog, nullptr, theLog);
        Log("Direct3D shader does not compile: %s", theLog);
    }
    return theShader;
}

// Under a GLScope: the program, its buffers and its uniforms, made on first use
bool StartProgram()
{
    if (gProgram.mName)
        return true;
    GLuint theVertex = CompileShader(GL_VERTEX_SHADER, kVertexShader);
    GLuint theFragment = CompileShader(GL_FRAGMENT_SHADER, kFragmentShader);
    gProgram.mName = gl.CreateProgram();
    gl.AttachShader(gProgram.mName, theVertex);
    gl.AttachShader(gProgram.mName, theFragment);
    gl.LinkProgram(gProgram.mName);
    gl.DeleteShader(theVertex);
    gl.DeleteShader(theFragment);
    GLint theOk = 0;
    gl.GetProgramiv(gProgram.mName, GL_LINK_STATUS, &theOk);
    if (!theOk) {
        char theLog[4096] = {};
        gl.GetProgramInfoLog(gProgram.mName, sizeof theLog, nullptr, theLog);
        Log("Direct3D shader does not link: %s", theLog);
        return false;
    }
#define EE_D3D_UNIFORM_FIND(name) gProgram.name = gl.GetUniformLocation(gProgram.mName, #name);
    EE_D3D_UNIFORMS(EE_D3D_UNIFORM_FIND)
#undef EE_D3D_UNIFORM_FIND
    gl.GenVertexArrays(1, &gProgram.mVertexArray);
    gl.BindVertexArray(gProgram.mVertexArray);
    gl.GenBuffers(1, &gProgram.mVertices);
    gl.GenBuffers(1, &gProgram.mIndices);
    gl.BindBuffer(GL_ARRAY_BUFFER, gProgram.mVertices);
    gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, gProgram.mIndices);
    gl.VertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(Vertex), (const void*)offsetof(Vertex, mPosition));
    gl.VertexAttribPointer(1, 3, GL_FLOAT, 0, sizeof(Vertex), (const void*)offsetof(Vertex, mNormal));
    // D3DCOLOR is B, G, R, A in memory
    gl.VertexAttribPointer(2, GL_BGRA, GL_UNSIGNED_BYTE, 1, sizeof(Vertex), (const void*)offsetof(Vertex, mDiffuse));
    gl.VertexAttribPointer(3, 2, GL_FLOAT, 0, sizeof(Vertex), (const void*)offsetof(Vertex, mTexel));
    for (GLuint i = 0; i < 4; ++i)
        gl.EnableVertexAttribArray(i);
    gl.BindVertexArray(0);
    return true;
}

// The sampler for stage 0's address modes, filters and level-of-detail bias
GLuint SamplerOf(const DWORD* inStage, bool inMipmapped)
{
    DWORD theU = inStage[D3DTSS_ADDRESSU], theV = inStage[D3DTSS_ADDRESSV];
    DWORD theMag = inStage[D3DTSS_MAGFILTER], theMin = inStage[D3DTSS_MINFILTER];
    DWORD theMip = inMipmapped ? inStage[D3DTSS_MIPFILTER] : D3DTFP_NONE;
    uint64_t theKey = (uint64_t)theU | (uint64_t)theV << 4 | (uint64_t)theMag << 8 | (uint64_t)theMin << 12 |
                      (uint64_t)theMip << 16 | (uint64_t)inStage[D3DTSS_MIPMAPLODBIAS] << 32;
    GLuint& theSampler = gProgram.mSamplers[theKey];
    if (theSampler)
        return theSampler;
    gl.GenSamplers(1, &theSampler);
    auto theWrap = [](DWORD inMode) -> GLint {
        return inMode == D3DTADDRESS_MIRROR ? GL_MIRRORED_REPEAT
             : inMode == D3DTADDRESS_CLAMP || inMode == D3DTADDRESS_BORDER ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    };
    gl.SamplerParameteri(theSampler, GL_TEXTURE_WRAP_S, theWrap(theU));
    gl.SamplerParameteri(theSampler, GL_TEXTURE_WRAP_T, theWrap(theV));
    // anisotropic and the cubic filters filter linearly: D3DTSS_MAXANISOTROPY stays at its default of 1
    gl.SamplerParameteri(theSampler, GL_TEXTURE_MAG_FILTER, theMag == D3DTFG_POINT ? GL_NEAREST : GL_LINEAR);
    bool theLinear = theMin != D3DTFN_POINT;
    GLint theMinFilter = theMip == D3DTFP_POINT  ? (theLinear ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST)
                       : theMip == D3DTFP_LINEAR ? (theLinear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR)
                                                 : (theLinear ? GL_LINEAR : GL_NEAREST);
    gl.SamplerParameteri(theSampler, GL_TEXTURE_MIN_FILTER, theMinFilter);
    gl.SamplerParameterf(theSampler, GL_TEXTURE_LOD_BIAS, FloatOf(inStage[D3DTSS_MIPMAPLODBIAS]));
    return theSampler;
}

void Colour(D3DCOLOR inColour, float out[4])
{
    out[0] = ((inColour >> 16) & 0xff) / 255.0f;
    out[1] = ((inColour >> 8) & 0xff) / 255.0f;
    out[2] = (inColour & 0xff) / 255.0f;
    out[3] = (inColour >> 24) / 255.0f;
}

// The bytes of one vertex of an FVF, and where its parts start; false for a format the layer does not draw
struct Layout {
    int mStride = 0, mNormal = -1, mDiffuse = -1, mTexel = -1;
};

bool LayoutOf(DWORD inFVF, Layout* outLayout)
{
    Layout theLayout;
    if ((inFVF & D3DFVF_POSITION_MASK) != D3DFVF_XYZ)
        return false;
    int theOffset = 12;
    if (inFVF & D3DFVF_NORMAL) {
        theLayout.mNormal = theOffset;
        theOffset += 12;
    }
    if (inFVF & D3DFVF_RESERVED1)
        theOffset += 4;
    if (inFVF & D3DFVF_DIFFUSE) {
        theLayout.mDiffuse = theOffset;
        theOffset += 4;
    }
    if (inFVF & D3DFVF_SPECULAR)
        theOffset += 4;
    int theSets = (inFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    for (int i = 0; i < theSets; ++i) {
        static const int kFloats[4] = {2, 3, 4, 1};
        if (i == 0)
            theLayout.mTexel = theOffset;
        theOffset += 4 * kFloats[(inFVF >> (16 + 2 * i)) & 3];
    }
    theLayout.mStride = theOffset;
    *outLayout = theLayout;
    return true;
}

class VertexBuffer;
class Device;
std::set<Device*> gDevices;
bool gSubmitting;

class Direct3D final : public IDirect3D7 {
public:
    explicit Direct3D(IDirectDraw7* inOwner) : mOwner(inOwner) { mOwner->AddRef(); }

    IDirectDraw7* mOwner;
    std::atomic<long> mReferences{1};

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        if (!outObject)
            return E_POINTER;
        *outObject = nullptr;
        if (!IsEqualGUID(inID, IID_IUnknown) && !IsEqualGUID(inID, IID_IDirect3D7))
            return E_NOINTERFACE;
        AddRef();
        *outObject = this;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef() override { return ULONG(++mReferences); }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release() override
    {
        long theCount = --mReferences;
        if (theCount == 0) {
            mOwner->Release();
            delete this;
        }
        return ULONG(theCount);
    }

    static void Describe(D3DDEVICEDESC7* outDesc, bool inTnL)
    {
        memset(outDesc, 0, sizeof *outDesc);
        outDesc->dwDevCaps = D3DDEVCAPS_FLOATTLVERTEX | D3DDEVCAPS_EXECUTESYSTEMMEMORY | D3DDEVCAPS_TLVERTEXSYSTEMMEMORY |
                             D3DDEVCAPS_TEXTUREVIDEOMEMORY | D3DDEVCAPS_DRAWPRIMTLVERTEX | D3DDEVCAPS_CANRENDERAFTERFLIP |
                             D3DDEVCAPS_TEXTURENONLOCALVIDMEM | D3DDEVCAPS_DRAWPRIMITIVES2 |
                             D3DDEVCAPS_DRAWPRIMITIVES2EX | D3DDEVCAPS_HWRASTERIZATION;
        if (inTnL)
            outDesc->dwDevCaps |= D3DDEVCAPS_HWTRANSFORMANDLIGHT;
        D3DPRIMCAPS thePrimitives = {sizeof thePrimitives};
        thePrimitives.dwMiscCaps = D3DPMISCCAPS_CULLCW | D3DPMISCCAPS_CULLCCW | D3DPMISCCAPS_CULLNONE |
                                   D3DPMISCCAPS_MASKZ;
        thePrimitives.dwRasterCaps = D3DPRASTERCAPS_ZTEST | D3DPRASTERCAPS_FOGVERTEX | D3DPRASTERCAPS_MIPMAPLODBIAS;
        thePrimitives.dwZCmpCaps = thePrimitives.dwAlphaCmpCaps = 0xff;
        thePrimitives.dwSrcBlendCaps = thePrimitives.dwDestBlendCaps = 0x1fff;
        thePrimitives.dwShadeCaps = D3DPSHADECAPS_COLORGOURAUDRGB | D3DPSHADECAPS_COLORFLATRGB |
                                    D3DPSHADECAPS_ALPHAGOURAUDBLEND | D3DPSHADECAPS_FOGGOURAUD;
        thePrimitives.dwTextureCaps = D3DPTEXTURECAPS_PERSPECTIVE | D3DPTEXTURECAPS_ALPHA |
                                      D3DPTEXTURECAPS_TRANSPARENCY | D3DPTEXTURECAPS_NONPOW2CONDITIONAL;
        thePrimitives.dwTextureFilterCaps = D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MINFLINEAR |
                                            D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR |
                                            D3DPTFILTERCAPS_MAGFPOINT | D3DPTFILTERCAPS_MAGFLINEAR;
        thePrimitives.dwTextureBlendCaps = D3DPTBLENDCAPS_MODULATE | D3DPTBLENDCAPS_MODULATEALPHA;
        thePrimitives.dwTextureAddressCaps = D3DPTADDRESSCAPS_WRAP | D3DPTADDRESSCAPS_MIRROR |
                                             D3DPTADDRESSCAPS_CLAMP | D3DPTADDRESSCAPS_INDEPENDENTUV;
        outDesc->dpcLineCaps = outDesc->dpcTriCaps = thePrimitives;
        outDesc->dwDeviceRenderBitDepth = DDBD_16 | DDBD_32;
        outDesc->dwDeviceZBufferBitDepth = DDBD_16 | DDBD_24 | DDBD_32;
        outDesc->dwMinTextureWidth = outDesc->dwMinTextureHeight = 1;
        outDesc->dwMaxTextureWidth = outDesc->dwMaxTextureHeight = 4096;
        outDesc->dwMaxTextureRepeat = 8192;
        outDesc->dwMaxTextureAspectRatio = 4096;
        outDesc->dwMaxAnisotropy = 1;
        outDesc->dvGuardBandLeft = outDesc->dvGuardBandTop = -8192;
        outDesc->dvGuardBandRight = outDesc->dvGuardBandBottom = 8192;
        outDesc->dwFVFCaps = 1;
        outDesc->dwTextureOpCaps = D3DTEXOPCAPS_DISABLE | D3DTEXOPCAPS_SELECTARG1 | D3DTEXOPCAPS_SELECTARG2 |
                                   D3DTEXOPCAPS_MODULATE | D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_MODULATE4X |
                                   D3DTEXOPCAPS_ADD | D3DTEXOPCAPS_ADDSIGNED | D3DTEXOPCAPS_SUBTRACT |
                                   D3DTEXOPCAPS_BLENDDIFFUSEALPHA | D3DTEXOPCAPS_BLENDTEXTUREALPHA;
        outDesc->wMaxTextureBlendStages = 1;
        outDesc->wMaxSimultaneousTextures = 1;
        outDesc->dwMaxActiveLights = kLights;
        outDesc->dvMaxVertexW = 1e10f;
        outDesc->deviceGUID = inTnL ? IID_IDirect3DTnLHalDevice : IID_IDirect3DHALDevice;
        outDesc->dwVertexProcessingCaps = D3DVTXPCAPS_DIRECTIONALLIGHTS | D3DVTXPCAPS_POSITIONALLIGHTS;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumDevices(LPD3DENUMDEVICESCALLBACK7 inCallback,
                                                              void* inContext) override
    {
        if (!inCallback)
            return DDERR_INVALIDPARAMS;
        char theHal[] = "Direct3D HAL on OpenGL", theHalName[] = "Direct3D HAL";
        char theTnL[] = "Direct3D T&L HAL on OpenGL", theTnLName[] = "Direct3D T&L HAL";
        D3DDEVICEDESC7 theDesc;
        Describe(&theDesc, false);
        if (inCallback(theHal, theHalName, &theDesc, inContext) == D3DENUMRET_CANCEL)
            return D3D_OK;
        Describe(&theDesc, true);
        inCallback(theTnL, theTnLName, &theDesc, inContext);
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateDevice(REFCLSID inKind, IDirectDrawSurface7* inTarget,
                                                               IDirect3DDevice7** outDevice) override;
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateVertexBuffer(D3DVERTEXBUFFERDESC* inDesc,
                                                                     IDirect3DVertexBuffer7** outBuffer,
                                                                     DWORD inFlags) override;

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumZBufferFormats(REFCLSID, LPD3DENUMPIXELFORMATSCALLBACK inCallback,
                                                                     void* inContext) override
    {
        if (!inCallback)
            return DDERR_INVALIDPARAMS;
        for (DWORD theBits : {16u, 24u, 32u}) {
            DDPIXELFORMAT theFormat = {sizeof theFormat};
            theFormat.dwFlags = DDPF_ZBUFFER;
            theFormat.dwZBufferBitDepth = theBits;
            theFormat.dwZBitMask = theBits == 32 ? 0xffffffffu : (1u << theBits) - 1;
            if (inCallback(&theFormat, inContext) == D3DENUMRET_CANCEL)
                break;
        }
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EvictManagedTextures() override { return D3D_OK; }
};

class VertexBuffer final : public IDirect3DVertexBuffer7 {
public:
    D3DVERTEXBUFFERDESC mDesc = {};
    Layout mLayout;
    std::vector<uint8_t> mData;
    int mLocks = 0;
    std::atomic<long> mReferences{1};

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        if (!outObject)
            return E_POINTER;
        *outObject = nullptr;
        if (!IsEqualGUID(inID, IID_IUnknown) && !IsEqualGUID(inID, IID_IDirect3DVertexBuffer7))
            return E_NOINTERFACE;
        AddRef();
        *outObject = this;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef() override { return ULONG(++mReferences); }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release() override
    {
        long theCount = --mReferences;
        if (theCount == 0)
            delete this;
        return ULONG(theCount);
    }

    // Draws copy the vertices they use, so a lock never waits and always gives the start of the buffer
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Lock(DWORD, void** outData, DWORD* outSize) override
    {
        if (!outData)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        *outData = mData.data();
        if (outSize)
            *outSize = mDesc.dwNumVertices * (DWORD)mLayout.mStride;
        ++mLocks;
        return D3D_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Unlock() override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        if (!mLocks)
            return D3DERR_VERTEXBUFFERUNLOCKFAILED;
        --mLocks;
        return D3D_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ProcessVertices(DWORD, DWORD, DWORD, IDirect3DVertexBuffer7*, DWORD,
                                                                  IDirect3DDevice7*, DWORD) override
    {
        EE_PORT_UNSUPPORTED("IDirect3DVertexBuffer7::ProcessVertices");
        return DDERR_UNSUPPORTED;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetVertexBufferDesc(D3DVERTEXBUFFERDESC* outDesc) override
    {
        if (!outDesc)
            return DDERR_INVALIDPARAMS;
        *outDesc = mDesc;
        return D3D_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Optimize(IDirect3DDevice7*, DWORD) override { return D3D_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ProcessVerticesStrided(DWORD, DWORD, DWORD,
                                                                         D3DDRAWPRIMITIVESTRIDEDDATA*, DWORD,
                                                                         IDirect3DDevice7*, DWORD) override
    {
        EE_PORT_UNSUPPORTED("IDirect3DVertexBuffer7::ProcessVerticesStrided");
        return DDERR_UNSUPPORTED;
    }
};

// The vertices the game may write past a vertex buffer's end: the plug-in does not bound a model's vertex count
// by the buffer's size (DrawModel), and a driver's buffer was rarely the exact size asked for
const DWORD kVertexSlack = 65536;

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Direct3D::CreateVertexBuffer(D3DVERTEXBUFFERDESC* inDesc, IDirect3DVertexBuffer7** outBuffer,
                                                       DWORD)
{
    if (!inDesc || !outBuffer || inDesc->dwSize != sizeof(D3DVERTEXBUFFERDESC) || !inDesc->dwNumVertices)
        return DDERR_INVALIDPARAMS;
    *outBuffer = nullptr;
    Layout theLayout;
    if (!LayoutOf(inDesc->dwFVF, &theLayout)) {
        Log("Direct3D draws no vertex format 0x%lx", (unsigned long)inDesc->dwFVF);
        return DDERR_INVALIDPARAMS;
    }
    VertexBuffer* theBuffer = new VertexBuffer;
    theBuffer->mDesc = *inDesc;
    theBuffer->mLayout = theLayout;
    theBuffer->mData.assign((size_t)(inDesc->dwNumVertices + kVertexSlack) * theLayout.mStride, 0);
    *outBuffer = theBuffer;
    return D3D_OK;
}

class Device final : public IDirect3DDevice7 {
public:
    Device(Direct3D* inDirect3D, Surface* inTarget, bool inTnL) : mDirect3D(inDirect3D), mTnL(inTnL)
    {
        mDirect3D->AddRef();
        Target(inTarget);
        State& s = mState;
        memset(&s, 0, sizeof s);
        s.mWorld = s.mView = s.mProjection = Identity();
        s.mViewport = {0, 0, (DWORD)inTarget->mWidth, (DWORD)inTarget->mHeight, 0, 1};
        DWORD* r = s.mRender;
        r[D3DRENDERSTATE_ZENABLE] = inTarget->mZ ? D3DZB_TRUE : D3DZB_FALSE;
        r[D3DRENDERSTATE_FILLMODE] = D3DFILL_SOLID;
        r[D3DRENDERSTATE_SHADEMODE] = D3DSHADE_GOURAUD;
        r[D3DRENDERSTATE_ZWRITEENABLE] = TRUE;
        r[D3DRENDERSTATE_SRCBLEND] = D3DBLEND_ONE;
        r[D3DRENDERSTATE_DESTBLEND] = D3DBLEND_ZERO;
        r[D3DRENDERSTATE_CULLMODE] = D3DCULL_CCW;
        r[D3DRENDERSTATE_ZFUNC] = D3DCMP_LESSEQUAL;
        r[D3DRENDERSTATE_ALPHAFUNC] = D3DCMP_ALWAYS;
        r[D3DRENDERSTATE_FOGEND] = BitsOf(1.0f);
        r[D3DRENDERSTATE_FOGDENSITY] = BitsOf(1.0f);
        r[D3DRENDERSTATE_TEXTUREFACTOR] = 0xffffffff;
        r[D3DRENDERSTATE_COLORVERTEX] = TRUE;
        r[D3DRENDERSTATE_LIGHTING] = TRUE;
        r[D3DRENDERSTATE_CLIPPING] = TRUE;
        r[D3DRENDERSTATE_LOCALVIEWER] = TRUE;
        r[D3DRENDERSTATE_DIFFUSEMATERIALSOURCE] = D3DMCS_COLOR1;
        r[D3DRENDERSTATE_SPECULARMATERIALSOURCE] = D3DMCS_COLOR2;
        for (int i = 0; i < kStages; ++i) {
            DWORD* t = s.mStage[i];
            t[D3DTSS_COLOROP] = i == 0 ? D3DTOP_MODULATE : D3DTOP_DISABLE;
            t[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
            t[D3DTSS_COLORARG2] = D3DTA_CURRENT;
            t[D3DTSS_ALPHAOP] = i == 0 ? D3DTOP_SELECTARG1 : D3DTOP_DISABLE;
            t[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
            t[D3DTSS_ALPHAARG2] = D3DTA_CURRENT;
            t[D3DTSS_TEXCOORDINDEX] = (DWORD)i;
            t[D3DTSS_ADDRESS] = t[D3DTSS_ADDRESSU] = t[D3DTSS_ADDRESSV] = D3DTADDRESS_WRAP;
            t[D3DTSS_MAGFILTER] = D3DTFG_POINT;
            t[D3DTSS_MINFILTER] = D3DTFN_POINT;
            t[D3DTSS_MIPFILTER] = D3DTFP_NONE;
            t[D3DTSS_MAXANISOTROPY] = 1;
        }
        gDevices.insert(this);
    }

    ~Device()
    {
        gDevices.erase(this);
        Drop();
        for (Surface*& theTexture : mTextures)
            if (theTexture) {
                theTexture->Release();
                theTexture = nullptr;
            }
        Target(nullptr);
        mDirect3D->Release();
    }

    // Runs the recorded draws into the target, in order
    void Submit()
    {
        if (mCommands.empty() || gSubmitting)
            return;
        gSubmitting = true;
        Run();
        gSubmitting = false;
        Drop();
    }

    Surface* mTarget = nullptr;
    std::vector<Command> mCommands;
    std::vector<State> mStates;

private:
    Direct3D* mDirect3D;
    bool mTnL;
    std::atomic<long> mReferences{1};
    State mState;
    bool mStateChanged = true;
    Surface* mTextures[kStages] = {};
    bool mInScene = false;
    std::vector<Vertex> mVertices;
    std::vector<WORD> mIndices;

    void Target(Surface* inTarget)
    {
        if (inTarget) {
            inTarget->AddRef();
            ++inTarget->mDeviceUses;
        }
        if (mTarget) {
            --mTarget->mDeviceUses;
            mTarget->Release();
        }
        mTarget = inTarget;
    }

    // Forgets the recorded draws and lets their textures go
    void Drop()
    {
        for (State& theState : mStates)
            if (theState.mTexture) {
                --theState.mTexture->mRecordedUses;
                theState.mTexture->Release();
            }
        mStates.clear();
        mCommands.clear();
        mVertices.clear();
        mIndices.clear();
        mStateChanged = true;
    }

    int Snapshot()
    {
        if (mStateChanged || mStates.empty()) {
            mState.mTexture = mTextures[0] ? mTextures[0]->TextureRoot() : nullptr;
            if (mState.mTexture) {
                mState.mTexture->AddRef();
                ++mState.mTexture->mRecordedUses;
            }
            mStates.push_back(mState);
            mStateChanged = false;
        }
        return (int)mStates.size() - 1;
    }

    void Apply(const State& inState, Surface* inTarget, bool inDepth)
    {
        const DWORD* r = inState.mRender;
        const D3DVIEWPORT7& theViewport = inState.mViewport;
        gl.Viewport((GLint)theViewport.dwX, (GLint)theViewport.dwY, (GLsizei)theViewport.dwWidth,
                    (GLsizei)theViewport.dwHeight);
        gl.DepthRange(theViewport.dvMinZ, theViewport.dvMaxZ);
        gl.UseProgram(gProgram.mName);
        gl.UniformMatrix4fv(gProgram.uWorld, 1, 0, &inState.mWorld._11);
        gl.UniformMatrix4fv(gProgram.uView, 1, 0, &inState.mView._11);
        gl.UniformMatrix4fv(gProgram.uProjection, 1, 0, &inState.mProjection._11);
        D3DMATRIX theWorldView = Multiply(inState.mWorld, inState.mView);
        float theNormal[9];
        NormalMatrix(theWorldView, theNormal);
        gl.UniformMatrix3fv(gProgram.uNormal, 1, 0, theNormal);
        // Direct3D samples a pixel at its top-left corner, OpenGL at its centre; 63/128 rather than 1/2 keeps an
        // edge that falls exactly on a centre on one side, as Wine does
        const float kOffset = 63.0f / 128.0f;
        gl.Uniform2f(gProgram.uPixelOffset, 2 * kOffset / std::max(1u, (unsigned)theViewport.dwWidth),
                     2 * kOffset / std::max(1u, (unsigned)theViewport.dwHeight));

        gl.Uniform1i(gProgram.uLighting, r[D3DRENDERSTATE_LIGHTING] != 0);
        gl.Uniform1i(gProgram.uNormalize, r[D3DRENDERSTATE_NORMALIZENORMALS] != 0);
        gl.Uniform1i(gProgram.uColourVertex, r[D3DRENDERSTATE_COLORVERTEX] != 0);
        auto theSource = [&](DWORD inState) { return r[inState] == D3DMCS_COLOR1 ? 1 : 0; };
        GLint theSources[3] = {theSource(D3DRENDERSTATE_AMBIENTMATERIALSOURCE),
                               theSource(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE),
                               theSource(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE)};
        gl.Uniform3i(gProgram.uSource, theSources[0], theSources[1], theSources[2]);
        const D3DMATERIAL7& m = inState.mMaterial;
        gl.Uniform4f(gProgram.uMaterialAmbient, m.ambient.r, m.ambient.g, m.ambient.b, m.ambient.a);
        gl.Uniform4f(gProgram.uMaterialDiffuse, m.diffuse.r, m.diffuse.g, m.diffuse.b, m.diffuse.a);
        gl.Uniform4f(gProgram.uMaterialEmissive, m.emissive.r, m.emissive.g, m.emissive.b, m.emissive.a);
        float theAmbient[4];
        Colour(r[D3DRENDERSTATE_AMBIENT], theAmbient);
        gl.Uniform4fv(gProgram.uAmbient, 1, theAmbient);

        GLint theKinds[kLights] = {};
        float theDiffuse[kLights][4] = {}, theLightAmbient[kLights][4] = {}, theAttenuation[kLights][4] = {};
        float thePosition[kLights][3] = {}, theDirection[kLights][3] = {};
        int theCount = 0;
        for (int i = 0; i < kLights; ++i) {
            if (!inState.mLightOn[i])
                continue;
            const D3DLIGHT7& theLight = inState.mLights[i];
            theKinds[theCount] = theLight.dltType == D3DLIGHT_DIRECTIONAL ? 3 : 1;
            memcpy(theDiffuse[theCount], &theLight.dcvDiffuse, 16);
            memcpy(theLightAmbient[theCount], &theLight.dcvAmbient, 16);
            theAttenuation[theCount][0] = theLight.dvAttenuation0;
            theAttenuation[theCount][1] = theLight.dvAttenuation1;
            theAttenuation[theCount][2] = theLight.dvAttenuation2;
            theAttenuation[theCount][3] = theLight.dvRange > 0 ? theLight.dvRange : 1e30f;
            Transform(inState.mView, theLight.dvPosition, 1, thePosition[theCount]);
            Transform(inState.mView, theLight.dvDirection, 0, theDirection[theCount]);
            float* d = theDirection[theCount];
            float theLength = std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
            if (theLength > 0)
                for (int j = 0; j < 3; ++j)
                    d[j] /= theLength;
            ++theCount;
        }
        gl.Uniform1i(gProgram.uLightCount, theCount);
        gl.Uniform1iv(gProgram.uLightKind, kLights, theKinds);
        gl.Uniform4fv(gProgram.uLightDiffuse, kLights, &theDiffuse[0][0]);
        gl.Uniform4fv(gProgram.uLightAmbient, kLights, &theLightAmbient[0][0]);
        gl.Uniform4fv(gProgram.uLightAttenuation, kLights, &theAttenuation[0][0]);
        gl.Uniform3fv(gProgram.uLightPosition, kLights, &thePosition[0][0]);
        gl.Uniform3fv(gProgram.uLightDirection, kLights, &theDirection[0][0]);

        bool theFog = r[D3DRENDERSTATE_FOGENABLE] != 0;
        DWORD theFogMode = r[D3DRENDERSTATE_FOGTABLEMODE] != D3DFOG_NONE ? r[D3DRENDERSTATE_FOGTABLEMODE]
                                                                         : r[D3DRENDERSTATE_FOGVERTEXMODE];
        gl.Uniform1i(gProgram.uFogMode, theFog ? (GLint)theFogMode : 0);
        gl.Uniform1i(gProgram.uFogOn, theFog && theFogMode != D3DFOG_NONE);
        float theFogValues[3] = {FloatOf(r[D3DRENDERSTATE_FOGSTART]), FloatOf(r[D3DRENDERSTATE_FOGEND]),
                                 FloatOf(r[D3DRENDERSTATE_FOGDENSITY])};
        gl.Uniform3fv(gProgram.uFog, 1, theFogValues);
        float theFogColour[4];
        Colour(r[D3DRENDERSTATE_FOGCOLOR], theFogColour);
        gl.Uniform4fv(gProgram.uFogColour, 1, theFogColour);

        const DWORD* t = inState.mStage[0];
        Surface* theTexture = inState.mTexture;
        bool theKeyed = r[D3DRENDERSTATE_COLORKEYENABLE] && theTexture && theTexture->mHasKey;
        bool theTextured = theTexture && theTexture->EnsureGpu(theKeyed);
        gl.ActiveTexture(GL_TEXTURE0);
        gl.BindTexture(GL_TEXTURE_2D, theTextured ? theTexture->mStore.mTexture : 0);
        if (theTextured)
            gl.BindSampler(0, SamplerOf(t, theTexture->mChain.size() > 0 &&
                                               (theTexture->mDesc.ddsCaps.dwCaps & DDSCAPS_MIPMAP)));
        gl.Uniform1i(gProgram.uTexture, 0);
        gl.Uniform1i(gProgram.uTextured, theTextured);
        gl.Uniform1i(gProgram.uKeyed, theKeyed && theTextured);
        gl.Uniform1i(gProgram.uFlat, r[D3DRENDERSTATE_SHADEMODE] == D3DSHADE_FLAT);
        gl.Uniform2i(gProgram.uOp, (GLint)t[D3DTSS_COLOROP], (GLint)t[D3DTSS_ALPHAOP]);
        gl.Uniform4i(gProgram.uArgument, (GLint)t[D3DTSS_COLORARG1], (GLint)t[D3DTSS_COLORARG2],
                     (GLint)t[D3DTSS_ALPHAARG1], (GLint)t[D3DTSS_ALPHAARG2]);
        float theFactor[4];
        Colour(r[D3DRENDERSTATE_TEXTUREFACTOR], theFactor);
        gl.Uniform4fv(gProgram.uFactor, 1, theFactor);
        gl.Uniform1i(gProgram.uAlphaTest, r[D3DRENDERSTATE_ALPHATESTENABLE] != 0);
        gl.Uniform1i(gProgram.uAlphaFunction, (GLint)r[D3DRENDERSTATE_ALPHAFUNC]);
        gl.Uniform1i(gProgram.uAlphaReference, (GLint)(r[D3DRENDERSTATE_ALPHAREF] & 0xff));

        if (r[D3DRENDERSTATE_ALPHABLENDENABLE]) {
            gl.Enable(GL_BLEND);
            DWORD theSourceBlend = r[D3DRENDERSTATE_SRCBLEND], theTargetBlend = r[D3DRENDERSTATE_DESTBLEND];
            if (theSourceBlend == D3DBLEND_BOTHSRCALPHA)
                gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            else if (theSourceBlend == D3DBLEND_BOTHINVSRCALPHA)
                gl.BlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
            else
                gl.BlendFunc(BlendOf(theSourceBlend), BlendOf(theTargetBlend));
        } else
            gl.Disable(GL_BLEND);
        // a target without alpha keeps alpha 1, so that DESTALPHA reads 1 as on the card
        GLboolean theAlpha = inTarget->mFormat.HasAlpha();
        gl.ColorMask(1, 1, 1, theAlpha);
        if (inDepth && r[D3DRENDERSTATE_ZENABLE]) {
            gl.Enable(GL_DEPTH_TEST);
            gl.DepthFunc(CompareOf(r[D3DRENDERSTATE_ZFUNC]));
            gl.DepthMask(r[D3DRENDERSTATE_ZWRITEENABLE] ? 1 : 0);
        } else
            gl.Disable(GL_DEPTH_TEST);
        // a clockwise triangle on Direct3D's screen turns counter-clockwise in OpenGL's window, where row 0 is at
        // the bottom: D3DCULL_CW culls OpenGL's front faces
        if (r[D3DRENDERSTATE_CULLMODE] == D3DCULL_CW || r[D3DRENDERSTATE_CULLMODE] == D3DCULL_CCW) {
            gl.Enable(GL_CULL_FACE);
            gl.FrontFace(GL_CCW);
            gl.CullFace(r[D3DRENDERSTATE_CULLMODE] == D3DCULL_CW ? GL_FRONT : GL_BACK);
        } else
            gl.Disable(GL_CULL_FACE);
        gl.PolygonMode(GL_FRONT_AND_BACK, r[D3DRENDERSTATE_FILLMODE] == D3DFILL_POINT       ? GL_POINT
                                          : r[D3DRENDERSTATE_FILLMODE] == D3DFILL_WIREFRAME ? GL_LINE
                                                                                            : GL_FILL);
    }

    void Run()
    {
        GLScope theScope;
        Surface* theTarget = mTarget;
        if (!theScope.Ok() || !theTarget || !StartProgram() || !theTarget->EnsureGpu())
            return;
        Surface* theZ = theTarget->mZ;
        bool theDepth = theZ && theZ->mWidth == theTarget->mWidth && theZ->mHeight == theTarget->mHeight;
        BindTarget(theTarget->ColourTexture(), theDepth ? theZ->DepthBuffer() : 0);
        if (gl.CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Log("Direct3D's render target is not complete");
            gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
            return;
        }
        gl.BindVertexArray(gProgram.mVertexArray);
        gl.BindBuffer(GL_ARRAY_BUFFER, gProgram.mVertices);
        gl.BufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(mVertices.size() * sizeof(Vertex)), mVertices.data(), GL_STREAM_DRAW);
        gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, gProgram.mIndices);
        gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(mIndices.size() * sizeof(WORD)), mIndices.data(),
                      GL_STREAM_DRAW);
        gl.ProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
        gl.Disable(GL_SCISSOR_TEST);
        int theApplied = -1;
        for (const Command& theCommand : mCommands) {
            const State& theState = mStates[theCommand.mState];
            if (theCommand.mKind == Command::kClear) {
                const RECT& theRect = theCommand.mClearRect;
                gl.Enable(GL_SCISSOR_TEST);
                gl.Scissor(theRect.left, theRect.top, theRect.right - theRect.left, theRect.bottom - theRect.top);
                GLbitfield theMask = 0;
                if (theCommand.mClearFlags & D3DCLEAR_TARGET) {
                    float theColour[4];
                    Colour(theCommand.mClearColour, theColour);
                    if (!theTarget->mFormat.HasAlpha())
                        theColour[3] = 1;
                    gl.ColorMask(1, 1, 1, 1);
                    gl.ClearColor(theColour[0], theColour[1], theColour[2], theColour[3]);
                    theMask |= GL_COLOR_BUFFER_BIT;
                }
                if ((theCommand.mClearFlags & D3DCLEAR_ZBUFFER) && theDepth) {
                    gl.DepthMask(1);
                    gl.ClearDepth(theCommand.mClearZ);
                    theMask |= GL_DEPTH_BUFFER_BIT;
                }
                if (theMask)
                    gl.Clear(theMask);
                gl.Disable(GL_SCISSOR_TEST);
                theApplied = -1;
                continue;
            }
            if (theCommand.mState != theApplied) {
                Apply(theState, theTarget, theDepth);
                gl.BindVertexArray(gProgram.mVertexArray);
                theApplied = theCommand.mState;
            }
            if (theCommand.mIndexed)
                gl.DrawElementsBaseVertex(theCommand.mMode, theCommand.mCount, GL_UNSIGNED_SHORT,
                                          (const void*)(uintptr_t)(theCommand.mFirst * sizeof(WORD)), theCommand.mBase);
            else
                gl.DrawArrays(theCommand.mMode, theCommand.mFirst, theCommand.mCount);
        }
        gl.BindVertexArray(0);
        gl.BindSampler(0, 0);
        gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
        theTarget->mStore.mGpuValid = true;
        theTarget->mStore.mCpuValid = false;
    }

    HRESULT Record(D3DPRIMITIVETYPE inType, const uint8_t* inVertices, const Layout& inLayout, DWORD inAvailable,
                   DWORD inStart, DWORD inCount, const WORD* inIndices, DWORD inIndexCount)
    {
        if (!mInScene)
            return D3DERR_SCENE_NOT_IN_SCENE;
        GLenum theMode;
        switch (inType) {
        case D3DPT_POINTLIST: theMode = GL_POINTS; break;
        case D3DPT_LINELIST: theMode = GL_LINES; break;
        case D3DPT_LINESTRIP: theMode = GL_LINE_STRIP; break;
        case D3DPT_TRIANGLELIST: theMode = GL_TRIANGLES; break;
        case D3DPT_TRIANGLESTRIP: theMode = GL_TRIANGLE_STRIP; break;
        case D3DPT_TRIANGLEFAN: theMode = GL_TRIANGLE_FAN; break;
        default: return DDERR_INVALIDPARAMS;
        }
        if (!inCount || (inIndices && !inIndexCount))
            return D3D_OK;
        mTarget->TakeGdi();
        DWORD theCount = inCount;
        if (inIndices)
            for (DWORD i = 0; i < inIndexCount; ++i)
                theCount = std::max<DWORD>(theCount, inIndices[i] + 1u);
        if (inStart >= inAvailable)
            return DDERR_INVALIDPARAMS;
        theCount = std::min(theCount, inAvailable - inStart);
        Command theCommand = {};
        theCommand.mKind = Command::kDraw;
        theCommand.mState = Snapshot();
        theCommand.mMode = theMode;
        int theBase = (int)mVertices.size();
        mVertices.resize(mVertices.size() + theCount);
        for (DWORD i = 0; i < theCount; ++i) {
            const uint8_t* theIn = inVertices + (size_t)(inStart + i) * inLayout.mStride;
            Vertex& theOut = mVertices[theBase + i];
            memcpy(theOut.mPosition, theIn, 12);
            if (inLayout.mNormal >= 0)
                memcpy(theOut.mNormal, theIn + inLayout.mNormal, 12);
            else
                memset(theOut.mNormal, 0, 12);
            if (inLayout.mDiffuse >= 0)
                memcpy(&theOut.mDiffuse, theIn + inLayout.mDiffuse, 4);
            else
                theOut.mDiffuse = 0xffffffff;
            if (inLayout.mTexel >= 0)
                memcpy(theOut.mTexel, theIn + inLayout.mTexel, 8);
            else
                theOut.mTexel[0] = theOut.mTexel[1] = 0;
        }
        if (inIndices) {
            theCommand.mIndexed = true;
            theCommand.mFirst = (int)mIndices.size();
            theCommand.mCount = (int)inIndexCount;
            theCommand.mBase = theBase;
            mIndices.insert(mIndices.end(), inIndices, inIndices + inIndexCount);
        } else {
            theCommand.mFirst = theBase;
            theCommand.mCount = (int)inCount;
        }
        mCommands.push_back(theCommand);
        return D3D_OK;
    }

public:
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        if (!outObject)
            return E_POINTER;
        *outObject = nullptr;
        if (!IsEqualGUID(inID, IID_IUnknown) && !IsEqualGUID(inID, IID_IDirect3DDevice7))
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
            Submit();
            delete this;
        }
        return ULONG(theCount);
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetCaps(D3DDEVICEDESC7* outDesc) override
    {
        if (!outDesc)
            return DDERR_INVALIDPARAMS;
        Direct3D::Describe(outDesc, mTnL);
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumTextureFormats(LPD3DENUMPIXELFORMATSCALLBACK inCallback,
                                                                     void* inContext) override
    {
        if (!inCallback)
            return DDERR_INVALIDPARAMS;
        static const DWORD kFormats[][5] = {
            {32, 0xff0000, 0xff00, 0xff, 0xff000000}, {32, 0xff0000, 0xff00, 0xff, 0}, {16, 0x0f00, 0x00f0, 0x000f, 0xf000},
            {16, 0xf800, 0x07e0, 0x001f, 0},          {16, 0x7c00, 0x03e0, 0x001f, 0x8000}, {16, 0x7c00, 0x03e0, 0x001f, 0}};
        for (const auto& theRow : kFormats) {
            DDPIXELFORMAT theFormat = {sizeof theFormat};
            theFormat.dwFlags = DDPF_RGB | (theRow[4] ? DDPF_ALPHAPIXELS : 0);
            theFormat.dwRGBBitCount = theRow[0];
            theFormat.dwRBitMask = theRow[1];
            theFormat.dwGBitMask = theRow[2];
            theFormat.dwBBitMask = theRow[3];
            theFormat.dwRGBAlphaBitMask = theRow[4];
            if (inCallback(&theFormat, inContext) == D3DENUMRET_CANCEL)
                break;
        }
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE BeginScene() override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        if (mInScene)
            return D3DERR_SCENE_IN_SCENE;
        mInScene = true;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EndScene() override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        if (!mInScene)
            return D3DERR_SCENE_NOT_IN_SCENE;
        mInScene = false;
        Submit();
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDirect3D(IDirect3D7** outDirect3D) override
    {
        if (!outDirect3D)
            return DDERR_INVALIDPARAMS;
        mDirect3D->AddRef();
        *outDirect3D = mDirect3D;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetRenderTarget(IDirectDrawSurface7* inSurface, DWORD) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        Surface* theSurface = SurfaceOf(inSurface);
        if (!theSurface || theSurface->mFormat.mZ)
            return DDERR_INVALIDPARAMS;
        if (theSurface == mTarget)
            return D3D_OK;
        Submit();
        Target(theSurface);
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetRenderTarget(IDirectDrawSurface7** outSurface) override
    {
        if (!outSurface)
            return DDERR_INVALIDPARAMS;
        mTarget->AddRef();
        *outSurface = mTarget;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Clear(DWORD inCount, D3DRECT* inRects, DWORD inFlags,
                                                        D3DCOLOR inColour, D3DVALUE inZ, DWORD) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        if (inCount && !inRects)
            return DDERR_INVALIDPARAMS;
        const D3DVIEWPORT7& theViewport = mState.mViewport;
        RECT theView = {(LONG)theViewport.dwX, (LONG)theViewport.dwY, (LONG)(theViewport.dwX + theViewport.dwWidth),
                        (LONG)(theViewport.dwY + theViewport.dwHeight)};
        mTarget->TakeGdi();
        Command theCommand = {};
        theCommand.mKind = Command::kClear;
        theCommand.mState = Snapshot();
        theCommand.mClearFlags = inFlags;
        theCommand.mClearColour = inColour;
        theCommand.mClearZ = inZ;
        // Direct3D clears inside the viewport only
        for (DWORD i = 0; i < std::max<DWORD>(inCount, 1); ++i) {
            RECT theRect = theView;
            if (inCount) {
                RECT theAsked = {inRects[i].x1, inRects[i].y1, inRects[i].x2, inRects[i].y2};
                if (!IntersectRect(&theRect, &theAsked, &theView))
                    continue;
            }
            theCommand.mClearRect = theRect;
            mCommands.push_back(theCommand);
        }
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetTransform(D3DTRANSFORMSTATETYPE inType, D3DMATRIX* inMatrix) override
    {
        if (!inMatrix)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        D3DMATRIX* theMatrix = MatrixOf(inType);
        if (!theMatrix)
            return DDERR_INVALIDPARAMS;
        *theMatrix = *inMatrix;
        mStateChanged = true;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetTransform(D3DTRANSFORMSTATETYPE inType, D3DMATRIX* outMatrix) override
    {
        if (!outMatrix)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        D3DMATRIX* theMatrix = MatrixOf(inType);
        if (!theMatrix)
            return DDERR_INVALIDPARAMS;
        *outMatrix = *theMatrix;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE MultiplyTransform(D3DTRANSFORMSTATETYPE inType,
                                                                    D3DMATRIX* inMatrix) override
    {
        if (!inMatrix)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        D3DMATRIX* theMatrix = MatrixOf(inType);
        if (!theMatrix)
            return DDERR_INVALIDPARAMS;
        *theMatrix = Multiply(*inMatrix, *theMatrix);
        mStateChanged = true;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetViewport(D3DVIEWPORT7* inViewport) override
    {
        if (!inViewport)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        if (inViewport->dwX + inViewport->dwWidth > (DWORD)mTarget->mWidth ||
            inViewport->dwY + inViewport->dwHeight > (DWORD)mTarget->mHeight)
            return DDERR_INVALIDPARAMS;
        mState.mViewport = *inViewport;
        mStateChanged = true;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetViewport(D3DVIEWPORT7* outViewport) override
    {
        if (!outViewport)
            return DDERR_INVALIDPARAMS;
        *outViewport = mState.mViewport;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetMaterial(D3DMATERIAL7* inMaterial) override
    {
        if (!inMaterial)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        mState.mMaterial = *inMaterial;
        mStateChanged = true;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetMaterial(D3DMATERIAL7* outMaterial) override
    {
        if (!outMaterial)
            return DDERR_INVALIDPARAMS;
        *outMaterial = mState.mMaterial;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetLight(DWORD inIndex, D3DLIGHT7* inLight) override
    {
        if (!inLight)
            return DDERR_INVALIDPARAMS;
        if (inIndex >= kLights) {
            EE_PORT_UNSUPPORTED("a light index above 7");
            return DDERR_INVALIDPARAMS;
        }
        if (inLight->dltType == D3DLIGHT_SPOT)
            EE_PORT_UNSUPPORTED("a spot light, which lights as a point light");
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        mState.mLights[inIndex] = *inLight;
        mLightSet[inIndex] = true;
        mStateChanged = true;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetLight(DWORD inIndex, D3DLIGHT7* outLight) override
    {
        if (!outLight || inIndex >= kLights || !mLightSet[inIndex])
            return DDERR_INVALIDPARAMS;
        *outLight = mState.mLights[inIndex];
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetRenderState(D3DRENDERSTATETYPE inState, DWORD inValue) override
    {
        if ((DWORD)inState >= 256)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        if (mState.mRender[inState] != inValue) {
            mState.mRender[inState] = inValue;
            mStateChanged = true;
        }
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetRenderState(D3DRENDERSTATETYPE inState, LPDWORD outValue) override
    {
        if (!outValue || (DWORD)inState >= 256)
            return DDERR_INVALIDPARAMS;
        *outValue = mState.mRender[inState];
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE BeginStateBlock() override { return Unsupported("BeginStateBlock"); }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EndStateBlock(LPDWORD) override { return Unsupported("EndStateBlock"); }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE PreLoad(IDirectDrawSurface7*) override { return D3D_OK; }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawPrimitive(D3DPRIMITIVETYPE inType, DWORD inFVF, void* inVertices,
                                                                DWORD inCount, DWORD) override
    {
        Layout theLayout;
        if (!inVertices || !LayoutOf(inFVF, &theLayout))
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        return Record(inType, (const uint8_t*)inVertices, theLayout, inCount, 0, inCount, nullptr, 0);
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawIndexedPrimitive(D3DPRIMITIVETYPE inType, DWORD inFVF,
                                                                       void* inVertices, DWORD inCount, WORD* inIndices,
                                                                       DWORD inIndexCount, DWORD) override
    {
        Layout theLayout;
        if (!inVertices || !inIndices || !LayoutOf(inFVF, &theLayout))
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        return Record(inType, (const uint8_t*)inVertices, theLayout, inCount, 0, inCount, inIndices, inIndexCount);
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetClipStatus(D3DCLIPSTATUS*) override { return Unsupported("SetClipStatus"); }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetClipStatus(D3DCLIPSTATUS*) override { return Unsupported("GetClipStatus"); }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawPrimitiveStrided(D3DPRIMITIVETYPE, DWORD,
                                                                       D3DDRAWPRIMITIVESTRIDEDDATA*, DWORD,
                                                                       DWORD) override
    {
        return Unsupported("DrawPrimitiveStrided");
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE, DWORD,
                                                                              D3DDRAWPRIMITIVESTRIDEDDATA*, DWORD,
                                                                              WORD*, DWORD, DWORD) override
    {
        return Unsupported("DrawIndexedPrimitiveStrided");
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawPrimitiveVB(D3DPRIMITIVETYPE inType, IDirect3DVertexBuffer7* inBuffer,
                                                                  DWORD inStart, DWORD inCount, DWORD) override
    {
        VertexBuffer* theBuffer = static_cast<VertexBuffer*>(inBuffer);
        if (!theBuffer)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        return Record(inType, theBuffer->mData.data(), theBuffer->mLayout, Capacity(theBuffer), inStart, inCount,
                      nullptr, 0);
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE inType,
                                                                         IDirect3DVertexBuffer7* inBuffer, DWORD inStart,
                                                                         DWORD inCount, WORD* inIndices,
                                                                         DWORD inIndexCount, DWORD) override
    {
        VertexBuffer* theBuffer = static_cast<VertexBuffer*>(inBuffer);
        if (!theBuffer || !inIndices)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        return Record(inType, theBuffer->mData.data(), theBuffer->mLayout, Capacity(theBuffer), inStart, inCount,
                      inIndices, inIndexCount);
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ComputeSphereVisibility(D3DVECTOR*, D3DVALUE*, DWORD, DWORD,
                                                                          DWORD*) override
    {
        return Unsupported("ComputeSphereVisibility");
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetTexture(DWORD inStage, IDirectDrawSurface7** outSurface) override
    {
        if (!outSurface || inStage >= kStages)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        *outSurface = mTextures[inStage];
        if (mTextures[inStage])
            mTextures[inStage]->AddRef();
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetTexture(DWORD inStage, IDirectDrawSurface7* inSurface) override
    {
        if (inStage >= kStages)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        Surface* theSurface = SurfaceOf(inSurface);
        if (inSurface && !theSurface)
            return DDERR_INVALIDPARAMS;
        if (theSurface == mTextures[inStage])
            return D3D_OK;
        if (theSurface)
            theSurface->AddRef();
        if (mTextures[inStage])
            mTextures[inStage]->Release();
        mTextures[inStage] = theSurface;
        if (inStage == 0)
            mStateChanged = true;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetTextureStageState(DWORD inStage, D3DTEXTURESTAGESTATETYPE inType,
                                                                       LPDWORD outValue) override
    {
        if (!outValue || inStage >= kStages || (DWORD)inType >= 32)
            return DDERR_INVALIDPARAMS;
        *outValue = mState.mStage[inStage][inType];
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetTextureStageState(DWORD inStage, D3DTEXTURESTAGESTATETYPE inType,
                                                                       DWORD inValue) override
    {
        if (inStage >= kStages || (DWORD)inType >= 32)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        DWORD* theStage = mState.mStage[inStage];
        if (inType == D3DTSS_ADDRESS)
            theStage[D3DTSS_ADDRESSU] = theStage[D3DTSS_ADDRESSV] = inValue;
        if (inStage > 0 && inType == D3DTSS_COLOROP && inValue != D3DTOP_DISABLE)
            EE_PORT_UNSUPPORTED("texture stages after the first");
        theStage[inType] = inValue;
        mStateChanged = true;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ValidateDevice(LPDWORD outPasses) override
    {
        if (outPasses)
            *outPasses = 1;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ApplyStateBlock(DWORD) override { return Unsupported("ApplyStateBlock"); }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CaptureStateBlock(DWORD) override { return Unsupported("CaptureStateBlock"); }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DeleteStateBlock(DWORD) override { return Unsupported("DeleteStateBlock"); }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateStateBlock(D3DSTATEBLOCKTYPE, LPDWORD) override
    {
        return Unsupported("CreateStateBlock");
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Load(IDirectDrawSurface7*, POINT*, IDirectDrawSurface7*, RECT*,
                                                       DWORD) override
    {
        return Unsupported("Load");
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE LightEnable(DWORD inIndex, BOOL inEnable) override
    {
        if (inIndex >= kLights)
            return DDERR_INVALIDPARAMS;
        std::lock_guard<std::recursive_mutex> theGuard(gLock);
        if (inEnable && !mLightSet[inIndex]) {
            // Direct3D enables a light that was never set with its default: white, directional, along +z
            D3DLIGHT7& theLight = mState.mLights[inIndex];
            memset(&theLight, 0, sizeof theLight);
            theLight.dltType = D3DLIGHT_DIRECTIONAL;
            theLight.dcvDiffuse.r = theLight.dcvDiffuse.g = theLight.dcvDiffuse.b = 1;
            theLight.dvDirection.z = 1;
            mLightSet[inIndex] = true;
        }
        mState.mLightOn[inIndex] = inEnable != FALSE;
        mStateChanged = true;
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetLightEnable(DWORD inIndex, BOOL* outEnable) override
    {
        if (!outEnable || inIndex >= kLights || !mLightSet[inIndex])
            return DDERR_INVALIDPARAMS;
        *outEnable = mState.mLightOn[inIndex];
        return D3D_OK;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetClipPlane(DWORD, D3DVALUE*) override { return Unsupported("SetClipPlane"); }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetClipPlane(DWORD, D3DVALUE*) override { return Unsupported("GetClipPlane"); }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetInfo(DWORD, void*, DWORD) override { return S_FALSE; }

private:
    bool mLightSet[kLights] = {};

    static DWORD Capacity(const VertexBuffer* inBuffer) { return inBuffer->mDesc.dwNumVertices + kVertexSlack; }

    D3DMATRIX* MatrixOf(D3DTRANSFORMSTATETYPE inType)
    {
        switch (inType) {
        case D3DTRANSFORMSTATE_WORLD: return &mState.mWorld;
        case D3DTRANSFORMSTATE_VIEW: return &mState.mView;
        case D3DTRANSFORMSTATE_PROJECTION: return &mState.mProjection;
        default: return nullptr;
        }
    }

    static HRESULT Unsupported(const char* inName)
    {
        (void)inName;
        EE_PORT_UNSUPPORTED("an IDirect3DDevice7 method the game does not call");
        return DDERR_UNSUPPORTED;
    }
};

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Direct3D::CreateDevice(REFCLSID inKind, IDirectDrawSurface7* inTarget,
                                                 IDirect3DDevice7** outDevice)
{
    if (!outDevice)
        return DDERR_INVALIDPARAMS;
    *outDevice = nullptr;
    std::lock_guard<std::recursive_mutex> theGuard(gLock);
    Surface* theTarget = SurfaceOf(inTarget);
    if (!theTarget || theTarget->mFormat.mZ || !theTarget->IsRenderable())
        return DDERR_INVALIDPARAMS;
    bool theTnL = IsEqualGUID(inKind, IID_IDirect3DTnLHalDevice);
    if (!theTnL && !IsEqualGUID(inKind, IID_IDirect3DHALDevice) && !IsEqualGUID(inKind, IID_IDirect3DRGBDevice))
        return DDERR_INVALIDPARAMS;
    *outDevice = new Device(this, theTarget, theTnL);
    return D3D_OK;
}

} // namespace

void SubmitDraws(Surface* inSurface)
{
    if (gSubmitting)
        return;
    bool theUsed = !inSurface || inSurface->TextureRoot()->mRecordedUses > 0;
    std::vector<Device*> theDevices(gDevices.begin(), gDevices.end());
    for (Device* theDevice : theDevices)
        if (!theDevice->mCommands.empty() &&
            (theUsed || theDevice->mTarget == inSurface || theDevice->mTarget->mZ == inSurface))
            theDevice->Submit();
}

HRESULT NewDirect3D(IDirectDraw7* inOwner, void** outObject)
{
    *outObject = static_cast<IDirect3D7*>(new Direct3D(inOwner));
    return S_OK;
}

} // namespace dx
} // namespace eeport
