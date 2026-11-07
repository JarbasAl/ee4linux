// Original-backed public ElGamal block envelopes and MD5 signature verification.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <algorithm>
#include <string.h>
#include <list>
#include <map>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONElGamal.h"
#include "ee/wonapi/WONBuffer.h"
#include "ee/wonapi/WONIntegerExtractor.h"
#include "ee/wonapi/WONArithmeticException.h"
namespace WONAPI {
// FUNCTION: WON 0x007d7b50
ByteBufferPtr ElGamal::Encrypt(const void *thePlainText, int thePlainTextLen) const
{
	if(!IsPublic())
		return NULL;

	const unsigned char *aPlainText = (const unsigned char*)thePlainText;

	int aBlockLen = modulusLen - 3;
	int aNumBlock = thePlainTextLen / aBlockLen;
	if ((thePlainTextLen % aBlockLen) != 0) aNumBlock++;

	WriteBuffer anEncrypt(0,true);
	anEncrypt.Reserve(4+modulusLen*2*aNumBlock);

	int anOffset = 0;

	anEncrypt.AppendLong(aNumBlock);

	while(anOffset < thePlainTextLen)
	{	
		int thisBlockLen = aBlockLen;
		
		if(thePlainTextLen - anOffset < aBlockLen)
			thisBlockLen = thePlainTextLen - anOffset;
		
		RawBuffer anEncryptBlock(modulusLen-1,(unsigned char)0);

		for(int k=0,j=modulusLen-2-thisBlockLen; j<modulusLen-2; j++,k++)
			anEncryptBlock[j] = aPlainText[anOffset+k];

		anEncryptBlock[modulusLen - 2] = (unsigned char)thisBlockLen;


		BigInteger ab[2];
		
		if(!encrypt(BigInteger(anEncryptBlock),ab))
			return NULL;


		RawBuffer aa,bb;
		ab[0].toBinary(aa);
		ab[1].toBinary(bb);
		
		if(aa.length()==modulusLen)
			anEncrypt.AppendBytes(aa.data(),aa.length());
		else if(aa.length()>modulusLen)
			anEncrypt.AppendBytes(aa.data()+aa.length()-modulusLen,modulusLen);
		else
		{
			for(int i=aa.length(); i<modulusLen; i++)
				anEncrypt.AppendByte(0);
			
			anEncrypt.AppendBytes(aa.data(),aa.length());
		}
		

		if(bb.length()==modulusLen)
			anEncrypt.AppendBytes(bb.data(),bb.length());
		else if(bb.length()>modulusLen)
			anEncrypt.AppendBytes(bb.data()+bb.length()-modulusLen,modulusLen);
		else
		{
			for(int i=bb.length(); i<modulusLen; i++)
				anEncrypt.AppendByte(0);
			
			anEncrypt.AppendBytes(bb.data(),bb.length());
		}						
		anOffset+=thisBlockLen;		
	}		

	return anEncrypt.ToByteBuffer(true);
}
// FUNCTION: WON 0x007d7fc0
ByteBufferPtr ElGamal::Decrypt(const void *theCipherText, int theCipherTextLen) const
{
	if(!IsPrivate())
		return NULL;

	const unsigned char *in = (const unsigned char*)theCipherText;
	int inOffset = 0;

	if(theCipherTextLen-inOffset<4) return NULL;
	int aNumBlocks = *(const int*)in; inOffset+=4;

	if(theCipherTextLen-inOffset < aNumBlocks*modulusLen*2-inOffset)
		return NULL;

	RawBuffer aBuf(modulusLen,(unsigned char)0);
	RawBuffer bBuf(modulusLen,(unsigned char)0);

	WriteBuffer aDecrypt(0,true);

	BigInteger a;;
	BigInteger b;
		
	BigInteger aPlainText;

	for(int i=0; i<aNumBlocks; i++)
	{
		aBuf.assign(in+inOffset,modulusLen); inOffset+=modulusLen;
		bBuf.assign(in+inOffset,modulusLen); inOffset+=modulusLen;
		
		a.fromBinary(aBuf);
		b.fromBinary(bBuf);

		if(!decrypt(a,b,aPlainText))
			return NULL;
		
		RawBuffer aBigIntArray;
		aPlainText.toBinary(aBigIntArray);
		

		if(aBigIntArray.length()==0) return NULL;
		int aPlainLen = aBigIntArray[aBigIntArray.length() - 1];
		
		if(aPlainLen>modulusLen - 3)
			return NULL;
		
		aDecrypt.AppendBytes(aBigIntArray.data()+aBigIntArray.length()-1-aPlainLen,aPlainLen);
	}

	return aDecrypt.ToByteBuffer(true);
}
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
// FUNCTION: WON 0x007d8620
bool ElGamal::Verify(const void *theMessage, int theMessageLen, const void *theSignature, int theSignatureLen) const
{
	if(!IsPublic())
		return false;

	if(theSignatureLen!=q.byteLength()*2)
		return false;
	
	
	MD5Digest anMD5;
	anMD5.update(theMessage,theMessageLen);
	RawBuffer aDigest = anMD5.digest();
			

	BigInteger M;
	if(!EncodeDigest(aDigest,M))
		return false;

	const unsigned char *in = (const unsigned char*)theSignature;

	RawBuffer aBuf(q.byteLength(),(unsigned char)0);
	RawBuffer bBuf(q.byteLength(),(unsigned char)0);

	aBuf.assign(in,aBuf.length());
	bBuf.assign(in+aBuf.length(),bBuf.length());
					
	BigInteger a(aBuf);
	BigInteger b(bBuf);
	
	return BogusVerify(M,a,b);	
}


using namespace std;
// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
ByteBufferPtr ElGamal::Sign(const void* theMessage, int theMessageLen) const
{		
	if(!IsPrivate())
		return NULL;

	MD5Digest anMD5;
	anMD5.update(theMessage, theMessageLen);
	RawBuffer aDigest = anMD5.digest();
	
	BigInteger M;
	if(!EncodeDigest(aDigest,M))
		return NULL;
	
	BigInteger ab[2];
	
	if(!BogusSign(M,ab))
		return NULL;

	WriteBuffer aSignature(0,true);
		
	int qLen = q.byteLength();
	RawBuffer a,b;
	ab[0].toBinary(a);
	ab[1].toBinary(b);

	if(a.length()==qLen)
		aSignature.AppendBytes(a.data(),a.length());
	else if(a.length()>qLen)
		aSignature.AppendBytes(a.data()+a.length()-qLen,qLen);
	else
	{
		for(int i=a.length(); i<qLen; i++)
			aSignature.AppendByte(0);
		
		aSignature.AppendBytes(a.data(),a.length());
	}
	

	if(b.length()==qLen)
		aSignature.AppendBytes(b.data(),b.length());
	else if(b.length()>qLen)
		aSignature.AppendBytes(b.data()+b.length()-qLen,qLen);
	else
	{
		for(int i=b.length(); i<qLen; i++)
			aSignature.AppendByte(0);
		
		aSignature.AppendBytes(b.data(),b.length());
	}						
	
	return aSignature.ToByteBuffer(true);
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
