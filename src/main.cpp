#include <iostream>
#include "lazy.hpp"
#include "Cube.hpp"
#include "Chunk.hpp"
#include "Time.hpp"
#include "Chunk.hpp"
#include "ChunkRenderer.hpp"
#include "ChunkFactory.hpp"
#include "TextRenderer.hpp"
#include "FPSCounter.hpp"
#include "WorldRenderer.hpp"

using namespace lazy;
using namespace graphics;
using namespace inputs;
using namespace utils;

int main()
{
	Display display("LazyGL", 1280, 720);
	display.enableCap(GL_DEPTH_TEST);
	display.enableCap(GL_CULL_FACE);
	display.enableCap(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	TextRenderer tr;

	Shader shader;
	shader.addVertexShader("shaders/basic.vs.glsl")
			.addFragmentShader("shaders/basic.fs.glsl");
	shader.link();

	glm::vec3 camPos(0, 64, 300);

	Camera camera(display, (maths::transform){camPos, glm::quat(), glm::vec3(1), nullptr});
	camera.setProjection(70.0f, 0.1f, 1000.0f);

	WorldRenderer wr(camera, camPos);

	wr.generateChunks();

	FPSCounter fpsCounter;

	float ms = 30.0f;
	while (!display.isClosed())
	{
		Time::instance().update();
		display.update();
		display.updateInputs();
		camera.update();

		float deltaTime = Time::getDeltaTime();

		fpsCounter.update(deltaTime);

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

		wr.renderChunks();

		tr.drawText(std::to_string(fpsCounter.getFPS()) + " FPS", glm::vec2(10, 10), .3f, glm::vec3(1.0f));
	}

	return EXIT_SUCCESS;
}
