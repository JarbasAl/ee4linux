// C:\EEX\Low-Level Engine\Graphics Engine\GEMovie.h (RECONSTRUCTION). A movie played by a rasterizer plug-in
// (GERasterizer::CreateMovie). Abstract: every slot after the dtor is pure (vtable 0x10097ae8); the inline members are
// emitted by GEBoundingBox.cpp (ctor 0x10011be5, copy ctor 0x10011bf5). Slot names and signatures are proven by the
// overrides DX7HRDisplay.dll exports (DX7Movie vtable 0x10568: ?GetFilename@DX7Movie@@UBE?AVUString@@XZ, ...); UIMovie
// calls slot 0x0c (CopyData), 0x18 (DrawTextA) and 0x1c (dtor). sizeof 8.
#ifndef LLE_GEMOVIE_H
#define LLE_GEMOVIE_H

class LLE_API GEMovie : public UReferenceCounter {
public:
    virtual UString GetFilename() const = 0;    // slot 0x04
    virtual bool GetIsLooping() const = 0;      // slot 0x08
    virtual bool GetIsPlaying() const = 0;      // slot 0x0c
    virtual bool GetIsReady() const = 0;        // slot 0x10
    virtual bool GetJustStarted() const = 0;    // slot 0x14
    virtual long RenderFrame() = 0;             // slot 0x18
    virtual void Stop() = 0;                    // slot 0x1c
protected:
    GEMovie() {}
    virtual ~GEMovie() {}
};

#endif
