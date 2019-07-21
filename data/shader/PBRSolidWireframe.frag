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

#pragma include Light.inc.frag
#pragma include PBR.inc.frag
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

    vec3 normal;
    if ((renderMode & 0x4) == 0) { normal = fs_in.worldNormal; }
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

    vec3 c = shadeMetalRough(fs_in.worldPosition,
                             fs_in.worldNormal,
                             fs_in.lightSpacePosition,
                             color,
                             r,
                             m);
    fragColor = shadeLine(vec4(c, 1.0f), data);
}
