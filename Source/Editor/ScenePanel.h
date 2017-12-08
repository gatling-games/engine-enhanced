#pragma once

#include "Editor/EditorPanel.h"

class GameObject;

class ScenePanel : public EditorPanel
{
public:
    // EditorPanel overrides
    virtual std::string name() const { return "Scene Panel"; }
    virtual void drawMenu(const std::string menuName) override;
    virtual void draw();

private:
    // Draws the tree node for the given game objects.
    // This includes any child objects.
    void drawNode(GameObject* gameObject);

    // Called when a gameobject is selected in the tree.
    void gameObjectSelected(GameObject* gameObject);
};