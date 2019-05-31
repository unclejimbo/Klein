#version 330

in vec2 texCoord;
in vec3 vcolor;

out vec4 fragColor;

uniform int colorMode = 0;
uniform vec3 baseColor;
uniform sampler2D baseColorMap;

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
    fragColor = vec4(color, 1.0f);
}
