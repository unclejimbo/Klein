#version 330

in vec2 texCoord;
in vec3 vcolor;

out vec4 fragColor;

uniform int renderMode = 0;
uniform vec3 baseColor;
uniform sampler2D baseColorMap;

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
    fragColor = vec4(color, 1.0f);
}
