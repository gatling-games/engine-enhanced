#include "ShadowMap.h"

#include "Math/Bounds.h"

#include "SceneManager.h"
#include "Scene/GameObject.h"
#include "Scene/Transform.h"
#include "Scene/Camera.h"

ShadowMap::ShadowMap()
    : texture_(TextureFormat::ShadowMap, RESOLUTION, RESOLUTION, CASCADE_COUNT),
    uniformBuffer_(UniformBufferType::ShadowsBuffer)
{
    // Create a framebuffer and camera for each shadow cascade
    for (int i = 0; i < CASCADE_COUNT; ++i)
    {
        // Attach layer i of the shadow array texture to the framebuffer
        cascades_[i].framebuffer.attachDepthTexture(&texture_, i);

        // Create the camera and set to orthographic
        GameObject* cameraGameObject = new GameObject("Shadows Camera [Cascade " + std::to_string(i) + "]");
        cameraGameObject->setFlag(GameObjectFlag::NotShownOrSaved, true);
        cameraGameObject->setFlag(GameObjectFlag::SurviveSceneChanges, true);
        cascades_[i].cameraTransform = cameraGameObject->transform();
        cascades_[i].camera = cameraGameObject->createComponent<Camera>();
        cascades_[i].camera->setType(CameraType::Orthographic);
        cascades_[i].camera->setNearPlane(-2000.0f);
        cascades_[i].camera->setFarPlane(2000.0f);
    }
}

Framebuffer& ShadowMap::cascadeFramebuffer(int cascade)
{
    return cascades_[cascade].framebuffer;
}

Camera* ShadowMap::cascadeCamera(int cascade)
{
    return cascades_[cascade].camera;
}

void ShadowMap::bind()
{
    uniformBuffer_.use();
    texture_.bind(10);
}

void ShadowMap::updatePosition(const Camera* viewCamera, float viewCameraAspect)
{
    const Scene* scene = SceneManager::instance()->currentScene();

    // Ensure each cascade is drawn from the correct direction
    const Quaternion rotation = scene->sunRotation();
    for (int i = 0; i < CASCADE_COUNT; ++i)
    {
        cascades_[i].cameraTransform->setRotationLocal(rotation);
    }

    // Get the world to light space transformation matrix (without translation)
    Matrix4x4 worldToLight = cascades_[0].cameraTransform->worldToLocal();
    worldToLight.set(0, 3, 0.0);
    worldToLight.set(1, 3, 0.0);
    worldToLight.set(2, 3, 0.0);

    // Get the light to world transformation matrix (without translation)
    Matrix4x4 lightToWorld = cascades_[0].cameraTransform->localToWorld();
    lightToWorld.set(0, 3, 0.0);
    lightToWorld.set(1, 3, 0.0);
    lightToWorld.set(2, 3, 0.0);

    // Compute the view to light matrix
    Matrix4x4 viewToLight = worldToLight * viewCamera->gameObject()->transform()->localToWorld();

    // Set up each cascade camera size and centre
    for (int i = 0; i < CASCADE_COUNT; ++i)
    {
        // Calculate the min and max distance of the cascade
        cascades_[i].minDistance = getCascadeMin(i);
        cascades_[i].maxDistance = getCascadeMax(i);

        // Compute the corners of the frustum segment in view space.
        Point3 corners[8];
        viewCamera->getFrustumCorners(cascades_[i].minDistance, corners, viewCameraAspect);
        viewCamera->getFrustumCorners(cascades_[i].maxDistance, corners + 4, viewCameraAspect);

        // Size the cascade camera to cover the frustum bounds.
        // Compute in view space so the size doesn't change during camera rotation.
        Bounds viewSpaceBounds = Bounds::covering(corners, 8);
        Vector3 viewSpaceSize = viewSpaceBounds.size();
        cascades_[i].camera->setOrthographicSize(viewSpaceSize.magnitude());

        // Calculate the shadow map centre in light space
        Point3 viewSpaceCentre = viewSpaceBounds.centre();
        Point3 lightSpaceCentre = viewToLight * viewSpaceCentre;

        // Snap the centre to the nearest texel in light space.
        float texelSize = cascades_[i].camera->orthographicSize() / (float)RESOLUTION;
        lightSpaceCentre.x = roundf(lightSpaceCentre.x / texelSize) * texelSize;
        lightSpaceCentre.y = roundf(lightSpaceCentre.y / texelSize) * texelSize;

        // Compute the cascade centre in world space
        Point3 centre = lightToWorld * lightSpaceCentre;

        // The camera has a -ve near clip plane, so placing the camera in
        // the centre of the cascade will render the full cascade.
        cascades_[i].cameraTransform->setPositionLocal(centre);
    }

    // Update the uniform buffer to match the camera position
    ShadowUniformData data;
    for (int i = 0; i < CASCADE_COUNT; ++i)
    {
        // Store the max distance
        data.cascadeMaxDistances[i] = cascades_[i].maxDistance;

        // Offset matrix transforms coordinates from [-1 - 1] to [0-1]
        // and encodes a depth bias to prevent acne
        Matrix4x4 offsetMatrix;
        offsetMatrix.setRow(0, 0.5f, 0.0f, 0.0f, 0.5f);
        offsetMatrix.setRow(1, 0.0f, 0.5f, 0.0f, 0.5f);
        offsetMatrix.setRow(2, 0.0f, 0.0f, 0.5f, 0.5f - DEPTH_BIAS_PER_CASCADE[i]);
        offsetMatrix.setRow(3, 0.0f, 0.0f, 0.0f, 1.0f);

        // Store the world to shadow matrix
        data.worldToShadow[i] = offsetMatrix * cascades_[i].camera->getWorldToCameraMatrix(1.0f);
    }
    uniformBuffer_.update(data);
}

float ShadowMap::getCascadeMin(int cascade) const
{
    // Ensure cascade 0 starts at distance 0
    if (cascade == 0)
    {
        return 0.0f;
    }

    // Otherwise, use the draw distance for the previous cascade
    return getCascadeMax(cascade - 1);
}

float ShadowMap::getCascadeMax(int cascade) const
{
    return DRAW_DISTANCE_PER_CASCADE[cascade];
}
