// Native simple-control configuration handlers; original control lifetimes remain external.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbySimpleConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00794ba0
bool RectangleComponentConfig::Virtual24(const std::string& key)
{
    if(key=="SOLIDCOLOR") { int color=Fn385510(); mRectangleComponent->SetSolidColor(color); }
    else if(key=="OUTLINECOLOR") { int color=Fn385510(); mRectangleComponent->SetOutlineColor(color); }
    else return ComponentConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x00794c10
bool RectangleComponent3DConfig::Virtual24(const std::string& key)
{
    if(key=="SOLIDCOLOR") { int color=Fn385510(); mRectangleComponent3D->SetSolidColor(color); }
    else if(key=="LIGHTCOLOR") { int color=Fn385510(); mRectangleComponent3D->SetLightColor(color); }
    else if(key=="DARKCOLOR") { int color=Fn385510(); mRectangleComponent3D->SetDarkColor(color); }
    else if(key=="UP") { bool up=Fn3837F0(); mRectangleComponent3D->SetIsUp(up); }
    else if(key=="THICKNESS") { int thickness=ReadInt(); mRectangleComponent3D->SetThickness(thickness); }
    else return ComponentConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x00794cf0
bool ImageComponentConfig::Virtual24(const std::string& key)
{
    if(key=="IMAGE") mImageComponent->VirtualB0(ReadImage(false));
    else if(key=="TILE") { bool tile=Fn3837F0(); mImageComponent->mTile=tile; }
    else if(key=="STRETCH") { bool stretch=Fn3837F0(); mImageComponent->mStretch=stretch; }
    else return ComponentConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x00794e50
bool ScreenContainerConfig::Virtual24(const std::string& key)
{
    if(key=="ADDSCREEN") mScreenContainer->AddScreen(ReadComponent());
    else if(key=="SHOWSCREEN") mScreenContainer->ShowScreen(ReadComponent(),true);
    else return ContainerConfig::Virtual24(key);
    return true;
}
}


#include "ee/wonapi/WONLobbyAnimationComponent.h"
namespace WONAPI {
void RectangleComponentConfig::Virtual34(Component *theComponent)
{
	ComponentConfig::Virtual34(theComponent);
	mRectangleComponent = (RectangleComponent*)theComponent;
}

void RectangleComponent3DConfig::Virtual34(Component *theComponent)
{
	ComponentConfig::Virtual34(theComponent);
	mRectangleComponent3D = (RectangleComponent3D*)theComponent;
}

void ImageComponentConfig::Virtual34(Component *theComponent)
{
	ComponentConfig::Virtual34(theComponent);
	mImageComponent = (ImageComponent*)theComponent;
}

void AnimationComponentConfig::Virtual34(Component *theComponent)
{
	ComponentConfig::Virtual34(theComponent);
	mAnimationComponent = (AnimationComponent*)theComponent;
}

void ScreenContainerConfig::Virtual34(Component *theComponent)
{
	ComponentConfig::Virtual34(theComponent);
	mScreenContainer = (ScreenContainer*)theComponent;
}

bool AnimationComponentConfig::Virtual24(const std::string &theInstruction)
{
	if(ComponentConfig::Virtual24(theInstruction))
		return true;

	return false;
}

// FUNCTION: WON 0x0079b030
Component* RectangleComponentConfig::CompFactory()
{
    return new RectangleComponent;
}

// FUNCTION: WON 0x0079b0e0
Component* RectangleComponent3DConfig::CompFactory()
{
    return new RectangleComponent3D;
}

// FUNCTION: WON 0x0079b190
Component* ImageComponentConfig::CompFactory()
{
    return new ImageComponent;
}

// FUNCTION: WON 0x0079b240
Component* AnimationComponentConfig::CompFactory()
{
    return new AnimationComponent;
}

// FUNCTION: WON 0x0079b2f0
Component* ScreenContainerConfig::CompFactory()
{
    return new ScreenContainer;
}
}
