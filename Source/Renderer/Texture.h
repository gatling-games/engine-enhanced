#pragma once

#include "ResourceManager.h"

enum class TextureType
{
	Texture2D, // Single 2D texture
	TextureArray2D, // Array of 2D textures
	Texture3D, // Single 3D texture
	Cubemap, // Single cubemap texture
	CubemapArray, // Array of cubemap textures
};

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

struct TextureSettings
{
	TextureType type;
	TextureFormat format;
	TextureWrapMode wrapMode;
	TextureFilterMode filterMode;
	int width;
	int height;
	int depth;
	int mipLevels;
};

class Texture : protected Resource<TextureSettings>
{
public:
	Texture();

protected:
	bool Load(const TextureSettings* settings, std::ifstream &file) override;
	void Unload() override;

private:
	TextureSettings settings_;
};