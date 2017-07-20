#version 430 core

uniform vec3 diffuseColor;
uniform float alpha;

out vec4 fragColor;

void main()
{
	fragColor = vec4(diffuseColor, alpha);
}