#pragma once

#include "lazy.hpp"

class GBuffer
{
public:
	GBuffer()
	{
		glGenFramebuffers(1, &_Framebuffer);
		glGenTextures(1, &_TexturePosition);
		glGenTextures(1, &_TextureNormal);
		glGenTextures(1, &_TextureColor);
		glGenTextures(1, &_TextureDepth);

		glBindFramebuffer(GL_FRAMEBUFFER, _Framebuffer);

		glBindTexture(GL_TEXTURE_2D, _TextureColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _TextureColor, 0);

		glBindTexture(GL_TEXTURE_2D, _TextureNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _TextureNormal, 0);

		glBindTexture(GL_TEXTURE_2D, _TexturePosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _TexturePosition, 0);

		glBindTexture(GL_TEXTURE_2D, _TextureDepth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1920, 1080, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _TextureDepth, 0);

		GLenum const attachments[3]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, (GLenum*)&attachments);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		_Shader = std::make_unique<lazy::graphics::Shader>();
		_Shader->addVertexShader("shaders/gbuffer.vs.glsl")
			.addFragmentShader("shaders/gbuffer.fs.glsl")
			.link();
	}

	~GBuffer()
	{
		glDeleteFramebuffers(1, &_Framebuffer);
		glDeleteTextures(1, &_TexturePosition);
		glDeleteTextures(1, &_TextureNormal);
		glDeleteTextures(1, &_TextureColor);
		glDeleteTextures(1, &_TextureDepth);
	}

	GBuffer(GBuffer const &) = delete;
	GBuffer(GBuffer &&) = delete;

	GBuffer &operator=(GBuffer const &) & = delete;

	auto framebuffer() const { return _Framebuffer; }
	auto positionTex() const { return _TexturePosition; }
	auto normalTex() const { return _TextureNormal; }
	auto colorTex() const { return _TextureColor; }
	auto depthTex() const { return _TextureDepth; }
	auto &shader() { return *_Shader; }

private:
	GLuint _Framebuffer;
	GLuint _TexturePosition;
	GLuint _TextureNormal;
	GLuint _TextureColor;
	GLuint _TextureDepth;
	std::unique_ptr<lazy::graphics::Shader> _Shader;
};
