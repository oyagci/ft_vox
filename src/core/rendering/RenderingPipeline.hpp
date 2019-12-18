#pragma once

#include <lazy.hpp>

#include "Scene.hpp"
#include "core/rendering/primitives/QuadPrimitive.hpp"
#include "core/rendering/DeferredRenderer.hpp"

using namespace lazy;
using namespace graphics;

class RenderingPipeline
{
private:
    Framebuffer         _fbo;
    DeferredRenderer    _deferred;
    QuadPrimitive       _screenQuad;
    Shader              _quadShader;
    int                 _width;
    int                 _height;

public:
    RenderingPipeline();
    ~RenderingPipeline();

    void resize(int width, int height);

    void bind();
    void unbind();

    void renderScene(Camera &camera, Scene &scene);
    void renderDeferred(Light &light);
    void renderForward(std::vector<Light*> &lights);
    void renderScreenQuad();
    void render(std::vector<Light*> &lights);

    Shader getShader() const { return _deferred.getGBufferShader(); }
};
