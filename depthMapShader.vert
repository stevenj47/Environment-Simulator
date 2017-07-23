#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 light_proj;
uniform mat4 light_view;

void main()
{
    gl_Position = light_proj * light_view * model * vec4(position, 1.0f);
}  