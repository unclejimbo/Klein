#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;
in vec3 vertexColor;

out vec3 worldPosition;
out vec3 worldNormal;
out vec3 eyePosition;
out vec3 eyeNormal;
out vec2 texCoord;
out vec3 vcolor;
out vec4 lightSpacePosition;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;
uniform mat4 lightSpaceMatrix;

#pragma include Transform.inc.vert

void main()
{
    vec3 pos = normalShift(vertexPosition, vertexNormal);
    worldPosition = transformPosition(pos, modelMatrix);
    worldNormal = transformNormal(vertexNormal, modelNormalMatrix);
    eyePosition = transformPosition(pos, modelView);
    eyeNormal = transformNormal(vertexNormal, modelViewNormal);
    texCoord = transformTexCoord(vertexTexCoord);
    vcolor = vertexColor;
    lightSpacePosition = lightSpaceMatrix * vec4(worldPosition, 1.0);
    gl_Position = glPosition(pos, mvp);
}
