// Low-Level Engine\Utilities\UFtol.cpp (file name inferred: no __FILE__ string; the engine links it between
// UEncryption.cpp and UGraph.cpp, so its name sorts between those two)
// UNIT-FLAGS: -Ox -G6 -MD -GR
// No /Gy (-Ox, or -Od: the asm-only body is the same with either): as a COMDAT (-O1 implies /Gy) __ftol changes
// which copy LINK's /OPT:ICF keeps elsewhere in the module (DX7HRDisplay's operator= pair 0x11de moves to Draw2D);
// as a plain .text section the relink keeps the original's representative.
//
// A fast __ftol that replaces the CRT's: round with fistp, then correct the rounded value towards zero. The same
// 83-byte object is linked into the engine and into TCP Protocol, Default, DX7HRDisplay and DX7HRTnLDisplay right
// after their own units (byte-identical in all five). It is a C++ object of the game's compiler, not a MASM one: with
// it the relinked TCP Protocol.dll's Rich header has the original's three Utc12_CPP 8168 objects (unit, ti_inst, this)
// and one Masm613 object (msvcrt.lib's dllsupp). The body is inline asm because the original's is: a naked function
// with no frame, written in x87 asm (the labels follow the CRT's ftol.asm naming).

// FUNCTION: LLE 0x10057218
// FUNCTION: TCP 0x10004551
// FUNCTION: DEFAULT 0x10018af3
// FUNCTION: DX7HR 0x1000fc91
// FUNCTION: DX7HRTNL 0x10010512
// FUNCTION: EXE 0x548291
// Converts the float on the FPU stack to an integer, rounding towards zero (fast replacement for the CRT's)
#if defined(__x86_64__)
// A 64-bit build compiles float-to-integer conversions itself (cvttss2si), so the CRT's _ftol has no caller here.
extern "C" void _ftol()
{
}
#else
extern "C" __declspec(naked) void _ftol()
{
    __asm
    {
        fld     st(0)
        fistp   qword ptr [esp - 8]
        fild    qword ptr [esp - 8]
        mov     edx, dword ptr [esp - 4]
        mov     eax, dword ptr [esp - 8]
        test    eax, eax
        je      integer_QnaN_or_zero
    arg_is_not_integer_QnaN:
        fsubp   st(1), st
        test    edx, edx
        jns     positive
        fstp    dword ptr [esp - 8]
        mov     ecx, dword ptr [esp - 8]
        xor     ecx, 80000000h
        add     ecx, 7fffffffh
        adc     eax, 0
        ret
    positive:
        fstp    dword ptr [esp - 8]
        mov     ecx, dword ptr [esp - 8]
        add     ecx, 7fffffffh
        sbb     eax, 0
        ret
    integer_QnaN_or_zero:
        test    edx, 7fffffffh
        jne     arg_is_not_integer_QnaN
        fstp    st(0)
        fstp    st(0)
        ret
    }
}
#endif // __x86_64__
