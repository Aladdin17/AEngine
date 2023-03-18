#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D u_texture1;
uniform vec3 u_lightPos;
uniform vec3 u_lightColour;
uniform float u_ambient;

void main()
{
	// calculate normal and direction
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(u_lightPos - FragPos);

	// calculate light influence
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * u_lightColour;

	// object color at location
	vec3 objColour = texture(u_texture1, TexCoord).xyz;

	// result
	vec3 result = (diffuse + u_ambient) * objColour;
    FragColor = vec4(result, 1.0);
}
