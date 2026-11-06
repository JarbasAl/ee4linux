#ifndef EE_WONAPI_WONLOBBYTEXTIMAGE_H
#define EE_WONAPI_WONLOBBYTEXTIMAGE_H
#include "ee/wonapi/WONLobbyTextAreaConfig.h"
namespace WONAPI {
// Complete genuine SDK owner: existing TextSegmentBase plus retained image/text.
// Native identity unlocated; default construction/destruction remain implicit.
class TextImage : public TextSegmentBase {
public:
    SmartPtr<Image> mImage;
    GUIString mText;
    virtual void Paint(Graphics&,TextArea*);
    virtual void CalcDimensions();
    virtual void GetSelText(TextArea*,GUIString&);
};
}
#endif
