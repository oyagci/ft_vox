#include "./PointLight.hpp"

PointLight::PointLight()
:   PointLight(glm::vec4(1, 1, 1, 1), 1, glm::vec3(0, 0, 0))
{}

PointLight::PointLight(const glm::vec4 &color, float intensity, const glm::vec3 &position)
:   Light("pointlight", color, intensity), _position(position)
{}

PointLight::~PointLight()
{}

glm::mat4 PointLight::getViewMatrix(float width, float height, Camera &camera)
{
    return glm::mat4(1);
}

void PointLight::bind(Camera &camera)
{
    _shader.bind();
    _shader.setUniform4f("light.light.color", _color);
    _shader.setUniform1f("light.light.intensity", _intensity);
    _shader.setUniform3f("light.position", _position);
}