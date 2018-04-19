#pragma once

#include <vector>

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

    // Creates a renderer that draws to the specified framebuffer(s).
    explicit Renderer(Framebuffer* targetFramebuffer);
    explicit Renderer(std::vector<Framebuffer*> targetFramebuffers);

    ~Renderer();

    // Renders a new frame from the point of view
    // of the specified camera.
    void renderFrame(const Camera* camera);

private:
    // The framebuffer being rendered to
    const std::vector<Framebuffer*> targetFramebuffers_;

    // The depth buffer texture ids for each framebuffer
    std::vector<GLint> framebufferDepthTextures_;

    // The current GBuffer objects
    Texture* gbufferTextures_[GBUFFER_RENDER_TARGETS];
    std::vector<Framebuffer> gbufferFramebuffers_;

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
    Shader* deferredAmbientOcclusionShader_;
    Shader* deferredLightingShader_;
    Shader* deferredDebugShader_;

    // Shader used for the forward water pass
    Shader* waterShader_;

    // Shader used for the shield rendering pass
    Shader* shieldShader_;
    Texture* shieldFlowTexture_;
    Texture* shieldOpacityTexture_;
    Mesh* shieldMesh_;

    // Resources used for skybox shader pass
    Shader* skyboxShader_;
    Mesh* skyboxMesh_;

    // A shader used for generating the transmittance LUT
    Shader* skyTransmittanceShader_;

    // Sky lookup textures
    Texture skyTransmittanceLUT_;

    // The poisson disks used for ambient occlusion
    Vector4 poissonDisks_[16];

    // GBuffer management
    void createGBuffer();
    void destroyGBuffer();

    // Methods for updating the contents of uniform buffers
    void updateSceneUniformBuffer() const;
    void updateCameraUniformBuffer(const Camera* camera, EyeType eye) const;
    void updatePerDrawUniformBuffer(const Matrix4x4 &localToWorld, const Material* material) const;
    void updateTerrainUniformBuffer(const Terrain* terrain) const;
    void updateTerrainDetailsUniformBuffer(const DetailBatch& details) const;

    // Renders a full geometry pass using the specified camera
    void executeGeometryPass(const Camera* camera, ShaderFeatureList shaderFeatures) const;

    // Renders a full screen pass using the specifed shader
    void executeFullScreen(Shader* shader, ShaderFeatureList shaderFeatures) const;

    // Methods for each render pass
    void executeDeferredAmbientOcclusionPass() const;
    void executeDeferredLightingPass() const;
    void executeDeferredDebugPass() const;
    void executeWaterPass() const;
    void executeSkyboxPass(const Camera* camera) const;
    void executeShieldPass() const;

    // Computes the sky transmittance lut
    // This is slow and should only be done when needed (aka when the atmosphere composition changes).
    void regenerateSkyTransmittanceLUT();
};
