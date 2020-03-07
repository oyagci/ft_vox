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
        ALBEDO,
        DEPTH
    };

    int                         _width;
    int                         _height;
    Framebuffer                 _gbuffer;
    Shader                      _gbufferShader;
    Shader                      _quadShader;
    QuadPrimitive               _quad;
    std::array<glm::vec4, 8>    _frustumCorners;

    GLuint                      _ssaoKernelTexture;
    std::vector<GLfloat>        _ssaoKernel;
    GLuint                      _ssaoNoiseTexture;
    Framebuffer                 _ssaoPass;
    Shader                      _ssaoShader;

public:
    DeferredRenderer();
    ~DeferredRenderer();

    void resize(int width, int height);

    void genSSAOKernel(const int size);
    void genSSAONoise(const int size);
    void genSSAOPass(int width, int height);
    void bindSSAO(Camera &camera);

    void update(Camera &camera);
    void renderScene(Camera &camera, Scene &scene);
    void renderDebug();
    void bind(Shader &shader);
    void unbind();
    void render(Light &light, Camera &camera);

    Shader getGBufferShader() const { return _gbufferShader; }
    Framebuffer getGBuffer() const { return _gbuffer; }
    Framebuffer getSSAOPass() const { return _ssaoPass; }
};
