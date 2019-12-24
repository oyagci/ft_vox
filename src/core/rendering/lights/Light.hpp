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

public:
    Light();
    Light(const glm::vec4 &color, float intensity);
    Light(const std::string &name, const glm::vec4 &color, float intensity);
    virtual ~Light();

    virtual void bind(Camera &camera);

    Shader  &getShader() { return _shader; }
};
