#ifndef EE_WONAPI_WONLOBBYSIMPLECONFIG_H
#define EE_WONAPI_WONLOBBYSIMPLECONFIG_H
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
namespace WONAPI {
// Complete native RectangleComponent38, genuine SDK two-color scalar owner.
class RectangleComponent : public Component {
    int mSolidColor; //30, genuine SDK own scalar
    int mOutlineColor; //34, genuine SDK own scalar
public:
    RectangleComponent(); //306840 actual default ctor, real Component base
    void SetSolidColor(int); //306870 actual external setter
    void SetOutlineColor(int); //306880 actual external setter
    virtual void Paint(Graphics&); //original external override
};
class RectangleComponentConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //39b090 genuine SDK static API, original external provider only
    static Component* CompFactory(); //39b030 genuine SDK static API, original external provider only
    virtual bool Virtual24(const std::string&);
    virtual void Virtual34(Component*); //original external SetComponent
    RectangleComponent* mRectangleComponent; //78 borrowed
};
class RectangleComponent3DConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //39b140 genuine SDK static API, original external provider only
    static Component* CompFactory(); //39b0e0 genuine SDK static API, original external provider only
    virtual bool Virtual24(const std::string&);
    virtual void Virtual34(Component*); //original external SetComponent
    RectangleComponent3D* mRectangleComponent3D; //78 borrowed
};
class ImageComponentConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //39b1f0 genuine SDK static API, original external provider only
    static Component* CompFactory(); //39b190 genuine SDK static API, original external provider only
    virtual bool Virtual24(const std::string&);
    virtual void Virtual34(Component*); //original external SetComponent
    ImageComponent* mImageComponent; //78 borrowed
};
// Genuine SDK borrowed AnimationComponentConfig7c: RTTI4deb80/VT452a94,
// factory39b2a0 new7c -> external ComponentConfig3825b0 -> ownVT. No construction authority.
class AnimationComponent;
class AnimationComponentConfig : public ComponentConfig {
public:
    virtual bool Virtual24(const std::string&); //394e10 actual18B external base-dispatch/bool normalization
    virtual void Virtual34(Component*); //38f8d0 actual22B shared typed78 store; external
    static ComponentConfig* CfgFactory(); //39b2a0 genuine SDK, original external only
    static Component* CompFactory(); //39b240 genuine SDK, original external only
    AnimationComponent* mAnimationComponent; //78 borrowed; complete7c; implicit lifetime remains original
};
class ScreenContainerConfig : public ContainerConfig {
public:
    static ComponentConfig* CfgFactory(); //39b350 genuine SDK static API, original external provider only
    static Component* CompFactory(); //39b2f0 genuine SDK static API, original external provider only
    virtual bool Virtual24(const std::string&);
    virtual void Virtual34(Component*); //original external SetComponent
    ScreenContainer* mScreenContainer; //98 borrowed
};
}
#endif
