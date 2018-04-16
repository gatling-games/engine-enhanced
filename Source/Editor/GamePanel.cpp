#include "GamePanel.h"

#include <imgui.h>

#include "SceneManager.h"
#include "Scene/Camera.h"
#include "InputManager.h"
#include "Scene/Freecam.h"

GamePanel::GamePanel()
    : frameBuffer_(nullptr)
    , depthBuffer_(nullptr)
    , colorBuffer_(nullptr)
    , renderer_(nullptr)
    , camera_(nullptr)
{

}

GamePanel::~GamePanel()
{
    // Delete any existing framebuffer
    if (frameBuffer_ != nullptr)
    {
        delete frameBuffer_;
        delete depthBuffer_;
        delete colorBuffer_;
        delete renderer_;
    }
}

void GamePanel::draw()
{
    // When using the game panel, disable input if focus is lost.=
    InputManager::instance()->setInputEnabled(ImGui::IsWindowFocused());

    // Determine the size of the region we need to render for
    // and create / recreate the framebuffer when needed.
    const ImVec2 regionSize = ImGui::GetContentRegionAvail();
    const int renderTextureWidth = (int)regionSize.x;
    const int renderTextureHeight = (int)regionSize.y;
    if (frameBuffer_ == nullptr
        || colorBuffer_->width() != renderTextureWidth
        || colorBuffer_->height() != renderTextureHeight)
    {
        createFramebuffer(renderTextureWidth, renderTextureHeight);
    }

    // When the game panel is created, add a camera in the scene
    // Make it hidden so that a) the user cant modify it, and b) it doesnt get deleted
    if (camera_ == nullptr)
    {
        GameObject* cameraGO = new GameObject("GamePanelCamera");
        cameraGO->setFlag(GameObjectFlag::NotShownOrSaved, true);
        cameraGO->setFlag(GameObjectFlag::SurviveSceneChanges, true);
        cameraGO->createComponent<Freecam>();
        camera_ = cameraGO->createComponent<Camera>();
    }

    // Make sure the game camera only moves in edit mode
    camera_->gameObject()->findComponent<Freecam>()->setUpdateEnabled(Application::instance()->isEditing());

    // Re-render the framebuffer on each draw
    // Use the game panel camera in edit mode, and the scene main camera in play mode
    renderer_->renderFrame(Application::instance()->isPlaying() ? SceneManager::instance()->mainCamera() : camera_);

    // Draw the texture
    ImGui::Image((ImTextureID)(uint64_t)colorBuffer_->glid(), ImGui::GetContentRegionAvail(), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
}

void GamePanel::createFramebuffer(int width, int height)
{
    // Delete any existing framebuffer
    if (frameBuffer_ != nullptr)
    {
        delete frameBuffer_;
        delete depthBuffer_;
        delete colorBuffer_;
        delete renderer_;
    }

    // Create new framebuffer and textures with new panel dimensions
    frameBuffer_ = new Framebuffer();
    depthBuffer_ = new Texture(TextureFormat::Depth, width, height);
    colorBuffer_ = new Texture(TextureFormat::RGB8_SRGB, width, height);

    // Attach new textures to new framebuffer
    frameBuffer_->attachDepthTexture(depthBuffer_);
    frameBuffer_->attachColorTexture(colorBuffer_);

    // Then create a renderer for the framebuffer.
    renderer_ = new Renderer(frameBuffer_);
}

void GamePanel::drawFeatureToggle(ShaderFeature feature, const char* label) const
{
    bool selected = RenderManager::instance()->isFeatureGloballyEnabled(feature);
    if (ImGui::MenuItem(label, (const char*)0, selected))
    {
        RenderManager::instance()->globallyToggleFeature(feature);
    }
}

void GamePanel::drawDebugModeToggle(RenderDebugMode mode, const char* label) const
{
    bool on = RenderManager::instance()->debugMode() == mode;
    if (ImGui::MenuItem(label, (const char*)0, on))
    {
        RenderManager::instance()->setDebugMode(on ? RenderDebugMode::None : mode);
    }
}