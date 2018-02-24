#pragma once

#include <GL/gl3w.h>

#include "Editor/EditableObject.h"
#include "ResourceManager.h"

// GL bindless textures are stored as a gluint64 handle
typedef GLuint64 BindlessTextureHandle;

// The avaliable texture formats for a texture resource.
// For more details on each format, view the TextureFormatData 
// struct and formatsTable[] in Tetxture.cpp.
enum class TextureFormat
{
	RGB_DXT1, // RGB, block compressed 4 bits per pixel
    RGB_DXT1_SRGB, // RGB, block compressed 4 bits per pixel, srgb->linear rgb
	RGBA_DXT5, // RGBA, block compressed 8 bits per pixel
    RGBA_DXT5_SRGB, // RGBA, block compressed 8 bits per pixel, srgb->linear rgb, linear alpha
	RGB8, // RGB, 24 bits per pixel
    RGB8_SRGB, // RGB, 24 bits per pixel, srgb->linear rgb
	RGBA8, // RGBA, 32 bits per pixel
    RGBA8_SRGB, // RGBA, 32 bits per pixel, srgb->linear rgb, linear alpha
    RGBA1010102, // RGBA, 10 bits for rgb, 2 bits for a
	R8, // Single channel, 8 bits per pixel
	RFloat, // Single channel, 32 bit floating point
	Depth, // Depth texture, 24 bits per pixel
	ShadowMap, // Depth texture + hardware PCF, 24 bits per pixel

    // Must be last in the list.
    Unknown, 
};

// Wrapping modes for a texture resource.
enum class TextureWrapMode
{
	Repeat, // Texture repeats in all directions
	Clamp // Texture is clamped to edge in all directions
};

// Filtering modes for a texture.
// They handle mip sampling modes too.
enum class TextureFilterMode
{
	Nearest, // Nearest neighbour sampling
	Bilinear, // Bilinear sampling
	Trilinear, // Trilinear, falls back to bilinear if no mipmaps
	Anisotropic, // Anisotropic, falls back if no mipmap or aniso support
};

// Represents an array texture.
// This allows multiple textures of the same format and resolution to
// be stored in an array.
class ArrayTexture
{
public:
    ArrayTexture(TextureFormat format, int width, int height, int layers);
    ~ArrayTexture();

    // Do not allow an array texture to be copied
    ArrayTexture(const ArrayTexture&) = delete;
    ArrayTexture& operator=(const ArrayTexture&) = delete;

    // Allow an array texture to be moved
    ArrayTexture(ArrayTexture&& other);
    ArrayTexture& operator=(ArrayTexture&& other);

    // Gets the internal opengl ID of the texture
    GLuint glid() const { return glid_; }

    // Attaches the texture to the specified slot for use.
    void bind(int slot) const;

    // Basic settings
    TextureFormat format() const { return format_; }
    TextureFilterMode filterMode() const { return filterMode_; }
    int width() const { return width_; }
    int height() const { return height_; }
    int layers() const { return layers_; }

private:
    TextureFormat format_;
    TextureFilterMode filterMode_;
    int width_;
    int height_;
    int layers_;
    GLuint glid_;
};

// Stores a single texture resource.
// Can be either a stored texture, managed by the resource
// manager, or a texture created via new() at runtime.
class Texture : public Resource, public IEditableObject
{
public:
    // Create a texture with the given format and resolution.
    Texture(TextureFormat format, int width, int height);

    // Destroys remaining texture resources.
    ~Texture();

    // Constructor for texture resources.
    // Should only be used by ResourceManager.cpp
    explicit Texture(ResourceID resourceID);

    // Handles resource loading and unloading
    void load(std::ifstream &file) override;
    void unload() override;

    // Implements a custom editor
    void drawEditor() override;

    // Gets the internal opengl ID of the texture
    GLuint glid() const { return glid_; }

    // Gets the bindless handle of the texture.
    // This can be placed in uniform buffers to sample the texture.
    BindlessTextureHandle bindlessHandle() const { return handle_; }

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

    // Updates the data in the heightmap.
    // The data must be the correct format and size to replace the entire mip level.
    void setData(const void* data, int dataSizeBytes, int mipLevel);
    
    // Converts a texture format to a human-readable name.
    static const std::string& getFormatName(TextureFormat format);

protected:
    TextureFormat format_;
    TextureWrapMode wrapMode_;
    TextureFilterMode filterMode_;
    int width_;
    int height_;
    int levels_;
    GLuint glid_;
    GLuint64 handle_;
    bool created_;

    // Determines the size of a mip level for a texture.
    // mipLevel starts at 0
    static int getMipWidth(int fullWidth, int mipLevel);
    static int getMipHeight(int fullHeight, int mipLevel);
    static int getMipSize(TextureFormat format, int fullWidth, int fullHeight, int mipLevel);

    // Creates and destroys the internal opengl texture
    void createGLTexture(TextureFormat format, int width, int height, int mipLevels);
    void destroyGLTexture();

    // Updates internal opengl texture params
    void applySettings() const;
    GLint getWrapS() const;
    GLint getWrapT() const;
    GLint getMinFilter() const;
    GLint getMagFilter() const;
};
