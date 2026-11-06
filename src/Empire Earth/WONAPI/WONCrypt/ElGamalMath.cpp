// Native-backed ElGamal digest and modular encryption/verification algorithms.
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
#include "ee/wonapi/WONElGamal.h"
#include "ee/wonapi/WONIntegerExtractor.h"
#include "ee/wonapi/WONArithmeticException.h"
namespace WONAPI {
// FUNCTION: WON 0x007d84a0
bool ElGamal::EncodeDigest(const RawBuffer& digest, BigInteger &h) const
{
	IntegerExtractor aDecoder(digest.data(),digest.length(),false);
	if(digest.length()*8 < q.bitLength())
	{
		if(!aDecoder.Decode(digest.length(),h))
			return false;
	}
	else
	{
		if(!aDecoder.Decode(q.byteLength(),h))
			return false;

		h = h.shiftRight(q.byteLength()*8 - q.bitLength() + 1);
	}

	return true;
}

// FUNCTION: WON 0x007d8990
bool ElGamal::encrypt(const BigInteger &M, BigInteger ab[]) const
{
	try
	{
		BigInteger p_minus_1 = p.sub(BigInteger::ONE);
		// choose a random k, relatively prime to p-1.
		BigInteger k;
		do {
			// values of k with the same number of bits as p won't be chosen, but
			// that shouldn't be a problem.
			k = BigInteger(p.bitLength()-1, mRandom);
			if (!(k.testBit(0)))
				k = k.setBit(0); // make sure k is odd
		} while (!(k.gcd(p_minus_1).equals(BigInteger::ONE)));

		ab[0] /* a */ = g.modExp(k, p, recip, nb);
		ab[1] /* b */ = y.modExp(k, p, recip, nb).modMulRecip(M,p,recip,nb);

		return true;
	}
	catch(ArithmeticException&)
	{
	}

	return false;
}

// FUNCTION: WON 0x007d8b70
bool ElGamal::decrypt(const BigInteger &a, const BigInteger &b, BigInteger &out) const
{
    try 
	{
		out = b.modMulRecip(a.modExp(x, p, recip, nb).inverseModN(p), p, recip, nb);
		return true;
    } 
	catch (ArithmeticException &) 
	{
    }
	
	return false;
}

// FUNCTION: WON 0x007d8c50
bool ElGamal::BogusVerify(const BigInteger &M, const BigInteger &a, const BigInteger &b) const 
{

// check a != 0 && a == (g^b * y^a + m) mod q
	
	try
	{
		if(a.cmp(BigInteger::ZERO)==0)
			return false;

//		BigInteger a1 = a.mod(q);
//		BigInteger b1 = b.mod(q);
		
		BigInteger gbq = g.modExp(b,p,recip,nb);
		BigInteger yaq = y.modExp(a,p,recip,nb);

	/*
		cout << "p = " << p << endl;
		cout << "g = mod(" << g << ",p)" << endl;
		cout << "a = " << a << endl;
		cout << "b = " << b << endl;
		cout << "y = " << y << endl;

		cout << "gbq = " << gbq << endl;
		cout << "yaq = " << yaq << endl;*/
		
		BigInteger result;

		result = gbq.modMulRecip(yaq,p,recip,nb);
//		result = gbq.mul(yaq);
		result = result.add(M);
//		result = result.mod(p);
		result = result.mod(q);
		
		if(a.cmp(result)==0)
			return true;
		else
			return false;
	}
	catch(ArithmeticException &)
	{
	}

	return false;
}

}

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
namespace WONAPI {
using namespace std;
// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
BigInteger ElGamal::GetRandBetween(Random &rng, const BigInteger &min, const BigInteger &max)
{
	BigInteger range = max.sub(min);
	int nBits = range.bitLength();

	BigInteger anInt;
	do
	{
		anInt = BigInteger(nBits,rng);
	}
	while (anInt.cmp(range)>0);

	anInt = anInt.add(min);
	return anInt;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
bool ElGamal::BogusSign(const BigInteger &M, BigInteger ab[]) const
{
	try 
	{
		BigInteger TWO(2);		
		BigInteger k;
		
		do
		{
			k = GetRandBetween(mRandom, TWO, q.sub(TWO));			
			ab[0] = g.modExp(k,p);
			
			
			ab[0] = ab[0].add(M);
			
			ab[0] = ab[0].mod(p);
			
			ab[0] = ab[0].mod(q);
			
			ab[1] = k.sub(x.mul(ab[0]));

			
			ab[1] = ab[1].mod(q);
			
			
			//r = (gpc.Exponentiate(k) + m) % q;
			//s = (k - x*r) % q;
		} while (ab[0].cmp(BigInteger::ZERO)==0);			// make sure r != 0

		return true;
	}
	catch(ArithmeticException &)
	{
	}

	return false;
}
}
