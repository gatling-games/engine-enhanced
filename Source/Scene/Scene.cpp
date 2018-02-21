#include "Scene.h"

#include <imgui.h>

#include "SceneManager.h"
#include "GameObject.h"

Scene::Scene(ResourceID resourceID)
    : Resource(resourceID),
    gameObjects_(PropertyTableMode::Reading)
{

}

void Scene::onOpenAction()
{
    SceneManager::instance()->openScene(resourcePath());
}

void Scene::drawEditor()
{
    if (SceneManager::instance()->currentScene() == this)
    {
        ImGui::ColorEdit3("Sky Color", &skyColorTop_.r);
        ImGui::ColorEdit3("Horizon Color", &skyColorBottom_.r);
        ImGui::ColorEdit3("Ambient Light", &ambientLight_.r);
        ImGui::ColorEdit3("Sun Light", &sunColor_.r);

        ImGui::Spacing();
        ImGui::SliderFloat("Ambient Intensity", &ambientIntensity_, 0.1f, 5.0f);
        ImGui::SliderFloat("Sun Intensity", &sunIntensity_, 0.1f, 5.0f);

        ImGui::Spacing();
        ImGui::DragFloat("Sun Angle", &sunRotation_.x, 1.0f, 5.0f, 80.0f);
        ImGui::DragFloat("Sun Rotation", &sunRotation_.y, 1.0f, 0.0f, 360.0f);
        ImGui::DragFloat("Sun Size", &skySunSize_, 1.0f, 128.0f, 1024.0f, "%.3f", 2.0f);
        ImGui::DragFloat("Sun Falloff", &skySunFalloff_, 0.05f, 1.0f, 16.0f);
    }
}

void Scene::serialize(PropertyTable &table)
{
    // If we are about to write and this is the current scene, tell the
    // scene manager to save changes
    if (table.mode() == PropertyTableMode::Writing && SceneManager::instance()->currentScene() == this)
    {
        SceneManager::instance()->saveScene();
    }

    // The gameobject list is saved in an already-serialized form,
    // so we just need to write the entire thing to or from the table.
    if(table.mode() == PropertyTableMode::Writing)
    {
        table = gameObjects_;
    }
    else
    {
        gameObjects_ = table;
    }

    // Serialize all rendering settings
    table.serialize("ambient_light", ambientLight_, Color::white());
    table.serialize("ambient_intensity", ambientIntensity_, 1.0f);
    table.serialize("sun_color", sunColor_, Color::white());
    table.serialize("sun_intensity", sunIntensity_, 2.0f);
    table.serialize("sun_rotation", sunRotation_, Vector2(30.0f, 0.0f));
    table.serialize("sky_color_top", skyColorTop_, Color(0.5f, 0.8f, 1.0f, 1.0f));
    table.serialize("sky_color_bottom", skyColorBottom_, Color(0.2f, 0.15f, 0.02f, 1.0f));
    table.serialize("sky_sun_size", skySunSize_, 512.0f);
    table.serialize("sky_sun_falloff", skySunFalloff_, 4.0f);
}

void Scene::createGameObjects()
{
    // To create the objects, just read from the serialized form into the gameobject list
    gameObjects_.setMode(PropertyTableMode::Reading);

    // The propertytable supports reading out into a vector, use that.
    // We dont actually need the vector, since the scenemanager tracks the objects
    auto tempVector = std::vector<GameObject*>();
    gameObjects_.serialize("gameobjects", tempVector);
}

void Scene::saveGameObjects()
{
    // Store all the objects we want to save into a vector
    auto tempVector = std::vector<GameObject*>();
    for(GameObject* gameObject : SceneManager::instance()->gameObjects())
    {
        if(gameObject->hasFlag(GameObjectFlag::NotSavedInScene) == false)
        {
            tempVector.push_back(gameObject);
        }
    }

    // To save the objects, just read them into the serialized data.
    gameObjects_.clear();
    gameObjects_.setMode(PropertyTableMode::Writing);
    gameObjects_.serialize("gameobjects", tempVector);
}
