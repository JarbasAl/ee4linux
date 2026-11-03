// C:\EEX\Empire Earth\Scenario Editor\EESEEvents.cpp (file name inferred: no __FILE__; class prefix EESEE). The
// scenario editor's UI events: 0x16f892..0x16ff5f, the ProcessEvents in class order (ee/EESEEvents.h), with the
// inline COMDATs they are the first users of (EEUIFScenarioEditor::GetTriggerScreen 0x16fa2f, GetSelectedUnitCount
// 0x16fabb, EEACenterCamera's ctor 0x16fad4, EESETSTrigger's index steps 0x16fdfc..0x16fe41) and the static combo
// helper 0x16fe85 after its first user. After RTSPlayer.cpp (..0x16f892), before EESEIAExport.cpp's Virtual4
// 0x16ff5f. Unit flags: the exe module set.
#include <windows.h>
#include <mbstring.h>
#include <algorithm>

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
#include "lle/UIEditBox.h"
#include "lle/UIListBox.h"
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "lle/UILBString.h"
#include "lle/UIComboBox.h"
#include "lle/UIFileComboBox.h"
#include "lle/UIFormManager.h"
#include "lle/UResourceFile.h"

#include "lle/UISlider.h"
#include "lle/UICheckBox.h"

#include "ee/EmpireEarth.h"
#include "ee/EEAction.h"
#include "ee/EEUIManager.h"
#include "ee/EEUserInterface.h"
#include "ee/EECivilization.h"
#include "ee/EEUIFScenarioEditor.h"
#include "ee/EEUIFSEMainGroup.h"
#include "ee/EEAreaUtilities.h"
#include "ee/EEScenarioManager.h"
#include "ee/RTSWorld.h"
#include "ee/EECameraControl.h"
#include "ee/EEWorld.h"
#include "ee/EEACenterCamera.h"
#include "ee/EESETSBaseObject.h"
#include "ee/EESETSTrigger.h"
#include "ee/EESETSCondition.h"
#include "ee/EESETSEffect.h"
#include "ee/EESETSObject.h"
#include "ee/EESETSArea.h"
#include "ee/EESETriggerScreen.h"
#include "ee/EESETSEScenarioComboBox.h"
#include "ee/EESEEvents.h"
#include "ee/EESEEConditionPlayerResource.h"
#include "ee/EESEEChangeEffectAction.h"
#include "ee/EEStringIDs.h"
void FillScenarioComboBox(EESETSEScenarioComboBox* inComboBox, UITextLabel* inLabel, bool inAreas);

// FUNCTION: EXE 0x56f892
// Switches the trigger tab to the sub-screen of the clicked tab button
long EESEEShowTab::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    static_cast<EEUIFScenarioEditor*>(inForm)->GetTriggerScreen()->ShowTab(static_cast<UIControl*>(inEntity)->GetId());
    return kErrorOK;
}

// FUNCTION: EXE 0x56f8b3
// Loads the entry picked in the trigger tab's list
long EESEESelectEntry::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    static_cast<EEUIFScenarioEditor*>(inForm)->GetTriggerScreen()->SelectEntry();
    return kErrorOK;
}

// FUNCTION: EXE 0x56f8ca
// Adds a new entry to the trigger tab's current list
long EESEENewEntry::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    static_cast<EEUIFScenarioEditor*>(inForm)->GetTriggerScreen()->NewEntry();
    return kErrorOK;
}

// FUNCTION: EXE 0x56f8e1
// Duplicates the trigger tab's selected entry
long EESEEDuplicateEntry::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    static_cast<EEUIFScenarioEditor*>(inForm)->GetTriggerScreen()->DuplicateEntry();
    return kErrorOK;
}

// FUNCTION: EXE 0x56f8f8
// Deletes the trigger tab's selected entry
long EESEEDeleteEntry::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    static_cast<EEUIFScenarioEditor*>(inForm)->GetTriggerScreen()->DeleteEntry();
    return kErrorOK;
}

