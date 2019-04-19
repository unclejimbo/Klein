// Qt 3D built in uniforms
uniform vec3 eyePosition; // World space eye position

// PBR Material
uniform float roughness;
uniform float metalness;

// Exposure correction
uniform float exposure = 0.0;

// Gamma correction
uniform float gamma = 2.2;

// Constants
#define PI 3.1415926538

int mipLevelCount(const in samplerCube cube)
{
    int baseSize = textureSize(cube, 0).x;
    int nMips = int(log2(float(baseSize > 0 ? baseSize : 1))) + 1;
    return nMips;
}

float remapRoughness(const in float roughness)
{
    // As per page 14 of
    // http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
    // we remap the roughness to give a more perceptually linear response
    // of "bluriness" as a function of the roughness specified by the user.
    // r = roughness^2
    const float maxSpecPower = 999999.0;
    const float minRoughness = sqrt(2.0 / (maxSpecPower + 2));
    return max(roughness * roughness, minRoughness);
}

float alphaToMipLevel(float alpha)
{
    float specPower = 2.0 / (alpha * alpha) - 2.0;

    // We use the mip level calculation from Lys' default power drop, which in
    // turn is a slight modification of that used in Marmoset Toolbag. See
    // https://docs.knaldtech.com/doku.php?id=specular_lys for details.
    // For now we assume a max specular power of 999999 which gives
    // maxGlossiness = 1.
    const float k0 = 0.00098;
    const float k1 = 0.9921;
    float glossiness = (pow(2.0, -10.0 / sqrt(specPower)) - k0) / k1;

    // TODO: Optimize by doing this on CPU and set as
    // uniform int envLight.specularMipLevels say (if present in shader).
    // Lookup the number of mips in the specular envmap
    int mipLevels = mipLevelCount(envLight.specular);

    // Offset of smallest miplevel we should use (corresponds to specular
    // power of 1). I.e. in the 32x32 sized mip.
    const float mipOffset = 3.0;

    // The final factor is really 1 - g / g_max but as mentioned above g_max
    // is 1 by definition here so we can avoid the division. If we make the
    // max specular power for the spec map configurable, this will need to
    // be handled properly.
    float mipLevel = (mipLevels - 1.0 - mipOffset) * (1.0 - glossiness);
    return mipLevel;
}

float D_GGX(const in vec3 n, const in vec3 h, const in float alpha)
{
    // Normal Distribution Function - GGX (Trobridge-Reitz)
    float nDotH = max(dot(n, h), 0.0);
    float a = nDotH * alpha;
    float k = alpha / (1.0 - nDotH * nDotH + a * a);
    return k * k * (1.0 / PI);
}

float G_SmithGGX(const in float lDotN,
                 const in float vDotN,
                 const in float alpha)
{
    // Geometric Shadowing - Smith GGX
    // Optimized with the denominator of the BRDF
    float a2 = alpha * alpha;
    float G1 = lDotN * sqrt(vDotN * vDotN * (1.0 - a2) + a2);
    float G2 = vDotN * sqrt(lDotN * lDotN * (1.0 - a2) + a2);
    return 0.5 / (G1 + G2);
}

vec3 F_Schlick(const in vec3 F0, const in float cosineFactor)
{
    // Fresnel - Schlick
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosineFactor, 5.0);
}

vec3 F_SchlickRoughness(const in vec3 F0,
                        const in float cosineFactor,
                        const in float alpha)
{
    // Fresnel - Schlick
    return F0 +
           (max(vec3(1.0 - alpha), F0) - F0) * pow(1.0 - cosineFactor, 5.0);
}

