#ifndef EE_WONAPI_WONLOBBYBORDERCOMPONENT_H
#define EE_WONAPI_WONLOBBYBORDERCOMPONENT_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Complete genuine SDK owner: eight retained children and eight borrowed views.
// Source-only identity; no opaque allocation, field guesses or handwritten implicit lifetime.
class BorderComponent : public Container
{
public:
	enum BorderPos
	{
		Pos_LeftTop = 0, Pos_RightTop, Pos_LeftBottom, Pos_RightBottom,
		Pos_Left, Pos_Top, Pos_Right, Pos_Bottom, 
	};

protected:
	SmartPtr<Component> mComponents[8];
	Component *mLeftTop, *mRightTop, *mLeftBottom, *mRightBottom, *mLeft, *mTop, *mRight, *mBottom;
	bool mControlsAdded;

public:
	virtual void Virtual08();

public:
	BorderComponent();
	void SetComponent(BorderPos thePos, Component *theComponent, int theWidth = -1, int theHeight = -1);
	void AddControls();
};

}
#endif
