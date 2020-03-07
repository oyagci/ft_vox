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
    QuadPrimitive       _quad;
    Shader              _quadShader;
    Shader              _quadShaderDebug;
    int                 _width;
    int                 _height;
    bool                _resized;

public:
    RenderingPipeline();
    ~RenderingPipeline();

    void resize(int width, int height);

    void bind();
    void unbind();

    void update(Camera &camera);

    void renderScene(Camera &camera, Scene &scene);
    void renderShadows(Light &light, Camera &camera, Scene &scene);
    void renderDeferred(Light &light, Camera &camera);
    void renderForward(std::vector<Light*> &lights, Camera &camera);
    void renderScreenQuad();
    void render(std::vector<Light*> &lights, Camera &camera);

    Shader getShader() const { return _deferred.getGBufferShader(); }
};
