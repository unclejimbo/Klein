#version 430 core

uniform mat4 mvp;

layout (location = 0) in vec3 position_m;

void main()
{
	gl_Position = mvp * vec4(position_m, 1.0f);
}