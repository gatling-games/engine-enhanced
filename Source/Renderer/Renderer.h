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
    Mesh* fullScreenMesh_;

    // Uniform buffers used in different render stages
    UniformBuffer<SceneUniformData> sceneUniformBuffer_;
    UniformBuffer<CameraUniformData> cameraUniformBuffer_;
    UniformBuffer<PerDrawUniformData> perDrawUniformBuffer_;
    UniformBuffer<TerrainUniformData> terrainUniformBuffer_;
    UniformBuffer<TerrainDetailsData> terrainDetailsUniformBuffer_;

    // Shaders used for gbuffer pass
    Shader* standardShader_;
    Shader* terrainShader_;
    Shader* terrainDetailMeshShader_;

    // Shaders used for deferred passes
    Shader* deferredLightingShader_;
    Shader* deferredDebugShader_;

    // Shader used for the forward water pass
    Shader* waterShader_;

    // Resources used for skybox shader pass
    Shader* skyboxShader_;
    Mesh* skyboxMesh_;

    // A shader used for generating the transmittance LUT
    Shader* skyTransmittanceShader_;

    // Sky lookup textures
    Texture skyTransmittanceLUT_;

    // GBuffer management
    void createGBuffer();
    void destroyGBuffer();

    // Methods for updating the contents of uniform buffers
    void updateSceneUniformBuffer() const;
    void updateCameraUniformBuffer(const Camera* camera) const;
    void updatePerDrawUniformBuffer(const Matrix4x4 &localToWorld, const Material* material) const;
    void updateTerrainUniformBuffer(const Terrain* terrain) const;
    void updateTerrainDetailsUniformBuffer(const DetailBatch& details) const;

    // Renders a full geometry pass using the specified camera
    void executeGeometryPass(const Camera* camera, ShaderFeatureList shaderFeatures) const;

    // Renders a full screen pass using the specifed shader
    void executeFullScreen(Shader* shader, ShaderFeatureList shaderFeatures) const;

    // Methods for each render pass
    void executeDeferredLightingPass() const;
    void executeDeferredDebugPass() const;
    void executeWaterPass() const;
    void executeSkyboxPass(const Camera* camera) const;

    // Computes the sky transmittance lut
    // This is slow and should only be done when needed (aka when the atmosphere composition changes).
    void regenerateSkyTransmittanceLUT();
};
