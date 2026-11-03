// Actual seven-entry editor table4c46d0,32-byte records. Nominal names provisional.
// Original builders/readers19eba9/19ef5a/19f615/19e9fb prove this shared family.
#ifndef EE_EESEEDITORTAB_H
#define EE_EESEEDITORTAB_H
struct EESEEditorTab {
    long mGroup;                           //00
    unsigned long mOrder;                  //04
    unsigned long mHelp;                   //08
    float mNormalX, mNormalY;              //0c,10
    float mPressedX, mPressedY;            //14,18
    unsigned char mFlag;                   //1c
    bool mFullView;                        //1d
    bool mBrush;                           //1e
};
extern EESEEditorTab sEditorTabs[7];
#endif
