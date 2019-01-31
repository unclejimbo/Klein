#version 330

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texCoord;

out vec4 fragColor;

uniform bool useTexture;
uniform vec3 baseColor;
uniform sampler2D baseColorMap;

#pragma include light.inc.frag
#pragma include pbr.inc.frag

void main()
{
    vec3 bc;
    if (useTexture) {
        bc = vec3(texture(baseColorMap, texCoord));
        vec3 color = shadeMetalRough(worldPosition, worldNormal, bc);
        fragColor = vec4(color, 1.0f);
    }
    else {
        bc = baseColor;
        vec3 color = shadeMetalRough(worldPosition, worldNormal, bc);
        fragColor = vec4(color, 1.0f);
    }
}
