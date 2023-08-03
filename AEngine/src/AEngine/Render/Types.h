/**
 * \file
 * \author Christien Alden (34119981)
 * \brief Contains the type definitions for the graphics API
*/
#pragma once

namespace AEngine
{
		/**
		 * \enum RenderLibrary
		 * \brief Defines the available rendering libraries
		*/
	enum class RenderLibrary
	{
		OpenGL,   ///< OpenGL rendering library
		Invalid   ///< No rendering library
	};

		/**
		 * \enum BlendFunction
		 * \brief Defines the available blend functions
		*/
	enum class BlendFunction
	{
		Zero,                       ///< The source value is multiplied by zero
		One,                        ///< The source value is multiplied by one
		SourceColor,                ///< The source value is multiplied by the source color
		OneMinusSourceColor,        ///< The source value is multiplied by one minus the source color
		DestinationColor,           ///< The source value is multiplied by the destination color
		OneMinusDestinationColor,   ///< The source value is multiplied by one minus the destination color
		SourceAlpha,                ///< The source value is multiplied by the alpha value of the source color
		OneMinusSourceAlpha,        ///< The source value is multiplied by one minus the alpha value of the source color
		DestinationAlpha,           ///< The source value is multiplied by the alpha value of the destination color
		OneMinusDestinationAlpha,   ///< The source value is multiplied by one minus the alpha value of the destination color
		ConstantColor,              ///< The source value is multiplied by a constant color
		OneMinusConstantColor,      ///< The source value is multiplied by one minus a constant color
		ConstantAlpha,              ///< The source value is multiplied by a constant alpha value
		OneMinusConstantAlpha,      ///< The source value is multiplied by one minus a constant alpha value
		Invalid                     ///< Invalid blend function
	};

		/**
		 * \enum DepthTestFunction
		 * \brief Defines the available depth test functions
		*/
	enum class DepthTestFunction
	{
		LessEqual,      ///< Passes if the incoming depth value is less than or equal to the stored depth value
		GreaterEqual,   ///< Passes if the incoming depth value is greater than or equal to the stored depth value
		Less,           ///< Passes if the incoming depth value is less than the stored depth value
		Greater,        ///< Passes if the incoming depth value is greater than the stored depth value
		Equal           ///< Passes if the incoming depth value is equal to the stored depth value
	};

		/**
		 * \enum PolygonFace
		 * \brief Defines the available polygon faces
		*/
	enum class PolygonFace
	{
		Front,         ///< Sets the polygon mode for front faces
		Back,          ///< Sets thee polygon mode for back faces
		FrontAndBack   ///< Sets the polygon mode for both front and back faces
	};

		/**
		 * \enum PolygonDraw
		 * \brief Defines the available polygon draw modes
		*/
	enum class PolygonDraw
	{
		Point,    ///< Draws the vertices as points
		Fill,     ///< Fills the polygon
		Line,     ///< Wireframe of the polygon
		Invalid   ///< Invalid polygon draw mode
	};

	enum class Winding
	{
		Clockwise,
		CounterClockwise
	};

		/**
		 * \enum Primitive
		 * \brief Defines the available primitive draw modes
		*/
	enum class Primitive
	{
		Points,
		Lines,
		LineStrip,
		LineLoop,
		Triangles,
		TriangleStrip,
		TriangleFan
	};

		/**
		 * \enum BufferUsage
		 * \brief Rendering API agnostic buffer usage
		 * \details
		 * The buffer usage is used to describe how the buffer will be used.
		*/
	enum class BufferUsage
	{
		StaticDraw,    ///< The data will be uploaded once and drawn many times
		DynamicDraw,   ///< The data will be uploaded many times and drawn many times
		StreamDraw     ///< The data will be uploaded once and used a few times
	};

		/**
		 * \enum BufferElementPrecision
		 * \brief Rendering API agnostic buffer element precision
		 * \remark
		 * This can be used in the implementation to optimise what kind of buffer layout is used.
		*/
	enum class BufferElementPrecision
	{
		Integer,   ///< The buffer element of integer type
		Float,     ///< The buffer element of floating point type
		Double     ///< The buffer element of double precision floating point type
	};

		/**
		 * \enum BufferElementType
		 * \brief Rendering API agnostic buffer element type
		 * \details
		 * The buffer element type is used to describe the type of the buffer element.
		*/
	enum class BufferElementType
	{
		Byte,  Byte2,  Byte3,  Byte4,    // Signed Byte
		Ubyte, Ubyte2, Ubyte3, Ubyte4,   // Unsigned Byte
		Int,   Int2,   Int3,   Int4,     // Signed Integer
		Uint,  Uint2,  Uint3,  Uint4,    // Unsigned Integer
		Float, Float2, Float3, Float4,   // Floating Point
		Mat3,  Mat4                      // Matrix
	};
}
