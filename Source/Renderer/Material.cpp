#include "Material.h"

#include <imgui.h>
#include "Utils/ImGuiExtensions.h"

Material::Material()
    : Resource(NOT_SAVED_RESOURCE)
{
    
}

Material::Material(ResourceID resourceID)
    : Resource(resourceID)
{
    
}

void Material::serialize(PropertyTable& table)
{
    table.serialize("color", color_, Color::white());
    table.serialize("albedo_texture", albedoTexture_);
    table.serialize("normal_map_texture", normalMapTexture_);
    table.serialize("smoothness", smoothness_, 0.5f);
    table.serialize("cutout", cutout_, false);
}

void Material::drawEditor()
{
    ImGui::ColorEdit3("Color", &color_.r);
    ImGui::ResourceSelect("Albedo", "Select Albedo Texture", albedoTexture_);
    ImGui::ResourceSelect("Normal Map", "Select Normal Map Texture", normalMapTexture_);
    ImGui::SliderFloat("Smoothness", &smoothness_, 0.0f, 1.0f);
    ImGui::Checkbox("Cutout", &cutout_);
}

void Material::setColor(const Color& color)
{
    color_ = color;
}

void Material::setAlbedoTexture(Texture* albedoTexture)
{
    albedoTexture_ = albedoTexture;
}

void Material::setNormalMapTexture(Texture* normalMapTexture)
{
    normalMapTexture_ = normalMapTexture;
}

void Material::setSmoothness(float smoothness)
{
    smoothness_ = smoothness;
}

void Material::setCutout(bool cutout)
{
    cutout_ = cutout;
}

ShaderFeatureList Material::supportedFeatures() const
{
    // Start with no features
    ShaderFeatureList features = 0;

    // Use textures if an albedo texture is assigned
    if(albedoTexture_ != nullptr)
    {
        features |= SF_Texture;
    }

    // Use normal mapping if a normal map texture is assigned
    if(normalMapTexture_ != nullptr)
    {
        features |= SF_NormalMap;
    }

    // Enable alpha testing if the cutout flag is set
    if(cutout_)
    {
        features |= SF_Cutout;
    }

    return features;
}
