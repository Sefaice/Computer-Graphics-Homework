#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    vec4 result = texture(texture_diffuse1, TexCoords);
    FragColor = result;
    if(result.w==0){
        FragColor =vec4(1.0f,1.0f,1.0f,0.0f);
    }
}
