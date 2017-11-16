#include "Texture.h"

#include <algorithm>
#include <string>
#include <memory>
#include <cassert>

// Ensure that the anisotropic filtering gl extension
// parameters have been defined.
#ifndef GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

// Store required data for each texture format supported
// by the Texture class.
//
// This corresponds with the TextureFormat enum in Texture.h
struct TextureFormatData
{
    // A human-readable name for the format.
    std::string name;

    // Size of each texture block
    // non-block-based 
    int blockWidth;
    int blockHeight;
    int blockSize;

    // The opengl internal format
    GLint glInternalFormat;

    // Compressed flag
    bool compressed;
};

// See https://www.khronos.org/opengl/wiki/Image_Format
TextureFormatData formatsTable[] = {
    //  name            block size,  gl internal format,                 compressed 
    {   "RGB_DXT1",      4, 4, 8,     GL_COMPRESSED_RGB_S3TC_DXT1_EXT,    true        }, // RGB_DXT1s
    {   "RGBA_DXT5",     4, 4, 16,    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   true        }, // RGBA_DXT5
    {   "RGB8",          1, 1, 3,     GL_RGB8,                            false       }, // RGB8
    {   "RGBA8",         1, 1, 4,     GL_RGBA8,                           false       }, // RGBA8
    {   "R8",            1, 1, 1,     GL_R8,                              false       }, // R8
    {   "R32F",          1, 1, 4,     GL_R32F,                            false       }, // RFloat
    {   "Depth 16",      1, 1, 2,     GL_DEPTH_COMPONENT16,               false       }, // Depth
    {   "ShadowMap",     1, 1, 2,     GL_DEPTH_COMPONENT16,               false       }, // ShadowMap
};

TextureFormatData* getFormatData(TextureFormat type)
{
    return formatsTable + (int)type;
}

Texture::Texture(TextureFormat format, int width, int height)
    : Resource(NOT_SAVED_RESOURCE)
{
    // The texture is *not* created by the resource manager,
    // so we create the texture and set default wrap / filter
    // settings immediately.
    createGLTexture(format, width, height, 1);
    setWrapMode(TextureWrapMode::Clamp);
    setFilterMode(TextureFilterMode::Nearest);
}

Texture::Texture(ResourceID resourceID)
    : Resource(resourceID),
    format_(TextureFormat::Unknown),
    wrapMode_(TextureWrapMode::Repeat),
    filterMode_(TextureFilterMode::Anisotropic),
    width_(-1),
    height_(-1),
    levels_(0),
    glid_(0),
    created_(false)
{
    // This texture is a resource file.
    // The texture is created in load().
    // This is called automatically by the resource manager
}

Texture::~Texture()
{
    // Ensure any remaining texture data is destroyed.
    destroyGLTexture();
}

