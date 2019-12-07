#pragma once

#include <lazy.hpp>

#include "Scene.hpp"

using namespace lazy;
using namespace graphics;

class RenderingEngine {
private:
    const Display   &_display;

    CubePrimitive   _basicCube;
    Shader          _basicShader;
    Camera          _camera;

public:
    RenderingEngine(const Display &display);
    ~RenderingEngine();

    void update();
    void render(Scene &_scene);
};