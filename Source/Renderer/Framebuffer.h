#pragma once

#include <GL/gl3w.h>

class Texture;
class ArrayTexture;

class Framebuffer
{
public:
    const static int MAX_COLOR_ATTACHMENTS = 16;

public:
    Framebuffer();
    ~Framebuffer();

private:
    // Constructor for internal use.
    explicit Framebuffer(GLuint id);

public:
    // Gets a reference to the main backbuffer (aka framebuffer 0).
    static Framebuffer* backbuffer();

    // Prevent the framebuffer from being copied
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer) = delete;

    // Gets the internal opengl id.
    GLuint glid() const { return id_; }

    // The resolution of the framebuffer
    int width() const { return width_; }
    int height() const { return height_; }

    // Changes the framebuffer resolution
    // Note - this should be used for the back buffer only.
    void setResolution(int width, int height);

    // Sets the depth texture used by the framebuffer.
    void attachDepthTexture(const Texture* depthTexture);

    // Sets the depth texture used by the framebuffer to a layer of an array texture.
    void attachDepthTexture(const ArrayTexture* depthTexture, int layer);

    // Sets the depth texture used by the framebuffer to the
    // texture used by another framebuffer.
    void attachDepthTextureFromFramebuffer(const Framebuffer* other);

    // Sets the color texture used by the framebuffer.
    void attachColorTexture(const Texture* colorTexture);

    // Sets multiple color textures for use by the framebuffer.
    // Count can be up to Framebuffer::MAX_COLOR_ATTACHMENTS
    void attachColorTexturesMRT(int count, const Texture** colorTextures);

    // Sets this framebuffer as the active framebuffer.
    void use() const;

private:
    GLuint id_;
    int width_;
    int height_;
};
