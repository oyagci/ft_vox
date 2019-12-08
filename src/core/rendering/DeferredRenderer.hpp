#pragma once

#include <lazy.hpp>

#include "core/rendering/textures/Framebuffer.hpp"

using namespace lazy;
using namespace graphics;

class DeferredRenderer
{
private:
    enum Geom {
        POSITION,
        NORMAL,
        ALBEDO
    };

    int             _width;
    int             _height;
    Framebuffer     _gbuffer;
    Shader          _shader;

public:
    DeferredRenderer();
    ~DeferredRenderer();

    void resize(int width, int height);

    void update();
    void bind();
    void unbind();

    Shader getShader() const { return _shader; }
    Framebuffer getGBuffer() const { return _gbuffer; }
};
