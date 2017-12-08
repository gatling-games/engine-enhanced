#pragma once

#include "Renderer/Framebuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/UniformBuffer.h"

#include "Scene/Camera.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"

class Renderer
{
public:
    // Creates a renderer that draws directly to the back buffer
    Renderer();

    // Creates a renderer that draws to the specified framebuffer.
    Renderer(const Framebuffer* targetFramebuffer);

    // Renders a new frame from the point of view
    // of the specified camera.
    void renderFrame(const Camera* camera) const;

private:
    // The framebuffer being rendered to
    const Framebuffer* targetFramebuffer_;

    // Uniform buffers used in different render stages
    UniformBuffer<SceneUniformData> sceneUniformBuffer_;
    UniformBuffer<CameraUniformData> cameraUniformBuffer_;
    UniformBuffer<PerDrawUniformData> perDrawUniformBuffer_;

    // Shader used for forward pass
    ResourcePPtr<Shader> forwardShader_;

    // Resources used for skybox shader pass
    ResourcePPtr<Shader> skyboxShader_;
    ResourcePPtr<Mesh> skyboxMesh_;
    ResourcePPtr<Texture> skyboxTexture_;

    // Methods for updating the contents of uniform buffers
    void updateSceneUniformBuffer() const;
    void updateCameraUniformBuffer(const Camera* camera) const;
    void updatePerDrawUniformBuffer(const StaticMesh* draw) const;

    // Methods for each render pass
    void executeForwardPass() const;
    void executeSkyboxPass(const Camera* camera) const;
};
