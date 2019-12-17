#pragma once

#include <lazy.hpp>

#include "Scene.hpp"
#include "core/rendering/primitives/QuadPrimitive.hpp"
#include "core/rendering/lights/Light.hpp"

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
    Shader          _gbufferShader;
    Shader          _quadShader;
    QuadPrimitive   _quad;

public:
    DeferredRenderer();
    ~DeferredRenderer();

    void resize(int width, int height);

    void update();
    void renderScene(Camera &camera, Scene &scene);
    void renderDebug();
    void bind(Shader &shader);
    void unbind();
    void render(const Light &light);

    Shader getGBufferShader() const { return _gbufferShader; }
    Framebuffer getGBuffer() const { return _gbuffer; }
};
