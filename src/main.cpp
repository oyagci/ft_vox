#include <iostream>
#include "lazy.hpp"
#include "Cube.hpp"
#include "Chunk.hpp"
#include "Time.hpp"
#include "Chunk.hpp"
#include "ChunkRenderer.hpp"

using namespace lazy;
using namespace graphics;
using namespace inputs;
using namespace utils;

int main()
{
	Display display("LazyGL", 1280, 720);
	display.enableCap(GL_DEPTH_TEST);
	display.enableCap(GL_CULL_FACE);

	Mesh mesh;
	mesh.addPosition(glm::vec3(0, 0, 0))
		.addPosition(glm::vec3(1, 0, 0))
		.addPosition(glm::vec3(0, 1, 0))
		.addTriangle(glm::u32vec3(0, 1, 2));
	mesh.build();

	Shader shader;
	shader.addVertexShader("shaders/basic.vs.glsl")
			.addFragmentShader("shaders/basic.fs.glsl");
	shader.link();

	glm::vec3 camPos(0, 20, 50);

	Camera camera(display, (maths::transform){camPos, glm::quat(), glm::vec3(1), nullptr});
	camera.setProjection(70.0f, 0.1f, 1000.0f);

	Chunk chunk(&shader);
	ChunkRenderer chunkRenderer;

	chunkRenderer.setChunk(std::move(chunk));
	chunkRenderer.setShader(&shader);
	chunkRenderer.update();

	float ms = 10.0f;
	while (!display.isClosed())
	{
		Time::instance().update();
		display.update();
		display.updateInputs();
		camera.update();

		float deltaTime = Time::getDeltaTime();
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

		shader.setUniform4x4f("modelMatrix", glm::mat4(1.0f));
		mesh.draw();
		chunkRenderer.onRender();
	}

	return EXIT_SUCCESS;
}
