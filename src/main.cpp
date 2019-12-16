#include <iostream>
#include "lazy.hpp"
#include "Chunk.hpp"
#include "Time.hpp"
#include "Chunk.hpp"
#include "ChunkRenderer.hpp"
#include "ChunkFactory.hpp"
#include "TextRenderer.hpp"
#include "FPSCounter.hpp"
#include "World.hpp"

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
	display.setFullscreen(true);

	TextRenderer tr;

	glm::vec3 camPos(32, 64, 32);
	maths::transform t = { camPos, glm::quat(), glm::vec3(1), nullptr };

	Camera camera(display, std::move(t));
	camera.setProjection(glm::radians(80.0f), 0.1f, 1000.0f);

	World wr(camera, camPos);

	FPSCounter fpsCounter;

	while (!display.isClosed())
	{
		Time::instance().update();
		display.update();
		display.updateInputs();
		camera.update();

		float deltaTime = Time::getDeltaTime();

		fpsCounter.update(deltaTime);

		float ms = 30.0f;
		if (input::getKeyboard().getKey(GLFW_KEY_LEFT_SHIFT)) {
			ms = 500.0f;
		}
		if (input::getKeyboard().getKey(GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(display.getWindow(), GLFW_TRUE);
		}

		camera.input(ms * deltaTime, 0.001f, {
			GLFW_KEY_W,
			GLFW_KEY_S,
			GLFW_KEY_A,
			GLFW_KEY_D,
			GLFW_KEY_LEFT_CONTROL,
			GLFW_KEY_SPACE
		});


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		wr.update();
		wr.render();

		tr.drawText(std::to_string(fpsCounter.getFPS()) + " FPS", glm::vec2(10, 10), .3f, glm::vec3(1.0f));
	}

	return EXIT_SUCCESS;
}
