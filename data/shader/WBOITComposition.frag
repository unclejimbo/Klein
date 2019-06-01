#version 330

out vec4 fragColor;

uniform sampler2D accumMap;
uniform sampler2D revealageMap;

float maxComponent(vec4 v)
{
    return max(max(max(v.x, v.y), v.z), v.w);
}

void main()
{
    ivec2 texCoord = ivec2(gl_FragCoord.xy);
    float revealage = texelFetch(revealageMap, texCoord, 0).r;
    if (revealage == 1.0) { discard; }

    vec4 accum = texelFetch(accumMap, texCoord, 0);
    if (isinf(maxComponent(abs(accum)))) { accum.rgb = vec3(accum.a); }
    vec3 avgColor = accum.rgb / max(accum.a, 1e-5);

    fragColor = vec4(avgColor, 1.0 - revealage);
}
