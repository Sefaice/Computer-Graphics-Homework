#version 330 core
in vec2 TexCoords;
in vec3 Positions;
out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    if(Positions.y >140)
    {
        FragColor = texture(texture1,TexCoords);
    }
    else
    {
        float alpha = Positions.y/(140);  // 比例系数
        
        FragColor = texture(texture1,TexCoords)*alpha
        + texture(texture2,TexCoords)*(1-alpha);
    }
    
}
