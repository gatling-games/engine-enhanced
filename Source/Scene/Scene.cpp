#include "Scene.h"

#include <imgui.h>

#include "SceneManager.h"

Scene::Scene(ResourceID resourceID)
    : Resource(resourceID),
    gameObjects_()
{

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
    else
    {
        if (ImGui::Button("Open Scene", ImVec2(ImGui::GetContentRegionAvailWidth(), 40.0f)))
        {
            SceneManager::instance()->openScene(resourcePath());
        }
    }
}

void Scene::serialize(PropertyTable &table)
{
    // Serialize the entire list of gameobjects
    table.serialize("gameobjects", gameObjects_);

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