vec3 pbrModel(const in int lightIndex,
              const in vec3 wPosition,
              const in vec3 wNormal,
              const in vec3 wView,
              const in vec3 baseColor,
              const in float metalness,
              const in float alpha,
              const in vec3 F0)
{
    // Calculate some useful quantities
    vec3 n = wNormal;
    vec3 s = vec3(0.0);
    vec3 v = wView;
    vec3 h = vec3(0.0);

    float vDotN = dot(v, n);
    float sDotN = 0.0;
    float sDotH = 0.0;
    float att = 1.0;

    if (lights[lightIndex].type != TYPE_DIRECTIONAL) {
        // Point and Spot lights
        vec3 sUnnormalized = vec3(lights[lightIndex].position) - wPosition;
        s = normalize(sUnnormalized);

        // Calculate the attenuation factor
        sDotN = dot(s, n);
        if (sDotN > 0.0) {
            if (lights[lightIndex].constantAttenuation != 0.0 ||
                lights[lightIndex].linearAttenuation != 0.0 ||
                lights[lightIndex].quadraticAttenuation != 0.0) {
                float dist = length(sUnnormalized);
                att = 1.0 /
                      (lights[lightIndex].constantAttenuation +
                       lights[lightIndex].linearAttenuation * dist +
                       lights[lightIndex].quadraticAttenuation * dist * dist);
            }

            // The light direction is in world space already
            if (lights[lightIndex].type == TYPE_SPOT) {
                // Check if fragment is inside or outside of the spot light cone
                if (degrees(acos(dot(-s, lights[lightIndex].direction))) >
                    lights[lightIndex].cutOffAngle)
                    sDotN = 0.0;
            }
        }
    }
    else {
        // Directional lights
        // The light direction is in world space already
        s = normalize(-lights[lightIndex].direction);
        sDotN = dot(s, n);
    }

    sDotN = max(sDotN, 0.0);
    h = normalize(s + v);
    sDotH = max(dot(s, h), 0.0);

    vec3 diffuseColor = (1.0 - metalness) * baseColor;
    vec3 diffuse = diffuseColor / PI;

    float D = 0.0;
    float G = 0.0;
    vec3 F = vec3(0.0);
    if (sDotN > 0.0) {
        D = D_GGX(n, h, alpha);
        G = G_SmithGGX(sDotN, vDotN, alpha);
        F = F_Schlick(F0, sDotH);
    }
    vec3 specular = D * G * F;

    // Blend between diffuse and specular to conserver energy
    // Use Fresnel term as the fraction of specular reflectance
    vec3 radiance =
        lights[lightIndex].intensity * lights[lightIndex].color * att;
    vec3 irradiance = radiance * sDotN * (specular + diffuse * (vec3(1.0) - F));

    return irradiance;
}

vec3 pbrIblModel(const in vec3 wNormal,
                 const in vec3 wView,
                 const in vec3 baseColor,
                 const in float metalness,
                 const in float alpha,
                 const in vec3 F0)
{
    vec3 n = wNormal;
    vec3 l = reflect(-wView, n);
    vec3 v = wView;
    float vDotN = max(dot(v, n), 0.0);

    vec3 diffuseColor = (1.0 - metalness) * baseColor;
    vec3 diffuse = diffuseColor * texture(envLight.irradiance, n).rgb;

    float lod = alphaToMipLevel(alpha);
    vec3 specularColor = textureLod(envLight.specular, l, lod).rgb;
    vec2 envBRDF = texture(envLight.brdf, vec2(vDotN, alpha)).rg;
    vec3 F = F_SchlickRoughness(F0, vDotN, alpha);
    vec3 specular = specularColor * (F * envBRDF.x + envBRDF.y);

    // Blend between diffuse and specular to conserve energy
    // Use Fresnel term as the fraction of specular reflectance
    vec3 irradiance =
        envLight.intensity * (specular + diffuse * (vec3(1.0) - F));

    return irradiance;
}

vec3 toneMap_ACES(const in vec3 color)
{
    // Filmic tone mapping curve
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return (color * (a * color + b)) / (color * (c * color + d) + e);
}

vec3 gammaCorrect(const in vec3 color)
{
    return pow(color, vec3(1.0 / gamma));
}

vec3 shadeMetalRough(vec3 worldPosition,
                     vec3 worldNormal,
                     vec4 lightSpacePosition,
                     vec3 baseColor)
{
    vec3 cLinear = vec3(0.0);

    vec3 worldView = normalize(eyePosition - worldPosition);

    // Remap roughness for a perceptually more linear correspondence
    float alpha = remapRoughness(roughness);

    // Approaximate F0 using dielectric and metalness
    vec3 dielectricColor = vec3(0.04);
    vec3 F0 = mix(dielectricColor, baseColor, metalness);

    // Image-based lighting
    cLinear +=
        pbrIblModel(worldNormal, worldView, baseColor, metalness, alpha, F0);

    // Punctual lighting
    for (int i = 0; i < lightCount; ++i) {
        cLinear += (1.0 - shadowMapping_PCF(lightSpacePosition, worldNormal)) *
                   pbrModel(i,
                            worldPosition,
                            worldNormal,
                            worldView,
                            baseColor.rgb,
                            metalness,
                            alpha,
                            F0);
    }

    // Apply exposure correction
    cLinear *= pow(2.0, exposure);

    // Apply simple (Reinhard) tonemap transform to get into LDR range [0, 1]
    vec3 cToneMapped = toneMap_ACES(cLinear);

    // Apply gamma correction prior to display
    vec3 cGamma = gammaCorrect(cToneMapped);

    return cGamma;
}
