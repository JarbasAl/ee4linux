// C:\EEX\Empire Earth\User Interface Thread\EEUICChatLine.h (RECONSTRUCTION; file name inferred). One line of the
// in-game chat display: a bold sender name and the message, each drawn with a shadow copy, faded
// out by a timer (RTTI EEUICChatLine > UIControl; vtable 0x449df8; sizeof 0xd0, the user-defined control factory
// 0x27496f). EEUICChatLine.cpp is 0x257223..0x257881 (ctor, MouseMove, ??_G, dtor, then the out-of-line members in
// alphabetical order). The other inline overrides are ICF-folded elsewhere (the mouse and keyboard slots 0x1ad67e,
// the tab slots 0x1720c6). Names are provisional. Requires lle/UIControl.h, lle/UIControlsTable.h, lle/GEText.h,
// lle/TMPoll.h.
#ifndef EE_EEUICCHATLINE_H
#define EE_EEUICCHATLINE_H

class EEUICChatLine : public UIControl {
public:
    EEUICChatLine(const GERectangle& inRectangle, UIControlsTable& inTable, UIForm* inForm);   // 0x257223
    virtual ~EEUICChatLine();
    // 0x2574d5: shows another line's text (EEUIFMainGame scrolls the chat lines up: 0x264a68)
    unsigned long Copy(const EEUICChatLine& inLine);
    virtual long CopyData();
    // 0x2576f5: shows a line for inTime; returns the index the message was cut at to fit the control
    unsigned long Display(const GEColor& inColor, const UWideString& inName, const UWideString& inMessage,
                          unsigned long inTime);
    virtual long DrawPolygons();
    virtual long DrawTextA();
    // inline (the main game form's SetChatTop 0x26676e stores +0xc8) (provisional name)
    void SetShadowOffset(unsigned long inOffset) { this->mShadowOffset = inOffset; }
protected:
    virtual long MouseLButtonDown() { return 0; }
    virtual long MouseLButtonUp() { return 0; }
    virtual long MouseMove(const GE2DPoint& inLocation) { return 0; }
    virtual long MouseRButtonDown() { return 0; }
    virtual long MouseRButtonUp() { return 0; }
    virtual void TabbedTo() {}
    virtual void TabbedFrom() {}
    virtual long UpdateKeyboard() { return 0; }

    GERectangle mTextRectangle;             // +0x0a0 the copied draw rectangle moved down by mShadowOffset
    TMPoll* mTimer;                         // +0x0b0
    GEText* mName;                          // +0x0b4 bold
    GEText* mMessage;                       // +0x0b8
    GEText* mDrawnName;                     // +0x0bc copies made by CopyData
    GEText* mDrawnMessage;                  // +0x0c0
    float mOpacity;                         // +0x0c4 the table's background opacity
    unsigned long mShadowOffset;            // +0x0c8
    bool mShowing;                          // +0x0cc
};

#endif
