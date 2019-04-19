const int MAX_LIGHTS = 8;
const int TYPE_POINT = 0;
const int TYPE_DIRECTIONAL = 1;
const int TYPE_SPOT = 2;
const int MAX_POISSON_SAMPLES = 64;
const vec2 poissonDisk[64] = vec2[64](vec2(-0.04117257f, -0.1597612f),
                                      vec2(0.06731031f, -0.4353096f),
                                      vec2(-0.206701f, -0.4089882f),
                                      vec2(0.1857469f, -0.2327659f),
                                      vec2(-0.2757695f, -0.159873f),
                                      vec2(-0.2301117f, 0.1232693f),
                                      vec2(0.05028719f, 0.1034883f),
                                      vec2(0.236303f, 0.03379251f),
                                      vec2(0.1467563f, 0.364028f),
                                      vec2(0.516759f, 0.2052845f),
                                      vec2(0.2962668f, 0.2430771f),
                                      vec2(0.3650614f, -0.1689287f),
                                      vec2(0.5764466f, -0.07092822f),
                                      vec2(-0.5563748f, -0.4662297f),
                                      vec2(-0.3765517f, -0.5552908f),
                                      vec2(-0.4642121f, -0.157941f),
                                      vec2(-0.2322291f, -0.7013807f),
                                      vec2(-0.05415121f, -0.6379291f),
                                      vec2(-0.7140947f, -0.6341782f),
                                      vec2(-0.4819134f, -0.7250231f),
                                      vec2(-0.7627537f, -0.3445934f),
                                      vec2(-0.7032605f, -0.13733f),
                                      vec2(0.8593938f, 0.3171682f),
                                      vec2(0.5223953f, 0.5575764f),
                                      vec2(0.7710021f, 0.1543127f),
                                      vec2(0.6919019f, 0.4536686f),
                                      vec2(0.3192437f, 0.4512939f),
                                      vec2(0.1861187f, 0.595188f),
                                      vec2(0.6516209f, -0.3997115f),
                                      vec2(0.8065675f, -0.1330092f),
                                      vec2(0.3163648f, 0.7357415f),
                                      vec2(0.5485036f, 0.8288581f),
                                      vec2(-0.2023022f, -0.9551743f),
                                      vec2(0.165668f, -0.6428169f),
                                      vec2(0.2866438f, -0.5012833f),
                                      vec2(-0.5582264f, 0.2904861f),
                                      vec2(-0.2522391f, 0.401359f),
                                      vec2(-0.428396f, 0.1072979f),
                                      vec2(-0.06261792f, 0.3012581f),
                                      vec2(0.08908027f, -0.8632499f),
                                      vec2(0.9636437f, 0.05915006f),
                                      vec2(0.8639213f, -0.309005f),
                                      vec2(-0.03422072f, 0.6843638f),
                                      vec2(-0.3734946f, -0.8823979f),
                                      vec2(-0.3939881f, 0.6955767f),
                                      vec2(-0.4499089f, 0.4563405f),
                                      vec2(0.07500362f, 0.9114207f),
                                      vec2(-0.9658601f, -0.1423837f),
                                      vec2(-0.7199838f, 0.4981934f),
                                      vec2(-0.8982374f, 0.2422346f),
                                      vec2(-0.8048639f, 0.01885651f),
                                      vec2(-0.8975322f, 0.4377489f),
                                      vec2(-0.7135055f, 0.1895568f),
                                      vec2(0.4507209f, -0.3764598f),
                                      vec2(-0.395958f, -0.3309633f),
                                      vec2(-0.6084799f, 0.02532744f),
                                      vec2(-0.2037191f, 0.5817568f),
                                      vec2(0.4493394f, -0.6441184f),
                                      vec2(0.3147424f, -0.7852007f),
                                      vec2(-0.5738106f, 0.6372389f),
                                      vec2(0.5161195f, -0.8321754f),
                                      vec2(0.6553722f, -0.6201068f),
                                      vec2(-0.2554315f, 0.8326268f),
                                      vec2(-0.5080366f, 0.8539945f));

struct Light
{
    int type;
    vec3 position;
    vec3 color;
    float intensity;
    vec3 direction;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float cutOffAngle;
};
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

struct EnvironmentLight
{
    samplerCube irradiance;
    samplerCube specular;
    sampler2D brdf;
    float intensity;
};
uniform EnvironmentLight envLight;

uniform sampler2D shadowMap;
uniform vec3 lightDir;
uniform bool receiveShadow;

float rand(vec4 seed)
{
    float d = dot(seed, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(d) * 43758.5453);
}

float shadowMapping_PCF(vec4 lightSpacePosition, vec3 worldNormal)
{
    if (!receiveShadow) { return 0.0; }
    else {
        // convert to texcoords
        vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
        projCoords = projCoords * 0.5 + 0.5;

        // fragment depth with bias to avoid artifacts
        float fragDepth = projCoords.z;
        float bias = max(0.05 * (1.0 - dot(worldNormal, lightDir)), 0.005);
        fragDepth -= bias;

        // random rotation to sample poisson disk
        float theta = rand(vec4(projCoords.xy, gl_FragCoord.xy));
        mat2 rotation =
            mat2(vec2(cos(theta), sin(theta)), vec2(-sin(theta), cos(theta)));

        float shadow = 0.0;
        vec2 filterSize = 3.0 / textureSize(shadowMap, 0);
        for (int i = 0; i < MAX_POISSON_SAMPLES; ++i) {
            vec2 offset = (rotation * poissonDisk[i]) * filterSize;
            vec2 texCoords = projCoords.xy + offset;
            float pcfDepth = texture(shadowMap, texCoords).r;
            shadow += fragDepth > pcfDepth ? 1.0 : 0.0;
        }
        shadow /= MAX_POISSON_SAMPLES;

        if (projCoords.z > 1.0) { shadow = 0.0; }

        return shadow;
    }
}
