#include "./Light.hpp"

Light::Light()
:   Light(glm::vec4(1, 1 , 1, 1), 1)
{}

Light::Light(const glm::vec4 &color, float intensity)
:   _color(color), _intensity(intensity)
{}

Light::~Light()
{}

void Light::bind(Shader &shader)
{
    shader.setUniform1f("light.intensity", _intensity);
    shader.setUniform4f("light.color", _color);
}