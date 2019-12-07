#pragma once

#include <lazy.hpp>

class Framebuffer
{
private:
    int _width;
    int _height;

    GLuint _fbo;
    GLuint _rbo;
    GLuint _cbo;
    GLuint _dbo;

public:
    Framebuffer();
    ~Framebuffer();

    void setSize(int width, int height);

    void genFramebuffer();
    void genRenderbuffer(GLuint type, GLuint attachment);
    void genColorTexture(GLuint filterMode, GLuint wrapMode);
    void genDepthTexture(GLuint filterMode, GLuint wrapMode);

    void bind();
    void unbind();

    GLuint getFramebuffer() const { return _fbo; }
    GLuint getRenderbuffer() const { return _rbo; }
    GLuint getColorTexture() const { return _cbo; }
    GLuint getDepthTexture() const { return _dbo; }
};