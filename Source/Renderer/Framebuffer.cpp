#include "Framebuffer.h"

#include <assert.h>

#include "Texture.h"

Framebuffer::Framebuffer()
    : width_(0),
    height_(0)
{
    glCreateFramebuffers(1, &id_);
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
    glNamedFramebufferTexture(id_, GL_DEPTH_ATTACHMENT, depthTexture->glid(), 0);

    // Store the width and height of the framebuffer
    width_ = depthTexture->width();
    height_ = depthTexture->height();
}

void Framebuffer::attachDepthTexture(const ArrayTexture* depthTexture, int layer)
{
    glNamedFramebufferTextureLayer(id_, GL_DEPTH_ATTACHMENT, depthTexture->glid(), 0, layer);

    // Store the width and height of the framebuffer
    width_ = depthTexture->width();
    height_ = depthTexture->height();
}

void Framebuffer::attachDepthTextureFromFramebuffer(const Framebuffer* other)
{
    // Get the name of the depth texture on the other framebuffer
    GLint depthTexture;
    glGetNamedFramebufferAttachmentParameteriv(other->id_, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &depthTexture);
    
    // Attach the depth texture
    glNamedFramebufferTexture(id_, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    // Store the width and height of the framebuffer
    width_ = other->width();
    height_ = other->height();
}

void Framebuffer::attachColorTexture(const Texture* colorTexture)
{
    // Attach the texture to the framebuffer
    glNamedFramebufferTexture(id_, GL_COLOR_ATTACHMENT0, colorTexture->glid(), 0);

    // Store the width and height of the framebuffer
    width_ = colorTexture->width();
    height_ = colorTexture->height();
}

void Framebuffer::attachColorTexturesMRT(int count, const Texture** colorTextures)
{
    // Support up to MAX_COLOR_ATTACHMENTS textures
    assert(count > 0);
    assert(count <= MAX_COLOR_ATTACHMENTS);

    // Check all the textures have the same resolution
    for(int i = 0; i < count; ++i)
    {
        assert(colorTextures[i]->width() == colorTextures[0]->width());
        assert(colorTextures[i]->height() == colorTextures[0]->height());
    }

    // Attach the texture to the framebuffer
    for(int i = 0; i < count; ++i)
    {
        glNamedFramebufferTexture(id_, GL_COLOR_ATTACHMENT0 + i, colorTextures[i]->glid(), 0);
    }

    // Set up the link between fragment shader outputs and fbo attachments.
    // Make an array of GL_COLOR_ATTACHMENT0 to GL_COLOR_ATTACHMENT_N and pass it to the framebuffer
    GLenum buffers[MAX_COLOR_ATTACHMENTS];
    for (int i = 0; i < MAX_COLOR_ATTACHMENTS; ++i) buffers[i] = GL_COLOR_ATTACHMENT0 + i;
    glNamedFramebufferDrawBuffers(id_, count, buffers);

    // Store the width and height of the framebuffer
    width_ = colorTextures[0]->width();
    height_ = colorTextures[0]->height();
}

void Framebuffer::use() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glViewport(0, 0, width_, height_);
}
