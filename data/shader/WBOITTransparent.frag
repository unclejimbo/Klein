#version 330

in vec2 texCoord;
in vec3 vcolor;

layout(location = 0) out vec4 accum;
layout(location = 1) out float revealage;

uniform int colorMode = 0;
uniform vec3 baseColor;
uniform sampler2D baseColorMap;
uniform float depthScale = 200.0;

void main()
{
    vec3 albedo;
    if (colorMode == 0) { albedo = baseColor; }
    else if (colorMode == 1) {
        albedo = vec3(texture(baseColorMap, texCoord));
    }
    else {
        albedo = vcolor;
    }
    vec4 color = vec4(albedo, 0.5);

    float linearDepth = abs(1.0 / gl_FragCoord.w);
    float w =
        clamp(0.03 / (1e-5 + pow(linearDepth / depthScale, 4.0)), 1e-2, 3e3);
    accum = vec4(color.rgb * color.a, color.a) * w;
    revealage = color.a;
}
