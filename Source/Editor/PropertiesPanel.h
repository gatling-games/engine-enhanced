#pragma once

#include "Editor/EditorPanel.h"

#include "Scene/GameObject.h"

#include "Utils/Singleton.h"

class PropertiesPanel : public EditorPanel, public Singleton<PropertiesPanel>
{
public:
    PropertiesPanel();

    // EditorPanel overrides
    virtual std::string name() const { return "Properties Panel"; }
    virtual void draw();

    // The game object being inspected
    GameObject* current() const { return currentGameObject_; }

    // Changes the gameobject being inspected
    void inspect(GameObject* gameObject);

private:
    GameObject* currentGameObject_;
};