void Texture::load(std::ifstream& file)
{
    // We store all texture resource files as .DDS
    // See the texture importer for importing details.

    // DDS files store all required texture information, resolution details,
    // mip count and the actual mip bytes.

    // The textures are either DXT1 compressed (for RGB textures), or DXT5
    // compressed (RGBA textures). This is 8 bytes and 16 bytes per 4x4
    // pixel block, respectively.

    // Defines for the dds spec below
    typedef uint8_t BYTE;

    // From MSDN DDS file format docs
    struct DDS_PIXELFORMAT 
    {
        DWORD dwSize;
        DWORD dwFlags;
        DWORD dwFourCC;
        DWORD dwRGBBitCount;
        DWORD dwRBitMask;
        DWORD dwGBitMask;
        DWORD dwBBitMask;
        DWORD dwABitMask;
    };

    // From MSDN DDS file format docs
    struct DDS_HEADER 
    {
        DWORD           dwSize;
        DWORD           dwFlags;
        DWORD           dwHeight;
        DWORD           dwWidth;
        DWORD           dwPitchOrLinearSize;
        DWORD           dwDepth;
        DWORD           dwMipMapCount;
        DWORD           dwReserved1[11];
        DDS_PIXELFORMAT ddspf;
        DWORD           dwCaps;
        DWORD           dwCaps2;
        DWORD           dwCaps3;
        DWORD           dwCaps4;
        DWORD           dwReserved2;
    };

    // First, delete any texture that was created earlier.
    if(created_)
    {
        destroyGLTexture();
    }

    // DDS files start with the magic number 0x20534444
    // First, read the DDS magic number and check it is correct.
    uint32_t magicNumber;
    file.read((char*)&magicNumber, sizeof(uint32_t));
    assert(file.good());
    if(magicNumber != 0x20534444)
    {
        printf("Failed to load texture \n");
        printf("DDS magic number 0x%x incorrect \n", magicNumber);
        return;
    }

    // The header struct follows the magic number. Read it in from the file.
    DDS_HEADER header;
    file.read((char*)&header, sizeof(DDS_HEADER));
    assert(file.good());

    // Check that the image width and height are valid.
    const int imageWidth = header.dwWidth;
    const int imageHeight = header.dwHeight;
    if (imageWidth < 1 || imageWidth > 65536 || imageHeight < 1 || imageHeight > 65536)
    {
        printf("Failed to load texture \n");
        printf("Image resolution %dx%d invalid \n", width_, height_);
        return;
    }

    // Find out which type of DXT compression the file uses.
    const uint32_t DDS_DXT1_HEADER = 0x31545844;
    const uint32_t DDS_DXT5_HEADER = 0x35545844;
    TextureFormat format;
    if (header.ddspf.dwFourCC == DDS_DXT1_HEADER)
    {
        format = TextureFormat::RGB_DXT1;
    }
    else if (header.ddspf.dwFourCC == DDS_DXT5_HEADER)
    {
        format = TextureFormat::RGBA_DXT5;
    }
    else
    {
        printf("Failed to load texture \n");
        printf("Image format invalid \n");
        return;
    }

    // Check the number of mip map levels is valid
    const int mipLevels = header.dwMipMapCount;
    if (mipLevels <= 0 || mipLevels > 100)
    {
        printf("Failed to load texture \n");
        printf("Image levels %d invalid \n", levels_);
        return;
    }

    // Header parsing finished.
    // Now create the actual texture
    createGLTexture(format, imageWidth, imageHeight, mipLevels);

    // Get the format description
    TextureFormatData* typeData = getFormatData(format_);

    // Allocate a block of data to hold the mip data loaded from
    // the file. It needs to be big enough to hold the mips one
    // at a time, i.e. big enough to hold the biggest mip.
    const int biggestMipSize = getMipSize(format, imageWidth, imageHeight, 0);
    std::unique_ptr<uint8_t[]> mipData(new uint8_t[biggestMipSize]);

    // After the header, the rest of the DDS file is raw texture
    // data for each mip level, largest first. Load it.
    for (int mipLevel = 0; mipLevel < levels_; ++mipLevel)
    {
        // Compute the size of the mip
        const int mipWidth = getMipWidth(imageWidth, mipLevel);
        const int mipHeight = getMipHeight(imageHeight, mipLevel);
        const int mipSize = getMipSize(format, imageWidth, imageHeight, mipLevel);

        // Load the mip from the file
        file.read((char*)mipData.get(), mipSize);
        assert(file.good());

        // Pass the mip data to opengl.
        // We use glCompressedTex**Sub**Image2D as the DDS file contains compressed data
        // and the buffer uses immutable storage (via glTexStorage2D). We therefore
        // cannot use glCompressedImage2D as that function works only for mutable
        // texture storage.
        glCompressedTexSubImage2D(GL_TEXTURE_2D, mipLevel, 0, 0, mipWidth, mipHeight, typeData->glInternalFormat, mipSize, mipData.get());
    }

    // The texture is now loaded. We just need to check that all texture 
    // sampling setting are applied correctly to the texture.
    applySettings();
}

void Texture::unload()
{
    destroyGLTexture();
}

bool Texture::hasMipmaps() const
{
    return levels_ > 1;
}

bool Texture::isCompressed() const
{
    return getFormatData(format_)->compressed;
}

