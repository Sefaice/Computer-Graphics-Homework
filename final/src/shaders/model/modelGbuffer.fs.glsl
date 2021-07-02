#version 330 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture_diffuse1;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = vec4(fs_in.FragPos, 1.0);
    // also store the per-fragment normals into the gbuffer
    gNormal = vec4(fs_in.Normal, 1.0);

    // and the diffuse per-fragment color
	vec4 objectColor = texture(texture_diffuse1, fs_in.TexCoords);
	// alpha test
	if (objectColor.w <= 0.5)
		discard;
    gAlbedo = vec4(objectColor.rgb, 1.0);
}