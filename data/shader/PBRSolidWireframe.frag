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

uniform int colorMode = 0;
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
    if (colorMode == 0) {
        vec3 color = shadeMetalRough(fs_in.worldPosition,
                                     fs_in.worldNormal,
                                     fs_in.lightSpacePosition,
                                     baseColor);
        fragColor = shadeLine(vec4(color, 1.0f), data);
    }
    else if (colorMode == 1) {
        vec3 baseColor = vec3(texture(baseColorMap, fs_in.texCoord));
        vec3 color = shadeMetalRough(fs_in.worldPosition,
                                     fs_in.worldNormal,
                                     fs_in.lightSpacePosition,
                                     baseColor);
        fragColor = shadeLine(vec4(color, 1.0f), data);
    }
    else {
        vec3 color = shadeMetalRough(fs_in.worldPosition,
                                     fs_in.worldNormal,
                                     fs_in.lightSpacePosition,
                                     fs_in.color);
        fragColor = shadeLine(vec4(color, 1.0f), data);
    }
}
