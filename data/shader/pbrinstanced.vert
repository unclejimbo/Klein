#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;
in vec3 color;
in mat4 model;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 texCoord;
out vec3 vcolor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

#pragma include common.inc.vert

void main()
{
    mat4 modelView = viewMatrix * model;
    mat3 modelNormalMatrix = modelNormal(modelView);
    mat4 mvp = projectionMatrix * modelView;
    worldPosition = transformPosition(vertexPosition, model);
    worldNormal = transformNormal(vertexNormal, modelNormalMatrix);
    texCoord = transformTexCoord(vertexTexCoord);
    vcolor = color;
    gl_Position = glPosition(vertexPosition, mvp);
}
