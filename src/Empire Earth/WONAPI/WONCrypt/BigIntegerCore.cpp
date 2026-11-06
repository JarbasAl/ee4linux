// Native-backed signed BigInteger dispatch, limb shifts and unsigned add/subtract.
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
namespace WONAPI {
const int BigInteger::BITS = 32;
// FUNCTION: WON 0x007d3c30
void BigInteger::st_add(BigInteger &r, const BigInteger &a, const BigInteger &b)
{
    if (a.len == 0)
    {
        st_copy(r, b);
        return;
    }
    if (b.len == 0)
    {
        st_copy(r, a);
        return;
    }

    if (a.negative)
    {
        if (b.negative)
        {
            st_add_unsigned(r, a, b);
            r.negative = true;
        }
        else
        {
            st_sub_unsigned(r, b, a);
        }
    }
    else
    {
        if (b.negative)
        {
            st_sub_unsigned(r, a, b);
        }
        else
        {
            st_add_unsigned(r, a, b);
        }
    }
}

// FUNCTION: WON 0x007d3cc0
void BigInteger::st_sub(BigInteger &r, const BigInteger &a, const BigInteger &b)
{
    if (a.len == 0)
    {
        st_copy(r, b);
        if (b.len > 0)
            r.negative = true ^ b.negative;
        return;
    }
    if (b.len == 0)
    {
        st_copy(r, a);
        return;
    }

    if (a.negative)
    {
        if (b.negative)
        {
            st_sub_unsigned(r, b, a);
        }
        else
        {
            st_add_unsigned(r, b, a);
            r.negative = true;
        }
    }
    else
    {
        if (b.negative)
        {
            st_add_unsigned(r, a, b);
        }
        else
        {
            st_sub_unsigned(r, a, b);
        }
    }
}

// FUNCTION: WON 0x007d3d80
int BigInteger::st_cmp(const BigInteger &a, const BigInteger &b)
{
    if (a.len == 0 && b.len == 0) return 0;

    if (a.negative)
    {
        if (b.negative)
        {
            return st_ucmp(b, a);
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if (b.negative)
        {
            return 1;
        }
        else
        {
            return st_ucmp(a, b);
        }
    }
}

// FUNCTION: WON 0x007d3e00
void BigInteger::st_shiftLeft(BigInteger &r, const BigInteger &a, int n)
{
    if (a.len == 0)
    {
        st_zero(r);
        return;
    }
    int rem = n % BITS;
    int blocks = n / BITS;
    int len = a.len;

    st_grow(r, len + blocks);
    r.len = len + blocks;

    word *rn = r.n;

	st_CopyBuffer(rn+blocks,a.n,len);

    if (blocks > 0)
        for (int i = blocks-1; i >= 0 ; --i) { rn[i] = 0; }

    if (rem != 0)
    {
        word carry = 0;

        int rlen = r.len;
        for (int i = blocks; i < rlen; ++i)
        {
            word l = rn[i];

            rn[i] = ((l << rem) | carry);
            carry = (unsigned int)l >> (BITS-rem);
        }
        if (carry != 0)
        {
            rlen += 1;
            st_grow(r, rlen);
            r.n[rlen-1] = carry;
            r.len = rlen;
        }
    }
}

// FUNCTION: WON 0x007d3f10
void BigInteger::st_shiftRight(BigInteger &r, const BigInteger &a, int n)
{
    int rem = n % BITS;
    int blocks = n / BITS;

    if (blocks >= a.len)
    {
        st_zero(r);
        return;
    }

    st_grow(r, a.len - blocks);
    r.len = a.len - blocks;

	st_CopyBuffer(r.n,a.n+blocks,r.len);

    if (rem != 0)
    {
        word carry = 0;
        word *rn = r.n;

        int rlen = r.len;
        for (int i = rlen-1; i > 0; --i)
        {
            word l = rn[i];

            rn[i] = ((unsigned int)l >> rem) | carry;
            carry = (l << (BITS-rem));
        }
        int l = rn[0];

        rn[0] = ((unsigned int)l >> rem) | carry;
        if (rlen > 0 && rn[rlen-1] == 0)
            r.len--;
    }
}

// FUNCTION: WON 0x007d4000
void BigInteger::st_add_unsigned(BigInteger &r, const BigInteger &a, const BigInteger &b)
{
    // Ensure a is the longest
    if (a.len < b.len)
    {
		st_add_unsigned(r,b,a);
		return;
    }

    // Needed in case the result is same object as r
    int alen = a.len;
    int blen = b.len;

    st_grow(r, alen + 1);
   
	r.negative = false;

	r.len = AddWords(r.n,a.n,b.n,alen,blen);
}

// FUNCTION: WON 0x007d4070
void BigInteger::st_sub_unsigned(BigInteger &r, const BigInteger &a, const BigInteger &b, bool determine_sign)
{
    switch (st_ucmp(a, b))
    {
    case 0:
        st_zero(r);
        return;
    case -1:
		if(determine_sign)
			r.negative = true;

		st_sub_unsigned(r,b,a,false);
		return;
    case 1:
        if(determine_sign)
			r.negative = false;
    }

    // Now a is the largest

    st_grow(r, a.len);

    word borrow = SubWords(r.n,a.n,b.n,a.len,b.len);
	r.len = FindSize(r.n,a.len);
}

}
