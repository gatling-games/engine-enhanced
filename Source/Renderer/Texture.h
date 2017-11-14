#pragma once

#include <GL/gl3w.h>

#include "ResourceManager.h"

enum class TextureFormat
{
	RGB_DXT1, // RGB, block compressed 4 bits per pixel
	RGBA_DXT5, // RGBA, block compressed 8 bits per pixel
	RGB8, // RGB, 24 bits per pixel
	RGBA8, // RGBA, 32 bits per pixel
	R8, // Single channel, 8 bits per pixel
	RFloat, // Single channel, 32 bit floating point
	Depth, // Depth texture, 24 bits per pixel
	ShadowMap, // Depth texture + hardware PCF, 24 bits per pixel

    // Must be last in the list.
    Unknown, 
};

enum class TextureWrapMode
{
	Repeat, // Texture repeats in all directions
	Clamp // Texture is clamped to edge in all directions
};

enum class TextureFilterMode
{
	Nearest, // Nearest neighbour sampling
	Bilinear, // Bilinear sampling
	Trilinear, // Trilinear, falls back to bilinear if no mipmaps
	Anisotropic, // Anisotropic, falls back if no mipmap or aniso support
};

class Texture : public Resource
{
public:
    Texture(TextureFormat format, int width, int height);
    ~Texture();

    // Constructor for texture resources.
    // Should only be used by ResourceManager.cpp
    explicit Texture(ResourceID resourceID);

    // Handles resource loading and unloading
    void load(std::ifstream &file) override;
    void unload() override;

    // Basic settings
    TextureFormat format() const { return format_; }
    TextureWrapMode wrapMode() const { return wrapMode_; }
    TextureFilterMode filterMode() const { return filterMode_; }
    int width() const { return width_; }
    int height() const { return height_; }
    bool hasMipmaps() const;
    bool isCompressed() const;

    // Setters for basic settings
    void setWrapMode(TextureWrapMode wrapMode);
    void setFilterMode(TextureFilterMode filterMode);

    // Attaches the texture to the specified slot for use.
    // slot must be between 0 and 10, inclusive.
    void bind(int slot) const;

protected:
    TextureFormat format_;
    TextureWrapMode wrapMode_;
    TextureFilterMode filterMode_;
    int width_;
    int height_;
    int levels_;
    GLuint glid_;
    bool loaded_;

    // Determines the size of a mip level for the texture's format.
    int getMipWidth(int level) const;
    int getMipHeight(int level) const;
    int getMipSize(int level) const;

    // Creates and destroys the opengl texture
    void createGLTexture();
    void destroyGLTexture();

    // Updates internal opengl texture params
    void applySettings() const;
    GLint getWrapS() const;
    GLint getWrapT() const;
    GLint getMinFilter() const;
    GLint getMagFilter() const;
};
