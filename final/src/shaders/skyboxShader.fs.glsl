#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube Skybox;

void main()
{    
	vec3 tTexCoords = TexCoords;

	// rotate up and down skybox to dsiplay right
	if (tTexCoords.y >= 0.9999)
	{
		tTexCoords.x = -TexCoords.z;
		tTexCoords.z = TexCoords.x;
	}
	else if (tTexCoords.y <= -0.9999)
	{
		tTexCoords.x = -TexCoords.z;
		tTexCoords.z = TexCoords.x;
	}

    FragColor = texture(Skybox, tTexCoords);
	//FragColor = vec4(0.5f, 0.7f, 1.0f, 1.0f);
}