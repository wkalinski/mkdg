module;
#include <string>
#include <fstream>
#include <streambuf>

#include <glad/glad.h>
export module Utils;


export std::string LoadFile(std::string path)
{
	std::ifstream t(path);
	return std::string((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
}

export GLuint CreateShaderFromFile(std::string file, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);

	std::string vsSource = LoadFile(file);
	int shaderLength = vsSource.size();
	auto source = vsSource.c_str();
	glShaderSource(shader, 1, &source, &shaderLength);

	glCompileShader(shader);

	return shader;
}


