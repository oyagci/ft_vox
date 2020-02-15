#pragma once

#include <lazy.hpp>

using namespace lazy;
using namespace graphics;

class Light {
protected:
    Shader      _shader;
    glm::vec4   _color;
    float       _intensity;
    std::string _name;

    Shader      _shadowShader;
    Framebuffer _shadowFbo;
    bool        _emmitShadows;
    glm::mat4   _shadowProjection;
    int         _resolution;

public:
    Light();
    Light(const glm::vec4 &color, float intensity);
    Light(const std::string &name, const glm::vec4 &color, float intensity);
    virtual ~Light();

    virtual void bind(Camera &camera);
    virtual glm::mat4 getViewMatrix(float width, float height, Camera &camera);

    virtual void initShadows(int resolution) {};

    Shader  &getShader() { return _shader; }
    Shader  &getShadowShader() { return _shadowShader; }
    Framebuffer  &getShadowFBO() { return _shadowFbo; }
    int getResolution() { return _resolution; }
};
