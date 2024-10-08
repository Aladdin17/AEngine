#include "Model.h"
#include "AEngine/Core/Application.h"
#include "AEngine/Core/Logger.h"
#include "AEngine/Render/ResourceAPI.h"
#include "AEngine/Render/RenderCommand.h"
#include "Types.h"
#include "Platform/Assimp/AssimpModel.h"
#include "AEngine/Resource/AssetManager.h"

namespace AEngine
{
	Model::Model(const std::string& ident, const std::string& path)
		: Asset(ident, path) {}

	SharedPtr<Model> AEngine::Model::Create(const std::string& ident, const std::string& fname)
	{
		switch (ResourceAPI::GetLibrary())
		{
		case ModelLoaderLibrary::Assimp:
			return MakeShared<AssimpModel>(ident, fname);
		default:
			AE_LOG_FATAL("Model::Create::ModelLoaderLibrary::Error -> None selected");
		}
	}

	void Model::Clear()
	{
		m_meshes.clear();
		AE_LOG_DEBUG("Model::Clear");
	}

	void Model::RenderOpaque(const Math::mat4& transform, const Shader &shader, const Math::mat4 &projectionView) const
	{
		shader.Bind();
		shader.SetUniformMat4("u_transform", transform);
		shader.SetUniformMat4("u_projectionView", projectionView);

		for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
		{
			SharedPtr<Material> mat = AssetManager<Material>::Instance().Get(it->second.id);
			if(!mat->IsTransparent())
			{
				const VertexArray* va = (it->first).get();

				mat->Bind(shader);
				va->Bind();

				// draw
				RenderCommand::DrawIndexed(Primitive::Triangles, va->GetIndexBuffer()->GetCount(), 0);

				mat->Unbind(shader);
				va->Unbind();
			}
		}

		shader.Unbind();
	}

	void Model::RenderTransparent(const Math::mat4& transform, const Shader &shader, const Math::mat4 &projectionView) const
	{
		shader.Bind();
		shader.SetUniformMat4("u_transform", transform);
		shader.SetUniformMat4("u_projectionView", projectionView);

		for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
		{
			SharedPtr<Material> mat = AssetManager<Material>::Instance().Get(it->second.id);

				/// @todo Definitely need a better way to render transparent meshes
			if(mat->IsTransparent())
			{
				const VertexArray* va = (it->first).get();
				mat->Bind(shader);
				va->Bind();

				// draw
				RenderCommand::DrawIndexed(Primitive::Triangles, va->GetIndexBuffer()->GetCount(), 0);

				mat->Unbind(shader);
				va->Unbind();
			}
		}

		shader.Unbind();
	}

	void Model::Render(const Math::mat4& transform, const Shader& shader, const Math::mat4 & projectionView, Animator& animation, const TimeStep dt)
	{
		shader.Bind();
		//shader.SetUniformInteger("u_texture1", 0);
		shader.SetUniformMat4("u_transform", transform);
		shader.SetUniformMat4("u_projectionView", projectionView);

		animation.UpdateAnimation(dt);

		auto transforms = animation.GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			shader.SetUniformMat4("u_finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

		for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
		{
			SharedPtr<Material> mat = AssetManager<Material>::Instance().Get(it->second.id);
			const VertexArray* va = it->first.get();

			mat->Bind(shader);
			va->Bind();

			// draw
			RenderCommand::DrawIndexed(Primitive::Triangles, va->GetIndexBuffer()->GetCount(), 0);

			mat->Unbind(shader);
			va->Unbind();
		}

		shader.Unbind();
	}

	const VertexArray* Model::GetMesh(int index) const
	{
		if (index > m_meshes.size())
		{
			AE_LOG_FATAL("Model::GetMesh::Out of Bounds");
		}

		return (m_meshes[index].first).get();
	}

	const std::string& Model::GetMaterial(int index) const
	{
		if (index > m_meshes.size())
		{
			AE_LOG_FATAL("Model::GetMesh::Out of Bounds");
		}

		return m_meshes[index].second.id;
	}
}