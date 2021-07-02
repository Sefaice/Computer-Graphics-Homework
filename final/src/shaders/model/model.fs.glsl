#version 330 core
out vec4 FragColor;

in vec3 Positions;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    vec4 objectColor = texture(texture_diffuse1, TexCoords);

	// alpha test
	if (objectColor.w <= 0.5)
		discard;

    /*if (objectColor.w == 0) 
	{
        objectColor = vec4(1.0f, 1.0f, 1.0f, 0.0f);
    }*/

	//FragColor = objectColor;

	// ambient
    vec3 ambient = 0.7 * lightColor;
    // diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDirN = normalize(-lightDir);
    float diff = max(dot(lightDirN, normal), 0.0);
    vec3 diffuse = 0.3 * diff * lightColor;
//    // specular
//    vec3 viewDir = normalize(viewPos - Positions);
//    vec3 halfwayDir = normalize(lightDirN + viewDir);  
//    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
//    vec3 specular = 0.5 * spec * lightColor;

	vec3 result = vec3((ambient + diffuse) * objectColor.xyz);
	FragColor = vec4(result, 1.0);
}
