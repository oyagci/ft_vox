#include "Texture.hpp"
#include "stb_image.h"

Texture::Texture(std::string const &name) : _name(name)
{
	glGenTextures(1, &_glId);
}

Texture::~Texture()
{
	glDeleteTextures(1, &_glId);
}

bool Texture::load(std::string const &path)
{
	unsigned char *data = stbi_load(path.c_str(), &_width, &_height, &_nChannel, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0,
			GL_RGBA, _width, _height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
		return true;
	}
	return false;
}

void Texture::setParameter(GLenum target, GLenum param, GLenum value)
{
	glBindTexture(GL_TEXTURE_2D, _glId);
	glTexParameteri(target, param, value);
}

void Texture::bind(GLuint textureNumber)
{
	glActiveTexture(textureNumber);
	glBindTexture(GL_TEXTURE_2D, _glId);
}
