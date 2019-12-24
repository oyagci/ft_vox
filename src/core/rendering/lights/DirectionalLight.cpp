#include "DirectionalLight.hpp"

DirectionalLight::DirectionalLight()
:   DirectionalLight(glm::vec4(1, 1, 1, 1), 1, glm::quat(glm::vec3(0, 0, 0)))
{}

DirectionalLight::DirectionalLight(const glm::vec4 &color, float intensity, const glm::quat &rotation)
:   Light("directionallight", color, intensity), _rotation(rotation)
{}

DirectionalLight::~DirectionalLight()
{}

void DirectionalLight::bind(Camera &camera)
{
    _shader.bind();
    _shader.setUniform4f("light.light.color", _color);
    _shader.setUniform1f("light.light.intensity", _intensity);
    _shader.setUniform3f("light.direction", glm::vec3(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f) * glm::toMat4(_rotation)));
}