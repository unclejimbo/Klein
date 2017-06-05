#version 430 core

uniform uint bufferSpec;
uniform uint primitiveType;
uniform uint nodeID;
uniform uint bufferID;

out uvec4 fragColor;

void main()
{
	uint type = (bufferSpec << 16) + (primitiveType & 0x0000ffff);
	fragColor = uvec4(type, nodeID, bufferID, gl_PrimitiveID);
}