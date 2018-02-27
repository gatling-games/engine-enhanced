#pragma once

#include "Renderer/Framebuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/UniformBuffer.h"

#include "Scene/Camera.h"
#include "Renderer/Mesh.h"
#include "Renderer/ShadowMap.h"

class Renderer
{
private:
    const static int GBUFFER_RENDER_TARGETS = 2;

public:
    // Creates a renderer that draws directly to the back buffer
    Renderer();

    // Creates a renderer that draws to the specified framebuffer.
    explicit Renderer(const Framebuffer* targetFramebuffer);

    ~Renderer();

    // Renders a new frame from the point of view
    // of the specified camera.
    void renderFrame(const Camera* camera);

private:
    // The framebuffer being rendered to
    const Framebuffer* targetFramebuffer_;

    // The current GBuffer objects
    Texture* gbufferTextures_[GBUFFER_RENDER_TARGETS];
    Framebuffer gbufferFramebuffer_;

    // The shadow map rendering manager
    ShadowMap shadowMap_;

    // A full screen triangle used for screen space passes
    ResourcePPtr<Mesh> fullScreenMesh_;

    // Uniform buffers used in different render stages
    UniformBuffer<SceneUniformData> sceneUniformBuffer_;
    UniformBuffer<CameraUniformData> cameraUniformBuffer_;
    UniformBuffer<PerDrawUniformData> perDrawUniformBuffer_;
    UniformBuffer<TerrainUniformData> terrainUniformBuffer_;

    // Shaders used for gbuffer pass
    ResourcePPtr<Shader> standardShader_;
    ResourcePPtr<Shader> terrainShader_;

    // Shaders used for deferred passes
    ResourcePPtr<Shader> deferredLightingShader_;
    ResourcePPtr<Shader> deferredDebugShader_;

    // Shader used for the forward water pass
    ResourcePPtr<Shader> waterShader_;

    // Resources used for skybox shader pass
    ResourcePPtr<Shader> skyboxShader_;
    ResourcePPtr<Mesh> skyboxMesh_;
    ResourcePPtr<Texture> skyboxCloudThicknessTexture_;

    // GBuffer management
    void createGBuffer();
    void destroyGBuffer();

    // Methods for updating the contents of uniform buffers
    void updateSceneUniformBuffer() const;
    void updateCameraUniformBuffer(const Camera* camera) const;
    void updatePerDrawUniformBuffer(const StaticMesh* draw, const Texture* albedoTexture, const Texture* normalMapTexture) const;
    void updateTerrainUniformBuffer(const Terrain* terrain) const;

    // Renders a full geometry pass using the specified camera
    void executeGeometryPass(const Camera* camera, ShaderFeatureList shaderFeatures) const;

    // Renders a full screen pass using the specifed shader
    void executeFullScreen(Shader* shader, ShaderFeatureList shaderFeatures) const;

    // Methods for each render pass
    void executeDeferredLightingPass() const;
    void executeDeferredDebugPass() const;
    void executeWaterPass() const;
    void executeSkyboxPass(const Camera* camera) const;
};
