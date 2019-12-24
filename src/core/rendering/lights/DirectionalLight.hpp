#include "Light.hpp"

class DirectionalLight : public Light {
private:
    glm::quat   _rotation;

public:
    DirectionalLight();
    DirectionalLight(const glm::vec4 &color, float intensity, const glm::quat &rotation);
    ~DirectionalLight();

    void bind(Camera &camera);
};