#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexColor;

out vec3 worldPosition;
out vec2 texCoord;
out vec3 vcolor;

uniform mat4 modelMatrix;
uniform mat4 mvp;

#pragma include Transform.inc.vert

void main()
{
    worldPosition = transformPosition(vertexPosition, modelMatrix);
    texCoord = transformTexCoord(vertexTexCoord);
    vcolor = vertexColor;
    gl_Position = glPosition(vertexPosition, mvp);
}
