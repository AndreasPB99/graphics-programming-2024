#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform DiffuseReflection;
uniform vec3 LightColor;

void main()
{
	FragColor = Color * texture(ColorTexture, TexCoord);
}
