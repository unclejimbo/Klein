#version 430 core

uniform mat4 model;
uniform mat4 mvp;
uniform mat4 invTransModel;

layout (location = 0) in vec3 position_m;
layout (location = 1) in vec4 color;

out vec4 vertColor;

void main()
{
	gl_Position = mvp * vec4(position_m, 1.0f);

	vertColor = color;
}
