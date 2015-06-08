#version 150

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTextureUV;

out vec3 position;
out vec3 normal;
out vec2 textureUV;

uniform mat4 model;
uniform mat4 projectionView;

void main()
{
	position = (model * vec4(vertexPosition, 1)).xyz;
	normal = normalize((model * vec4(vertexNormal, 0)).xyz);
	textureUV = vertexTextureUV;
	gl_Position = projectionView * vec4(position, 1);
}
