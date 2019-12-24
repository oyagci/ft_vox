#pragma once

#include <lazy.hpp>
#include <vector>

#include "./RenderingPipeline.hpp"
#include "./primitives/QuadPrimitive.hpp"
#include "./lights/Light.hpp"
#include "Scene.hpp"

using namespace lazy;
using namespace graphics;

class RenderingEngine {
private:
    const Display       &_display;

    RenderingPipeline   _pipeline;
    Shader              _basicShader;
    Camera              _camera;

    std::vector<Light*>  _lights;

public:
    RenderingEngine(const Display &display);
    ~RenderingEngine();

    void addLight(Light *light);

    void handleResize();
    void update();
    void renderScene(Scene &_scene);
    void render();
};