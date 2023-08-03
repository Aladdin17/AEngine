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

	static constexpr rp3d::DebugRenderer::DebugCollisionShapeType g_reactDebugCollisionShapes[] = {
		rp3d::DebugRenderer::DebugCollisionShapeType::BOX,
		rp3d::DebugRenderer::DebugCollisionShapeType::SPHERE,
		rp3d::DebugRenderer::DebugCollisionShapeType::CAPSULE,
		rp3d::DebugRenderer::DebugCollisionShapeType::CONVEX_MESH,
		rp3d::DebugRenderer::DebugCollisionShapeType::TRIANGLE_MESH,
		rp3d::DebugRenderer::DebugCollisionShapeType::HEIGHTFIELD
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
		  m_linesVertexArray{ VertexArray::Create() },
		  m_numLines{ 0 }, m_numTriangles{ 0 }
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
		// capture render states
		PolygonDraw frontFace = RenderCommand::GetPolygonMode(PolygonFace::Front);
		PolygonDraw backFace = RenderCommand::GetPolygonMode(PolygonFace::Back);
		DepthTestFunction depthFunc = RenderCommand::GetDepthTestFunction();

		// check if any of the render states are invalid
		// depthFunc is not checked as it can not be invalid
		if (frontFace == PolygonDraw::Invalid || backFace == PolygonDraw::Invalid)
		{
			return;
		}

		// set render states
		RenderCommand::SetDepthTestFunction(DepthTestFunction::LessEqual);
		RenderCommand::PolygonMode(PolygonFace::FrontAndBack, PolygonDraw::Line);

		m_shader->Bind();
		m_shader->SetUniformMat4("u_projectionView", projectionView);

		m_trianglesVertexArray->Bind();
		RenderCommand::DrawArrays(Primitive::Triangles, 0, m_numTriangles * 3);
		m_trianglesVertexArray->Unbind();

		m_linesVertexArray->Bind();
		RenderCommand::DrawArrays(Primitive::Lines, 0, m_numLines * 2);
		m_linesVertexArray->Unbind();

		m_shader->Unbind();

		// reset render states
		RenderCommand::PolygonMode(PolygonFace::Front, frontFace);
		RenderCommand::PolygonMode(PolygonFace::Back, backFace);
		RenderCommand::SetDepthTestFunction(depthFunc);
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

	bool ReactPhysicsRenderer::IsRenderShapeEnabled(CollisionRenderShape shape) const
	{
		rp3d::DebugRenderer::DebugCollisionShapeType reactShape = g_reactDebugCollisionShapes[static_cast<int>(shape)];
		return m_renderer.getIsCollisionShapeDisplayed(reactShape);
	}

	void ReactPhysicsRenderer::SetRenderShape(CollisionRenderShape shape, bool enable) const
	{
		rp3d::DebugRenderer::DebugCollisionShapeType reactShape = g_reactDebugCollisionShapes[static_cast<int>(shape)];
		m_renderer.setIsCollisionShapeDisplayed(reactShape, enable);
	}

	void ReactPhysicsRenderer::GenerateRenderData()
	{
		m_numTriangles = m_renderer.getNbTriangles();
		if (m_numTriangles > 0)
		{
			const rp3d::DebugRenderer::DebugTriangle* triangles = m_renderer.getTrianglesArray();
			(m_trianglesVertexArray->GetVertexBuffers())[0]->SetData(triangles, m_numTriangles * sizeof(rp3d::DebugRenderer::DebugTriangle), BufferUsage::StreamDraw);
		}

		m_numLines = m_renderer.getNbLines();
		if (m_numLines > 0)
		{
			const rp3d::DebugRenderer::DebugLine* lines = m_renderer.getLinesArray();
			(m_linesVertexArray->GetVertexBuffers())[0]->SetData(lines, m_numLines * sizeof(rp3d::DebugRenderer::DebugLine), BufferUsage::StreamDraw);
		}
	}
}