// FUNCTION: EXE 0x56f90f
// Moves the trigger tab's selected entry one place up
long EESEEIncPriority::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    static_cast<EEUIFScenarioEditor*>(inForm)->GetTriggerScreen()->IncPriority();
    return kErrorOK;
}

// FUNCTION: EXE 0x56f926
// Moves the trigger tab's selected entry one place down
long EESEEDecPriority::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    static_cast<EEUIFScenarioEditor*>(inForm)->GetTriggerScreen()->DecPriority();
    return kErrorOK;
}

// FUNCTION: EXE 0x56f93d
// Sorts the trigger tab's current list alphabetically
long EESEEAlphaSort::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    static_cast<EEUIFScenarioEditor*>(inForm)->GetTriggerScreen()->AlphaSort();
    return kErrorOK;
}

// FUNCTION: EXE 0x56f954
// Toggles the debug ids shown in the trigger tab's names and reloads the selected entry
long EESEEShowDebugIDs::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    gWorld.mScenarioManager->mShowDebugIDs = !gWorld.mScenarioManager->mShowDebugIDs;
    static_cast<EEUIFScenarioEditor*>(inForm)->GetTriggerScreen()->SelectEntry();
    return kErrorOK;
}

// FUNCTION: EXE 0x56f97d
// Stores the trigger tab's entry after Enter in a name or text box
long EESEESelectName::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    static_cast<EEUIFScenarioEditor*>(inForm)->GetTriggerScreen()->SelectName();
    return kErrorOK;
}

// FUNCTION: EXE 0x56f994
// Switches the object screen between picking units on the map and its other modes; leaving map mode
// clears the editor's unit selection (twice) and the object's unit list
long EESEEObjectDetail::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    EEUIFScenarioEditor* theEditor = static_cast<EEUIFScenarioEditor*>(inForm);
    unsigned long theID = static_cast<UIControl*>(inEntity)->GetId();
    EESETSObject* theObject = static_cast<EESETSObject*>(theEditor->GetTriggerScreen()->GetScreen(kSETriggerScreenObject));
    theObject->mIsSelectOnMap = theID == kSETSObjectUnitsRadio;
    theObject->Refresh();
    if (!static_cast<EESETSObject*>(theEditor->GetTriggerScreen()->GetScreen(kSETriggerScreenObject))->GetIsSelectOnMap()) {
        gUIManager->mScenarioEditorForm->ClearSelection();
        static_cast<EESETSObject*>(theEditor->GetTriggerScreen()->GetScreen(kSETriggerScreenObject))->ClearUnitList();
        theEditor->ClearSelection();
        static_cast<EESETSObject*>(theEditor->GetTriggerScreen()->GetScreen(kSETriggerScreenObject))->mUnitIDs.clear();
    }
    static_cast<EESETSObject*>(theEditor->GetTriggerScreen()->GetScreen(kSETriggerScreenObject))->UpdateSelection();
    return kErrorOK;
}

// FUNCTION: EXE 0x56fa2f
// Returns the scenario editor's trigger tab
// EEUIFScenarioEditor::GetTriggerScreen (inline, ee/EEUIFScenarioEditor.h; refused in 0x56f994/0x56fcd6)

// FUNCTION: EXE 0x56fa39
// Passes a check box change to the trigger tab's object screen
long EESEEObjectCheck::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    ((EESETSObject*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenObject))->CheckChanged(inEntity);
    return kErrorOK;
}

// FUNCTION: EXE 0x56fa57
// Centers the camera on the selected unit picked in the object screen's unit list
long EESEEObjectUnitSelect::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    EEUIFScenarioEditor* theEditor = static_cast<EEUIFScenarioEditor*>(inForm);
    unsigned long theSelection;
    if (static_cast<UILBString*>(inEntity)->GetCurrentSelection(theSelection) == kErrorOK &&
        theSelection < theEditor->GetSelectedUnitCount()) {
        EEACenterCamera* theAction = new EEACenterCamera;
        theAction->SetUnit(theEditor->GetSelectedUnit(theSelection));
        theAction->Dereference();
    }
    return kErrorOK;
}

// FUNCTION: EXE 0x56fabb
// Returns the number of units selected in the scenario editor
// EEUIFScenarioEditor::GetSelectedUnitCount (inline, ee/EEUIFScenarioEditor.h)

