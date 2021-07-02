#version 330 core

in vec3 Positions;
in float height;
in vec3 Normals;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D shadowMap;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	float shadow;
	if (projCoords.z > 1.0)
	{
        shadow = 0.0;
	}
	else 
	{
		// depth from light angle
		float closestDepth = texture(shadowMap, projCoords.xy).r;
		float currentDepth = projCoords.z;

		// use shadow bias to avoid shadow acne
		//shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
		
		shadow = 0.0;
		vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
		for(int x = -3; x <= 3; ++x)
		{
			for(int y = -3; y <= 3; ++y)
			{
				float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
			}    
		}
		shadow /= 49;
	}

	return shadow;
}

void main()
{

	// calculate shadow
	float bias = 0.005;  
    float shadow = ShadowCalculation(FragPosLightSpace, bias);       

	vec3 color;
	if (height < 10)
	{
		color = texture(texture0, TexCoords).xyz;
	}
	else if (height < 30)
	{
		float alpha = (height - 10) / (20);  // 比例系数
        
        color = (texture(texture1, TexCoords) * alpha + texture(texture0, TexCoords) * (1 - alpha)).xyz;
	}
    else if (height < 60)
    {
		color = texture(texture1, TexCoords).xyz;
    }
	else if (height < 120)
    {
		float alpha = (height - 60) / (60);
        
        color = (texture(texture2, TexCoords) * alpha + texture(texture1, TexCoords) * (1 - alpha)).xyz;
    }
    else
    {
        color = texture(texture2, TexCoords).xyz;
    }

	// only use shadow without lighting model
	// --------------------------------------
	FragColor = vec4((1.0 - shadow / 2.5) * color, 1.0);

//	// use lighting model
//	// ------------------------
//	// gen random Normals
//	vec3 normal = normalize(Normals);
//
//	// ambient
//    vec3 ambient = 0.7 * lightColor;
//    // diffuse
//    vec3 lightDirN = normalize(-lightDir);
//    float diff = max(dot(lightDirN, normal), 0.0);
//    vec3 diffuse = 0.3 * diff * lightColor;
//    // specular
//    vec3 viewDir = normalize(viewPos - Positions);
//    vec3 halfwayDir = normalize(lightDirN + viewDir);  
//    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
//    vec3 specular = 0.5 * spec * lightColor;    
//
//	FragColor = vec4((ambient * 0.7 + (1.0 - shadow) * (diffuse + specular)) * color, 1.0);
}
