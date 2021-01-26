module;
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdexcept>
#include <string>
export module Texture;

export class Texture
{

public:
	GLuint textureId;
	GLuint width, height;

	Texture(GLuint textureId, GLuint width, GLuint height) :
		textureId(textureId),
		width(width),
		height(height)
	{
	}

	static Texture* LoadTexture(std::string file)
	{
		int imgWidth, imgHeight, nrChannels;
		unsigned char* data = stbi_load(file.c_str(), &imgWidth, &imgHeight, &nrChannels, 0);
		if (!data)
		{
			throw std::runtime_error("Could not load image");
		}
		Texture* tex = LoadTexture(imgWidth, imgHeight, data, nrChannels);
		stbi_image_free(data);
		return tex;
	}
	static Texture* LoadTexture(int width, int height, unsigned char* data, int channels = 4)
	{
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		int format = GL_RGBA;
		if (channels == 3)
		{
			format = GL_RGB;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);

		return new Texture(textureId, width, height);
	}
	
	~Texture()
	{
		glDeleteTextures(1, &textureId);
	}
};