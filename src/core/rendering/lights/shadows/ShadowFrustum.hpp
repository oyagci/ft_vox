#include <lazy.hpp>

using namespace lazy;
using namespace graphics;

class ShadowFrustum
{
private:
    std::array<glm::vec4, 8>    _corners;
    glm::vec3                   _min;
    glm::vec3                   _max;

public:
    ShadowFrustum();
    ~ShadowFrustum() {};

    void calculate(Light &light);

    std::array<glm::vec4, 8> getCorners() const { return _corners; }
    glm::vec3 getMin() const { return _min; }
    glm::vec3 getMax() const { return _max; }
};