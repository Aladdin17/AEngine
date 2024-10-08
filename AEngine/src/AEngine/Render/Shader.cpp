#include "Shader.h"
#include "AEngine/Core/Application.h"
#include "AEngine/Core/Logger.h"
#include "AEngine/Render/RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace AEngine
{
	Shader::Shader(const char* shaderSource)
		: Asset({}, {})
	{

	}

	Shader::Shader(const std::string& ident, const std::string& path)
		: Asset(ident, path)
	{

	}

	SharedPtr<Shader> AEngine::Shader::Create(const std::string& ident, const std::string& fname)
	{
		switch (RenderCommand::GetLibrary())
		{
		case RenderLibrary::OpenGL:
			return MakeShared<OpenGLShader>(ident, fname);
		default:
			AE_LOG_FATAL("Shader::Create::RenderLibrary::Error -> None selected");
		}
	}
	SharedPtr<Shader> Shader::Create(const char *shaderSource)
	{
		switch (RenderCommand::GetLibrary())
		{
		case RenderLibrary::OpenGL:
			return MakeShared<OpenGLShader>(shaderSource);
		default:
			AE_LOG_FATAL("Shader::Create::RenderLibrary::Error -> None selected");
		}
	}
}
