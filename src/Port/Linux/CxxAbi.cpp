// The C++ runtime of the native build: the one place where MSVC's and the Itanium ABI's behaviour differ in a way the
// game can see.
//
// MSVC judges a dynamic_cast by the type in the object's own vtable. The Itanium ABI, which this build uses, also
// requires that the object derive from the pointer's static type, so a cast from a pointer to a class the object does
// not derive from fails where MSVC's succeeds. The game stores the RTSDummyUnit heads of its unit lists in RTSUnit
// pointers and casts them back (RTSUnit::Link and its callers): a cast the Itanium ABI refuses therefore succeeds
// when the object's own type is the destination type, as it does in the original image.
//
// tools/build_port.py and tools/test_port_linux.py link with --wrap=__dynamic_cast, which sends every cast of an image
// that holds this library here.
#include <cstddef>
#include <typeinfo>

extern "C" void* __real___dynamic_cast(const void* inPointer, const std::type_info* inSource,
                                       const std::type_info* inTarget, ptrdiff_t inOffset);
extern "C" void* __wrap___dynamic_cast(const void* inPointer, const std::type_info* inSource,
                                       const std::type_info* inTarget, ptrdiff_t inOffset)
{
    void* theResult = __real___dynamic_cast(inPointer, inSource, inTarget, inOffset);
    if (theResult || !inPointer || !inTarget)
        return theResult;
    const void* theVtable = *reinterpret_cast<const void* const*>(inPointer);
    if (!theVtable)
        return nullptr;
    const std::type_info* theType = reinterpret_cast<const std::type_info* const*>(theVtable)[-1];
    return theType && *theType == *inTarget ? const_cast<void*>(inPointer) : nullptr;
}
