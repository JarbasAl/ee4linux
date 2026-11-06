// Native ASN.1 integer extractor used by ElGamal key loading.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONIntegerExtractor.h"
#include "ee/wonapi/WONElGamal.h"
namespace WONAPI {
// SUPPORT-ONLY: EXE 0x007d7690 (thin initializer plus generated seven owners, conservative0quota)
ElGamal::ElGamal()
{
	Invalidate();
}

// SUPPORT-ONLY: EXE 0x007d7740 (constructor setter delegation plus generated owners, conservative0quota)
ElGamal::ElGamal(const void *theKey, int theKeyLen, bool isPrivate)
{
	if(isPrivate)
		SetPrivateKey(theKey,theKeyLen);
	else
		SetPublicKey(theKey,theKeyLen);
}

// FUNCTION: WON 0x007d7850
bool ElGamal::SetPrivateKey(const void *theKey, int theKeyLen)
{		
	Invalidate();

	IntegerExtractor anExtractor(theKey,theKeyLen);
	bool aSuccess = anExtractor.Extract(p) && anExtractor.Extract(q) && anExtractor.Extract(g) 
			   && anExtractor.Extract(y) && anExtractor.Extract(x);	
	
	if(!aSuccess)
		return false;
	
	modulusLen = p.bitLength()/8;

	recip = p.recip(nb);
	

	mIsPrivate = true;
	mIsPublic = true;
	return true;
}

// FUNCTION: WON 0x007d79e0
bool ElGamal::SetPublicKey(const void *theKey, int theKeyLen)
{		
	Invalidate();

	IntegerExtractor anExtractor(theKey,theKeyLen);
	bool aSuccess = anExtractor.Extract(p) && anExtractor.Extract(q) && anExtractor.Extract(g)
		&& anExtractor.Extract(y);
	
	if(!aSuccess)
		return false;

	modulusLen = p.bitLength()/8;

	recip = p.recip(nb);

	mIsPublic = true;
	return true;
}

}

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
namespace WONAPI {
using namespace std;
// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
ByteBufferPtr ElGamal::GetRawPrivateKey() const
{
	if(!IsPrivate())
		return NULL;

	IntegerInserter anInserter;
	anInserter.Insert(p);
	anInserter.Insert(q);
	anInserter.Insert(g);
	anInserter.Insert(y);
	anInserter.Insert(x);

	return anInserter.Get();
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
ByteBufferPtr ElGamal::GetRawPublicKey() const
{
	if(!IsPublic())
		return NULL;

	IntegerInserter anInserter;
	anInserter.Insert(p);
	anInserter.Insert(q);
	anInserter.Insert(g);
	anInserter.Insert(y);

	return anInserter.Get();
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
IntegerInserter::IntegerInserter()
{
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
void IntegerInserter::AddLength(RawBuffer &theBuf, int theLen)
{
	if(theLen>0x7f)
	{
		if(theLen>0xff)
		{
			theBuf+=0x82; // 2 length bytes

			unsigned char aLowLen = ((unsigned short)theLen)>>8;
			unsigned char aHighLen = theLen & 0xff;
			theBuf+=aHighLen;
			theBuf+=aLowLen;
		}
		else
		{
			theBuf+=0x81; // 1 length byte
			theBuf+=(unsigned char)theLen;
		}
	}
	else
	{
		theBuf+=(unsigned char)theLen;
	}

}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
ByteBufferPtr IntegerInserter::Get()
{
	RawBuffer aHeader;
	aHeader += (unsigned char)(SEQUENCE | CONSTRUCTED);
	AddLength(aHeader,out.length());
	
	char *aBuf = new char[aHeader.length() + out.length()];
	memcpy(aBuf,aHeader.data(),aHeader.length());
	memcpy(aBuf+aHeader.length(),out.data(),out.length());
	return new ByteBuffer(aBuf,aHeader.length() + out.length(),true);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing body; zero Native/primary/physical credit.
void IntegerInserter::Insert(const BigInteger &theInt)
{
	out+=(unsigned char)INTEGER;
	RawBuffer aBuf;
	theInt.toBinary(aBuf);
	AddLength(out,aBuf.length());
	out+=aBuf;	
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007d7690
// (public: __thiscall WONAPI::ElGamal::ElGamal(void)    [??0ElGamal@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007d7740
// (public: __thiscall WONAPI::ElGamal::ElGamal(void const *,int,bool)    [??0ElGamal@WONAPI@@QAE@PBXH_N@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006e93e0
// (public: __thiscall WONAPI::IntegerInserter::IntegerInserter(void)    [??0IntegerInserter@WONAPI@@QAE@XZ])
