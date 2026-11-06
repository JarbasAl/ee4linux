// Original-backed public ElGamal block envelopes and MD5 signature verification.
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
#include "ee/wonapi/WONBuffer.h"
#include "ee/wonapi/WONIntegerExtractor.h"
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

}

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
namespace WONAPI {
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
}
