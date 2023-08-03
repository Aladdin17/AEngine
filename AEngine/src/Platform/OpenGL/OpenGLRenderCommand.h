/**
 * \file
 * \author Ben Hawkins
 * \author Christien Alden (34119981)
*/
#include "AEngine/Render/RenderCommandImpl.h"

namespace AEngine
{
		/**
		 * \class OpenGLRenderCommand
		 * \brief OpenGL implementation of RenderCommandImpl
		*/
	class OpenGLRenderCommand : public RenderCommandImpl
	{
	public:
		OpenGLRenderCommand() = default;
			/**
			 * \copydoc RenderCommandImpl::Clear
			*/
		void Clear() override;
			/**
			 * \copydoc RenderCommandImpl::SetClearColor
			*/
		void SetClearColor(const Math::vec4& color) override;
			/**
			 * \copydoc RenderCommandImpl::EnableDepthTest
			*/
		virtual void EnableDepthTest(bool set) override;
			/**
			 * \copydoc RenderCommandImpl::SetDepthTestFunction
			*/
		virtual void SetDepthTestFunction(DepthTestFunction function) override;
			/**
			 * \copydoc RenderCommandImpl::EnableBlend
			*/
		virtual void EnableBlend(bool set) override;
			/**
			 * \copydoc RenderCommandImpl::SetBlendFunction
			*/
		virtual void SetBlendFunction(BlendFunction source, BlendFunction destination) override;
			/**
			 * \copydoc RenderCommandImpl::SetBlendConstant
			*/
		virtual void SetBlendConstant(const Math::vec4& color) override;
			/**
			 * \copydoc RenderCommandImpl::EnableFaceCulling
			*/
		virtual void EnableFaceCulling(bool set) override;
			/**
			 * \copydoc RenderCommandImpl::SetCullFace
			*/
		virtual void SetCullFace(PolygonFace face) override;
			/**
			 * \copydoc RenderCommandImpl::SetFrontFace
			*/
		virtual void SetFrontFace(Winding direction) override;
			/**
			 * \copydoc RenderCommandImpl::SetViewport
			*/
		virtual void SetViewport(int x, int y, int width, int height) override;

//--------------------------------------------------------------------------------
// Inspection
//--------------------------------------------------------------------------------
			/**
			 * \copydoc RenderCommandImpl::IsDepthTestEnabled
			*/
		virtual bool IsDepthTestEnabled() override;
			/**
			 * \copydoc RenderCommandImpl::IsBlendEnabled
			*/
		virtual bool IsBlendEnabled() override;
			/**
			 * \copydoc RenderCommandImpl::IsFaceCullingEnabled
			*/
		virtual bool IsFaceCullingEnabled() override;
			/**
			 * \copydoc RenderCommandImpl::GetClearColor
			*/
		virtual Math::vec4 GetClearColor() override;
			/**
			 * \copydoc RenderCommandImpl::GetDepthTestFunction
			*/
		virtual DepthTestFunction GetDepthTestFunction() override;
			/**
			 * \copydoc RenderCommandImpl::GetBlendSourceFunction
			*/
		virtual BlendFunction GetBlendSourceFunction() override;
			/**
			 * \copydoc RenderCommandImpl::GetBlendDestinationFunction
			*/
		virtual BlendFunction GetBlendDestinationFunction() override;
			/**
			 * \copydoc RenderCommandImpl::GetBlendConstant
			*/
		virtual Math::vec4 GetBlendConstant() override;
			/**
			 * \copydoc RenderCommandImpl::GetCullFace
			*/
		virtual PolygonFace GetCullFace() override;
			/**
			 * \copydoc RenderCommandImpl::GetFrontFace
			*/
		virtual Winding GetFrontFace() override;
			/**
			 * \copydoc RenderCommandImpl::GetPolygonMode
			*/
		virtual PolygonDraw GetPolygonMode(PolygonFace face) override;
			/**
			 * \copydoc RenderCommandImpl::GetViewport
			*/
		virtual Math::ivec4 GetViewport() override;

//--------------------------------------------------------------------------------
// Drawing
//--------------------------------------------------------------------------------
			/**
			 * \copydoc RenderCommandImpl::PolygonMode
			*/
		virtual void PolygonMode(PolygonFace face, PolygonDraw type) override;
			/**
			 * \copydoc RenderCommandImpl::DrawIndexed
			*/
		virtual void DrawIndexed(Primitive type, Intptr_t count, void* offset) override;
			/**
			 * \copydoc RenderCommandImpl::DrawArrays
			*/
		virtual void DrawArrays(Primitive type, int offset, Intptr_t count) override;
			/**
			 * \copydoc RenderCommandImpl::GetLibrary
			*/
		virtual RenderLibrary GetLibrary() override;
	};
}