#version 430 core

uniform float alpha;

in vec3 vertColor;

out vec4 fragColor;

void main()
{
	fragColor = vec4(vertColor, alpha);
}