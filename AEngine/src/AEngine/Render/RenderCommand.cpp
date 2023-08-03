/**
 * \file
 * \author Christien Alden (34119981)
*/
#include "RenderCommand.h"
#include "RenderCommandImpl.h"

namespace AEngine
{
	UniquePtr<RenderCommandImpl> RenderCommand::s_impl{ nullptr };

	void RenderCommand::Initialise(RenderLibrary graphics)
	{
		if (!s_impl)
		{
			s_impl = RenderCommandImpl::Create(graphics);
		}
	}

	void RenderCommand::Clear()
	{
		s_impl->Clear();
	}

	void RenderCommand::SetClearColor(const Math::vec4& color)
	{
		s_impl->SetClearColor(color);
	}

	void RenderCommand::EnableDepthTest(bool set)
	{
		s_impl->EnableDepthTest(set);
	}

	void RenderCommand::SetDepthTestFunction(DepthTestFunction function)
	{
		s_impl->SetDepthTestFunction(function);
	}

	void RenderCommand::EnableBlend(bool value)
	{
		s_impl->EnableBlend(value);
	}

	void RenderCommand::SetBlendFunction(BlendFunction source, BlendFunction destination)
	{
		s_impl->SetBlendFunction(source, destination);
	}

	void RenderCommand::SetBlendConstant(const Math::vec4 &color)
	{
		s_impl->SetBlendConstant(color);
	}

	void RenderCommand::EnableFaceCulling(bool value)
	{
		s_impl->EnableFaceCulling(value);
	}

	void RenderCommand::SetCullFace(PolygonFace face)
	{
		s_impl->SetCullFace(face);
	}

	void RenderCommand::SetFrontFace(Winding direction)
	{
		s_impl->SetFrontFace(direction);
	}

	void RenderCommand::SetViewport(int x, int y, int width, int height)
	{
		s_impl->SetViewport(x, y, width, height);
	}

	bool RenderCommand::IsDepthTestEnabled()
	{
		return s_impl->IsDepthTestEnabled();
	}

	bool RenderCommand::IsBlendEnabled()
	{
		return s_impl->IsBlendEnabled();
	}

	bool RenderCommand::IsFaceCullingEnabled()
	{
		return s_impl->IsFaceCullingEnabled();
	}

	Math::vec4 RenderCommand::GetClearColor()
	{
		return s_impl->GetClearColor();
	}

	DepthTestFunction RenderCommand::GetDepthTestFunction()
	{
		return s_impl->GetDepthTestFunction();
	}

	BlendFunction RenderCommand::GetBlendSourceFunction()
	{
		return s_impl->GetBlendSourceFunction();
	}

	BlendFunction RenderCommand::GetBlendDestinationFunction()
	{
		return s_impl->GetBlendDestinationFunction();
	}

	Math::vec4 RenderCommand::GetBlendConstant()
	{
		return s_impl->GetBlendConstant();
	}

	PolygonFace RenderCommand::GetCullFace()
	{
		return s_impl->GetCullFace();
	}

	Winding RenderCommand::GetFrontFace()
	{
		return s_impl->GetFrontFace();
	}

	PolygonDraw RenderCommand::GetPolygonMode(PolygonFace face)
	{
		return s_impl->GetPolygonMode(face);
	}

	Math::ivec4 RenderCommand::GetViewport()
	{
		return s_impl->GetViewport();
	}

	void RenderCommand::PolygonMode(PolygonFace face, PolygonDraw type)
	{
		s_impl->PolygonMode(face, type);
	}

	void RenderCommand::DrawIndexed(Primitive type, Intptr_t count, void* offset)
	{
		s_impl->DrawIndexed(type, count, offset);
	}

	void RenderCommand::DrawArrays(Primitive type, int offset, Intptr_t count)
	{
		s_impl->DrawArrays(type, offset, count);
	}

	RenderLibrary RenderCommand::GetLibrary()
	{
		return s_impl->GetLibrary();
	}
}
