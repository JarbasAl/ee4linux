// Proposed shared true SDK config contracts; call/reference use only.
// No allocation, implicit lifetime provider or fake fieldless class is introduced here.
#ifndef EE_WONAPI_WONLOBBYCONTROLCONFIG_H
#define EE_WONAPI_WONLOBBYCONTROLCONFIG_H
#include "ee/wonapi/WONLobbyControls.h"
#include "ee/wonapi/WONLobbyScreens.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
#include "ee/wonapi/WONLobbyImageButton.h"
#include "ee/wonapi/WONLobbyCrossPromotion.h"
#include "ee/wonapi/WONLobbyMSConfig.h"
namespace WONAPI {
class ImageTextButton;
class CrossPromotionButton;
class MaxSplitterButton;
class MultiListHeader;
class DirectConnectCtrl : public Container {
public:
    DirectConnectCtrl(); //3bada0,38B emptyctor candidate,zeroquota
    virtual ~DirectConnectCtrl(); //actualownVTstore3bae0f; emptybodycandidatezeroquota
    virtual void InitComponent(ComponentInit& inInit); //3baeb0..3baf1c,108B
    virtual void HandleComponentEvent(ComponentEvent* inEvent); //3baf20..3baffe,222B
    SmartPtr<ServerListCtrl> mConnectionList; //84
    SmartPtr<Button> mAddButton; //88
    SmartPtr<Button> mRemoveButton; //8c
};

class ChatCtrlConfig : public ContainerConfig {
public:
    static ComponentConfig* CfgFactory();
    static Component* CompFactory() { return new ChatCtrl; }
    virtual bool Virtual24(const std::string&); //3b9e10
    virtual void Virtual34(Component*); //3c8380..399, support zeroquota
    ChatCtrl* mChatCtrl; //98, borrowed actual writer3c838e
};

class RoomLabelCtrlConfig : public ContainerConfig {
public:
    static ComponentConfig* CfgFactory();
    static Component* CompFactory() { return new RoomLabelCtrl; }
    virtual bool Virtual24(const std::string&);
    virtual void Virtual34(Component*);
    RoomLabelCtrl* mRoomLabel; //98 borrowed, writer39afb0 beforeContainerConfig34
};

class ServerListCtrlConfig : public ContainerConfig {
public:
    ServerListCtrlConfig(); //3c8360 genuine SDK external constructor, not implemented here
    static ComponentConfig* CfgFactory();
    static Component* CompFactory() { return new ServerListCtrl; }
    virtual bool Virtual24(const std::string&);
    virtual bool Virtual2C(const std::string&,int&);
    virtual void Virtual34(Component*);
    void HandleAddColumn();
    ServerListCtrl* mServerList; //98 borrowed
    MultiListHeader* mHeader; //9c borrowed
    int mNumColumns; //a0
};

class ImageTextButtonConfig : public ImageButtonConfig {
public:
    static ComponentConfig* CfgFactory();
    static Component* CompFactory() { return new ImageTextButton; }
    virtual bool Virtual24(const std::string&);
    virtual void Virtual34(Component*);
    ImageTextButton* mImageTextButton; //7c borrowed, actual RTTI451d64
};

class CrossPromotionButtonConfig : public ImageButtonConfig {
public:
    static ComponentConfig* CfgFactory();
    static Component* CompFactory() { return new CrossPromotionButton; }
    virtual bool Virtual24(const std::string&);
    virtual void Virtual34(Component*);
    CrossPromotionButton* mCrossPromotionButton; //7c borrowed
};

class GameBrowserCtrlConfig : public ContainerConfig {
public:
    static ComponentConfig* CfgFactory();
    static Component* CompFactory() { return new GameBrowserCtrl; }
    virtual bool Virtual24(const std::string&); //3bc450, actual SDK HandleInstruction
    virtual void Virtual34(Component* component) { ContainerConfig::Virtual34(component); mBrowser=static_cast<GameBrowserCtrl*>(component); } //genuine SDK inline SetComponent
    GameBrowserCtrl* mBrowser; //98, borrowed
};

class GameUserListCtrlConfig : public ContainerConfig {
public:
    static ComponentConfig* CfgFactory();
    static Component* CompFactory() { return new GameUserListCtrl; }
    virtual bool Virtual24(const std::string&); //3bda70, actual SDK HandleInstruction
    virtual void Virtual34(Component* component) { ContainerConfig::Virtual34(component); mUserList=static_cast<GameUserListCtrl*>(component); } //genuine SDK inline SetComponent
    GameUserListCtrl* mUserList; //98, borrowed
};

class UserListCtrlConfig : public ContainerConfig {
public:
    static ComponentConfig* CfgFactory();
    static Component* CompFactory() { return new UserListCtrl; }
    virtual bool Virtual24(const std::string&); //3cb770, actual SDK HandleInstruction
    virtual void Virtual34(Component* component) { ContainerConfig::Virtual34(component); mUserList=static_cast<UserListCtrl*>(component); } //genuine SDK inline SetComponent
    UserListCtrl* mUserList; //98, borrowed
};

class MaxSplitterButtonConfig : public MSArrowButtonConfig {
public:
    static ComponentConfig* CfgFactory();
    static Component* CompFactory();
    virtual bool Virtual24(const std::string&); //3c9680, actual SDK HandleInstruction
    virtual void Virtual34(Component*); //3658a0, actual SDK SetComponent
    MaxSplitterButton* mButton; //80, borrowed
};

class BrowserComponentConfig : public ContainerConfig {
public:
    static void Init(ResourceConfigTable*); //3fab70, external true SDK setup provider
    static ComponentConfig* CfgFactory(); //3facb0, external provider only
    static Component* CompFactory(); //3fac50, external provider only; no BrowserComponent allocation authority
    virtual bool Virtual24(const std::string&); //3fa8a0, actual SDK HandleInstruction
    virtual bool Virtual28(const std::string&,ComponentConfig*&,Component*&); //3fa7e0, GetPredefinedComponent
    virtual void Virtual34(Component*); //38f960, actual SetComponent
    BrowserComponent* mBrowserComponent; //98, borrowed; complete config9c, no lifetime implementation
};
}
#endif
