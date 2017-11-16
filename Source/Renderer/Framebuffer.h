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
    static const Framebuffer* backbuffer();

    // Prevent the framebuffer from being copied
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer) = delete;

    // Sets the depth texture used by the framebuffer.
    void attachDepthTexture(const Texture* depthTexture);

    // Sets the color texture used by the framebuffer.
    void attachColorTexture(const Texture* colorTexture);

    // Sets this framebuffer as the active framebuffer.
    void use() const;

private:
    GLuint id_;
};
