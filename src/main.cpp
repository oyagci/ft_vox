#include <iostream>
#include "lazy.hpp"
#include "Cube.hpp"
#include "Chunk.hpp"
#include "Time.hpp"

using namespace lazy;
using namespace graphics;
using namespace inputs;
using namespace utils;

int main()
{
	Display display("LazyGL", 1280, 720);
	display.enableCap(GL_DEPTH_TEST);
	display.enableCap(GL_CULL_FACE);

	Camera camera(display, (maths::transform){glm::vec3(0, 0, 5), glm::quat(), glm::vec3(1), nullptr});
	camera.setProjection(70.0f, 0.1f, 1000.0f);

	Cube cube(&camera, 0, 0, 0);

	Shader shader;

	shader.addVertexShader("shaders/basic.vs.glsl")
		.addFragmentShader("shaders/basic.fs.glsl");
	shader.link();
	shader.bind();
	shader.setUniform4x4f("projectionMatrix", camera.getProjectionMatrix());
	shader.setUniform4x4f("viewMatrix", camera.getViewMatrix());
	shader.setUniform4x4f("viewProjectionMatrix", camera.getViewProjectionMatrix());
	shader.unbind();

	Chunk chunk(&shader);

	glm::vec3 camPos(0, 0, 5);

	while (!display.isClosed())
	{
		Time::instance().update();
		display.update();
		camera.update();
		display.updateInputs();

		float deltaTime = Time::getDeltaTime();

		float ms = 10.0f;
		if (input::getKeyboard().getKey(GLFW_KEY_W)) {
			camPos += deltaTime * vec3(0, 0, 1) * -ms;
			camera.setPosition(std::move(camPos));
		}
		if (input::getKeyboard().getKey(GLFW_KEY_S)) {
			camPos += deltaTime * vec3(0, 0, 1) * ms;
			camera.setPosition(std::move(camPos));
		}
		if (input::getKeyboard().getKey(GLFW_KEY_D)) {
			camPos += deltaTime * vec3(1, 0, 0) * ms;
			camera.setPosition(std::move(camPos));
		}
		if (input::getKeyboard().getKey(GLFW_KEY_A)) {
			camPos += deltaTime * vec3(1, 0, 0) * -ms;
			camera.setPosition(std::move(camPos));
		}
		if (input::getKeyboard().getKey(GLFW_KEY_SPACE)) {
			camPos += deltaTime * vec3(0, 1, 0) * ms;
			camera.setPosition(std::move(camPos));
		}
		if (input::getKeyboard().getKey(GLFW_KEY_LEFT_CONTROL)) {
			camPos += deltaTime * vec3(0, 1, 0) * -ms;
			camera.setPosition(std::move(camPos));
		}
		if (input::getKeyboard().getKey(GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(display.getWindow(), GLFW_TRUE);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.bind();
			shader.setUniform4x4f("projectionMatrix", camera.getProjectionMatrix());
			shader.setUniform4x4f("viewMatrix", camera.getViewMatrix());
			shader.setUniform4x4f("viewProjectionMatrix", camera.getViewProjectionMatrix());
		shader.unbind();

		chunk.onRender();
	}

	return 0;
}
