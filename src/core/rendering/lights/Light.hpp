#pragma once

#include <lazy.hpp>

using namespace lazy;
using namespace graphics;

class Light {
private:
    glm::vec4   _color;
    float       _intensity;

public:
    Light();
    Light(const glm::vec4 &color, float intensity);
    virtual ~Light();

    virtual void bind(Shader &shader);
};