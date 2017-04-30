#version 430 core

uniform mat4 mvp;

layout (location = 0) in vec3 position_m;
layout (location = 1) in vec3 color;

out vec3 vertColor;

void main()
{
	gl_Position = mvp * vec4(position_m, 1.0f);
	vertColor = color;
}