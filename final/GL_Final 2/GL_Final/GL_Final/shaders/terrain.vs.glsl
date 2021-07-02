#version 330 core
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_texcoord;

out vec2 TexCoords;
out vec3 Positions;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    TexCoords = v_texcoord.xy;
    Positions = v_position;
    gl_Position = projection* view * model * vec4(v_position, 1.0);
}
