#version 430 core

uniform mat4 model;
uniform mat4 mvp;
uniform mat4 invTransModel;

layout (location = 0) in vec3 position_m;
layout (location = 1) in vec3 normal_m;
layout (location = 2) in vec3 color;

out vec3 vertPosition_w;
out vec3 vertNormal_w;
out vec3 vertColor;

void main()
{
	gl_Position = mvp * vec4(position_m, 1.0f);

	vertPosition_w = (model * vec4(position_m, 1.0f)).xyz;
	vertNormal_w = normalize(mat3(invTransModel) * normal_m);
	vertColor = color;
}