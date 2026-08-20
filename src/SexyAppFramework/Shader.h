#pragma once

#include <string>

namespace Sexy
{
	class Shader
	{
	  public:
		Shader();
		~Shader();

		virtual bool LoadFromSource(const std::string &theVertShader, const std::string &theFragShader);

	  private:
	};
}