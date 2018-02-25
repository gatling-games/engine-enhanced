#include "Texture.h"

#include <algorithm>
#include <memory>
#include <cassert>

#include "imgui.h"

// Ensure that srgb dxt parameters have been defined
#ifndef GL_COMPRESSED_SRGB_S3TC_DXT1_EXT
#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT 0x8C4C
#endif
#ifndef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT 0x8C4F
#endif

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
    {   "RGB_DXT1",      4, 4, 8,     GL_COMPRESSED_RGB_S3TC_DXT1_EXT,    true        }, // RGB_DXT1
    {   "RGB_DXT1_SRGB", 4, 4, 8,     GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,   true        }, // RGB_DXT1_SRGB
    {   "RGBA_DXT5",     4, 4, 16,    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,   true        }, // RGBA_DXT5
    {   "RGBA_DXT5_SRGB",4, 4, 16,    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, true    }, // RGBA_DXT5_SRGB
    {   "RGB8",          1, 1, 3,     GL_RGB8,                            false       }, // RGB8
    {   "RGB8_SRGB",     1, 1, 3,     GL_SRGB8,                           false       }, // RGB8_SRGB
    {   "RGBA8",         1, 1, 4,     GL_RGBA8,                           false       }, // RGBA8
    {   "RGBA8_SRGB",    1, 1, 4,     GL_SRGB8_ALPHA8,                    false       }, // RGBA8_SRGB
    {   "RGBA1010102",   1, 1, 4,     GL_RGB10_A2,                        false       }, // RGBA1010102
    {   "R8",            1, 1, 1,     GL_R8,                              false       }, // R8
    {   "R16",           1, 1, 2,     GL_R16,                             false       }, // R16
    {   "R32F",          1, 1, 4,     GL_R32F,                            false       }, // RFloat
    {   "Depth 24",      1, 1, 2,     GL_DEPTH32F_STENCIL8,               false       }, // Depth
    {   "ShadowMap",     1, 1, 2,     GL_DEPTH_COMPONENT16,               false       }, // ShadowMap
};

TextureFormatData* getFormatData(TextureFormat type)
{
    return formatsTable + (int)type;
}

