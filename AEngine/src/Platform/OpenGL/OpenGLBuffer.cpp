/**
 * \file
 * \author Christien Alden (34119981)
*/
#include "OpenGLBuffer.h"

namespace
{
	static constexpr GLenum g_glBufferUsage[] = {
		GL_STATIC_DRAW,
		GL_DYNAMIC_DRAW,
		GL_STREAM_DRAW
	};
}

namespace AEngine
{
//--------------------------------------------------------------------------------
// VertexBuffer
//--------------------------------------------------------------------------------
	OpenGLVertexBuffer::OpenGLVertexBuffer()
		: m_id{ 0 }, m_size{ 0 }
	{
		glGenBuffers(1, &m_id);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_id);
		m_id = 0;
		m_size = 0;
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	Intptr_t OpenGLVertexBuffer::Size() const
	{
		return static_cast<Intptr_t>(m_size);
	}

	Intptr_t OpenGLVertexBuffer::GetCount() const
	{
		if (m_size == 0)
		{
			return 0;
		}

		Intptr_t layoutStride = m_layout.GetStride();
		if (layoutStride == 0)
		{
			return 0;
		}

		return m_size / layoutStride;
	}

	void OpenGLVertexBuffer::SetData(const void* data, Intptr_t bytes, BufferUsage usage)
	{
		m_size = static_cast<GLsizeiptr>(bytes);

		Bind();
		GLenum glUsage = g_glBufferUsage[static_cast<int>(usage)];
		glBufferData(GL_ARRAY_BUFFER, m_size, data, glUsage);
		Unbind();
	}

	void OpenGLVertexBuffer::SetSubData(const void* data, Intptr_t bytes, Intptr_t offset)
	{
		Bind();
		glBufferSubData(GL_ARRAY_BUFFER, offset, bytes, data);
		Unbind();
	}

//--------------------------------------------------------------------------------
// IndexBuffer
//--------------------------------------------------------------------------------
	OpenGLIndexBuffer::OpenGLIndexBuffer()
		: m_id{ 0 }, m_count{ 0 }
	{
		glGenBuffers(1, &m_id);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_id);
		m_id = 0;
		m_count = 0;
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	Intptr_t OpenGLIndexBuffer::GetCount() const
	{
		return static_cast<Intptr_t>(m_count);
	}

	void OpenGLIndexBuffer::SetData(const Uint32* data, Intptr_t count, BufferUsage usage)
	{
		m_count = static_cast<GLsizeiptr>(count);

		Bind();
		GLenum glUsage = g_glBufferUsage[static_cast<int>(usage)];
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(Uint32), data, glUsage);
		Unbind();
	}
}