#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D gPositionTexture;
uniform sampler2D gNormalTexture;
uniform sampler2D gAlbedoSpecTexture;

// CSM
const int CascadeNum = 3;
uniform sampler2D depthMap[CascadeNum];
uniform mat4 lightSpaceMatrix[CascadeNum];
uniform float cascadeEndClipSpace[CascadeNum];

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform mat4 view;
uniform mat4 projection;

float calcShadow(int index, vec4 fragPosLightSpace)
{
	float shadow = 0;

	vec3 ndcCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	ndcCoords = (ndcCoords + 1.0) / 2;

	float closestDepth = 0;
	closestDepth = texture(depthMap[index], ndcCoords.xy).r;

	float currentDepth = ndcCoords.z;
	if(currentDepth > 1.0)
		shadow = 0;
	else
	{
		float bias = 0.005;
		shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;  
		vec2 texelSize = 1.0 / textureSize(depthMap[index], 0);
		for(int x = -3; x <= 3; ++x)
		{
			for(int y = -3; y <= 3; ++y)
			{
				float pcfDepth = texture(depthMap[index], ndcCoords.xy + vec2(x, y) * texelSize).r; 
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
			}
		}
		shadow /= 49.0;
	}

	return shadow;
}

void main()
{
	vec3 FragPos = texture(gPositionTexture, TexCoords).rgb;
    vec3 Normal = texture(gNormalTexture, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpecTexture, TexCoords).rgb;

	// calc shadow
	float ClipSpacePosZ = vec4(projection * view * vec4(FragPos, 1.0)).z;
	vec4 fragPosLightSpace[CascadeNum];
	for(int i = 0; i < CascadeNum; i++)
	{
		fragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0);
	}

	float shadow;
	vec3 debugColor;
	for(int i = 0; i < CascadeNum; i++)
	{
		if (ClipSpacePosZ <= cascadeEndClipSpace[i])
		{
			debugColor[i] = 0.1;

			shadow = calcShadow(i, fragPosLightSpace[i]);

			// blend
			if(i > 0 && abs(ClipSpacePosZ - cascadeEndClipSpace[i-1]) < 1.0)
			{
				float factor = abs(ClipSpacePosZ - cascadeEndClipSpace[i-1]) / 1.0;
				//factor = factor / 2.0 + 0.5;
				shadow = shadow * factor + calcShadow(i-1, fragPosLightSpace[i-1]) * (1.0 - factor);
				//shadow = calcShadow(i-1, norm, lightDir);
			}
			else if(i < CascadeNum-1 && abs(ClipSpacePosZ - cascadeEndClipSpace[i+1]) < 1.0)
			{
				float factor = abs(ClipSpacePosZ - cascadeEndClipSpace[i+1]) / 1.0;
				//factor = factor / 2.0 + 0.5;
				shadow = shadow * (1.0 - factor) + calcShadow(i+1, fragPosLightSpace[i+1]) * factor;
				//shadow = calcShadow(i+1, norm, lightDir);
			}
			
			break;
		}
	}

//	// lighting
//	// ambient
//    vec3 ambient = 0.7 * Albedo;
//    // diffuse
//	vec3 normal = normalize(Normal);
//	vec3 lightDirN = normalize(-lightDir);
//    float diff = max(dot(lightDirN, normal), 0.0);
//    vec3 diffuse = 0.3 * diff * lightColor;

	//FragColor = vec4((1.0 - shadow / 2.5) * Albedo + debugColor, 1.0);
	FragColor = vec4((1.0 - shadow / 2.5) * Albedo, 1.0);
}