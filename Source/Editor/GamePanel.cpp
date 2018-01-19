#include "GamePanel.h"

#include <imgui.h>

#include "SceneManager.h"
#include "Scene/Camera.h"

GamePanel::GamePanel()
    : frameBuffer_(nullptr)
    , depthBuffer_(nullptr)
    , colorBuffer_(nullptr)
    , renderer_(nullptr)
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

void GamePanel::drawMenu(const std::string menuName)
{
    if (menuName == "View")
    {
        // Draw the render feature toggles
        if (ImGui::BeginMenu("Render Features"))
        {
            drawFeatureToggle(SF_Texture, "Textures");
            drawFeatureToggle(SF_NormalMap, "Normal Maps");
            drawFeatureToggle(SF_Specular, "Specular Highlights");
            drawFeatureToggle(SF_Cutout, "Alpha Cutout");
            drawFeatureToggle(SF_Fog, "Fog");
            ImGui::EndMenu();
        }

        // Draw the render debug mode toggles
        if (ImGui::BeginMenu("Debug Mode"))
        {
            drawDebugModeToggle(RenderDebugMode::None, "None");
            drawDebugModeToggle(RenderDebugMode::Albedo, "Albedo");
            drawDebugModeToggle(RenderDebugMode::Gloss, "Gloss");
            drawDebugModeToggle(RenderDebugMode::Normals, "Normals");
            drawDebugModeToggle(RenderDebugMode::Occlusion, "Occlusion");
            ImGui::EndMenu();
        }
    }
}

void GamePanel::draw()
{
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

    // Re-render the framebuffer on each draw
    renderer_->renderFrame(SceneManager::instance()->mainCamera());

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
    colorBuffer_ = new Texture(TextureFormat::RGB8, width, height);

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