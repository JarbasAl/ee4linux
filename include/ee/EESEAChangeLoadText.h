// C:\EEX\Empire Earth\Scenario Editor\EESEAChangeLoadText.h (RECONSTRUCTION; file name inferred; layout proven:
// EESEAChangeLoadText sizeof 0x20 = its new-site size, ctor 0x172226 exact in EESELoadThread.cpp; member names are
// provisional). The action that shows a loading text in the editor (RTTI EESEAChangeLoadText > EEAction; vtable
// 0x442e44, sizeof 0x20). Entirely inline: its first user EESELoadThread.cpp emits the ctor 0x172226 (refused at the
// `new`), Virtual4 0x17224d and ??_G 0x172262. Requires ee/EEAction.h, lle/UWideString.h, ee/EEUIManager.h,
// ee/EECivilization.h, ee/EEUIFScenarioEditor.h, ee/EEUserInterface.h.
#ifndef EE_EESEACHANGELOADTEXT_H
#define EE_EESEACHANGELOADTEXT_H

class EESEAChangeLoadText : public EEAction {
public:
    EESEAChangeLoadText() {}
    virtual ~EESEAChangeLoadText() {}
    virtual void Virtual4() { gUIManager->mScenarioEditorForm->SetLoadText(this->mText); }  // +0x04
    virtual void Virtual8() {}              // +0x08 (the shared empty slot 0x1720c6)

    // inline (EESELoadThread's handlers: `this` read once, the temporary `""` lives until after PostAction,
    // 0x1723ef..0x172418; provisional name)
    void Post(const UWideString& inText) { this->mText = inText; gUserInterface->PostAction(this); }

    UWideString mText;                      // +0x0c the text
};

#endif
