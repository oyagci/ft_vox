#include "./Framebuffer.hpp"

Framebuffer::Framebuffer()
{}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &_fbo);
}

void Framebuffer::setSize(int width, int height)
{
    _width = width;
    _height = height;
}

void Framebuffer::genFramebuffer()
{
    glGenFramebuffers(1, &_fbo);
}

void Framebuffer::genRenderbuffer(GLuint type, GLuint attachment)
{
    this->bind();
    glGenRenderbuffers(1, &_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, type, _width, _height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_RENDERBUFFER, attachment, GL_RENDERBUFFER, _rbo);
    this->unbind();
}

void Framebuffer::genColorTexture(GLuint internalFormat, GLint format, GLuint type, GLuint filterMode, GLuint wrapMode)
{
    this->bind();
    GLuint cbo;
    glGenTextures(1, &cbo);
    glBindTexture(GL_TEXTURE_2D, cbo);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
    if (wrapMode != NULL)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _cbos.size(), GL_TEXTURE_2D, cbo, 0);
    this->unbind();

    _cbos.push_back(cbo);
}

void Framebuffer::genDepthTexture(GLuint filterMode, GLuint wrapMode)
{
    this->bind();
    glGenTextures(1, &_dbo);
    glBindTexture(GL_TEXTURE_2D, _dbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
    if (wrapMode != NULL)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _dbo, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    this->unbind();
}

void Framebuffer::drawBuffers(std::vector<GLenum> attachments)
{
    glDrawBuffers(attachments.size(), attachments.data());
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}