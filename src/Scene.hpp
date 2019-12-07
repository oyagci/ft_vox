#pragma once

#include "./core/rendering/primitives/CubePrimitive.hpp"

class Scene
{
private:
    CubePrimitive _basicCube;

public:
    Scene();
    ~Scene();

    void update();
    void render(Shader &shader);
};