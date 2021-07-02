#version 330 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;

in VS_OUT {
	float height;
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = vec4(fs_in.FragPos, 1.0);
    // also store the per-fragment normals into the gbuffer
    gNormal = vec4(fs_in.Normal, 1.0);
    // and the diffuse per-fragment color
	vec3 color;
	if (fs_in.height < 10)
	{
		color = texture(texture0, fs_in.TexCoords).xyz;
	}
	else if (fs_in.height < 30)
	{
		float alpha = (fs_in.height - 10) / (20);  // ±ÈÀýÏµÊý
        color = (texture(texture1, fs_in.TexCoords) * alpha + texture(texture0, fs_in.TexCoords) * (1 - alpha)).xyz;
	}
    else if (fs_in.height < 60)
    {
		color = texture(texture1, fs_in.TexCoords).xyz;
    }
	else if (fs_in.height < 120)
    {
		float alpha = (fs_in.height - 60) / (60);
        
        color = (texture(texture2, fs_in.TexCoords) * alpha + texture(texture1, fs_in.TexCoords) * (1 - alpha)).xyz;
    }
    else
    {
        color = texture(texture2, fs_in.TexCoords).xyz;
    }

    gAlbedo = vec4(color, 1.0);
}