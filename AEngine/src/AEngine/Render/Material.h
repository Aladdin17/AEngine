#include "AEngine/Core/Types.h"
#include "Shader.h"
#include "Texture.h"

#include <map>

namespace AEngine
{
	enum AE_TEXTURETYPE
	{
		NONE = 0,
		DIFFUSE = 1,
		SPECULAR = 2,
		AMBIENT = 3,
		EMISSIVE = 4,
		HEIGHT = 5,
		NORMALS = 6,
		SHININESS = 7,
		OPACITY = 8,
		DISPLACEMENT = 9,
		LIGHTMAP = 10,
		REFLECTION = 11,
		BASE_COLOR = 12,
		NORMAL_CAMERA = 13,
		EMISSION_COLOR = 14,
		METALNESS = 15,
		DIFFUSE_ROUGHNESS = 16,
		AMBIENT_OCCLUSION = 17,
		UNKNOWN = -1
	};


	class Material  
	{
	public:
		Material();
		void addTexture(AE_TEXTURETYPE type, SharedPtr<Texture> texture);
		void setShader(SharedPtr<Shader> shader);
			/// @todo Material should hold shader
		void Bind(const Shader& shader) const;
		void Unbind(const Shader& shader) const;

	private:
		std::map<AE_TEXTURETYPE, SharedPtr<Texture>> m_textures;
		SharedPtr<Shader> m_shader;
		Math::vec3 m_color;
	};
}