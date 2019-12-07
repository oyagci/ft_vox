#include <iostream>
#include <lazy.hpp>

#include "./core/rendering/RenderingEngine.hpp"

using namespace lazy;
using namespace graphics;
using namespace inputs;

int main()
{
	Display display("LazyGL", 1280, 720);
	display.enableCap(GL_DEPTH_TEST);
	display.enableCap(GL_CULL_FACE);

	Scene scene;
	RenderingEngine renderingEngine(display);

	float angle = 0;
	int frames = 0;
	double startTime = glfwGetTime();
	while (!display.isClosed())
	{
		display.update();

		double currentTime = glfwGetTime();
		if (currentTime - startTime >= 1.0 / 60.0)
		{
			display.updateInputs();
			renderingEngine.update();
			scene.update();
			startTime = currentTime;
		}
	
		renderingEngine.render(scene);
	}

	return 0;
}
