#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 u_projectionView;
uniform mat4 u_transform;
uniform mat3 u_normalMatrix;

void main()
{
	gl_Position = u_projectionView * u_transform * vec4(aPos, 1.0);

	FragPos = vec3(u_transform * vec4(aPos, 1.0));
	Normal = u_normalMatrix * aNormal;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