// FUNCTION: EXE 0x56fad4
// Creates a center-camera action with no target yet
// EEACenterCamera::EEACenterCamera (inline, ee/EEACenterCamera.h)

// FUNCTION: EXE 0x56faf7
// Puts the area screen in rectangle mode when the combo box's first entry is picked, else leaves it
long EESEEAreaCheck::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    unsigned long theSelection;
    static_cast<UIComboBox*>(inEntity)->GetCurrentSelection(theSelection);
    ((EESETSArea*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenArea))->SetAreaMode(theSelection == 0);
    return kErrorOK;
}

// FUNCTION: EXE 0x56fb20
// Centers the camera on the selected area, if it has a center
long EESEECenterCameraOnArea::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    GE2DPoint theCenter;
    EESETSArea* theArea = (EESETSArea*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenArea);
    if (theArea->GetCenter(theCenter)) {
        EEACenterCamera* theAction = new EEACenterCamera;
        theAction->SetLocation((long)theCenter.GetX(), (long)theCenter.GetY());
        theAction->Dereference();
    }
    return kErrorOK;
}

// FUNCTION: EXE 0x56fb8d
// Switches the condition screen to the type picked in its type combo box
long EESEEChangeConditionType::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    ((EESETSCondition*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenCondition))->ChangeConditionType();
    return kErrorOK;
}

// FUNCTION: EXE 0x56fba7
// Shows the amount box when 'use amount' (0x91a) is checked, else the other player's combo and attribute
// combo (the local names do not match the control ids 0x910 amount, 0x92f player, 0x935 attribute)
long EESEEConditionPlayerResource::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    UIControl* theResource = inForm->GetControlById(0x910);
    UIControl* theAmount = inForm->GetControlById(0x92f);
    UIControl* theCompare = inForm->GetControlById(0x935);
    if (static_cast<UIControl*>(inEntity)->GetId() == 0x91a) {
        theResource->SetDisplayMode(kDisplayModeShown);
        theAmount->SetDisplayMode(kDisplayModeHidden);
        theCompare->SetDisplayMode(kDisplayModeHidden);
    } else {
        theResource->SetDisplayMode(kDisplayModeHidden);
        theAmount->SetDisplayMode(kDisplayModeShown);
        theCompare->SetDisplayMode(kDisplayModeShown);
    }
    return kErrorOK;
}

// FUNCTION: EXE 0x56fc20
// Switches the effect screen to the picked effect type
long EESEEChangeEffectType::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    ((EESETSEffect*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenEffect))->ChangeEffectType();
    return kErrorOK;
}

// FUNCTION: EXE 0x56fc3a
// Switches the effect screen to the picked effect action
long EESEEChangeEffectAction::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    ((EESETSEffect*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenEffect))->ChangeEffectAction();
    return kErrorOK;
}

// FUNCTION: EXE 0x56fc54
// Applies the and/or picked in one of the trigger screen's operator combo boxes
long EESEETriggerOperator::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    ((EESETSTrigger*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenTrigger))->ChangeOperator(static_cast<UIControl*>(inEntity)->GetId());
    return kErrorOK;
}

// FUNCTION: EXE 0x56fc78
// Shows the condition duration box and its seconds label while the timer check box is checked
long EESEETriggerConditionTimer::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    if (static_cast<UICheckBox*>(inEntity)->GetIsChecked()) {
        inForm->GetControlById(kSETSTriggerConditionDurationEditBox)->SetDisplayMode(kDisplayModeShown);
        inForm->GetControlById(kSETSTriggerSecondsLabel)->SetDisplayMode(kDisplayModeShown);
    } else {
        inForm->GetControlById(kSETSTriggerConditionDurationEditBox)->SetDisplayMode(kDisplayModeHidden);
        inForm->GetControlById(kSETSTriggerSecondsLabel)->SetDisplayMode(kDisplayModeHidden);
    }
    return kErrorOK;
}

