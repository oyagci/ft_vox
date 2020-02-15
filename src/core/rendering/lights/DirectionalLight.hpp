#include "Light.hpp"

class DirectionalLight : public Light {
private:
    glm::quat   _rotation;
    glm::vec3   _direction;

public:
    DirectionalLight();
    DirectionalLight(const glm::vec4 &color, float intensity, const glm::quat &rotation);
    ~DirectionalLight();

    void initShadows(int resolution);

    void bind(Camera &camera);
    glm::mat4 getViewMatrix(float width, float height, Camera &camera);
};