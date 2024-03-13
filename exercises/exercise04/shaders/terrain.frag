#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;
in float Height;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform sampler2D ColorTexture2;
uniform sampler2D ColorTexture3;
uniform sampler2D ColorTexture4;
uniform vec2 ColorTextureScale;
uniform vec2 ColorTextureRange1;
uniform vec2 ColorTextureRange2;
uniform vec2 ColorTextureRange3;

float inverseMix(vec2 range, float height)
{
	return clamp((height - range.x) / (range.y - range.x), 0, 1);

}

void main()
{
	vec4 color1 = texture(ColorTexture, TexCoord * ColorTextureScale);
	vec4 color2 = texture(ColorTexture2, TexCoord * ColorTextureScale);
	vec4 color3 = texture(ColorTexture3, TexCoord * ColorTextureScale);
	vec4 color4 = texture(ColorTexture4, TexCoord * ColorTextureScale);

	vec4 color = color1;

	color = mix(color, color2, inverseMix(ColorTextureRange1, Height));
	color = mix(color, color3, inverseMix(ColorTextureRange2, Height));
	color = mix(color, color4, inverseMix(ColorTextureRange3, Height));

	FragColor = Color * color;
}
