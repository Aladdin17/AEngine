/**
 * \file
 * \author Ben Hawkins
 * \author Christien Alden (34119981)
*/
#include "OpenGLRenderCommand.h"
#include "AEngine/Core/Logger.h"
#include <glad/glad.h>

namespace
{
	static constexpr GLenum g_glBlendFunctions[] = {
		GL_ZERO, GL_ONE,
		GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR,
		GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA,
		GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR,
		GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA,
		GL_INVALID_ENUM
	};

	static constexpr GLenum g_glDepthTestFunctions[] = {
		GL_LEQUAL, GL_GEQUAL, GL_LESS, GL_GREATER, GL_EQUAL
	};

	static constexpr GLenum g_glPolygonFaces[] = {
		GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
	};

	static constexpr GLenum g_glPolygonDraws[] = {
		GL_POINT, GL_FILL, GL_LINE,
		GL_INVALID_ENUM
	};

	static constexpr GLenum g_glWindingDirections[] = {
		GL_CW, GL_CCW
	};

	static constexpr GLenum g_glPrimitiveDraws[] = {
		GL_POINTS,
		GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP,
		GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN
	};

	inline AEngine::BlendFunction GetAEBlendFunction(GLenum func)
	{
		switch (func)
		{
		case GL_ZERO:                       return AEngine::BlendFunction::Zero;
		case GL_ONE:                        return AEngine::BlendFunction::One;
		case GL_SRC_COLOR:                  return AEngine::BlendFunction::SourceColor;
		case GL_ONE_MINUS_SRC_COLOR:        return AEngine::BlendFunction::OneMinusSourceColor;
		case GL_DST_COLOR:                  return AEngine::BlendFunction::DestinationColor;
		case GL_ONE_MINUS_DST_COLOR:        return AEngine::BlendFunction::OneMinusDestinationColor;
		case GL_SRC_ALPHA:                  return AEngine::BlendFunction::SourceAlpha;
		case GL_ONE_MINUS_SRC_ALPHA:        return AEngine::BlendFunction::OneMinusSourceAlpha;
		case GL_DST_ALPHA:                  return AEngine::BlendFunction::DestinationAlpha;
		case GL_ONE_MINUS_DST_ALPHA:        return AEngine::BlendFunction::OneMinusDestinationAlpha;
		case GL_CONSTANT_COLOR:             return AEngine::BlendFunction::ConstantColor;
		case GL_ONE_MINUS_CONSTANT_COLOR:   return AEngine::BlendFunction::OneMinusConstantColor;
		case GL_CONSTANT_ALPHA:             return AEngine::BlendFunction::ConstantAlpha;
		case GL_ONE_MINUS_CONSTANT_ALPHA:   return AEngine::BlendFunction::OneMinusConstantAlpha;
		default:                            return AEngine::BlendFunction::Invalid;
		}
	}
}

