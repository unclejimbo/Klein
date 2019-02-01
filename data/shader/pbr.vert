#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;
in vec3 vertexColor;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 texCoord;
out vec3 vcolor;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 mvp;

#pragma include common.inc.vert

void main()
{
    worldPosition = transformPosition(vertexPosition, modelMatrix);
    worldNormal = transformNormal(vertexNormal, modelNormalMatrix);
    texCoord = transformTexCoord(vertexTexCoord);
    vcolor = vertexColor;
    gl_Position = glPosition(vertexPosition, mvp);
}
