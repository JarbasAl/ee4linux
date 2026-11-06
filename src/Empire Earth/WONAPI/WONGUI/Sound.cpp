
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/EELobbySound.h"
namespace WONAPI {
// Native329810/25, direct default initialization, support0.
SoundDescriptor::SoundDescriptor() { mL00=0; }
// Native329830/341 including complete std::string copy expansion, support0.
SoundDescriptor::SoundDescriptor(const std::string& path,long flags) :mPath(path)
{
    mL00=flags;
}
// Native329990/36 flag setter, support0.
// FUNCTION: WON 0x00729990
void SoundDescriptor::SetSoundFlags(int flags,bool on)
{
    if(on) mL00|=flags;
    else mL00&=~flags;
}
// Native3299c0/378, direct complete descriptor-field copy and default state, support0.
Sound::Sound(const SoundDescriptor& descriptor)
    :mL08(descriptor.mL00),mPath(descriptor.mPath)
{
    mIsLooping=false;
}
// Native329bd0/21, public play gate wrapper, support0.
void Sound::Fn329BD0()
{
    if((mL08&4)==0) PlayHook();
}
// FUNCTION: WON 0x00729bf0
void Sound::Fn329BF0(bool muted)
{
    if(muted==((mL08&4)!=0)) return;
    if(muted) mL08|=4;
    else mL08&=~4;
    if(muted) Stop();
}
}

// Genuine SDK Sound.cpp public APIs. Existing complete Sound20 receiver and virtual hooks.
void WONAPI::Sound::Stop()
{
    StopHook();
}
void WONAPI::Sound::Resume()
{
    ResumeHook();
}

// Genuine SDK Sound.cpp GetMuted; native329c20/9. Existing flags08 and SoundFlag_Muted4.
bool WONAPI::Sound::GetMuted()
{
    return (mL08&4)!=0;
}

namespace WONAPI {


SoundDescriptor::SoundDescriptor(const char *thePath) : mPath(thePath)
{
	mL00 = 0;
}


void Sound::Rewind()
{
	RewindHook();
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00729810
// (public: __thiscall WONAPI::SoundDescriptor::SoundDescriptor(void)    [??0SoundDescriptor@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x00729b60
// (public: virtual __thiscall WONAPI::Sound::~Sound(void)    [??1Sound@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00729b40
// (public: virtual void * __thiscall WONAPI::Sound::`scalar deleting destructor'(unsigned int)    [??_GSound@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00729830
// (public: __thiscall WONAPI::SoundDescriptor::SoundDescriptor(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > co)
// TEMPLATE: WON 0x007299c0
// (public: __thiscall WONAPI::Sound::Sound(struct WONAPI::SoundDescriptor const &)    [??0Sound@WONAPI@@QAE@ABUSoundDescriptor@1@@Z])
