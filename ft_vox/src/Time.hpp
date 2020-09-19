#pragma once

#include <GLFW/glfw3.h>

namespace lazy {
	namespace utils {
		class Time {
		public:
			static Time &instance()
			{
				static Time t;

				return t;
			}

		private:
			Time() {};
			Time(Time const &) = delete;
			void operator=(Time const&) = delete;

		public:
			void update()
			{
				float currentFrame = (float)glfwGetTime();

				_deltaTime = currentFrame - _lastFrame;
				_lastFrame = currentFrame;
			}

			static float getDeltaTime()
			{
				return instance()._deltaTime;
			};

		private:
			float _deltaTime = 0.0f;
			float _lastFrame = 0.0f;
		};
	}
}
