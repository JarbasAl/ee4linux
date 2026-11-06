#ifndef EE_WONAPI_WONLOBBYCOMBOBOXCONFIG_H
#define EE_WONAPI_WONLOBBYCOMBOBOXCONFIG_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Borrowed original-owned config; no constructor/destructor/sizeof/new authority.
class ComboBoxConfig : public ContainerConfig {
public:
    virtual bool Virtual24(const std::string&); //3821b0
    virtual bool Virtual28(const std::string&,ComponentConfig*&,Component*&); //382070 external original
    virtual void Virtual34(Component*); //original SetComponent external
    ComboBox* mComboBox; //98
};
}
#endif
