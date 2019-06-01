#version 330

in vec3 vertexPosition;

uniform mat4 modelMatrix;

#pragma include Transform.inc.vert

void main()
{
    gl_Position = glPosition(vertexPosition, modelMatrix);
}
