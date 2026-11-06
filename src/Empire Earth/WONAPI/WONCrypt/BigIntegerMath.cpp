// Native-backed bounded BigInteger multiplication, modular and inverse algorithms.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <algorithm>
#include <memory>
#include <string.h>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONBigInteger.h"
#include "ee/wonapi/WONArithmeticException.h"
namespace WONAPI {
// FUNCTION: WON 0x007d4120
void BigInteger::st_mul(BigInteger &r, const BigInteger &a, const BigInteger &b)
{
    if (&r == &a || &r == &b)
		throw ArithmeticException("Result must not be either Parameter (a or b)");

    if (a.len == 0 || b.len == 0)
    {
        st_zero(r);
        return;
    }

    r.negative = a.negative ^ b.negative;


    st_grow(r, a.len + b.len,false);
    r.len = a.len + b.len;

	MultiplyWords(r.n,a.n,b.n,a.len,b.len);
	r.len = FindSize(r.n,r.len);
}

// FUNCTION: WON 0x007d4500
void BigInteger::st_kmul(BigInteger &r, const BigInteger &a, const BigInteger &b, word *workspace)
{
	word *tn = workspace;

    if (&r == &a || &r == &b)
		throw ArithmeticException("Result must not be either Parameter (a or b)");

    if (a.len == 0 || b.len == 0)
    {
        st_mul(r,a,b);
        return;
    }

	if(a.len<b.len)
	{
		st_kmul(r,b,a,tn);
		return;
 	}

    r.negative = a.negative ^ b.negative;
	int aSize = a.len;

	if(tn==NULL)
		tn = new word[aSize*3];

	st_grow(a,aSize,true);
	st_grow(b,aSize,true);


	st_grow(r,aSize<<1,false);
	r.len = a.len+b.len;

	word *an = a.n;
	word *bn = b.n;

//	memset(an+a.len,0,(aSize-a.len)*sizeof(word));
//	memset(bn+b.len,0,(aSize-b.len)*sizeof(word));
//	memset(tn,0,aSize*3*sizeof(word));

//	memcpy(an,a.n,a.len*sizeof(word));
//	memcpy(bn,b.n,b.len*sizeof(word));


	KaratsubaMultiplyWords(r.n,tn,an,bn,a.len,a.len,b.len);
	r.len = FindSize(r.n,r.len);
		
	if(workspace==NULL)
		delete[] tn;
}

// FUNCTION: WON 0x007d4740
void BigInteger::st_mod(BigInteger &r, const BigInteger &m, const BigInteger &d)
{
    if (&r == &m)
		throw(ArithmeticException("Result must not be the same object as m"));

    st_copy(r, m);
    if (st_ucmp(m, d) < 0)
	{
		if(m.negative)
		{
			r.negative = true;
			st_add(r,d,r);
		}
        return;
	}

	r.negative = false;
	
    int i = st_bitLength(m) - st_bitLength(d);

    BigInteger ds;
    st_shiftLeft(ds, d, i);

    for (; i>= 0; --i)
    {
        if (st_cmp(r, ds) >= 0)
            st_sub(r, r, ds);
        st_shiftRight(ds, ds, (short)1);
    }
	
	if(m.negative)
	{
		r.negative = true;
		st_add(r,d,r);
	}
}

// FUNCTION: WON 0x007d4960
void BigInteger::st_div(BigInteger *dvOrig, BigInteger *remOrig, const BigInteger &m, const BigInteger &d)
{
    if (d.len == 0)
		throw(ArithmeticException("div: Divide by zero"));

    if (st_cmp(m, d) < 0)
    {
        if (remOrig != NULL)
            st_copy(*remOrig, m);
        if (dvOrig != NULL)
            st_zero(*dvOrig);

        return;
    }

	BigInteger *dvPtr = dvOrig, *remPtr = remOrig;

    if (dvPtr == NULL)
        dvPtr = new BigInteger();
    if (remPtr == NULL)
        remPtr = new BigInteger();

	std::auto_ptr<BigInteger> aDelDv(dvPtr), aDelRem(remPtr);
	if(dvOrig!=NULL)
		aDelDv.release();
	if(remOrig!=NULL)
		aDelRem.release();


	BigInteger &dv = *dvPtr;
	BigInteger &rem = *remPtr;

    BigInteger ds;
    st_copy(rem, m);
    st_zero(dv);

    int i = st_bitLength(m) - st_bitLength(d);
    st_shiftLeft(ds, d, i);

    for (; i >= 0; --i)
    {
        if (dv.len == 0)
        {
            if (st_cmp(rem, ds) >= 0)
            {
                st_one(dv);
                st_sub(rem, rem, ds);
            }
        }
        else
        {
            st_shiftLeft(dv, dv, 1);
            if (st_cmp(rem, ds) >= 0)
            {
                dv.n[0] |= 1;
                st_sub(rem, rem, ds);
            }
        }
        st_shiftRight(ds, ds, 1);
    }
    dv.negative = m.negative ^ d.negative;
}

// FUNCTION: WON 0x007d4c90
void BigInteger::st_modExp(BigInteger &r, const BigInteger &a, const BigInteger &power, const BigInteger &modulo, const BigInteger &d, short nb)
{
    BigInteger v;

    st_mod(v, a, modulo);
    int bits = st_bitLength(power);

    if ((power.n[0] & 1) != 0)
        st_mod(r, a, modulo);
    else
        st_one(r);

	int aSize = modulo.len;
	word *workspace = new word[aSize*6];

/*
	BigInteger nprime, bsquared;

	int nbits = MontgomeryRepresentation(nprime,bsquared,modulo);
	BigInteger vMontgomery;
	BigInteger rMontgomery;
	BigInteger temp1, temp2;

	mul(temp1,v,bsquared);
	MontgomeryReduce(vMontgomery,temp2,temp1,modulo,nbits,nprime,workspace);

	mul(temp1,r,bsquared);
	MontgomeryReduce(rMontgomery,temp2,temp1,modulo,nbits,nprime,workspace);
*/

     for (int i = 1; i < bits; i++)
    {

        st_modMulRecip(v, v, v, modulo, d, nb,workspace);

//		kmul(temp1,vMontgomery,vMontgomery,workspace);
//		MontgomeryReduce(vMontgomery,temp2,temp1,modulo,nbits,nprime,workspace);

	
        if (st_bit(power, i))
		{
			st_modMulRecip(r, r, v, modulo, d, (short)nb,workspace);
			//kmul(temp1,rMontgomery,vMontgomery,workspace);
			//MontgomeryReduce(rMontgomery,temp2,temp1,modulo,nbits,nprime,workspace);

		}
    }

//	MontgomeryReduce(r,temp2,rMontgomery,modulo,nbits,nprime,workspace);
	delete[] workspace;

	if(power.negative)
		st_inverseModN(r,r,modulo);
}

// FUNCTION: WON 0x007d4dd0
int BigInteger::st_recip(BigInteger &r, const BigInteger &m)
{
    BigInteger t;
    st_one(t);

    int mbits = st_bitLength(m);

    st_shiftLeft(t, t, 2*mbits);
    st_div(&r, NULL, t, m);
    return mbits+1;
}

// FUNCTION: WON 0x007d4e60
void BigInteger::st_euclid(BigInteger &r, const BigInteger &x, const BigInteger &y)
{
    BigInteger aOrig;
    BigInteger bOrig;

    st_copy(aOrig, x);
    st_copy(bOrig, y);

	BigInteger *a = &aOrig;
	BigInteger *b = &bOrig;

    int shifts = 0;

    while (b->len != 0)
    {
        if ((a->n[0] & 1) != 0)    // a odd
            if ((b->n[0] & 1) != 0)    // b odd
            {
                st_sub(*a, *a, *b);
                st_shiftRight(*a, *a, 1);
                if (st_cmp(*a, *b) < 0)
                {
                    BigInteger *t = a;
                    a = b;
                    b = t;
                }
            }
            else
            {
                st_shiftRight(*b, *b, 1);
                if (st_cmp(*a, *b) < 0)
                {
                    BigInteger *t = a;
                    a = b;
                    b = t;
                }
            }
        else
            if ((b->n[0] & 1) != 0)    // b odd
            {
                st_shiftRight(*a, *a, 1);
                if (st_cmp(*a, *b) < 0)
                {
                    BigInteger *t = a;
                    a = b;
                    b = t;
                }
            }
            else
            {
                st_shiftRight(*a, *a, 1);
                st_shiftRight(*b, *b, 1);
                shifts++;
            }
    }
    if (shifts > 0)
        st_shiftLeft(r, *a, shifts);
    else
        st_copy(r, *a);
}

// FUNCTION: WON 0x007d4fe0
void BigInteger::st_modMulRecip(BigInteger &r, const BigInteger &x, const BigInteger &y, const BigInteger &m, const BigInteger &i,short nb, word *t)
{
    BigInteger a;
    BigInteger b;
    BigInteger c;
    BigInteger d;

    st_kmul(a, x, y,t);			
	st_shiftRight(d, a, nb-1);		
    st_kmul(b, d, i,t);			
    st_shiftRight(c, b, nb-1);	
    st_kmul(b, m, c,t);		
    st_sub(r, a, b);		

    int j = 0;
    while (st_cmp(r, m) >= 0)
    {
        if (j++ > 2)
		{
			throw ArithmeticException("modMulRecip");
		}

        st_sub(r, r, m);
    }
}

// FUNCTION: WON 0x007d5270
void BigInteger::st_inverseModN(BigInteger &r, const BigInteger &a, const BigInteger &n)
{
    if (a.negative || n.negative)
		throw ArithmeticException("inverseModN");

	BigInteger x1orig;
	BigInteger x2orig;
	BigInteger x3orig; st_copy(x3orig,a);

	BigInteger y1orig;
	BigInteger y2orig;
	BigInteger y3orig; st_copy(y3orig,n);

    BigInteger *x1 = &x1orig;
    BigInteger *x2 = &x2orig;
    BigInteger *x3 = &x3orig;
    BigInteger *y1 = &y1orig;
    BigInteger *y2 = &y2orig;
	BigInteger *y3 = &y3orig;

    st_one(*x1); st_one(*y2);
    st_zero(*x2); st_zero(*y1);

	BigInteger t1orig;
	BigInteger t2orig;
	BigInteger t3orig;
	BigInteger qorig; 
	BigInteger porig;

	BigInteger *t1 = &t1orig;
	BigInteger *t2 = &t2orig;
	BigInteger *t3 = &t3orig;
	BigInteger *q = &qorig;
	BigInteger *p = &porig;


    while (y3->len != 0)
    {
        st_div(q, t3, *x3, *y3);
        st_mul(*t1, *q, *y2);		
        st_sub(*t2, *x2, *t1);
        st_mul(*p, *q, *y1);
        st_sub(*t1, *x1, *p);

		BigInteger *tt1 = x1;
		BigInteger *tt2 = x2;
		BigInteger *tt3 = x3;

        x1 = y1; x2 = y2; x3 = y3;
        y1 = t1; y2 = t2; y3 = t3;
		t1 = tt1; t2 = tt2; t3 = tt3;
    }

    if (x1->negative)
        st_add(*x1, *x1, n);
	
    st_copy(r, *x1);
}

}

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
namespace WONAPI {
using namespace std;
// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
void BigInteger::st_exp(BigInteger &r, const BigInteger &a, const BigInteger &power)
{
    BigInteger v1Orig;
	BigInteger v2Orig;

	st_copy(v1Orig,a);

	BigInteger *v1 = &v1Orig;
	BigInteger *v2 = &v2Orig;
	BigInteger *temp;
    
	int bits = st_bitLength(power);

    if ((power.n[0] & 1) != 0)
        st_copy(r,a);
    else
        st_one(r);


    for (int i = 1; i < bits; i++)
    {
        st_mul(*v2, *v1, *v1);
		temp = v1; v1 = v2; v2 = temp;

        if (st_bit(power, i))
		{
			st_copy(*v2,r);
			st_mul(r, *v1, *v2);
		}
    }
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
void BigInteger::st_modExp(BigInteger &r, const BigInteger &a, const BigInteger &power, const BigInteger &modulo)
{
    BigInteger d;
	int nb = st_recip(d, modulo);

	st_modExp(r,a,power,modulo,d,nb);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
void BigInteger::st_div(BigInteger &dv, const BigInteger &m, const BigInteger &d)
{
	st_div(&dv, NULL, m, d);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007d4310
// (public: virtual __thiscall WONAPI::ArithmeticException::~ArithmeticException(void)    [??1ArithmeticException@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007d42f0
// (public: virtual void * __thiscall WONAPI::ArithmeticException::`scalar deleting destructor'(unsigned int)    [??_GArithmeticException@WONAPI@@UAEPAXI@)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007d4380
// (public: __thiscall WONAPI::ArithmeticException::ArithmeticException(class WONAPI::ArithmeticException const &)    [??0ArithmeticException@WONAPI@@QAE@)
