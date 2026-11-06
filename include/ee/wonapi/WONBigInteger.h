// Complete original-backed BigInteger20 storage/lifetime contract only.
// No mathematics implementation or source breadth credit from this declaration.
#ifndef EE_WONAPI_WONBIGINTEGER_H
#define EE_WONAPI_WONBIGINTEGER_H
#include "ee/wonapi/WONBlowfish.h"
namespace WONAPI {
class Random; //actual SDK random-bits constructor parameter, no owner field
class BigInteger {
public:
    typedef unsigned long word;
    typedef unsigned __int64 dword;
protected:
    mutable word stackN[4]; //04..14, actual inline digit array
    mutable word* n; //14, owner iff distinct from stackN
    mutable unsigned short nlen; //18 allocation capacity
    mutable unsigned short len; //1a significant length
    bool negative; //1c; natural1d..20 alignment
    void NewBuffer(int,const word* = NULL,int = 0) const; //3d3230
    void Init(); //3d32b0 support0
    static const int BITS; //actual pinned SDK constant32, no object layout effect
public:
    void fromBinary(const RawBuffer&); //3d3530, actual ret4, borrowed const byte-string
    int toBinary(RawBuffer&) const; //3d3380, actual ret4
    static int st_bitLength(const BigInteger&); //3d2ff0
    static RawBuffer st_randomBits(int,Random&); //3d3020 actual native random-byte mask algorithm
    static void st_setBit(BigInteger&,int); //3d31b0 actual source primitive setter support0
    static void st_gcd(BigInteger&,const BigInteger&,const BigInteger&); //3d4fa0 actual ordered Euclid dispatch support0
    static int AddWords(word*,const word*,const word*,int,int); //3d3650 cdecl
    static word SubWords(word*,const word*,const word*,int,int); //3d3710 cdecl
    static void MultiplyWords(word*,const word*,const word*,int,int); //3d37c0 cdecl
    static void MultiplyWord(word*,const word*,word,int); //3d38c0 cdecl
    static int CompareWords(const word*,const word*,int,int); //3d3920 cdecl
    static void KaratsubaMultiplyWords(word*,word*,const word*,const word*,int,int,int); //3d3990 cdecl
    static int FindSize(const word*,int); //3d3c10 cdecl
    static void st_copy(BigInteger&,const BigInteger&); //3d3310 support0
    static void st_grow(const BigInteger&,int,bool copyOld=true); //3d3340
    static bool st_bit(const BigInteger&,int); //3d3160 original external bit test
    static void st_mul(BigInteger&,const BigInteger&,const BigInteger&); //3d4120
    static void st_kmul(BigInteger&,const BigInteger&,const BigInteger&,word* = NULL); //3d4500
    static void st_mod(BigInteger&,const BigInteger&,const BigInteger&); //3d4740
    static void st_div(BigInteger*,BigInteger*,const BigInteger&,const BigInteger&); //3d4960
    static void st_modExp(BigInteger&,const BigInteger&,const BigInteger&,const BigInteger&,const BigInteger&,short); //3d4c90 actual6args
    static int st_recip(BigInteger&,const BigInteger&); //3d4dd0
    static void st_exp(BigInteger&,const BigInteger&,const BigInteger&);
    static void st_modExp(BigInteger&,const BigInteger&,const BigInteger&,const BigInteger&);
    static void st_div(BigInteger&,const BigInteger&,const BigInteger&);
    static void st_euclid(BigInteger&,const BigInteger&,const BigInteger&); //3d4e60
    static void st_modMulRecip(BigInteger&,const BigInteger&,const BigInteger&,const BigInteger&,const BigInteger&,short,word* = NULL); //3d4fe0
    static void st_inverseModN(BigInteger&,const BigInteger&,const BigInteger&); //3d5270
    static void st_CopyBuffer(word*,const word*,size_t); //3d31f0 actual external memcpy/memmove helper
    static void st_zero(BigInteger&); //3d3610 actual external fieldinitializer support0
    static void st_one(BigInteger&); //3d3630 actual external fieldinitializer support0
    static void st_add(BigInteger&,const BigInteger&,const BigInteger&); //3d3c30
    static void st_sub(BigInteger&,const BigInteger&,const BigInteger&); //3d3cc0
    static int st_cmp(const BigInteger&,const BigInteger&); //3d3d80
    static int st_ucmp(const BigInteger&,const BigInteger&); //3d3dd0 external compare forwarder support0
    static void st_shiftLeft(BigInteger&,const BigInteger&,int); //3d3e00
    static void st_shiftRight(BigInteger&,const BigInteger&,int); //3d3f10
    static void st_add_unsigned(BigInteger&,const BigInteger&,const BigInteger&); //3d4000
    static void st_sub_unsigned(BigInteger&,const BigInteger&,const BigInteger&,bool determine_sign=true); //3d4070
    int bitLength() const; //3d59b0, actual int inEAX/ret0
    int byteLength() const; //3d2fd0, genuine SDK external byte-length helper
    bool testBit(int); //3d58e0 actual external bit helper
    BigInteger& setBit(int); //3d5900 actual external bit setter
    BigInteger& shiftRight(int); //3d5cb0 actual external in-place shift
    BigInteger gcd(const BigInteger&) const; //3d5920 actual hiddenresult+arg RET8
    BigInteger sub(const BigInteger&) const; //3d5c00 actual hiddenresult+arg RET8
    BigInteger add(const BigInteger&) const; //3d5b70 actual hiddenresult+arg RET8
    BigInteger mod(const BigInteger&) const; //3d5ae0 actual hiddenresult+arg RET8
    BigInteger inverseModN(const BigInteger&) const; //3d5a50 actual hiddenresult+arg RET8
    BigInteger modExp(const BigInteger&,const BigInteger&,const BigInteger&,short) const; //3d59c0 actualRET14
    BigInteger modMulRecip(const BigInteger&,const BigInteger&,const BigInteger&,short) const; //3d5d80 actualRET14
    bool equals(const BigInteger&) const; //3d5cd0 actualRET4
    int cmp(const BigInteger&) const; //3d5c90 actualRET4
    static const BigInteger ZERO; //actual original-only owner566bc8, ctor3d56d0(0), no new definition
    static const BigInteger ONE; //actual original-only owner566ba8, ctor3d5720(1), no new definition
    BigInteger recip(int&) const; //3d5cf0, actual hidden full20 result +int outputref/ret8
    virtual void debugInitStr() {} //native slot0/326b60 actual empty
    virtual void initStr() {} //native slot4/326b60 actual empty
    virtual ~BigInteger(); //3d32d0 actual free(n) iff n!=stackN, deleting thunk3d32f0 slot8
    BigInteger(); //3d5760, Init3d32b0/NewBuffer3d3230 establish pointer and lengths
    BigInteger(int); //3d57b0 actual signed32 initializer/support0
    BigInteger(const RawBuffer&); //3d5800, actual native full20 owner constructorret4
    BigInteger(int,Random&); //3d5820 actual full20 random-bits constructorRET8
    BigInteger(const BigInteger&); //3d5780, initializes then st_copy3d3310 (real deep-copy owner contract)
    const BigInteger& operator=(const BigInteger&); //3d58c0, st_copy3d3310 then returns this; no shallow pointer copy
    BigInteger(const std::string&);
    void fromString(const std::string&);
    std::string toString() const;
    BigInteger modExp(const BigInteger&,const BigInteger&) const;
    BigInteger mul(const BigInteger&) const;
    BigInteger div(const BigInteger&) const;
    BigInteger divrem(const BigInteger&,BigInteger&) const;
    BigInteger negate() const;
    BigInteger& shiftLeft(int);
    bool operator<(const BigInteger&) const;

};
}
#endif
