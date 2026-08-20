#if SEXY_USE_OPENGL

#include "OpenGL/GLShader.h"
#include <fstream>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

using namespace Sexy;

GLShader::GLShader() : Shader()
{
}

GLShader::~GLShader()
{
	if (mProgramID)
		glDeleteProgram(mProgramID);
}

bool GLShader::LoadFromSource(const std::string &theVertShader, const std::string &theFragShader)
{
	GLuint vertex_shader = CompileShader(GL_VERTEX_SHADER, theVertShader);
	GLuint fragment_shader = CompileShader(GL_FRAGMENT_SHADER, theFragShader);

	if (!vertex_shader || !fragment_shader)
	{
		if (!vertex_shader)
			printf("[SexyAppFramework] - Failed to load Vertex Shader: \n%s\n", theVertShader.c_str());
		if (!vertex_shader)
			printf("[SexyAppFramework] - Failed to load Fragment Shader: \n%s\n", theFragShader.c_str());

		return false;
	}

	mProgramID = glCreateProgram();
	glAttachShader(mProgramID, vertex_shader);
	glAttachShader(mProgramID, fragment_shader);
	glLinkProgram(mProgramID);

	GLint success;
	glGetProgramiv(mProgramID, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		printf("[SexyAppFramework] - Failed to load OpenGL Shader: - %d\n");
		return false;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	return true;
}

void GLShader::Use() const
{
	glUseProgram(mProgramID);
}

GLuint GLShader::CompileShader(GLenum type, const std::string &source)
{
	GLuint shader = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	return shader;
}

GLuint GLShader::GetUniformLocation(const std::string &name) const
{
	GLuint pos = glGetUniformLocation(mProgramID, name.c_str());
	return pos;
}

void GLShader::SetUniform(const std::string &name, int value) const
{
	glUniform1i(GetUniformLocation(name), value);
}
void GLShader::SetUniform(const std::string &name, float value) const
{
	glUniform1f(GetUniformLocation(name), value);
}
void GLShader::SetUniform(const std::string &name, const glm::vec2 &value) const
{
	glUniform2f(GetUniformLocation(name), value.x, value.y);
}
void GLShader::SetUniform(const std::string &name, const glm::vec4 &value) const
{
	glUniform4f(GetUniformLocation(name), value.r, value.g, value.b, value.a);
}
void GLShader::SetUniform(const std::string &name, const glm::mat4 &value) const
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

#endif