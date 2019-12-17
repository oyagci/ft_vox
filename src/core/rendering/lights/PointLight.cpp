#include "./PointLight.hpp"

PointLight::PointLight()
:   PointLight(glm::vec4(1, 1, 1, 1), 1, glm::vec3(0, 0, 0))
{}

PointLight::PointLight(const glm::vec4 &color, float intensity, const glm::vec3 &position)
:   Light(color, intensity), _position(position)
{}

PointLight::~PointLight()
{
    
}

void PointLight::bind(Shader &shader)
{
    Light::bind(shader);
    shader.setUniform3f("light.position", _position);
}