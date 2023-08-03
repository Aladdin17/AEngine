/**
 * \file
 * \author Christien Alden (34119981)
*/
#pragma once
#include "AEngine/Core/Types.h"
#include "AEngine/Math/Math.h"
#include "Types.h"

namespace AEngine
{

	class RenderCommandImpl;

	class RenderCommand
	{
	public:
			/**
			 * \brief Initialize the render command
			 * \param[in] graphics The graphics library to use
			*/
		static void Initialise(RenderLibrary graphics);
			/**
			 * \brief Clear the current buffer
			 * \todo Support clearing specific buffers
			*/
		static void Clear();
			/**
			 * \brief Set the clear color
			 * \param color The color to set
			*/
		static void SetClearColor(const Math::vec4& color);
			/**
			 * \brief Enable or disable the depth testing
			 * \param value True to enable, false to disable
			*/
		static void EnableDepthTest(bool value);
			/**
			 * \brief Set the depth test function
			 * \param function The function to set
			*/
		static void SetDepthTestFunction(DepthTestFunction function);
			/**
			 * \brief Enable or disable blending
			 * \param[in] value True to enable, false to disable
			*/
		static void EnableBlend(bool value);
			/**
			 * \brief Set the blend function
			 * \param[in] source The source function, this is applied to the color being drawn
			 * \param[in] destination The destination function, this is applied to the color already in the buffer
			*/
		static void SetBlendFunction(BlendFunction source, BlendFunction destination);
			/**
			 * \brief Set the blend constant
			 * \param[in] color The color to set
			 * \see BlendFunction::ConstantAlpha, BlendFunction::OneMinusConstantAlpha
			 * \see BlendFunction::ConstantColor, BlendFunction::OneMinusConstantColor
			*/
		static void SetBlendConstant(const Math::vec4& color);
			/**
			 * \brief Enable or disable face culling
			 * \param[in] value True to enable, false to disable
			*/
		static void EnableFaceCulling(bool value);
			/**
			 * \brief Set the cull face
			 * \param[in] face The face to cull
			*/
		static void SetCullFace(PolygonFace face);
			/**
			 * \brief Set the front face direction
			 * \param[in] direction The direction to set
			*/
		static void SetFrontFace(Winding direction);
			/**
			 * \brief Set the viewport
			 * \param[in] x viewport x position
			 * \param[in] y viewport y position
			 * \param[in] width viewport width
			 * \param[in] height viewport height
			*/
		static void SetViewport(int x, int y, int width, int height);

//--------------------------------------------------------------------------------
// Inspection
//--------------------------------------------------------------------------------
			/**
			 * \brief Check if depth testing is enabled
			 * \retval true if enabled
			 * \retval false if disabled
			*/
		static bool IsDepthTestEnabled();
			/**
			 * \brief Check if blending is enabled
			 * \retval true if enabled
			 * \retval false if disabled
			*/
		static bool IsBlendEnabled();
			/**
			 * \brief Check if face culling is enabled
			 * \retval true if enabled
			 * \retval false if disabled
			*/
		static bool IsFaceCullingEnabled();
			/**
			 * \brief Get the clear color
			 * \return The clear color
			*/
		static Math::vec4 GetClearColor();
			/**
			 * \brief Get the depth test function
			 * \return The depth test function
			 * \retval Nullopt if an value could not be retrieved
			*/
		static DepthTestFunction GetDepthTestFunction();
			/**
			 * \brief Get the source blend function
			 * \return The source blend function
			 * \retval Nullopt if an value could not be retrieved
			*/
		static BlendFunction GetBlendSourceFunction();
			/**
			 * \brief Get the destination blend function
			 * \return The destination blend function
			 * \retval Nullopt if an value could not be retrieved
			*/
		static BlendFunction GetBlendDestinationFunction();
			/**
			 * \brief Get the blend constant
			 * \return The blend constant
			*/
		static Math::vec4 GetBlendConstant();
			/**
			 * \brief Get the cull face
			 * \return The cull face
			 * \retval Nullopt if an value could not be retrieved
			*/
		static PolygonFace GetCullFace();
			/**
			 * \brief Get the front face direction
			 * \return The front face direction
			 * \retval Nullopt if an value could not be retrieved
			*/
		static Winding GetFrontFace();
			/**
			 * \brief Get the polygon draw mode
			 * \param[in] face to get the mode of.
			 * \return The polygon draw mode
			*/
		static PolygonDraw GetPolygonMode(PolygonFace face);
			/**
			 * \brief Get the viewport
			 * \return The viewport
			 * \details
			 * The first two values are the x and y position, the second two are the width and height
			*/
		static Math::ivec4 GetViewport();

//--------------------------------------------------------------------------------
// Drawing
//--------------------------------------------------------------------------------
			/**
			 * \brief Set the polygon draw mode.
			 * \param[in] face to set the mode of.
			 * \param[in] type to set the mode to.
			*/
		static void PolygonMode(PolygonFace face, PolygonDraw type);
			/**
			 * \brief Draw an indexed array
			 * \param[in] type The type of primitive to draw
			 * \param[in] count The number of indices to draw
			 * \param[in] offset The offset in the index buffer
			*/
		static void DrawIndexed(Primitive type, Intptr_t count, void* offset);
			/**
			 * \brief Draw an array
			 * \param[in] type The type of primitive to draw
			 * \param[in] offset The offset in the vertex buffer
			 * \param[in] count The number of vertices to draw
			*/
		static void DrawArrays(Primitive type, int offset, Intptr_t count);

			/**
			 * \brief Get the current graphics library
			 * \return The current graphics library
			*/
		static RenderLibrary GetLibrary();

	private:
		static UniquePtr<RenderCommandImpl> s_impl;
	};
}
