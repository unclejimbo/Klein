#version 430 core

uniform vec3 diffuseColor;

out vec4 fragColor;

void main()
{
	fragColor = vec4(diffuseColor, 1.0f);
}