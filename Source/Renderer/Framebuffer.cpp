#include "Framebuffer.h"

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
    : id_(id)
{
    
}

const Framebuffer* Framebuffer::backbuffer()
{
    static Framebuffer bb(0);
    return &bb;
}

void Framebuffer::attachDepthTexture(const Texture* depthTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->glid(), 0);
}

void Framebuffer::attachColorTexture(const Texture* colorTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture->glid(), 0);
}

void Framebuffer::use() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
}
