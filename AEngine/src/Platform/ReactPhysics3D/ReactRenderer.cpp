/**
 * \file
 * \author Christien Alden (34119981)
*/
#include "ReactRenderer.h"
#include "AEngine/Render/RenderCommand.h"

namespace
{
	static constexpr rp3d::DebugRenderer::DebugItem g_reactDebugItems[] = {
		rp3d::DebugRenderer::DebugItem::COLLIDER_AABB,
		rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB,
		rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE,
		rp3d::DebugRenderer::DebugItem::CONTACT_POINT,
		rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL
	};

	static constexpr char* g_physicsShader = R"(
		#type vertex
		#version 330 core
		layout(location = 0) in vec3 a_position;
		layout(location = 1) in uint a_color;

		out vec4 v_color;
		uniform mat4 u_projectionView;

		void main()
		{
			gl_Position = u_projectionView * vec4(a_position, 1.0);
			v_color = vec4((a_color & 0xFF0000u) >> 16, (a_color & 0x00FF00u) >> 8, a_color & 0x0000FFu, 0xFF);
		}

		#type fragment
		#version 330 core
		in vec4 v_color;
		out vec4 color;

		void main()
		{
			color = v_color;
		}
	)";
}

namespace AEngine
{
	ReactPhysicsRenderer::ReactPhysicsRenderer(rp3d::DebugRenderer& renderer)
		: m_renderer{ renderer }, m_shader{ Shader::Create(g_physicsShader) },
		  m_trianglesVertexArray{ VertexArray::Create() },
		  m_linesVertexArray{ VertexArray::Create() }
	{
		// setup triangles vertex array
		m_trianglesVertexArray->Bind();
		SharedPtr<VertexBuffer> trianglesVertexBuffer = VertexBuffer::Create();
		trianglesVertexBuffer->SetLayout({
			{ BufferElementType::Float3, false },
			{ BufferElementType::Uint, false }
		});
		m_trianglesVertexArray->AddVertexBuffer(trianglesVertexBuffer);
		m_trianglesVertexArray->Unbind();

		// setup lines vertex array
		m_linesVertexArray->Bind();
		SharedPtr<VertexBuffer> linesVertexBuffer = VertexBuffer::Create();
		linesVertexBuffer->SetLayout({
			{ BufferElementType::Float3, false },
			{ BufferElementType::Uint, false }
		});
		m_linesVertexArray->AddVertexBuffer(linesVertexBuffer);
		m_linesVertexArray->Unbind();
	}

	void ReactPhysicsRenderer::Render(const Math::mat4& projectionView) const
	{
		RenderCommand::SetDepthTestFunction(DepthTestFunction::LessEqual);
		RenderCommand::PolygonMode(PolygonFace::FrontAndBack, PolygonDraw::Line);
		m_shader->Bind();
		m_shader->SetUniformMat4("u_projectionView", projectionView);

		unsigned int numTriangles = m_renderer.getNbTriangles();
		if (numTriangles > 0)
		{
			const rp3d::DebugRenderer::DebugTriangle* triangles = m_renderer.getTrianglesArray();
			(m_trianglesVertexArray->GetVertexBuffers())[0]->SetData(triangles, numTriangles * sizeof(rp3d::DebugRenderer::DebugTriangle), BufferUsage::StreamDraw);
			m_trianglesVertexArray->Bind();
			RenderCommand::DrawArrays(Primitive::Triangles, 0, numTriangles * 3);
			m_trianglesVertexArray->Unbind();
		}

		unsigned int numLines = m_renderer.getNbLines();
		if (numLines > 0)
		{
			const rp3d::DebugRenderer::DebugLine* lines = m_renderer.getLinesArray();
			(m_linesVertexArray->GetVertexBuffers())[0]->SetData(lines, numLines * sizeof(rp3d::DebugRenderer::DebugLine), BufferUsage::StreamDraw);
			m_linesVertexArray->Bind();
			RenderCommand::DrawArrays(Primitive::Lines, 0, numLines * 2);
			m_linesVertexArray->Unbind();
		}

		m_shader->Unbind();
		RenderCommand::PolygonMode(PolygonFace::FrontAndBack, PolygonDraw::Fill);
		RenderCommand::SetDepthTestFunction(DepthTestFunction::Less);
	}

	void ReactPhysicsRenderer::SetRenderItem(PhysicsRendererItem item, bool enable) const
	{
		rp3d::DebugRenderer::DebugItem reactItem = g_reactDebugItems[static_cast<int>(item)];
		m_renderer.setIsDebugItemDisplayed(reactItem, enable);
	}

	bool ReactPhysicsRenderer::IsRenderItemEnabled(PhysicsRendererItem item) const
	{
		rp3d::DebugRenderer::DebugItem reactItem = g_reactDebugItems[static_cast<int>(item)];
		return m_renderer.getIsDebugItemDisplayed(reactItem);
	}
}
