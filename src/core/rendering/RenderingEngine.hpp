#pragma once

#include <lazy.hpp>

#include "./pipeline/RenderingPipeline.hpp"
#include "./primitives/QuadPrimitive.hpp"
#include "Scene.hpp"

using namespace lazy;
using namespace graphics;

class RenderingEngine {
private:
    const Display       &_display;

    RenderingPipeline   _pipeline;
    Shader              _basicShader;
    Camera              _camera;

public:
    RenderingEngine(const Display &display);
    ~RenderingEngine();

    void update();
    void renderScene(Scene &_scene);
    void render();
};