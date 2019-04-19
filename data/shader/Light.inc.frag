const int MAX_LIGHTS = 8;
const int TYPE_POINT = 0;
const int TYPE_DIRECTIONAL = 1;
const int TYPE_SPOT = 2;

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

        float shadow = 0.0;
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                float pcfDepth =
                    texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize)
                        .r;
                shadow += fragDepth > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0;

        if (projCoords.z > 1.0) { shadow = 0.0; }

        return shadow;
    }
}
