#version 330

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texCoord;
in vec3 vcolor;
in vec4 lightSpacePosition;

out vec4 fragColor;

uniform int renderMode = 0;
uniform vec3 baseColor;
uniform float roughness;
uniform float metalness;
uniform sampler2D baseColorMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform sampler2D metalnessMap;

#pragma include Light.inc.frag
#pragma include PBR.inc.frag

void main()
{
    vec3 color;
    if ((renderMode & 0x1) == 0) { color = baseColor; }
    else {
        if ((renderMode & 0x2) == 0) {
            color = vec3(texture(baseColorMap, texCoord));
        }
        else {
            color = vcolor;
        }
    }

    vec3 normal;
    if ((renderMode & 0x4) == 0) { normal = worldNormal; }
    else {
        normal = vec3(texture(normalMap, texCoord));
    }

    float r;
    if ((renderMode & 0xf) == 0) { r = roughness; }
    else {
        r = texture(roughnessMap, texCoord).x;
    }

    float m;
    if ((renderMode & 0x8) == 0) { m = metalness; }
    else {
        m = texture(metalnessMap, texCoord).x;
    }

    vec3 c = shadeMetalRough(
        worldPosition, worldNormal, lightSpacePosition, color, r, m);
    fragColor = vec4(c, 1.0f);
}
