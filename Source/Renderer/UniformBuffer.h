#pragma once

enum class UniformBufferType
{
    SceneBuffer = 0,
    CameraBuffer = 1,
    ShadowsBuffer = 2,
    PerDrawBuffer = 3,
    PerMaterialBuffer = 4
};

template <class T>
class UniformBuffer
{
    
};