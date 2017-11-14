#include "Texture.h"

#include <algorithm>
#include <memory>

// Store required information for each TextureFormat type.
struct TextureFormatData
{
    int blockWidth;
    int blockHeight;
    int blockSize;
    GLenum glType;
    GLenum glFormat;
    GLint glInternalFormat;
    bool compressed;
};

// See https://www.khronos.org/opengl/wiki/Image_Format
TextureFormatData formatsTable[] = {
    // blk w/h/sz,  gl type,        gl format,   gl int. format,                compressed 
    {   4, 4, 8,  GL_UNSIGNED_BYTE,  GL_RGB,    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,  true }, // RGB_DXT1
    {   4, 4, 16, GL_UNSIGNED_BYTE,  GL_RGBA,   GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, true }, // RGBA_DXT5
    {   1, 1, 3,  GL_UNSIGNED_BYTE,  GL_RGB,    GL_RGB8,                          false }, // RGB8
    {   1, 1, 4,  GL_UNSIGNED_BYTE,  GL_RGBA,   GL_RGBA8,                         false }, // RGBA8
    {   1, 1, 1,  GL_UNSIGNED_BYTE,  GL_RED,    GL_R8,                            false }, // R8
    {   1, 1, 4,  GL_FLOAT,          GL_RED,    GL_R32F,                          false }, // RFloat
    {   1, 1, 2,  GL_UNSIGNED_SHORT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,    false }, // Depth
    {   1, 1, 2,  GL_UNSIGNED_SHORT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,    false } // ShadowMap
};

TextureFormatData* getFormatData(TextureFormat type)
{
    return formatsTable + (int)type;
}

Texture::Texture(TextureFormat format, int width, int height)
    : Resource(NOT_SAVED_RESOURCE),
    format_(format),
    wrapMode_(TextureWrapMode::Clamp),
    filterMode_(TextureFilterMode::Nearest),
    width_(width),
    height_(height),
    levels_(1)
{
    // Create the actual data immediately.
    createGLTexture();
}

Texture::Texture(ResourceID resourceID)
    : Resource(resourceID),
    format_(TextureFormat::Unknown),
    wrapMode_(TextureWrapMode::Clamp),
    filterMode_(TextureFilterMode::Nearest),
    width_(0),
    height_(0),
    levels_(0),
    glid_(0),
    loaded_(false)
{
    // This texture is a resource file.
    // The texture is created in load().
    // This is called automatically by the resource manager
}

Texture::~Texture()
{
    destroyGLTexture();
}

void Texture::load(std::ifstream& file)
{
    // Unload any existing texture
    destroyGLTexture();

    // Texture resources are stored as DDS files.

    // Check the dds magic number is correct
    uint32_t ddsMagicNumber;
    file.read((char*)&ddsMagicNumber, sizeof(ddsMagicNumber));
    if (ddsMagicNumber != 0x20534444)
    {
        printf("Failed to load texture \n");
        printf("DDS magic number 0x%x incorrect \n", ddsMagicNumber);
        return;
    }

    // Next read the header data
    uint32_t ddsHeader[31];
    file.read((char*)ddsHeader, sizeof(ddsHeader));

    // Read the file width and height and check they are valid
    width_ = ddsHeader[3];
    height_ = ddsHeader[2];
    if (width_ < 1 || width_ > 2048 || height_ < 1 || height_ > 2048)
    {
        printf("Failed to load texture \n");
        printf("Image resolution %dx%d invalid \n", width_, height_);
        return;
    }

    // Read the file format and check it is valid
    const uint32_t ddsFileFormat = ddsHeader[20];
    const uint32_t DDS_DXT1_HEADER = 0x31545844;
    const uint32_t DDS_DXT5_HEADER = 0x35545844;
    if(ddsFileFormat == DDS_DXT1_HEADER)
    {
        format_ = TextureFormat::RGB_DXT1;
    }
    else if(ddsFileFormat == DDS_DXT5_HEADER)
    {
        format_ = TextureFormat::RGBA_DXT5;
    }
    else
    {
        printf("Failed to load texture \n");
        printf("Image format 0x%x invalid \n", ddsFileFormat);
        return;
    }

    // Read the number of mip levels and check it is valid
    levels_ = ddsHeader[6];
    if(levels_ <= 0 || levels_ > 100)
    {
        printf("Failed to load texture \n");
        printf("Image levels %d invalid \n", levels_);
    }

    // Now create the actual texture
    createGLTexture();

    // Get the format description
    TextureFormatData* typeData = getFormatData(format_);

    // Upload texture data from the dds file for each mip level
    for (int level = 0; level < levels_; ++level)
    {
        // Compute the size of the mip
        const int mipWidth = getMipWidth(level);
        const int mipHeight = getMipHeight(level);
        const int mipSize = getMipSize(level);

        // Load the mip from the file.
        std::unique_ptr<uint8_t[]> mipData(new uint8_t[mipSize]);
        file.read((char*)mipData.get(), mipSize);

        // Upload the mip data to the gpu.
        glCompressedTexImage2D(GL_TEXTURE_2D, level, typeData->glInternalFormat,
            mipWidth, mipHeight, 0, mipSize, mipData.get());
    }
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

int Texture::getMipWidth(int level) const
{
    return max(1, width_ >> level);
}

int Texture::getMipHeight(int level) const
{
    return max(1, height_ >> level);
}

int Texture::getMipSize(int level) const
{
    // Get the size of the mip in pixels
    const int mipWidth = getMipWidth(level);
    const int mipHeight = getMipHeight(level);

    // From this, compute the number of blocks
    TextureFormatData* typeData = getFormatData(format_);
    const int blocksX = (mipWidth + typeData->blockWidth - 1) / typeData->blockWidth;
    const int blocksY = (mipHeight + typeData->blockHeight - 1) / typeData->blockHeight;
    const int blocksCount = blocksX * blocksY;

    // Finally, compute the mip size
    return blocksCount * typeData->blockSize;
}

void Texture::createGLTexture()
{
    // Destroy any existing texture
    if (loaded_)
    {
        destroyGLTexture();
    }

    // Get the format description
    TextureFormatData* typeData = getFormatData(format_);

    // Create the actual texture
    glGenTextures(1, &glid_);
    glBindTexture(GL_TEXTURE_2D, glid_);
    glTexStorage2D(GL_TEXTURE_2D, levels_, typeData->glInternalFormat, width_, height_);

    // Mark as loaded
    loaded_ = true;
}

void Texture::destroyGLTexture()
{
    if (loaded_)
    {
        glDeleteTextures(1, &glid_);
        loaded_ = false;
    }
}

void Texture::applySettings() const
{
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