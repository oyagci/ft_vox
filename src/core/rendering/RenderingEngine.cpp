#include "./RenderingEngine.hpp"
#include "./lights/PointLight.hpp"
#include "./lights/DirectionalLight.hpp"

RenderingEngine::RenderingEngine(const Display &display)
:   _display(display),
    _camera(Camera(display, (maths::transform){glm::vec3(0, 0, 5), glm::quat(), glm::vec3(1), nullptr}))
{
    _camera.setProjection(70.0f, 0.5f, 1000.0f);
	_basicShader.addVertexShader("shaders/basic.vs.glsl")
			    .addFragmentShader("shaders/basic.fs.glsl");
	_basicShader.link();
    _pipeline.resize(display.getWidth(), display.getHeight());

    // addLight(new PointLight(glm::vec4(1, 0, 0, 1), 10, glm::vec3(3, -0.5, 0)));
    // addLight(new PointLight(glm::vec4(0, 0, 1, 1), 1, glm::vec3(-3, 5, 0)));       
    // addLight(new PointLight(glm::vec4(0, 1, 0, 1), 1, glm::vec3(0, 5, 0)));            
    Light *light = new DirectionalLight(glm::vec4(0, 1, 1, 1), 1, glm::quat(glm::vec3(M_PI / 4, M_PI / 4, 0)));
    light->initShadows(2048);

    addLight(light);
}

RenderingEngine::~RenderingEngine()
{
    _lights.clear();
}

void RenderingEngine::addLight(Light *light)
{
    _lights.push_back(light);
}

void RenderingEngine::handleResize()
{
    if (_display.hasResized())
    {
        _camera.updateProjection();
        _pipeline.resize(_display.getWidth(), _display.getHeight());
    }
}

void RenderingEngine::update()
{
    if (_display.isFocused())
    {
        _camera.input(0.1f, 0.01f, {
            GLFW_KEY_W,
            GLFW_KEY_S,
            GLFW_KEY_A,
            GLFW_KEY_D,
            GLFW_KEY_LEFT_SHIFT,
            GLFW_KEY_SPACE
        });
    }

    _pipeline.update(_camera);
}

void RenderingEngine::renderScene(Scene &_scene)
{
    _pipeline.renderScene(_camera, _scene);
    _pipeline.renderShadows(*_lights[0], _camera, _scene);
}

void RenderingEngine::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _pipeline.render(_lights, _camera);
}