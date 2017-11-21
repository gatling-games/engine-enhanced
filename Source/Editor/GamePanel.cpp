#include "GamePanel.h"

#include <imgui.h>

#include "SceneManager.h"
#include "Scene/Camera.h"

GamePanel::GamePanel()
    : frameBuffer_(nullptr)
    , depthBuffer_(nullptr)
    , colorBuffer_(nullptr)
    , sceneUniformBuffer_(UniformBufferType::SceneBuffer)
    , cameraUniformBuffer_(UniformBufferType::CameraBuffer)
    , perDrawUniformBuffer_(UniformBufferType::PerDrawBuffer)
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
    }
}

void GamePanel::draw()
{
    // Get the resolution of the render target
    const Rect rect = size();

    // Render to the target
    frameBuffer_->use();

    // First clear the target
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(180.0f, 0.0f, 180.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    SceneUniformData sud;
    sud.ambientLightColor = Color(0.6f, 0.5f, 0.5f, 1.0f);
    sud.lightColor = Color(1.0f, 1.0f, 1.0f);
    sud.toLightDirection = Vector4(Vector3(1.0f, 1.0f, 1.0f).normalized());
    sceneUniformBuffer_.update(sud);
    sceneUniformBuffer_.use();

    // First, get the camera for the scene and update the uniform buffer.
    Camera* camera = SceneManager::instance()->mainCamera();
    CameraUniformData cud;
    cud.cameraPosition = Vector4(camera->gameObject()->transformComponent()->positionWorld());
    cud.screenResolution = Vector4(rect.width, rect.height, 1.0f / rect.width, 1.0f / rect.height);
    cud.worldToClip = camera->getWorldToCameraMatrix(rect.width / (float)rect.height);
    cud.clipToWorld = Matrix4x4::identity();
    cameraUniformBuffer_.update(cud);
    cameraUniformBuffer_.use();

    perDrawUniformBuffer_.use();

    // Now draw every mesh in the scene.
    auto staticMeshes = SceneManager::instance()->staticMeshes();
    for(auto mesh = staticMeshes.begin(); mesh != staticMeshes.end(); ++mesh)
    {
        // Set up the correct shader and mesh
        mesh->get()->shader()->bind();
        mesh->get()->mesh()->bind();
        mesh->get()->texture()->bind(0);

        // Set the per draw ubo
        PerDrawUniformData perDrawData;
        perDrawData.localToWorld = mesh->get()->gameObject()->transformComponent()->localToWorld();
        perDrawUniformBuffer_.update(perDrawData);

        // Draw the mesh
        glDrawElements(GL_TRIANGLES, mesh->get()->mesh()->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
    }

    // Finally, switch back to the main buffer and draw
    // the texture into the UI.
    Framebuffer::backbuffer()->use();
    ImGui::Image(reinterpret_cast<ImTextureID>(colorBuffer_->glid()), ImVec2(rect.width, rect.height), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
}

void GamePanel::onResize(int width, int height)
{
    // Delete any existing framebuffer
    if (frameBuffer_ != nullptr)
    {
        delete frameBuffer_;
        delete depthBuffer_;
        delete colorBuffer_;
    }

    // Create new framebuffer and textures with new panel dimensions
    frameBuffer_ = new Framebuffer();
    depthBuffer_ = new Texture(TextureFormat::Depth, width, height);
    colorBuffer_ = new Texture(TextureFormat::RGB8, width, height);

    // Attach new textures to new framebuffer
    frameBuffer_->attachDepthTexture(depthBuffer_);
    frameBuffer_->attachColorTexture(colorBuffer_);
}