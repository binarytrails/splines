#version 330 core

in vec3 pos;
out vec4 color;

uniform bool colorize;

void main()
{
    vec3 c1 = vec3(abs(pos.x), abs(pos.y), abs(pos.z));
    vec3 c2 = vec3(0.7f, 0.0f, pos.z * 0.7f);
    vec3 c3 = vec3(0.0f, 0.0f, 0.0f);
    
    if (!colorize)
        color = vec4(c3, 1.0f);
    else
        color = vec4(c1 * 2, 1.0f);
}
