#version 330

in WireframeVertex
{
    vec3 worldPosition;
    vec3 worldNormal;
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    vec3 color;
    noperspective vec4 edgeA;
    noperspective vec4 edgeB;
    flat int configuration;
}
fs_in;

out vec4 fragColor;

uniform int colorMode;
uniform vec3 baseColor;
uniform sampler2D baseColorMap;

#pragma include light.inc.frag
#pragma include pbr.inc.frag
#pragma include pbrsolidwireframe.inc.frag

void main()
{
    if (colorMode == 0) {
        vec3 color =
            shadeMetalRough(fs_in.worldPosition, fs_in.worldNormal, baseColor);
        fragColor = shadeLine(vec4(color, 1.0f));
    }
    else if (colorMode == 1) {
        vec3 baseColor = vec3(texture(baseColorMap, fs_in.texCoord));
        vec3 color =
            shadeMetalRough(fs_in.worldPosition, fs_in.worldNormal, baseColor);
        fragColor = shadeLine(vec4(color, 1.0f));
    }
    else {
        vec3 color = shadeMetalRough(
            fs_in.worldPosition, fs_in.worldNormal, fs_in.color);
        fragColor = shadeLine(vec4(color, 1.0f));
    }
}
