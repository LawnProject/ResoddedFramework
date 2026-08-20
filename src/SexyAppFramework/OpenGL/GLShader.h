#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../Shader.h"

namespace Sexy
{
class GLShader : public Shader
{
  private:
	GLuint mProgramID = 0;

	GLuint CompileShader(GLenum type, const std::string &source);
	GLuint GetUniformLocation(const std::string &name) const;

  public:
	GLShader();
	~GLShader();

	virtual bool LoadFromSource(const std::string &theVertShader, const std::string &theFragShader);

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