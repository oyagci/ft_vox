#pragma once

#include "lazy.hpp"
#include <memory>
#include "Quad.hpp"
#include <random>

using lazy::graphics::Shader;

class AmbientOcclusion
{
public:
	AmbientOcclusion()
	{
		Init();
	}

	void render(glm::mat4 const &projectionMatrix, GLuint positionTexture, GLuint normalTexture)
	{
		{
			glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
			_shader->bind();
			_shader->setUniform4x4f("projectionMatrix", projectionMatrix);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, positionTexture);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalTexture);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, _noiseTexture);

			_renderQuad.draw();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		{
			glBindFramebuffer(GL_FRAMEBUFFER, _blurFramebuffer);
			_blurShader->bind();
			_blurShader->setUniform4x4f("projectionMatrix", projectionMatrix);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _texture);

			_renderQuad.draw();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	GLuint ssaoTexture() { return _blurTexture; }

private:
	void Init()
	{
		_blurShader = std::make_unique<Shader>();
		_blurShader->addVertexShader("shaders/ssao.vs.glsl")
			.addFragmentShader("shaders/ssaoblur.fs.glsl")
			.link();
		_blurShader->bind();
		_blurShader->setUniform1i("ssaoTexture", 0);

		{
			glGenFramebuffers(1, &_blurFramebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, _blurFramebuffer);

			glGenTextures(1, &_blurTexture);
			glBindTexture(GL_TEXTURE_2D, _blurTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920, 1080, 0, GL_RED, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _blurTexture, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		_shader = std::make_unique<Shader>();
		_shader->addVertexShader("shaders/ssao.vs.glsl")
			.addFragmentShader("shaders/ssao.fs.glsl")
			.link();

		_shader->bind();
		_shader->setUniform1i("positionTexture", 0);
		_shader->setUniform1i("normalTexture", 1);
		_shader->setUniform1i("ssaoNoise", 2);

		{
			glGenFramebuffers(1, &_framebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

			glGenTextures(1, &_texture);
			glBindTexture(GL_TEXTURE_2D, _texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920, 1080, 0, GL_RED, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		static auto lerp = [](float a, float b, float f) -> float {
			return a + f * (b - a);
		};

		std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
		std::default_random_engine generator;
		std::array<glm::vec3, 64> ssaoKernel{};

		// Generate SSAO Kernel
		// ====================

		for (size_t i = 0; i < 64; i++) {
			glm::vec3 sample{
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator),
			};

			sample = glm::normalize(sample);
			sample *= randomFloats(generator);

			float scale = static_cast<float>(i) / 64.0f;
			scale = lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;

			ssaoKernel[i] = sample;
		}

		_shader->bind();
		for (size_t i = 0; i < 64; i++) {
			_shader->setUniform3f("ssaoKernel[" + std::to_string(i) + "]", ssaoKernel[i]);
		}
		_shader->unbind();

		// Generate SSAO Noise
		// ===================

		std::array<glm::vec3, 16> ssaoNoise{};

		for (size_t i = 0; i < 16; i++) {
			glm::vec3 noise{
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f,
			};

			ssaoNoise[i] = noise;
		}

		glGenTextures(1, &_noiseTexture);
		glBindTexture(GL_TEXTURE_2D, _noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

private:
	Quad _renderQuad;

	GLuint _noiseTexture;
	GLuint _framebuffer;
	GLuint _texture;
	std::unique_ptr<Shader> _shader;

	GLuint _blurFramebuffer;
	GLuint _blurTexture;
	std::unique_ptr<Shader> _blurShader;
};
