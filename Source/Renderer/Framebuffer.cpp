#include "Framebuffer.h"

#include <assert.h>

#include "Texture.h"

Framebuffer::Framebuffer()
{
    glGenFramebuffers(1, &id_);
}

Framebuffer::~Framebuffer()
{
    if (id_ != 0)
    {
        glDeleteFramebuffers(1, &id_);
    }
}

Framebuffer::Framebuffer(GLuint id)
    : id_(id),
    width_(0),
    height_(0)
{
    
}

Framebuffer* Framebuffer::backbuffer()
{
    static Framebuffer bb(0);
    return &bb;
}

void Framebuffer::setResolution(int width, int height)
{
    // This should be used for the back buffer only.
    // Other framebuffers infer their resolution from the
    // textures that are attached to them.
    assert(this == backbuffer());

    width_ = width;
    height_ = height;
}

void Framebuffer::attachDepthTexture(const Texture* depthTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->glid(), 0);

    // Store the width and height of the framebuffer
    width_ = depthTexture->width();
    height_ = depthTexture->height();
}

void Framebuffer::attachColorTexture(const Texture* colorTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture->glid(), 0);

    // Store the width and height of the framebuffer
    width_ = colorTexture->width();
    height_ = colorTexture->height();
}

void Framebuffer::use() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
}
