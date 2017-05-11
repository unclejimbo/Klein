#version 430 core

uniform uint geomType;
uniform uint geomID;
uniform uint primitiveType;

out uvec4 fragColor;

void main()
{
	fragColor = uvec4(geomType, geomID, primitiveType, gl_PrimitiveID);
}