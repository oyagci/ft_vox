#include "./Light.hpp"

Light::Light()
:   Light("light", glm::vec4(1, 1 , 1, 1), 1)
{}

Light::Light(const glm::vec4 &color, float intensity)
:   Light("light", color, intensity)
{}

Light::Light(const std::string &name, const glm::vec4 &color, float intensity)
:   _name(name), _color(color), _intensity(intensity)
{
    _shader.addVertexShader("shaders/lights/light.vs.glsl")
        .addFragmentShader("shaders/lights/" + name + ".fs.glsl");
	_shader.link();
}

Light::~Light()
{}

void Light::bind(Camera &camera)
{}
