#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <Shader.h>
#include <SexyMatrix.h>

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

	virtual void SetUniform(const std::string &name, int value) const;
	virtual void SetUniform(const std::string &name, unsigned int value) const;
	virtual void SetUniform(const std::string &name, float value) const;
	virtual void SetUniform(const std::string &name, const float &x, const float &y) const;
	virtual void SetUniform(const std::string &name, const float &x, const float &y, const float &z, const float &w) const;
	virtual void SetUniform(const std::string &name, const SexyMatrix4 &matrix) const;
	virtual void SetUniform(const std::string &name, const glm::mat4 &matrix) const;
};

} // namespace PopLib