// FUNCTION: EXE 0x56fcd6
// Scrolls the trigger screen's shown condition or effect pair back or forward by one
long EESEETriggerChangeIndex::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    switch (static_cast<UIControl*>(inEntity)->GetId()) {
    case kSETSTriggerPrevConditionsButton:
        ((EESETSTrigger*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenTrigger))->DecrementA4();
        break;
    case kSETSTriggerNextConditionsButton:
        ((EESETSTrigger*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenTrigger))->IncrementA4();
        break;
    case kSETSTriggerPrevEffectsButton:
        ((EESETSTrigger*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenTrigger))->DecrementA8();
        break;
    case kSETSTriggerNextEffectsButton:
        ((EESETSTrigger*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenTrigger))->IncrementA8();
        break;
    }
    return kErrorOK;
}

// FUNCTION: EXE 0x56fd46
// Removes the condition or effect of the clicked remove button from the trigger
long EESEETriggerRemove::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    ((EESETSTrigger*)(static_cast<EEUIFScenarioEditor*>(inForm))->GetTriggerScreen()->GetScreen(kSETriggerScreenTrigger))->RemoveEntry(static_cast<UIControl*>(inEntity)->GetId());
    return kErrorOK;
}

// FUNCTION: EXE 0x56fd6a
// Prints the slider value times the scale plus the offset into the event's label
long EESEESliderChange::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    UWideString theText;
    sprintf(theText, this->mFormat, static_cast<UISlider*>(inEntity)->GetCurrentRangeValue() * this->mScale + this->mOffset);
    this->mLabel->SetText(theText);
    return kErrorOK;
}

// FUNCTION: EXE 0x56fdbf
// Shows the event's controls while the check box is checked, else hides them
long EESEECheckDisplay::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    unsigned long theMode;
    theMode = static_cast<UICheckBox*>(inEntity)->GetIsChecked() ? 1 : 0;
    for (unsigned long i = 0; i < this->mControls.size(); i++)
        this->mControls[i]->SetDisplayMode(theMode);
    return kErrorOK;
}

// FUNCTION: EXE 0x56fdfc
// Stores the trigger and shows the condition pair one step back
// EESETSTrigger::DecrementA4 (inline, ee/EESETSTrigger.h)

// FUNCTION: EXE 0x56fe13
// Stores the trigger and shows the effect pair one step back
// EESETSTrigger::DecrementA8 (inline, ee/EESETSTrigger.h)

// FUNCTION: EXE 0x56fe2a
// Stores the trigger and shows the condition pair one step forward
// EESETSTrigger::IncrementA4 (inline, ee/EESETSTrigger.h)

// FUNCTION: EXE 0x56fe41
// Stores the trigger and shows the effect pair one step forward
// EESETSTrigger::IncrementA8 (inline, ee/EESETSTrigger.h)

// FUNCTION: EXE 0x56fe58
// Refills the scenario combo box with area names (area control clicked) or object names and picks the first
long EESEEUseObjectArea::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    FillScenarioComboBox(this->mComboBox, this->mLabel, inEntity == this->mAreaControl);
    this->mComboBox->SetCurrentSelection(0);
    return kErrorOK;
}

// FUNCTION: EXE 0x56fe85
// Fills a scenario combo box with the area or object names and titles its label 'Area' or 'Object'
void FillScenarioComboBox(EESETSEScenarioComboBox* inComboBox, UITextLabel* inLabel, bool inAreas)
{
    UWideString theTitle;
    std::vector<UWideString> theNames;
    if (inAreas == true) {
        gWorld.mScenarioManager->GetAreaNames(theNames);
        gResource->GetString(IDS_AREA, theTitle);
        inComboBox->mMode = kSETriggerScreenArea;
    } else {
        gWorld.mScenarioManager->GetObjectNames(theNames);
        gResource->GetString(IDS_TRIGGER_SCREEN_TAB_OBJECT, theTitle);
        inComboBox->mMode = kSETriggerScreenObject;
    }
    inLabel->SetText(theTitle);
    inComboBox->DeleteAllEntries();
    for (unsigned long i = 0; i < theNames.size(); i++)
        inComboBox->AppendLine(theNames[i]);
}
