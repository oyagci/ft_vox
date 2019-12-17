#include "Light.hpp"

class PointLight : public Light {
private:
    glm::vec3   _position;

public:
    PointLight();
    PointLight(const glm::vec4 &color, float intensity, const glm::vec3 &position);
    ~PointLight();

    void bind(Shader &shader);
};