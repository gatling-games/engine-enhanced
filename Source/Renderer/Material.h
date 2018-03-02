#pragma once

#include "Math/Color.h"
#include "Renderer/Texture.h"
#include "Editor/EditableObject.h"
#include "Serialization/SerializedObject.h"
#include "ResourceManager.h"
#include "Renderer/Shader.h"

class Material : public Resource, public IEditableObject, public ISerializedObject
{
public:
    Material();
    Material(ResourceID resourceID);
    
    // Implements material serialization
    void serialize(PropertyTable& table) override;

    // Implements a custom editor
    void drawEditor() override;

    // Gets basic material settings
    Color color() const { return color_; }
    Texture* albedoTexture() const { return albedoTexture_; }
    Texture* normalMapTexture() const { return normalMapTexture_; }
    float smoothness() const { return smoothness_; }
    bool cutout() const { return cutout_; }

    // Sets basic material settings
    void setColor(const Color &color);
    void setAlbedoTexture(Texture* albedoTexture);
    void setNormalMapTexture(Texture* normalMapTexture);
    void setSmoothness(float smoothness);
    void setCutout(bool cutout);

    // Computes the set of enabled shader features, based on material settings
    ShaderFeatureList supportedFeatures() const;

private:
    Color color_;
    Texture* albedoTexture_;
    Texture* normalMapTexture_;
    float smoothness_;
    bool cutout_;
};