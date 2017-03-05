#version 430 core

uniform float alpha;

in vec4 vertColor;

out vec4 fragColor;

void main()
{
	fragColor = vertColor;
}