void Texture::setWrapMode(TextureWrapMode wrapMode)
{
    wrapMode_ = wrapMode;
    applySettings();
}

void Texture::setFilterMode(TextureFilterMode filterMode)
{
    filterMode_ = filterMode;
    applySettings();
}

void Texture::bind(int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, glid_);
}

const std::string& Texture::getFormatName(TextureFormat format)
{
    return getFormatData(format)->name;
}

int Texture::getMipWidth(int fullWidth, int mipLevel)
{
    // Each mip is half the width of the one before it.
    // Ensure that each mip is no smaller than 1px.
    return max(1, fullWidth >> mipLevel);
}

int Texture::getMipHeight(int fullHeight, int mipLevel)
{
    // Each mip is half the height of the one before it.
    // Ensure that each mip is no smaller than 1px.
    return max(1, fullHeight >> mipLevel);
}

int Texture::getMipSize(TextureFormat format, int fullWidth, int fullHeight, int mipLevel)
{
    // Get the size of the mip in pixels
    const int mipWidth = getMipWidth(fullWidth, mipLevel);
    const int mipHeight = getMipHeight(fullHeight, mipLevel);

    // Get details for the texture format.
    TextureFormatData* formatData = getFormatData(format);

    // From this, compute the number of blocks in the image
    // note - blocksx and blocksy are rounded UP.
    const int blocksX = (mipWidth + formatData->blockWidth - 1) / formatData->blockWidth;
    const int blocksY = (mipHeight + formatData->blockHeight - 1) / formatData->blockHeight;
    const int blocksCount = blocksX * blocksY;

    // Finally, compute the total size of the mip.
    return blocksCount * formatData->blockSize;
}

void Texture::createGLTexture(TextureFormat format, int width, int height, int mipLevels)
{
    // Destroy any existing texture
    if (created_)
    {
        destroyGLTexture();
    }

    // Store the texture settings
    format_ = format;
    width_ = width;
    height_ = height;
    levels_ = mipLevels;

    // We need the format data in order to create the actual texture.
    TextureFormatData* formatData = getFormatData(format_);

    // Create the actual texture
    glGenTextures(1, &glid_);
    glBindTexture(GL_TEXTURE_2D, glid_);

    // Assign texture storage for the correct format, resolution and mip count.
    glTexStorage2D(GL_TEXTURE_2D, levels_, formatData->glInternalFormat, width_, height_);

    // The texture is now created.
    created_ = true;
}

void Texture::destroyGLTexture()
{
    if (created_)
    {
        glDeleteTextures(1, &glid_);
        created_ = false;
    }
}

void Texture::applySettings() const
{
    // Skip applying settings if no texture exists.
    if(created_ == false)
    {
        return;
    }

    // First bind the texture
    glBindTexture(GL_TEXTURE_2D, glid_);

    // Set wrap mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getWrapS());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getWrapT());

    // Set filter mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getMinFilter());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getMagFilter());

    // Set anisotropic filtering
    if (filterMode_ == TextureFilterMode::Anisotropic)
    {
        GLfloat maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
    }
    else
    {
        const GLfloat minAniso = 1.0f;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, minAniso);
    }

    // Set shadow mapping compare mode
    if (format_ == TextureFormat::ShadowMap)
    {
        glBindTexture(GL_TEXTURE_2D, glid_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }
}

GLint Texture::getWrapS() const
{
    return wrapMode_ == TextureWrapMode::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
}

GLint Texture::getWrapT() const
{
    return wrapMode_ == TextureWrapMode::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
}

GLint Texture::getMinFilter() const
{
    switch (filterMode_)
    {
    case TextureFilterMode::Trilinear:
    case TextureFilterMode::Anisotropic:
        return hasMipmaps() ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

    case TextureFilterMode::Bilinear:
        return hasMipmaps() ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;

    default:
        return hasMipmaps() ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
    }
}

GLint Texture::getMagFilter() const
{
    return (filterMode_ == TextureFilterMode::Nearest ? GL_NEAREST : GL_LINEAR);
}