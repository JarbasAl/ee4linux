// Genuine used BigInteger lifetime/API bodies and bounded random-bit generation.

// ZERO/ONE remain external original-owned objects: no definitions or startup code here.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <algorithm>
#include <string.h>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONBigInteger.h"
#include "ee/wonapi/WONRandom.h"
namespace WONAPI {
// FUNCTION: WON 0x007d3020
RawBuffer BigInteger::st_randomBits(int numBits, Random &rndSrc)
{		
	int numBytes = (numBits+7)/8;
	RawBuffer randomBits(numBytes,(unsigned char)0);

	// Generate random bytes and mask out any excess bits
	if (numBytes > 0) 
	{
		rndSrc.nextBytes(randomBits);
		int excessBits = 8*numBytes - numBits;
		randomBits[0] &= (1 << (8-excessBits)) - 1;
	}
	return randomBits;
}

// FUNCTION: WON 0x007d2fd0
int BigInteger::byteLength() const
{
	return ((st_bitLength(*this) + 7) / 8);
}

// FUNCTION: WON 0x007d3160
bool BigInteger::st_bit(const BigInteger &n, int i)
{
    int bit = i % 32;
    i /= 32;

    if (i >= n.len || ((n.n[i] & (1L << bit)) == 0))
        return false;
	
    return true;
}

// FUNCTION: WON 0x007d31b0
void BigInteger::st_setBit(BigInteger &n, int i)
{
    int bit = i % 32;
    i /= 32;

	
    if (i >= n.len)
		return;
	
	n.n[i] |= (1L << bit);				
}

// FUNCTION: WON 0x007d31f0
void BigInteger::st_CopyBuffer(word *dest, const word *src, size_t count)
{
	if(dest!=src)
		memcpy(dest,src,sizeof(word)*count);
	else
		memmove(dest,src,sizeof(word)*count);
}

// FUNCTION: WON 0x007d32d0
BigInteger::~BigInteger()
{
	if(n!=stackN)
		delete[] n;
}

// FUNCTION: WON 0x007d3610
void BigInteger::st_zero(BigInteger &a)
{
    a.n[0] = 0;
    a.negative = false;
    a.len = 0;
}

// FUNCTION: WON 0x007d3630
void BigInteger::st_one(BigInteger &a)
{
    a.n[0] = 1;
    a.negative = false;
    a.len = 1;
}

// FUNCTION: WON 0x007d3dd0
int BigInteger::st_ucmp(const BigInteger &a, const BigInteger &b)
{
    return CompareWords(a.n,b.n,a.len,b.len);
/*	int alen = a.len;
    int blen = b.len;

    if (alen < blen) return -1;
    if (alen > blen) return 1;

    word *an = a.n;
    word *bn = b.n;

    for (int i = alen-1; i >= 0; --i)
    {
        if (an[i] < bn[i]) return -1;
        if (an[i] > bn[i]) return 1;
    }
    return 0;*/
}

// FUNCTION: WON 0x007d4fa0
void BigInteger::st_gcd(BigInteger &r, const BigInteger &a, const BigInteger &b)
{
    if (st_cmp(a, b) > 0)
        st_euclid(r, a, b);
    else
        st_euclid(r, b, a);
}

// FUNCTION: WON 0x007d5760
BigInteger::BigInteger()
{
	Init();
}

// FUNCTION: WON 0x007d5780
BigInteger::BigInteger(const BigInteger &theVal)
{
	Init();
	st_copy(*this,theVal);
}

// FUNCTION: WON 0x007d57b0
BigInteger::BigInteger(int theFrom)
{
	Init();

	negative = theFrom<0;
	if(negative)
		n[0] = theFrom*-1;
	else
		n[0] = theFrom;

	len = 1;
	if(theFrom==0)
		len = 0;
}

// FUNCTION: WON 0x007d5800
BigInteger::BigInteger(const RawBuffer &buffer)
{
	Init();
    fromBinary(buffer);
}

// FUNCTION: WON 0x007d5820
BigInteger::BigInteger(int numBits, Random &rndSrc)
{
	Init();
	fromBinary(st_randomBits(numBits,rndSrc));
}

// FUNCTION: WON 0x007d58c0
const BigInteger& BigInteger::operator =(const BigInteger& theVal)
{
	st_copy(*this,theVal);
	return *this;
}

// FUNCTION: WON 0x007d58e0
bool BigInteger::testBit(int theBit)
{
	return st_bit(*this,theBit);
}

// FUNCTION: WON 0x007d5900
BigInteger& BigInteger::setBit(int theBit)
{
	st_setBit(*this,theBit);
	return *this;
}

// FUNCTION: WON 0x007d5920
BigInteger BigInteger::gcd(const BigInteger &b) const
{
	BigInteger r;
	st_gcd(r,*this,b);

	return r;
}

// FUNCTION: WON 0x007d59b0
int BigInteger::bitLength() const
{ 
	return st_bitLength(*this); 
}

// FUNCTION: WON 0x007d59c0
BigInteger BigInteger::modExp(const BigInteger &power, const BigInteger &modulo, const BigInteger &recip, short nb) const
{
	BigInteger r;
	st_modExp(r, *this, power, modulo,recip,nb);

    return r;
}

// FUNCTION: WON 0x007d5a50
BigInteger BigInteger::inverseModN(const BigInteger &n) const
{
	BigInteger r;

	st_inverseModN(r,*this, n);

    return r;
}

// FUNCTION: WON 0x007d5ae0
BigInteger BigInteger::mod(const BigInteger &b) const
{
	BigInteger r;

	st_mod(r, *this, b);
   
	return r;
}

// FUNCTION: WON 0x007d5b70
BigInteger BigInteger::add(const BigInteger &b) const
{
	BigInteger r;
	
	st_add(r, *this, b);
    return r;
}

// FUNCTION: WON 0x007d5c00
BigInteger BigInteger::sub(const BigInteger &b) const
{
	BigInteger r;
	st_sub(r, *this, b);

    return r;
}

// FUNCTION: WON 0x007d5c90
int BigInteger::cmp(const BigInteger &a) const
{
    return st_cmp(*this, a);
}

// FUNCTION: WON 0x007d5cb0
BigInteger& BigInteger::shiftRight(int n)
{
	st_shiftRight(*this, *this, (short) n);
    return *this;
}

// FUNCTION: WON 0x007d5cd0
bool BigInteger::equals(const BigInteger &b) const
{
    return (cmp(b) == 0);
}

// FUNCTION: WON 0x007d5cf0
BigInteger BigInteger::recip(int &nb) const
{
	BigInteger r;

	nb = st_recip(r,*this);
	return r;
}

// FUNCTION: WON 0x007d5d80
BigInteger BigInteger::modMulRecip(const BigInteger &y, const BigInteger &m, const BigInteger &i, short nb) const
{
	BigInteger r;

	st_modMulRecip(r,*this,y,m,i,nb);
	
	return r;
}

}

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
namespace WONAPI {
using namespace std;
// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
void BigInteger::fromString(const std::string& theString)
{

	st_zero(*this);

	int aPos = 0;
	while(aPos<theString.length() && isspace(theString[aPos]))
		aPos++;

	int lastPos = aPos;
	while(lastPos<theString.length() && isdigit(theString[lastPos]))
		lastPos++;

	string aStr = theString.substr(aPos,lastPos-aPos);


	BigInteger thousand(1000);
	BigInteger r1(0), r2(0);

	if(aStr.size()%3!=0)
	{
		string aSubStr = aStr.substr(0,aStr.size()%3);
		r1 = r2 = atoi(aSubStr.c_str());
		aPos+=aStr.size()%3;
	}
	
	while(aPos<aStr.length())
	{
		string aSubStr = theString.substr(aPos,3);

		
		BigInteger anInt = atoi(aSubStr.c_str());

		st_mul(r1,thousand,r2);
		st_add(r1,r1,anInt);

		r2 = r1;
		aPos+=3;
	}

	st_copy(*this,r1);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
std::string BigInteger::toString() const
{
	string s;

	BigInteger N(1000);
	BigInteger r;
	BigInteger tmp = *this;

	char aBuf[20];

	while(!tmp.equals(BigInteger::ZERO))
	{
		tmp = tmp.divrem(N,r);
		sprintf(aBuf,"%03d",r.n[0]);
		s=aBuf+s;
	}


/*
    RawBuffer buffer = toByteArray();

	for(int i=buffer.length()-1; i>=0; i--)
	{
		int low = buffer[i] & 0x0f;
		int high = (buffer[i] & 0xf0) >> 4;
	
		s+=IntToHex(high);
		s+=IntToHex(low);
	}*/

    // remove leading 0's.
    int pos = 0, len = s.length();
    while ((pos < len) && (s[pos] == '0'))
        pos++;

    if (pos == len) 
	{
        return "0";
    }
	else 
	{	
        return s.substr(pos);
    }		
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
BigInteger::BigInteger(const std::string &theString)
{
	Init();
	fromString(theString);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
BigInteger BigInteger::modExp(const BigInteger &power, const BigInteger &modulo) const
{
	BigInteger r;
	st_modExp(r, *this, power, modulo);

    return r;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
BigInteger BigInteger::mul(const BigInteger &b) const
{
	BigInteger r;
	st_mul(r, *this, b);

    return r;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
BigInteger BigInteger::div(const BigInteger &b) const
{
	BigInteger r;
	
	st_div(r, *this, b);

	return r;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
BigInteger BigInteger::divrem(const BigInteger &a, BigInteger &rem) const
{
	BigInteger div;
	st_div(&div,&rem,*this,a);

	return div;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
BigInteger BigInteger::negate() const
{
	BigInteger r(*this);

	r.negative = !(this->negative);
	return r;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
BigInteger& BigInteger::shiftLeft(int n)
{
	st_shiftLeft(*this, *this, (short) n);
    return *this;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
bool BigInteger::operator<(const BigInteger &a) const
{
	return cmp(a)<0;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007d32f0
// (public: virtual void * __thiscall WONAPI::BigInteger::`scalar deleting destructor'(unsigned int)    [??_GBigInteger@WONAPI@@UAEPAXI@Z])
