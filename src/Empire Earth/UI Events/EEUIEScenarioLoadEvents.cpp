// C:\EEX\Empire Earth\UI Events\EEUIEScenarioLoadEvents.cpp (file name inferred: no __FILE__; class prefixes
// EEUIESCL/SCLE). The scenario load screen's events: 0x1e853a..0x1e8a24 (the SIE unit's deque COMDAT ends at
// 0x1e853a; the SPE unit starts at 0x1e8a24). EEUIESCLLoadScreen's ctor and dtor are out of line after its first
// user (Open), the ??_G between them. Unit flags: the exe module set.
#include <windows.h>
#include <mbstring.h>
#include <algorithm>
#include <string>
#include <list>

#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GESurface.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "GEWindow.h"
#include "lle/GEScreen.h"
#include "lle/GEViewport.h"
#include "lle/GEVertexArray.h"
#include "lle/GERasterizer.h"
#include "lle/GERhombus.h"
#include "lle/GEBitmap.h"
#include "lle/FSFileSpec.h"
#include "lle/FSArchive.h"
#include "lle/FSPath.h"
#include "lle/TMPoll.h"
#include "lle/TSCriticalSection.h"
#include "lle/TSScheduler.h"
#include "lle/SESample.h"
#include "lle/SESMInstanceNode.h"
#include "lle/SESoundManager.h"
#include "lle/Input System.h"
#include "lle/ISKeyboard.h"
#include "lle/ISMouse.h"
#include "lle/UIControl.h"
#include "lle/UIEvent.h"
#include "lle/UIEntityGroup.h"
#include "lle/UIBackground.h"
#include "lle/UIFontTable.h"
#include "lle/UIHotKey.h"
#include "lle/UIIMECandidate.h"
#include "lle/UIForm.h"
#include "lle/UIManager.h"
#include "lle/UIButton.h"
#include "lle/NEMessage.h"
#include "lle/NESynchronousCommunications.h"
#include "lle/GELine.h"
#include "lle/UITextLabel.h"
#include "lle/UIListBox.h"
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "lle/UILBString.h"
#include "lle/UIFormManager.h"
#include "lle/UResourceFile.h"
#include "lle/UIScrollBar.h"
#include "lle/UIFileListBox.h"
#include "lle/FSRestorable.h"
#include "ee/EEStringIDs.h"
#include "ee/EmpireEarth.h"
#include "ee/EEUserInterface.h"
#include "ee/EEUIFMainGame.h"
#include "ee/EEUIManager.h"
#include "ee/EEGameOptions.h"
#include "ee/EESMMediaStrings.h"
#include "ee/EEPSaveGameHeader.h"
#include "ee/EESaveGameFiles.h"
#include "ee/EEUISCIFormManager.h"
#include "ee/EEUIEScenarioLoadEvents.h"
EEUIFMainGame* gSavedMainGameForm;          // 0x563144 (.bss among the UI events' statics; ee/EmpireEarth.h)

// FUNCTION: EXE 0x5e853a
// Fills the scenario load list with the directory's files of the extension and preselects the last scenario
long EEUIESCLLoadScreen::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    FSFileSpec theDirectory;
    FSPath* thePath = new FSPath;
    UWideString theName;
    UString theVolume(gStartupVolume);
    UString theStartup(gStartupDirectory);
    UIForm* theForm = gUIManager->GetFormByID(kFormScenarioLoad);
    unsigned long theIndex = 0;
    UIFileListBox* theList = static_cast<UIFileListBox*>(theForm->GetControl(kScenarioLoadLoadFileList));
    if (theList) {
        theList->AddValidExtension(this->mExtension);
        theDirectory.SetVolume(gStartupVolume);
        theDirectory.SetDirectory(gStartupDirectory + this->mDirectory);
        thePath->AddDirectory(theDirectory, true);
        theList->SetSearchPaths(thePath);
        theList->CreateEntries(1);
        theName = gLoadScenarioName;
        if (theList->FindString(theName, theIndex) == true) {
            theList->ChangeSelectionIndex(theIndex, true, false);
            theList->SetScrollBarCurrentRange(theIndex);
        }
    }
    thePath->Dereference();
    return kErrorOK;
}

// FUNCTION: EXE 0x5e86b6
// Reads the selected scenario's header and opens the scenario intro with its media; reports an invalid file
long EEUIESCLELoadFile::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    EEPSaveGameHeader theHeader;
    UWideString theMessage;
    UWideString theText;
    UString theName;
    UString theFileName;
    FSFileSpec* theFile = NULL;
    UIFileListBox* theList = static_cast<UIFileListBox*>(inForm->GetControl(kScenarioLoadLoadFileList));
    UIForm* theForm = inForm->GetManager()->GetFormByID(kFormScenarioIntro);
    unsigned long theSelection;
    if (theList->GetCurrentSelection(theSelection) == kErrorOK) {
        theName = theList->GetData(theSelection);
        theFileName = theName + this->mExtension;
        if (EEOpenSaveGameFile(theFileName, this->mDirectory, theFile) == kErrorOK) {
            theHeader.SetMajorVersion(kSaveGameMajorVersion);
            theHeader.SetMinorVersion(kSaveGameMinorVersion);
            if (theHeader.LoadFromFile(*theFile) == kErrorOK) {
                EEUISCIFormManager* theManager = EEUISCIFormManager::Get();
                theManager->SetMediaStrings(theHeader.GetMediaStrings());
                theManager->SetFile(theFile);
                theManager->SetIsCampaign(false);
                gLoadScenarioName = theFile->GetFile().GetIndexedSubstring(0, theFile->GetFile().GetLength() - 5);
                gUserInterface->OpenForm(theForm);
            } else {
                gResource->GetString(IDS_SAVE_GAME_ISNT_VALID, theMessage);
                gUIManager->DisplayMessage(theMessage, true, NULL, 0, true);
                if (theFile)
                    delete theFile;
            }
        } else {
            gResource->GetString(IDS_SAVE_GAME_ISNT_VALID, theMessage);
            gUIManager->DisplayMessage(theMessage, true, NULL, 0, true);
        }
    }
    return kErrorOK;
}

// FUNCTION: EXE 0x5e88be
// Cancels scenario loading: returns to the saved main game form, else pops the form
long EEUIESCLECancel::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    if (gSavedMainGameForm)
        gUserInterface->OpenForm(gSavedMainGameForm);
    else
        gUIManager->Pop();
    return kErrorOK;
}

// FUNCTION: EXE 0x5e88e6
// Posts the event that lists data\Scenarios with the scenario extension
long EEUIESCLEOpen::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    UString theExtension;
    UString theDirectory;
    theExtension = gScenarioExtension;
    theDirectory = UString("data\\") + UString("Scenarios\\");
    UIEvent* theEvent = new EEUIESCLLoadScreen(theExtension, theDirectory);
    gUserInterface->PostEvent(theEvent, NULL);
    theEvent->Dereference();
    return kErrorOK;
}

// FUNCTION: EXE 0x5e899b
// Creates the scenario load-screen event for an extension and directory
// (then its ??_G and dtor)

// FUNCTION: EXE 0x5e89dd
// Destroys the scenario load-screen event and frees it when asked (scalar deleting destructor)

// FUNCTION: EXE 0x5e89f9
// Destroys the scenario load-screen event
