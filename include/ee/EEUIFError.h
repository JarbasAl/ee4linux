// C:\EEX\Empire Earth\User Interface Thread\EEUIFError.h (RECONSTRUCTION; file name inferred).
// The game's message box form (RTTI EEUIFError > UIForm; vtable 0x44a770; sizeof 0x1bc: EEUIManager's `new`).
// EEUIFError.cpp is 0x260c27..0x2624a6 (its button classes EEUIFEBOK/EEUIFEBCancel live in the .cpp). Layout from the
// ctor 0x260ffb (its ESVERIFY strings name mOk/mNo/mCancel/mError/mEditBox) and the dtor 0x2614b7. Names of the
// Display*/Layout* member names are provisional (by the mode they set). Requires lle/UIForm.h and ee/EEUIManager.h.
#ifndef EE_EEUIFERROR_H
#define EE_EEUIFERROR_H

class EEUIFEBOK;
class EEUIFEBCancel;
class EEUIBGenericBackground;
class UITextLabel;
class UIEditBox;

// what a button hands to the UI thread when the form closes (EEUIFError::Close news one: 8 bytes, no ctor;
// provisional name)
struct EEUIFErrorCallbackData {
    EEUIFErrorCallback mCallback;           // +0x00
    void* mData;                            // +0x04
};

class EEUIFError : public UIForm {
public:
    EEUIFError(UIManager* inManager);
    virtual ~EEUIFError();
    // mode 3: an edit box (a password box when inPassword) and OK; OK hands the edit box's text to inCallback
    void DisplayPassword(const UWideString& inMessage, unsigned long inAlignment, EEUIFPasswordCallback inCallback,
                         bool inPassword);   // 0x26151a
    // mode 4: Cancel only; Cancel closes inForm
    void DisplayCancel(const UWideString& inMessage, unsigned long inAlignment, void* inData,
                       EEUIFErrorCallback inCallback, UIForm* inForm);   // 0x26167f
    // mode 0: OK (shown when inShowOk)
    void DisplayMessage(const UWideString& inMessage, unsigned long inAlignment, void* inData,
                        EEUIFErrorCallback inCallback, bool inShowOk);   // 0x261788
    // mode 1: Yes/No
    void DisplayQuestion(const UWideString& inMessage, unsigned long inAlignment, void* inData,
                         EEUIFErrorCallback inYes, EEUIFErrorCallback inNo);   // 0x261894
    // mode 2: Yes/No/Cancel
    void DisplayYesNoCancel(const UWideString& inMessage, unsigned long inAlignment, void* inData,
                            EEUIFErrorCallback inYes, EEUIFErrorCallback inNo);   // 0x2619f9
protected:
    virtual void Close();                   // 0x261c4c (slot 0x38)
    virtual long DrawBackground(const GERectangle& inRectangle);   // 0x261baf (slot 0x18)
    virtual void Open();                    // 0x261bbc (slot 0x48)
private:
    void LayoutCancel();                    // 0x261cda (mode 4)
    void LayoutPassword();                  // 0x261e18 (mode 3)
    void LayoutQuestion();                  // 0x261f9c (mode 1)
    void LayoutYesNoCancel();               // 0x262121 (mode 2)
    void LayoutMessage();                   // 0x2622ed (mode 0)

    EEUIFEBOK* mOk;                         // +0x1a0 (id 1)
    EEUIFEBCancel* mNo;                     // +0x1a4 (id 2)
    EEUIFEBCancel* mCancel;                 // +0x1a8 (id 3)
    UITextLabel* mError;                    // +0x1ac the message
    long mMode;                             // +0x1b0 0..4 (the Display* members)
    EEUIBGenericBackground* mBackground;    // +0x1b4
    UIEditBox* mEditBox;                    // +0x1b8 (id 4)
};

#endif
