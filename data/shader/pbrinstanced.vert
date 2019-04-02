#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;
in vec3 vertexColor;
in mat4 instanceModel;
in vec3 instanceColor;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 texCoord;
out vec3 vcolor;

uniform bool useInstanceColor = false;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

#pragma include common.inc.vert

void main()
{
    mat4 modelMat = modelMatrix * instanceModel;
    mat4 modelView = viewMatrix * modelMat;
    mat3 modelNormalMatrix = modelNormal(modelMat);
    mat4 mvp = projectionMatrix * modelView;
    worldPosition = transformPosition(vertexPosition, modelMat);
    worldNormal = transformNormal(vertexNormal, modelNormalMatrix);
    texCoord = transformTexCoord(vertexTexCoord);
    gl_Position = glPosition(vertexPosition, mvp);
    if (useInstanceColor) { vcolor = instanceColor; }
    else {
        vcolor = vertexColor;
    }
}
