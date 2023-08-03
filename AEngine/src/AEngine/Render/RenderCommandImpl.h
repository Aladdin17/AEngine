#pragma once
#include "AEngine/Core/Types.h"
#include "AEngine/Math/Math.h"
#include "Types.h"

namespace AEngine
{
		/**
		 * \class RenderCommandImpl
		 * \brief Separates the internal implementation of a render command from the interface
		*/
	class RenderCommandImpl
	{
	public:
		virtual ~RenderCommandImpl() = default;
			/**
			 * \copydoc RenderCommand::Clear
			*/
		virtual void Clear() = 0;
			/**
			 * \copydoc RenderCommand::SetClearColor
			*/
		virtual void SetClearColor(const Math::vec4& color) = 0;
			/**
			 * \copydoc RenderCommand::EnableDepthTest
			*/
		virtual void EnableDepthTest(bool value) = 0;
			/**
			 * \copydoc RenderCommand::SetDepthTestFunction
			*/
		virtual void SetDepthTestFunction(DepthTestFunction function) = 0;
			/**
			 * \copydoc RenderCommand::EnableFaceCulling
			*/
		virtual void EnableFaceCulling(bool value) = 0;
			/**
			 * \copydoc RenderCommand::SetCullFace
			*/
		virtual void SetCullFace(PolygonFace face) = 0;
			/**
			 * \copydoc RenderCommand::SetFrontFace
			*/
		virtual void SetFrontFace(Winding direction) = 0;
			/**
			 * \copydoc RenderCommand::EnableBlend
			*/
		virtual void EnableBlend(bool value) = 0;
			/**
			 * \copydoc RenderCommand::SetBlendFunction
			*/
		virtual void SetBlendFunction(BlendFunction source, BlendFunction destination) = 0;
			/**
			 * \copydoc RenderCommand::SetBlendConstant
			*/
		virtual void SetBlendConstant(const Math::vec4& color) = 0;
			/**
			 * \copydoc RenderCommand::SetViewport
			*/
		virtual void SetViewport(int x, int y, int width, int height) = 0;

//--------------------------------------------------------------------------------
// Inspection
//--------------------------------------------------------------------------------
			/**
			 * \copydoc RenderCommand::IsDepthTestEnabled
			*/
		virtual bool IsDepthTestEnabled() = 0;
			/**
			 * \copydoc RenderCommand::IsBlendEnabled
			*/
		virtual bool IsBlendEnabled() = 0;
			/**
			 * \copydoc RenderCommand::IsFaceCullingEnabled
			*/
		virtual bool IsFaceCullingEnabled() = 0;
			/**
			 * \copydoc RenderCommand::GetClearColor
			*/
		virtual Math::vec4 GetClearColor() = 0;
			/**
			 * \copydoc RenderCommand::GetDepthTestFunction
			*/
		virtual DepthTestFunction GetDepthTestFunction() = 0;
			/**
			 * \copydoc RenderCommand::GetBlendSourceFunction
			*/
		virtual BlendFunction GetBlendSourceFunction() = 0;
			/**
			 * \copydoc RenderCommand::GetBlendDestinationFunction
			*/
		virtual BlendFunction GetBlendDestinationFunction() = 0;
			/**
			 * \copydoc RenderCommand::GetBlendConstant
			*/
		virtual Math::vec4 GetBlendConstant() = 0;
			/**
			 * \copydoc RenderCommand::GetCullFace
			*/
		virtual PolygonFace GetCullFace() = 0;
			/**
			 * \copydoc RenderCommand::GetFrontFace
			*/
		virtual Winding GetFrontFace() = 0;
			/**
			 * \copydoc RenderCommand::GetPolygonMode
			*/
		virtual PolygonDraw GetPolygonMode(PolygonFace face) = 0;
			/**
			 * \copydoc RenderCommand::GetViewport
			*/
		virtual Math::ivec4 GetViewport() = 0;

//--------------------------------------------------------------------------------
// Drawing
//--------------------------------------------------------------------------------
			/**
			 * \copydoc RenderCommand::PolygonMode
			*/
		virtual void PolygonMode(PolygonFace face, PolygonDraw type) = 0;
			/**
			 * \copydoc RenderCommand::DrawIndexed
			*/
		virtual void DrawIndexed(Primitive type, Intptr_t count, void* offset = 0) = 0;
			/**
			 * \copydoc RenderCommand::DrawArrays
			*/
		virtual void DrawArrays(Primitive type, int offset, Intptr_t count) = 0;

			/**
			 * \copydoc RenderCommand::GetLibrary
			*/
		virtual RenderLibrary GetLibrary() = 0;
			/**
			 * \brief Create a new graphics API
			 * \param[in] graphics The graphics library to use
			 * \return The created graphics API
			 * \note This must be called before any other graphics API function
			*/
		static UniquePtr<RenderCommandImpl> Create(RenderLibrary graphics);
	};
}
