#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;
in vec3 vertexColor;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 texCoord;
out vec3 vcolor;
out vec4 lightSpacePosition;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 mvp;
uniform mat4 lightSpaceMatrix;

#pragma include Transform.inc.vert

void main()
{
    worldPosition = transformPosition(vertexPosition, modelMatrix);
    worldNormal = transformNormal(vertexNormal, modelNormalMatrix);
    texCoord = transformTexCoord(vertexTexCoord);
    vcolor = vertexColor;
    lightSpacePosition = lightSpaceMatrix * vec4(worldPosition, 1.0);
    gl_Position = glPosition(vertexPosition, mvp);
}
