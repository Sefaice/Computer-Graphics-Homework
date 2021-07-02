#version 330 core

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
	// tree models need alpha test or its shadow will be a plane, not leaves
	// so we need alpha test here while drawing depth map
	vec4 objectColor = texture(texture_diffuse1, TexCoords);
	if (objectColor.w <= 0.5)
		discard;

    // gl_FragDepth = gl_FragCoord.z;
}