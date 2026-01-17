#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Sexy
{
class GLShader
{
  private:
	GLuint mProgramID = 0;

	GLuint CompileShader(GLenum type, const std::string &source);
	GLuint GetUniformLocation(const std::string &name) const;

  public:
	GLShader() = default;
	~GLShader();

	bool LoadFromSource(const std::string &vertexSrc, const std::string &fragmentSrc);

	void Use() const;
	GLuint GetID() const
	{
		return mProgramID;
	}

	void SetUniform(const std::string &name, int value) const;
	void SetUniform(const std::string &name, float value) const;
	void SetUniform(const std::string &name, const glm::vec2 &value) const;
	void SetUniform(const std::string &name, const glm::vec4 &value) const;
	void SetUniform(const std::string &name, const glm::mat4 &value) const;
};

} // namespace PopLib