namespace AEngine
{
	void OpenGLRenderCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRenderCommand::SetClearColor(const Math::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRenderCommand::EnableDepthTest(bool set)
	{
		set ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRenderCommand::SetDepthTestFunction(DepthTestFunction function)
	{
		GLenum func = g_glDepthTestFunctions[static_cast<int>(function)];
		glDepthFunc(func);
	}

	void OpenGLRenderCommand::EnableBlend(bool value)
	{
		value ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	}

	void OpenGLRenderCommand::SetBlendFunction(BlendFunction source, BlendFunction destination)
	{
		GLenum src = g_glBlendFunctions[static_cast<int>(source)];
		GLenum dst = g_glBlendFunctions[static_cast<int>(destination)];
		glBlendFunc(src, dst);
	}

	void OpenGLRenderCommand::SetBlendConstant(const Math::vec4 &color)
	{
		glBlendColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRenderCommand::EnableFaceCulling(bool set)
	{
		set ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	}

	void OpenGLRenderCommand::SetCullFace(PolygonFace face)
	{
		GLenum faceEnum = g_glPolygonFaces[static_cast<int>(face)];
		glCullFace(faceEnum);
	}

	void OpenGLRenderCommand::SetFrontFace(Winding direction)
	{
		GLenum directionEnum = g_glWindingDirections[static_cast<int>(direction)];
		glFrontFace(directionEnum);
	}

	void OpenGLRenderCommand::SetViewport(int x, int y, int width, int height)
	{
		glViewport(x, y, width, height);
	}

	bool OpenGLRenderCommand::IsDepthTestEnabled()
	{
		return glIsEnabled(GL_DEPTH_TEST);
	}

	bool OpenGLRenderCommand::IsBlendEnabled()
	{
		return glIsEnabled(GL_BLEND);
	}

	bool OpenGLRenderCommand::IsFaceCullingEnabled()
	{
		return glIsEnabled(GL_CULL_FACE);
	}

	Math::vec4 OpenGLRenderCommand::GetClearColor()
	{
		Math::vec4 color;
		glGetFloatv(GL_COLOR_CLEAR_VALUE, &color.r);
		return color;
	}

	DepthTestFunction OpenGLRenderCommand::GetDepthTestFunction()
	{
		GLenum func;
		glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint*>(&func));
		switch (func)
		{
		case GL_LEQUAL:    return DepthTestFunction::LessEqual;
		case GL_GEQUAL:    return DepthTestFunction::GreaterEqual;
		case GL_LESS:      return DepthTestFunction::Less;
		case GL_GREATER:   return DepthTestFunction::Greater;
		case GL_EQUAL:     return DepthTestFunction::Equal;
		}

		AE_LOG_FATAL("Invalid depth test function");
	}

	BlendFunction OpenGLRenderCommand::GetBlendSourceFunction()
	{
		GLenum func;
		glGetIntegerv(GL_BLEND_SRC, reinterpret_cast<GLint*>(&func));
		return GetAEBlendFunction(func);
	}

	BlendFunction OpenGLRenderCommand::GetBlendDestinationFunction()
	{
		GLenum func;
		glGetIntegerv(GL_BLEND_DST, reinterpret_cast<GLint*>(&func));
		return GetAEBlendFunction(func);
	}

	Math::vec4 OpenGLRenderCommand::GetBlendConstant()
	{
		Math::vec4 color;
		glGetFloatv(GL_BLEND_COLOR, &color.r);
		return color;
	}

	PolygonFace OpenGLRenderCommand::GetCullFace()
	{
		GLenum face;
		glGetIntegerv(GL_CULL_FACE_MODE, reinterpret_cast<GLint*>(&face));
		switch (face)
		{
		case GL_FRONT:            return PolygonFace::Front;
		case GL_BACK:             return PolygonFace::Back;
		case GL_FRONT_AND_BACK:   return PolygonFace::FrontAndBack;
		}

		// this should never happen as all cull face modes are valid
		AE_LOG_FATAL("Invalid cull face");
	}

	Winding OpenGLRenderCommand::GetFrontFace()
	{
		GLenum direction;
		glGetIntegerv(GL_FRONT_FACE, reinterpret_cast<GLint*>(&direction));
		switch (direction)
		{
		case GL_CW:    return Winding::Clockwise;
		case GL_CCW:   return Winding::CounterClockwise;
		}

		// this should never happen as all winding directions are valid
		AE_LOG_FATAL("Invalid winding direction");
	}

	PolygonDraw OpenGLRenderCommand::GetPolygonMode(PolygonFace face)
	{
		GLenum mode[2];
		glGetIntegerv(GL_POLYGON_MODE, reinterpret_cast<GLint*>(&mode));

		// only retrieve the target face
		GLenum target = (face == PolygonFace::Front) ? mode[0] : mode[1];
		switch (target)
		{
		case GL_POINT:   return PolygonDraw::Point;
		case GL_LINE:    return PolygonDraw::Line;
		case GL_FILL:    return PolygonDraw::Fill;
		default:         return PolygonDraw::Invalid;
		}
	}

	Math::ivec4 OpenGLRenderCommand::GetViewport()
	{
		Math::ivec4 viewport;
		glGetIntegerv(GL_VIEWPORT, &viewport.x);
		return viewport;
	}

	void OpenGLRenderCommand::PolygonMode(PolygonFace face, PolygonDraw type)
	{
		GLenum faceEnum = g_glPolygonFaces[static_cast<int>(face)];
		GLenum typeEnum = g_glPolygonDraws[static_cast<int>(type)];
		glPolygonMode(faceEnum, typeEnum);
	}

	void OpenGLRenderCommand::DrawIndexed(Primitive type, Intptr_t count, void* offset)
	{
		GLenum mode = g_glPrimitiveDraws[static_cast<int>(type)];
		glDrawElements(mode, static_cast<GLsizei>(count), GL_UNSIGNED_INT, offset);
	}

	void OpenGLRenderCommand::DrawArrays(Primitive type, int offset, Intptr_t count)
	{
		GLenum mode = g_glPrimitiveDraws[static_cast<int>(type)];
		glDrawArrays(mode, offset, static_cast<GLsizei>(count));
	}

	RenderLibrary OpenGLRenderCommand::GetLibrary()
	{
		return RenderLibrary::OpenGL;
	}
}