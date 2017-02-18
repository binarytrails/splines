#version 330 core
layout (location = 0) in vec3 position;

out vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // reversed because the mult on matrices is that way
    gl_Position = projection * view * model * vec4(position, 1.0f);
    gl_PointSize = 5.0;
    pos = position;
}
