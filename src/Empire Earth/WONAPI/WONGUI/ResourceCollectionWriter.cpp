// Genuine SDK resource collection writer, split from the original combined reader/writer unit.
// SOURCE_ONLY_NO_NATIVE_ID: Native entry identities are unlocated, zero primary/physical credit.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <stdio.h>
#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONResourceCollection.h"
namespace WONAPI {
using namespace std;
static const int RESOURCE_VERSION = 1;
ResourceCollectionWriter::ResourceCollectionWriter()
{
	mTrackByName = true;
	mDoInStringFilter = false;
	mCurId = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceCollectionWriter::~ResourceCollectionWriter()
{
	Close();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::OpenNew(const GUIString &theFileName)
{
	WONFile aFile(theFileName);
	aFile.Delete(true);
	return Open(theFileName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::Open(const GUIString &theFileName)
{
	ResourceCollection::Open(theFileName);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::Close()
{
	ResourceCollection::Close();
	mFileWriter.Close();
	mCurId = 0;
	mHadDelete = false;
	mHadRename = false;
	mHadIdChange = false;
	mHadAdd = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::Save()
{
	SaveAs(mFileName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::SaveAs(const GUIString &theName)
{
	try
	{
		string anOldName = mFileName;
		string aNewName = theName;
		mFileName = theName;

		bool overwrite = mFileReader.IsOpen() && StringCompareNoCase(anOldName,aNewName)==0;		
		bool writeHeader = true;
		if(overwrite)
		{
			if(!IsModified()) // no change
				return;

			if(!IsHeaderModified())
				writeHeader = false;

			aNewName += ".new";
		}

		if(!mFileWriter.Open(aNewName.c_str(),true))
		{
			mErrorString = "Couldn't open file: " + aNewName;
			return;
		}

		WriteResources();
		if(writeHeader)
			WriteHeaderFile();

		Close();
		if(overwrite)
		{
			WONFile anOldFile = anOldName;
			anOldFile.Delete(true);
			rename(aNewName.c_str(),anOldName.c_str());
		}
	}
	catch(FileWriterException &anEx)
	{
		mErrorString = anEx.mDescription;
	}

	Close();
	Open(mFileName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::ExportToCFG(const GUIString &theName)
{
	string aName = theName;
	WONFile aWONFile(aName);
	aWONFile.Delete(true);


	FILE *aFile = fopen(aName.c_str(),"w");
	if(aFile==NULL)
		return false;

	ResourceIdMap::iterator anItr = mResourceIdMap.begin();
	while(anItr!=mResourceIdMap.end())
	{
		const ResourceEntry *anEntry = anItr->second;
		const char *aTypeName = "Unknown";
		switch(anEntry->mType)
		{
			case ResourceType_Image: aTypeName = "Image:"; break;
			case ResourceType_String: aTypeName = "String:"; break;
			case ResourceType_Binary: aTypeName = "Binary:"; break;
			case ResourceType_Color: aTypeName = "Color:"; break;
			case ResourceType_Font: aTypeName = "Font:"; break;
		}

		string aName = anEntry->mName;
		string aDesc = anEntry->mDesc;
		fprintf(aFile,"%-10s %-8d %-50s %s\n",aTypeName,anEntry->mId,aName.c_str(),aDesc.c_str());
		
		++anItr;
	}

	fclose(aFile);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::IsHeaderModified()
{
	if(mHadDelete || mHadRename || mHadIdChange || mHadAdd)
		return true;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::IsModified()
{
	if(IsHeaderModified())
		return true;

	for(ResourceIdMap::iterator anItr=mResourceIdMap.begin(); anItr!=mResourceIdMap.end(); ++anItr)
	{
		if(anItr->second->NeedUpdate())
			return true;
	}	

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ResourceCollectionWriter::GetHeaderFileName(const GUIString &theFilePath)
{
	WONFile aFileParse(mFileName);
	aFileParse.GetFileExtension(); // existing target cache also computes basename in mS30
	string aName = aFileParse.mS30 + "WRS";
	string aFileName = aFileParse.GetFileDirectory() + '\\' + aName + ".h";
	return aFileName;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteResourceCollectionResource(ResourceEntry *theEntry)
{	
	try
	{
		mFileReader.SetPos(theEntry->mFileOffset);

		const int CHUNK_SIZE = 32768;
		char aBuf[CHUNK_SIZE];
		int aBytesLeft = theEntry->mFileSize;
		while(aBytesLeft>0)
		{
			int aNumToRead = CHUNK_SIZE<aBytesLeft ? CHUNK_SIZE : aBytesLeft;
			mFileReader.ReadBytes(aBuf,aNumToRead);
			mFileWriter.WriteBytes(aBuf,aNumToRead);
			aBytesLeft-=aNumToRead;
		}
	}
	catch(FileReaderException&)
	{
/*		char aBuf[512];
		string aName = theEntry->mDesc;
		sprintf(aBuf,"Error reading old resource: %s\n",aName.c_str());

		throw FileWriterException(aBuf);*/
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteFileResource(const ResourceEntry *theEntry)
{
	if(theEntry->mRawData.get()!=NULL)
	{
		mFileWriter.WriteBytes(theEntry->mRawData->data(),theEntry->mRawData->length());
		return;
	}

	string aFileName = theEntry->mDesc;
	if(aFileName.empty()) // prevent assert in MS library for fopen of empty string
		return;

	FILE *aFile = fopen(aFileName.c_str(),"rb");
	if(aFile==NULL)
		return;
//		throw FileWriterException("Couldn't open file: " + aFileName);

	AutoFile aCloseFile(aFile);

	const int CHUNK_SIZE = 32768;
	char aBuf[CHUNK_SIZE];
	int aLen;

	while(!feof(aFile))
	{
		aLen = fread(aBuf,1,CHUNK_SIZE,aFile);
		if(aLen<=0)
			break;

		mFileWriter.WriteBytes(aBuf,aLen);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteAsciiString(const GUIString &theString)
{
	string aStr = theString;
	mFileWriter.WriteShort(aStr.length());
	mFileWriter.WriteBytes(aStr.data(),aStr.length());
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteWideString(const GUIString &theString)
{
	mFileWriter.WriteShort(theString.length());
	for(int i=0; i<theString.length(); i++)
		mFileWriter.WriteShort(theString[i]);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteResources()
{
	mFileWriter.WriteBytes("WRS",3);
	mFileWriter.WriteLong(RESOURCE_VERSION);

	mFileWriter.WriteLong(0); // save place for table of contents position
	int anItemCount = 0;
	for(ResourceIdMap::iterator anItr=mResourceIdMap.begin(); anItr!=mResourceIdMap.end(); ++anItr)
	{
		ResourceEntry *anEntry = const_cast<ResourceEntry*>(anItr->second.get());
		if(anEntry->NeedRemove())
			continue;

		anItemCount++;
		int aFileOffset = mFileWriter.pos();
		if(anEntry->NeedUpdate())
		{
			switch(anEntry->mType)
			{
				case ResourceType_None: break;
				case ResourceType_String: break;
				case ResourceType_Color: break;
				default: 
					WriteFileResource(anEntry);
			}
		}
		else
			WriteResourceCollectionResource(anEntry);

		anEntry->mFileOffset = aFileOffset;
	}		
	WriteTableOfContents(anItemCount);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteTableOfContents(int numItems)
{
	int aPos = mFileWriter.pos();
	mFileWriter.SetPos(7);
	mFileWriter.WriteLong(aPos);
	mFileWriter.SetPos(aPos);
	mFileWriter.WriteLong(numItems);
	for(ResourceIdMap::iterator anItr=mResourceIdMap.begin(); anItr!=mResourceIdMap.end(); ++anItr)
	{
		const ResourceEntry *anEntry = anItr->second;
		if(!anEntry->NeedRemove())
		{
			mFileWriter.WriteLong(anEntry->mId);
			mFileWriter.WriteShort((unsigned short)anEntry->mType);
			mFileWriter.WriteLong(anEntry->mFileOffset);
			WriteAsciiString(anEntry->mName);
			WriteWideString(anEntry->mDesc);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteHeaderFile()
{
	WONFile aFileParse(mFileName);
	aFileParse.GetFileExtension(); // existing target cache also computes basename in mS30
	string aName = aFileParse.mS30 + "WRS";
	string aFileName = aFileParse.GetFileDirectory() + '\\' + aName + ".h";

	FILE *aFile = fopen(aFileName.c_str(),"w");
	if(aFile==NULL)
	{
		WONFile aHeaderFile(aFileName);
		aHeaderFile.SetReadOnly(false);
		aFile = fopen(aFileName.c_str(),"w");
		if(aFile==NULL)
		{
			mErrorString = "Couldn't open file: " + aFileName;
			return;
		}
	}

	fprintf(aFile,"#ifndef __%s_H__\n",aName.c_str());
	fprintf(aFile,"#define __%s_H__\n",aName.c_str());
	fprintf(aFile,"\n\n");
	fprintf(aFile,"enum\n");
	fprintf(aFile,"{\n");

	for(ResourceIdMap::iterator anItr=mResourceIdMap.begin(); anItr!=mResourceIdMap.end(); ++anItr)
	{
		const ResourceEntry *anEntry = anItr->second;
		if(!anEntry->NeedRemove())
		{
			string anEnumName = anEntry->mName;
			string aComment = anEntry->mDesc;
//			fprintf(aFile, "\t%s\t\t\t=\t%d,\t//%s\n",anEnumName.c_str(),anEntry->mId,aComment.c_str());
			fprintf(aFile, "\t%s = %d,\n",anEnumName.c_str(),anEntry->mId);
		}
	}

	fprintf(aFile,"};\n");
	fprintf(aFile,"\n\n");
	fprintf(aFile,"#endif\n");
	fclose(aFile);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ResourceCollectionWriter::GetNextFreeId()
{
	mCurId++;
	ResourceIdMap::iterator anItr = mResourceIdMap.find(mCurId);
	while(anItr!=mResourceIdMap.end())
	{
		if(mCurId==anItr->first)
		{
			++mCurId;
			++anItr;
		}
		else
			break;
	}
	
	return mCurId;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::InStringFilter(ResourceEntry *theEntry)
{
	if(theEntry->mType!=ResourceType_String)
		return;

	if(!mDoInStringFilter)
		return;

	GUIString &aStr = theEntry->mDesc;
	GUIString aNewStr;
	int aStartPos = 0;
	while(true)
	{
		int aSlashPos = aStr.find('\\',aStartPos);
		if(aSlashPos<0) 
		{
			aSlashPos = aStr.length();
			break;
		}
		aNewStr.append(aStr,aStartPos,aSlashPos-aStartPos);
		aStartPos = aSlashPos+1;
		int aNextChar = aStr[aStartPos];
		switch(aNextChar)
		{			
			case 'n': aNewStr.append('\n'); break;
			case 't': aNewStr.append('\t'); break;
			case '\\': aNewStr.append('\\'); break;
		}
		aStartPos++;
	}

	if(aStartPos!=0) // found some slashes
	{
		if(aStartPos<aStr.length()) // get last bit
			aNewStr.append(aStr,aStartPos,aStr.length()-aStartPos);

		theEntry->mDesc = aNewStr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const ResourceEntry* ResourceCollectionWriter::AddResource(ResourceType theType, const GUIString &theName, const GUIString &theDesc, const ByteBuffer *theRawData)
{
	ResourceEntryPtr anEntry = new ResourceEntry;
	ResourceNameMap::iterator anItr = mResourceNameMap.insert(ResourceNameMap::value_type(theName,ConstResourceEntryPtr(anEntry.get()))).first;
	if(anItr->second.get()!=anEntry.get())
	{
		mCollisionEntry = anItr->second;
		return NULL;
	}

	mHadAdd = true;
	anEntry->mType = theType;
	anEntry->mName = theName;
	anEntry->mDesc = theDesc;
	anEntry->mRawData = theRawData;
	anEntry->mId = GetNextFreeId();
	mResourceIdMap[anEntry->mId] = anEntry;

	InStringFilter(anEntry);
	UpdateResource(anEntry);
	return anEntry;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::ModifyResource(const ResourceEntry *theEntry, const GUIString &theNewDesc)
{
	ResourceEntry *anEntry = const_cast<ResourceEntry*>(theEntry);
	anEntry->mDesc = theNewDesc;
	InStringFilter(anEntry);
	return UpdateResource(theEntry);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::ModifyResourceId(const ResourceEntry *theEntry, int theNewId)
{
	if(theNewId<=0)
		return false;

	ResourceIdMap::iterator anItr = mResourceIdMap.find(theNewId);
	if(anItr!=mResourceIdMap.end())
	{
		if(anItr->second==theEntry)
			return true;
		else
			return false;
	}

	if(theNewId >= mCurId)
		mCurId++;

	ResourceEntry *anEntry = const_cast<ResourceEntry*>(theEntry);
	int anOldId = anEntry->mId;
	anEntry->mId = theNewId;

	mResourceIdMap[theNewId] = anEntry;
	mResourceIdMap.erase(anOldId);
	mHadIdChange = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::UpdateResource(const ResourceEntry *theEntry)
{
	ResourceEntry *anEntry = const_cast<ResourceEntry*>(theEntry);
	anEntry->mUpdateType = ResourceUpdateType_Update;
	anEntry->mData = NULL;
	if(anEntry->mType!=ResourceType_String)
	{
		WONFile aFile = (const std::string&)anEntry->mDesc;
		anEntry->mFileSize = aFile.GetSize();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::RenameResource(const ResourceEntry *theEntry, const GUIString &theNewName)
{
	const ResourceEntry* aNew = GetResource(theNewName,false);
	if(aNew!=NULL)
	{
		if(aNew==theEntry) // renaming to self
			return true;

		mCollisionEntry = aNew;
		return false;
	}

	ResourceEntryPtr anEntry = const_cast<ResourceEntry*>(theEntry);
	RemoveEntry(theEntry,true);

	mHadRename = true;

	anEntry->mName = theNewName;
	AddEntry(anEntry);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::DeleteResource(const ResourceEntry *theEntry)
{
	if(RemoveEntry(theEntry,false))
	{
		mHadDelete = true;
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
GUIString ResourceCollectionWriter::GetTempName(const GUIString &thePrefix)
{
	int anId = mCurId;
	GUIString aName;
	while(true)
	{
		anId++;

		char aBuf[50];
		sprintf(aBuf,"%d",anId);
		aName = thePrefix;
		aName.append(aBuf);
		if(mResourceNameMap.find(aName)==mResourceNameMap.end())
			break;
	}

	return aName;
}
*/
}
