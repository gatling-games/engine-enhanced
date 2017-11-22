#pragma once

#include <GL/gl3w.h>

class Texture;

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

private:
    // Constructor for internal use.
    Framebuffer(GLuint id);

public:
    // Gets a reference to the main backbuffer (aka framebuffer 0).
    static Framebuffer* backbuffer();

    // Prevent the framebuffer from being copied
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer) = delete;

    // The resolution of the framebuffer
    int width() const { return width_; }
    int height() const { return height_; }

    // Changes the framebuffer resolution
    // Note - this should be used for the back buffer only.
    void setResolution(int width, int height);

    // Sets the depth texture used by the framebuffer.
    void attachDepthTexture(const Texture* depthTexture);

    // Sets the color texture used by the framebuffer.
    void attachColorTexture(const Texture* colorTexture);

    // Sets this framebuffer as the active framebuffer.
    void use() const;

private:
    GLuint id_;
    int width_;
    int height_;
};
