//Inputs
in vec2 TexCoord;

//Outputs
out vec4 FragColor;

//Uniforms
uniform sampler2D SourceTexture;
uniform float Exposure;

void main()
{
	vec3 hdrColor = texture(SourceTexture, TexCoord).rgb;
	vec3 color = vec3(1.0f) - exp(-hdrColor * Exposure);

	FragColor = vec4(color, 1.0f);
}
