#version 430 core

uniform vec3 primitiveColor;

out vec4 fragColor;

void main()
{
	fragColor = vec4(primitiveColor, 1.0f);
}
