// Genuine same-library SDK MSFileDialog implementation; SOURCE_ONLY_NO_NATIVE_ID.
// Missing unit proposed profile: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <commdlg.h>
#include <string>
#include <list>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONMSFileDialog.h"
using namespace std;
namespace WONAPI {
MSFileDialog::MSFileDialog(bool isSave)
{
	mIsSave = isSave;
	mAllowMultiple = false;
	mWarnIfExists = false;
	mGetDir = false;
	mNoChangeDir = false;

	mFileMustExist = !isSave;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) && !defined(_XBOX)
static MSFileDialog *gFileDialog = NULL;
static string gLastPath;
static bool gGotDir;
static UINT CALLBACK MyOFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if(gFileDialog==NULL)
		return 0;

	if (uiMsg == WM_NOTIFY && gFileDialog->IsGetDir())
    {
     	LPOFNOTIFYA lpofn = (LPOFNOTIFYA)lParam;
        if(lpofn->hdr.code==CDN_FOLDERCHANGE)
		{
			string aNewPath;
			char aBuf[MAX_PATH+1];
			SendMessageA(::GetParent(hdlg),CDM_GETFOLDERPATH,MAX_PATH,(LPARAM)aBuf);
			aNewPath = aBuf;
			if(aNewPath==gLastPath) // user hit ok button
			{
				PostMessageA(::GetParent(hdlg),WM_COMMAND,MAKELONG( IDCANCEL, 0 ) ,NULL);
				gGotDir = true;
	       		return 1;
			}
			else
				gLastPath = aNewPath;
		}
    } 
	else if(uiMsg==CDN_INCLUDEITEM && gFileDialog->IsGetDir())
	{
		OFNOTIFYEXA *anInfo = (OFNOTIFYEXA*)lParam;

	}
	else if(uiMsg==WM_INITDIALOG)
	{

		if(gFileDialog->GetOkTitle().length()!=0)
		{
			string aStr = gFileDialog->GetOkTitle();
			SendMessageA(::GetParent(hdlg),CDM_SETCONTROLTEXT,IDOK,(LPARAM)aStr.c_str()); 
		}
	}
	

	return 0;
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetGetDir(bool getDir)
{
	mGetDir = getDir;
#if defined(_WIN32)
	mFilterList.clear();
	AddFilter("*.zzz","Directories");
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetTitle(const GUIString &theTitle)
{
	mTitle = theTitle;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetOkTitle(const GUIString &theOkTitle)
{
	mOkTitle = theOkTitle;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetInitialFileName(const GUIString &theFileName)
{
	mInitialFileName = theFileName;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetInitialDir(const GUIString &theDir)
{
	mInitialDir = theDir;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetAllowMultiple(bool allowMultiple)
{
	if(!mIsSave)
		mAllowMultiple = allowMultiple;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetFileMustExist(bool mustExist)
{
	mFileMustExist = mustExist;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetWarnIfExists(bool warnIfExists)
{
	if(mIsSave)
		mWarnIfExists = warnIfExists;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetNoChangeDir(bool noChange)
{
	mNoChangeDir = noChange;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::AddFilter(const GUIString &theFilter, const GUIString &theDescription)
{
	mFilterList.push_back(FilterPair(theFilter,theDescription));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const MSFileDialog::FileList& MSFileDialog::GetFileList()
{
	return mFileList;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString MSFileDialog::GetFile()
{
	if(mFileList.empty())
		return "";
	else
		return mFileList.front();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSFileDialog::DoDialog(Window *theParent)
{
	mFileList.clear();
#if defined(_WIN32) && !defined(_XBOX)
	const int MAX_FILE_NAME = 8192;
	char aFileName[MAX_FILE_NAME];
	char aInitialDir[MAX_FILE_NAME];
	OPENFILENAMEA aStruct;
	memset(&aStruct,0,sizeof(aStruct));
	aStruct.lStructSize = sizeof(aStruct);
	aStruct.hwndOwner = ((MSWindow*)theParent)->GetHWND();

	std::string aTitle = mTitle;
	aStruct.lpstrTitle = aTitle.c_str();
	aStruct.lpfnHook = MyOFNHookProc;
	gFileDialog = this;
	gGotDir = false;
	gLastPath.erase();

	std::string aFilterStr;
	for(FilterList::iterator anItr = mFilterList.begin(); anItr!=mFilterList.end(); ++anItr)
	{
		const FilterPair &aPair = *anItr;
		aFilterStr.append(aPair.mDesc);
		aFilterStr.append(1,'\0');
		aFilterStr.append(aPair.mFilter);
		aFilterStr.append(1,'\0');
	}
	aFilterStr.append(2,'\0');
	aStruct.lpstrFilter = aFilterStr.c_str();

	aFileName[0] = '\0';
	std::string strFileName = mInitialFileName;
	strcpy(aFileName, strFileName.c_str());
	aStruct.lpstrFile = aFileName;
	aStruct.nMaxFile = MAX_FILE_NAME-1;

	if(mInitialDir.length())
	{
		std::string strInitialDir = mInitialDir;
		strcpy(aInitialDir, strInitialDir.c_str());
		aStruct.lpstrInitialDir = aInitialDir;
	}

	aStruct.Flags = OFN_EXPLORER;
	if(mAllowMultiple)
		aStruct.Flags |= OFN_ALLOWMULTISELECT;
	if(mWarnIfExists)
		aStruct.Flags |= OFN_OVERWRITEPROMPT;
	if(mFileMustExist)
		aStruct.Flags |= OFN_FILEMUSTEXIST;
	if(mNoChangeDir)
		aStruct.Flags |= OFN_NOCHANGEDIR;
	if(mGetDir || mOkTitle.length()!=0)
		aStruct.Flags |= OFN_ENABLEHOOK;

	BOOL aResult = FALSE;
	if(mIsSave)
		aResult = GetSaveFileNameA(&aStruct);
	else
		aResult = GetOpenFileNameA(&aStruct);

	gFileDialog = NULL;
	if(gGotDir) // user just selected a directory
		strcpy(aFileName,gLastPath.c_str());
	else if(!aResult)
		return false;
	else if(mGetDir) // strip filename off the folder
	{
		WONFile aFile(aFileName);
		string aFileNameStr = aFile.GetFileDirectory();
		strcpy(aFileName,aFileNameStr.c_str());
	}

	if(mAllowMultiple)
	{
		const char *aPtr = aFileName;
		GUIString aDirectory = aPtr;
		aPtr+=aDirectory.length()+1;
		while(true)
		{
			GUIString aStr = aPtr;
			if(aStr.length()==0)
			{
				if(mFileList.empty()) // only one file
					mFileList.push_back(aDirectory);
			
				break;
			}
			else
			{
				GUIString aFullPath = aDirectory;
				aFullPath.append('/');
				aFullPath.append(aStr);
				mFileList.push_back(aFullPath);
			}

			aPtr += aStr.length() + 1;
		}
	}
	else
		mFileList.push_back(aFileName);

	return true;
#else
	return false;
#endif	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

}
