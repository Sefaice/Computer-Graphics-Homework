#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
// for gen shadows
in vec2 TexCoords;
in vec4 FragPosLightSpace[3];
in float ClipSpacePosZ;

const int CascadeNum = 3;

uniform sampler2D depthMap[CascadeNum];
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform float cascadeEndClipSpace[CascadeNum];
// DEBUG
// --------------------
in vec4 ViewSpacePos;
uniform float dxn;
uniform float dxf;
uniform float dyn;
uniform float dyf;
uniform float dzn;
uniform float dzf;
// ---------------------

float calcShadow(int index, vec3 norm, vec3 lightDir)
{
	float shadow = 0;

	vec3 ndcCoords = FragPosLightSpace[index].xyz / FragPosLightSpace[index].w;
	ndcCoords = (ndcCoords + 1.0) / 2;

	float closestDepth = 0;
	closestDepth = texture(depthMap[index], ndcCoords.xy).r;

	// DEBUG
	// -------------------------------------------
//			if(closestDepth > 0.0 && closestDepth < 1.0)
//				FragColor = vec4(debugColor * 3, 1.0);
//			else 
//				FragColor = vec4(1.0, 0.0, 0.0, 1.0);
//			FragColor = vec4(vec3(closestDepth), 1.0);
	// --------------------------------------

	float currentDepth = ndcCoords.z;
	if(currentDepth > 1.0)
		shadow = 0;
	else
	{
		float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
		shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;  
		vec2 texelSize = 1.0 / textureSize(depthMap[index], 0);
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				float pcfDepth = texture(depthMap[index], ndcCoords.xy + vec2(x, y) * texelSize).r; 
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
			}
		}
		shadow /= 9.0;
	}

	return shadow;
}

void main()
{	
	// 计算光照
	vec3 lightDir = normalize(lightPos - FragPos);
	// ambient 环境光
	vec3 ambient = 0.3 * lightColor;
	// diffuse 散射/漫射
	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = 1.0 * diff * lightColor;
	// specular 反射
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = 1.0 * spec * lightColor;
	//FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
	//FragColor = vec4(gl_FragCoord.x / 1280, gl_FragCoord.y / 720, gl_FragCoord.z, 1.0);

	// DEBUG
	// -----------------
//	if( ((ViewSpacePos.x >= -dxn && ViewSpacePos.x <= dxn) || (ViewSpacePos.x >= -dxf && ViewSpacePos.x <= dxf)) &&
//		((ViewSpacePos.y >= -dyn && ViewSpacePos.y <= dyn) || (ViewSpacePos.y >= -dyf && ViewSpacePos.y <= dyf)) && 
//		 (ViewSpacePos.z >=  dzf && ViewSpacePos.z <= dzn) )
//	{
//		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
//	}
//	else
//		FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	// ------------------

	// 计算阴影
	float shadow;
	vec3 debugColor;
	for(int i = 0; i < CascadeNum; i++)
	{
		if (ClipSpacePosZ <= cascadeEndClipSpace[i])
		{
			debugColor[i] = 0.1;

			shadow = calcShadow(i, norm, lightDir);
			// blend
			if(i > 0 && abs(ClipSpacePosZ - cascadeEndClipSpace[i-1]) < 1.0)
			{
				float factor = abs(ClipSpacePosZ - cascadeEndClipSpace[i-1]) / 1.0;
				//factor = factor / 2.0 + 0.5;
				shadow = shadow * factor + calcShadow(i-1, norm, lightDir) * (1.0 - factor);
				//shadow = calcShadow(i-1, norm, lightDir);
			}
			else if(i < CascadeNum-1 && abs(ClipSpacePosZ - cascadeEndClipSpace[i+1]) < 1.0)
			{
				float factor = abs(ClipSpacePosZ - cascadeEndClipSpace[i+1]) / 1.0;
				//factor = factor / 2.0 + 0.5;
				shadow = shadow * (1.0 - factor) + calcShadow(i+1, norm, lightDir) * factor;
				//shadow = calcShadow(i+1, norm, lightDir);
			}
			
			break;
		}
	}

	FragColor = vec4((ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor + debugColor, 1.0);
}