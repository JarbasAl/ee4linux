// C:\EEX\Low-Level Engine\User Interface\UIEntityGroup.h (RECONSTRUCTION). A form's group of entities that are
// enabled and disabled together. UIEntityGroup.cpp (0x1006e424..) holds Disable/Enable/RemoveEntity; the inline
// members are emitted first by UIBackground.cpp (0x10058f03..0x10058f73). sizeof 0x14 (not polymorphic).
// Requires <vector>, lle/UIEntity.h.
#ifndef LLE_UIENTITYGROUP_H
#define LLE_UIENTITYGROUP_H

#include <vector>

class UIEntity;

class LLE_API UIEntityGroup {
    friend class UIForm;                    // AddControl/RemoveControl call AddEntity/RemoveEntity (private)
public:
    UIEntityGroup() { this->mDisableCount = 1; }
    ~UIEntityGroup() {}
    void Disable();
    void Enable();
    UIEntity* GetEntity(unsigned long inIndex) { return this->mEntities[inIndex]; }
    unsigned long GetEntityCount();
    bool GetIsEnabled() { return this->mDisableCount == 0; }
private:
    long AddEntity(UIEntity* inEntity)
    {
        this->mEntities.push_back(inEntity);
        return kErrorOK;
    }
    long RemoveEntity(UIEntity* inEntity);

    std::vector<UIEntity*> mEntities;       // +0x00
    unsigned long mDisableCount;            // +0x10 0: enabled (1 at construction)
};

// inline, but defined after the class: UIForm.cpp expands it (CopyData, RedrawIntersectControls), while the exported
// body is emitted where it is first called out of line (0x1007ba0b, after UIManager.cpp's code)
inline unsigned long UIEntityGroup::GetEntityCount()
{
    return this->mEntities.size();
}

#endif
