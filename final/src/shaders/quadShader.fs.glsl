#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ReTexture;

void main()
{               
	FragColor = vec4(texture(ReTexture, TexCoords).rgb, 1.0);
}