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
}

void RenderingEngine::renderScene(Scene &_scene)
{
    _pipeline.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _pipeline.getShader().bind();
    _pipeline.getShader().setUniform4x4f("projectionMatrix", _camera.getProjectionMatrix());
    _pipeline.getShader().setUniform4x4f("viewMatrix", _camera.getViewMatrix());
    _pipeline.getShader().setUniform4x4f("viewProjectionMatrix", _camera.getViewProjectionMatrix());
    Shader shader = _pipeline.getShader();
    _scene.render(shader);
    
    _pipeline.unbind();
}

void RenderingEngine::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _pipeline.render();
}