ArrayTexture::ArrayTexture(TextureFormat format, int width, int height, int layers)
    : format_(format),
    filterMode_(TextureFilterMode::Bilinear),
    width_(width),
    height_(height)
{
    // Get details for the texture format.
    TextureFormatData* formatData = getFormatData(format);

    // Create the texture array and allocate storage
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &glid_);
    glTextureStorage3D(glid_, 1, formatData->glInternalFormat, width, height, layers);

    // Set shadow mapping compare mode if required
    if (format_ == TextureFormat::ShadowMap)
    {
        glTextureParameteri(glid_, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTextureParameteri(glid_, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        // Also give the texture a border so that sampling outside the texture does
        // not cause issues
        glTextureParameteri(glid_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(glid_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        const float borderValues[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTextureParameterfv(glid_, GL_TEXTURE_BORDER_COLOR, borderValues);
    }
}

ArrayTexture::~ArrayTexture()
{
    if (glid_ > 0)
    {
        glDeleteTextures(1, &glid_);
    }
}

ArrayTexture::ArrayTexture(ArrayTexture&& other)
{
    format_ = other.format_;
    filterMode_ = other.filterMode_;
    width_ = other.width_;
    height_ = other.height_;
    layers_ = other.layers_;
    glid_ = other.glid_;

    other.glid_ = 0;
}

ArrayTexture& ArrayTexture::operator=(ArrayTexture&& other)
{
    if(this != &other)
    {
        format_ = other.format_;
        filterMode_ = other.filterMode_;
        width_ = other.width_;
        height_ = other.height_;
        layers_ = other.layers_;
        glid_ = other.glid_;

        other.glid_ = 0;
    }

    return *this;
}

void ArrayTexture::bind(int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, glid_);
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
    if (created_)
    {
        destroyGLTexture();
    }

    // DDS files start with the magic number 0x20534444
    // First, read the DDS magic number and check it is correct.
    uint32_t magicNumber;
    file.read((char*)&magicNumber, sizeof(uint32_t));
    assert(file.good());
    if (magicNumber != 0x20534444)
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

    // Determine whether to use srgb formats.
    // For now, use this only for albedo textures.
    const bool srgb = resourceName().find("_albedo.") != std::string::npos;

    // Find out which type of DXT compression the file uses.
    const uint32_t DDS_DXT1_HEADER = 0x31545844;
    const uint32_t DDS_DXT5_HEADER = 0x35545844;
    TextureFormat format;
    if (header.ddspf.dwFourCC == DDS_DXT1_HEADER)
    {
        format = srgb ? TextureFormat::RGB_DXT1_SRGB : TextureFormat::RGB_DXT1;
    }
    else if (header.ddspf.dwFourCC == DDS_DXT5_HEADER)
    {
        format = srgb ? TextureFormat::RGBA_DXT5_SRGB : TextureFormat::RGBA_DXT5;
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
        glCompressedTextureSubImage2D(glid_, mipLevel, 0, 0, mipWidth, mipHeight, typeData->glInternalFormat, mipSize, mipData.get());
    }

    // The texture is now loaded. We just need to check that all texture 
    // sampling setting are applied correctly to the texture.
    applySettings();

    // Settings are now applied. Make the texture resident in memory.
    PFNGLGETTEXTUREHANDLEARBPROC gettexturehandle = (PFNGLGETTEXTUREHANDLEARBPROC)gl3wGetProcAddress("glGetTextureHandleARB");
    handle_ = gettexturehandle(glid_);
    PFNGLMAKETEXTUREHANDLERESIDENTARBPROC makeresident = (PFNGLMAKETEXTUREHANDLERESIDENTARBPROC)gl3wGetProcAddress("glMakeTextureHandleResidentARB");
    makeresident(handle_);
}

void Texture::unload()
{
    destroyGLTexture();
}

void Texture::drawEditor()
{
    int resolution[] = { width(), height() };
    ImGui::InputInt2("Resolution", resolution, ImGuiInputTextFlags_ReadOnly);

    const std::string levels = std::to_string(levels_);
    ImGui::InputText("Levels", (char*)levels.c_str(), levels.length(), ImGuiInputTextFlags_ReadOnly);

    const std::string& formatName = getFormatName(format());
    ImGui::InputText("Format", (char*)formatName.c_str(), formatName.length(), ImGuiInputTextFlags_ReadOnly);

    const float width = ImGui::GetContentRegionAvailWidth();
    const float height = width * (resolution[0] / (float)resolution[1]);
    ImGui::Image((ImTextureID)glid_, ImVec2(width, height));
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

void Texture::setData(const void* data, int dataSizeBytes, int mipLevel)
{
    assert(!isCompressed());
    assert(dataSizeBytes == getMipSize(format_, width_, height_, mipLevel));
    
    // This currently only works for R16 textures (for the terrain)
    assert(format_ == TextureFormat::R16);

    glTextureSubImage2D(glid_, mipLevel, 0, 0, width_ >> mipLevel, height_ >> mipLevel,
        GL_RED, GL_UNSIGNED_SHORT, data);
}

const std::string& Texture::getFormatName(TextureFormat format)
{
    return getFormatData(format)->name;
}

int Texture::getMipWidth(int fullWidth, int mipLevel)
{
    // Each mip is half the width of the one before it.
    // Ensure that each mip is no smaller than 1px.
    return std::max(1, fullWidth >> mipLevel);
}

int Texture::getMipHeight(int fullHeight, int mipLevel)
{
    // Each mip is half the height of the one before it.
    // Ensure that each mip is no smaller than 1px.
    return std::max(1, fullHeight >> mipLevel);
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
    glCreateTextures(GL_TEXTURE_2D, 1, &glid_);

    // Assign texture storage for the correct format, resolution and mip count.
    glTextureStorage2D(glid_, levels_, formatData->glInternalFormat, width_, height_);

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
    if (created_ == false)
    {
        return;
    }

    // Set wrap mode
    glTextureParameteri(glid_, GL_TEXTURE_WRAP_S, getWrapS());
    glTextureParameteri(glid_, GL_TEXTURE_WRAP_T, getWrapT());

    // Set filter mode
    glTextureParameteri(glid_, GL_TEXTURE_MIN_FILTER, getMinFilter());
    glTextureParameteri(glid_, GL_TEXTURE_MAG_FILTER, getMagFilter());

    // Set anisotropic filtering
    if (filterMode_ == TextureFilterMode::Anisotropic)
    {
        GLfloat maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
        glTextureParameterf(glid_, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
    }
    else
    {
        const GLfloat minAniso = 1.0f;
        glTextureParameterf(glid_, GL_TEXTURE_MAX_ANISOTROPY_EXT, minAniso);
    }

    // Set shadow mapping compare mode
    if (format_ == TextureFormat::ShadowMap)
    {
        glTextureParameteri(glid_, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTextureParameteri(glid_, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
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