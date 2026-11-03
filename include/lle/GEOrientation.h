// C:\EEX\Low-Level Engine\Graphics Engine\GEOrientation.h (RECONSTRUCTION, minimal: what GETransformation's
// orientation overloads read). Yaw/pitch/roll order from GETransformation::Rotate(const GEOrientation&) 0xd828.
#ifndef LLE_GEORIENTATION_H
#define LLE_GEORIENTATION_H

class FSFile;

class LLE_API GEOrientation {               // sizeof 0xc
public:
#ifdef LLE_EXE_HEADERS
    // The game's view: all inline, like GEVector (whose bodies the engine's are ICF-folded with). The exe imports no
    // GEOrientation ctor/dtor; 0x1582d6 default-constructs a local inline (three fldz/fstp) and assigns it with
    // inline field moves (a user-written operator=), while 12 sites call __imp_??4GEOrientation (the same inline,
    // refused by the budget). Spelling of the copy ctor/dtor provisional until an exe unit copies/destroys one.
    GEOrientation() : mYaw(0.0f), mPitch(0.0f), mRoll(0.0f) {}
    GEOrientation(float inYaw, float inPitch, float inRoll) : mYaw(inYaw), mPitch(inPitch), mRoll(inRoll) {}
    GEOrientation(const GEOrientation& inOrientation)
        : mYaw(inOrientation.mYaw), mPitch(inOrientation.mPitch), mRoll(inOrientation.mRoll) {}
    ~GEOrientation() {}
    GEOrientation& operator=(const GEOrientation& inOrientation)
        { this->mYaw = inOrientation.mYaw; this->mPitch = inOrientation.mPitch; this->mRoll = inOrientation.mRoll; return *this; }
#else
    // out of line in GEOrientation.cpp; every body is ICF-folded with GEVector's (ctors 0xc240/0xd56f, the copy
    // 0xd588), the empty dtor 0xc16a and the float setters 0xc56c/0xc576/0x54ae4
    GEOrientation();
    GEOrientation(float inYaw, float inPitch, float inRoll);
    GEOrientation(const GEOrientation& inOrientation);
    ~GEOrientation();
#if defined(LLE_CURRENT_HEADERS) && !defined(LOWLEVELENGINE_EXPORTS)
    // the exe calls __imp_??4GEOrientation@@QAEAAV0@ABV0@@Z at 12 sites (0x15d617: a temporary built inline, then
    // the call); an implicit operator= of this class inlines (probe). Engine export 0xd588 (ICF with GEVector's).
    GEOrientation& operator=(const GEOrientation& inOrientation);
#endif
#endif
    float GetYaw() const { return this->mYaw; }  // exported 0x10011ec0 (ICF `fld [ecx]; ret`)
    float GetPitch() const { return this->mPitch; }
    float GetRoll() const { return this->mRoll; }
    // GEOrientation.cpp (0x10020d20..0x10020e09); Load is ICF-folded with GEVector::Load (kept at 0x1000dc23)
    long Load(FSFile& inFile);
    void Normalize();
    long Save(FSFile& inFile);
#ifdef LLE_EXE_HEADERS
    void SetPitch(float inPitch) { this->mPitch = inPitch; }
    void SetRoll(float inRoll) { this->mRoll = inRoll; }
    void SetYaw(float inYaw) { this->mYaw = inYaw; }
#else
    void SetPitch(float inPitch);
    void SetRoll(float inRoll);
    void SetYaw(float inYaw);
#endif

    float mYaw;                             // +0x00
    float mPitch;                           // +0x04
    float mRoll;                            // +0x08
};

#endif
