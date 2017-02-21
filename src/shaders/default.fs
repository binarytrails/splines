#version 330 core

in vec3 pos;
out vec4 color;

void main()
{
    vec3 c1 = vec3(abs(pos.x), abs(pos.y), abs(pos.z));
    vec3 c2 = vec3(0.7f, 0.0f, pos.z * 0.7f);
    vec3 splinePointColor = vec3(0.0f, 0.0f, 0.0f);
    color = vec4(splinePointColor, 1.0f);
}
