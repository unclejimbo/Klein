#version 330

in WireframeVertex
{
    vec3 worldPosition;
    vec3 worldNormal;
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    vec3 color;
    vec4 lightSpacePosition;
    noperspective vec4 edgeA;
    noperspective vec4 edgeB;
    flat int configuration;
}
fs_in;

out vec4 fragColor;

uniform int renderMode = 0;
uniform vec3 baseColor;
uniform sampler2D baseColorMap;

#pragma include SolidWireframe.inc.frag

void main()
{
    SolidWireframeData data;
    data.edgeA = fs_in.edgeA;
    data.edgeB = fs_in.edgeB;
    data.configuration = fs_in.configuration;
    vec3 color;
    if ((renderMode & 0x1) == 0) { color = baseColor; }
    else {
        if ((renderMode & 0x2) == 0) {
            color = vec3(texture(baseColorMap, fs_in.texCoord));
        }
        else {
            color = fs_in.color;
        }
    }
    fragColor = shadeLine(vec4(color, 1.0f), data);
}
