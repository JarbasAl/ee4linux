#ifndef EE_WONAPI_WONLOBBYHTMLANCHORSEGMENT_H
#define EE_WONAPI_WONLOBBYHTMLANCHORSEGMENT_H
#include "ee/wonapi/WONLobbyLineLayout.h"
namespace WONAPI {
// Actual complete18 owner: native339f3a new18, RefCount4=0, retained GUIString14.
// Normal33a0f0/94 natural implicit cleanup releases14 then non-owning base.
class AnchorLineSegment : public LineSegment {
public:
    GUIString mName; //14
    AnchorLineSegment(const GUIString& name):mName(name) {}
    virtual bool PerformInstructions(MultiLineBuilder*); //3099f0/5 actualRET4
    virtual void SetViewport(ScrollAreaViewport*,int,int); //309a00/51 actualRET0c
    virtual char GetVertAlign() { return 2; } //33a0c0 actualfinite3B rawleaf,0quota
};
}
#endif
