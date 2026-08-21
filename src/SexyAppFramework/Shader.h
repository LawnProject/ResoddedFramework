#pragma once

#include <string>

namespace Sexy
{
	class SexyMatrix4;

	class Shader
	{
	  public:
		Shader();
		~Shader();

		virtual bool LoadFromSource(const std::string &theVertShader, const std::string &theFragShader);

		virtual void SetUniform(const std::string &name, int value) const = 0;
		virtual void SetUniform(const std::string &name, unsigned int value) const = 0;
		virtual void SetUniform(const std::string &name, float value) const = 0;
		virtual void SetUniform(const std::string &name, const float &x, const float &y) const = 0;
		virtual void SetUniform(const std::string &name, const float &x, const float &y, const float &z, const float &w) const = 0;
		virtual void SetUniform(const std::string &name, const SexyMatrix4 &matrix) const = 0;
	  private:
	};
}