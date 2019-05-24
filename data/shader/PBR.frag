#version 330

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texCoord;
in vec3 vcolor;
in vec4 lightSpacePosition;

out vec4 fragColor;

uniform int colorMode = 0;
uniform vec3 baseColor;
uniform sampler2D baseColorMap;

#pragma include Light.inc.frag
#pragma include PBR.inc.frag

void main()
{
    vec3 color;
    if (colorMode == 0) { color = baseColor; }
    else if (colorMode == 1) {
        color = vec3(texture(baseColorMap, texCoord));
    }
    else {
        color = vcolor;
    }
    vec3 c =
        shadeMetalRough(worldPosition, worldNormal, lightSpacePosition, color);
    fragColor = vec4(c, 1.0f);
}
