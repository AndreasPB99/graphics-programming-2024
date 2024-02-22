#version 330 core

out vec4 FragColor;

// (todo) 02.5: Add Color input variable here
in vec4 Color;


void main()
{

	float alpha = 1 - length(gl_PointCoord * 2 - 1);

	FragColor = vec4(Color[0], Color[1], Color[2], Color[3] * alpha);
}
