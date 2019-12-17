#include "./RenderingEngine.hpp"

RenderingEngine::RenderingEngine(const Display &display)
:   _display(display),
    _camera(Camera(display, (maths::transform){glm::vec3(0, 0, 5), glm::quat(), glm::vec3(1), nullptr}))
{
    _camera.setProjection(70.0f, 0.1f, 10.0f);

	_basicShader.addVertexShader("shaders/basic.vs.glsl")
			    .addFragmentShader("shaders/basic.fs.glsl");
	_basicShader.link();

    _pipeline.resize(display.getWidth(), display.getHeight());
}

RenderingEngine::~RenderingEngine()
{

}

void RenderingEngine::update()
{
    _camera.update();
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
}

void RenderingEngine::renderScene(Scene &_scene)
{
    _pipeline.renderScene(_camera, _scene);
}

void RenderingEngine::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _pipeline.render(_ligths);
}