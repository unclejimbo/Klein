// Qt 3D built in uniforms
uniform vec3 eyePosition; // World space eye position

// Exposure correction
uniform float exposure = 0.0;

// Gamma correction
uniform float gamma = 2.2;

// F0 of the Fresnel for dielectric materials, should be remapped before use
uniform float specular = 0.5;

// Clearcoat strength and roughness
uniform float clearcoat = 0.0f;
uniform float clearcoatRoughness = 0.0f;

// Constants
#define PI 3.1415926538

float remapRoughness(const in float roughness)
{
    // As per page 14 of
    // http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
    // we remap the roughness to give a more perceptually linear response
    // of "bluriness" as a function of the roughness specified by the user.
    // r = roughness^2
    return roughness * roughness;
}

float remapSpecular(const in float specular)
{
    // Perceptual linear specular response, see also frosbite's note
    return 0.16 * specular * specular;
}

float alphaToMipLevel(float roughness)
{
    // Using roughness as suggested by frosbite's note (eq.63)
    return (envLight.mipLevels - 1.0 - envLight.mipOffset) * roughness;
}

float D_GGX(const in float nDotH, const in float alpha)
{
    // Normal Distribution Function - GGX (Trobridge-Reitz)
    float a = nDotH * alpha;
    float k = alpha / (1.0 - nDotH * nDotH + a * a + 1e-8);
    return k * k * (1.0 / PI);
}

float G_SmithGGX(const in float lDotN,
                 const in float vDotN,
                 const in float alpha)
{
    // Geometric Shadowing - Smith GGX
    // Optimized with the denominator of the BRDF
    float a2 = alpha * alpha;
    float V1 = lDotN * sqrt(vDotN * vDotN * (1.0 - a2) + a2);
    float V2 = vDotN * sqrt(lDotN * lDotN * (1.0 - a2) + a2);
    return 0.5 / (V1 + V2 + 1e-8);
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
    // Prefiltered Fresnel term is factored according to
    // https://seblagarde.wordpress.com/2011/08/17/hello-world/
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

    float vDotN = max(dot(v, n), 0.0);
    float nDotH = max(dot(n, h), 0.0);
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
        D = D_GGX(nDotH, alpha);
        G = G_SmithGGX(sDotN, vDotN, alpha);
        F = F_Schlick(F0, sDotH);
    }
    vec3 specular = D * G * F;

    // Clearcoat layer
    float Dc = 0.0;
    float Gc = 0.0;
    vec3 Fc = vec3(0.0);
    float alphac = clearcoatRoughness * clearcoatRoughness;
    if (sDotN > 0.0) {
        Dc = D_GGX(nDotH, alphac);
        Gc = G_SmithGGX(sDotN, vDotN, alphac);
        Fc = F_Schlick(vec3(0.04), sDotH);
    }
    vec3 specularc = Dc * Gc * Fc * clearcoat;

    // Blend between diffuse and specular to conserver energy
    // Use Fresnel term as the fraction of specular reflectance
    vec3 radiance =
        lights[lightIndex].intensity * lights[lightIndex].color * att;
    vec3 irradiance =
        radiance * sDotN *
        ((specular + diffuse * (vec3(1.0) - F)) * (vec3(1.0) - Fc) + specularc);
    return irradiance;
}

vec3 pbrIblModel(const in vec3 wNormal,
                 const in vec3 wView,
                 const in vec3 baseColor,
                 const in float metalness,
                 const in float roughness,
                 const in float alpha,
                 const in vec3 F0)
{
    vec3 n = wNormal;
    vec3 l = reflect(-wView, n);
    vec3 v = wView;
    float vDotN = max(dot(v, n), 0.0);

    vec3 diffuseColor = (1.0 - metalness) * baseColor;
    vec3 diffuse = diffuseColor * texture(envLight.irradiance, n).rgb;

    float lod = alphaToMipLevel(roughness);
    vec3 specularColor = textureLod(envLight.specular, l, lod).rgb;
    // Use (1 - alpha) as the coordinate because Qt loads dds image without
    // flipping, so the brdf lut image is upside down
    vec2 envBRDF = texture(envLight.brdf, vec2(vDotN, 1.0 - alpha)).rg;
    vec3 F = F_SchlickRoughness(F0, vDotN, alpha);
    vec3 specular = specularColor * (F * envBRDF.x + envBRDF.y);

    // Clearcoat layer
    float alphaCoat = clearcoatRoughness * clearcoatRoughness;
    float lodCoat = alphaToMipLevel(alphaCoat);
    vec3 specularColorCoat = textureLod(envLight.specular, l, lodCoat).rgb;
    vec2 envBRDFCoat = texture(envLight.brdf, vec2(vDotN, 1.0 - alphaCoat)).rg;
    vec3 FCoat = F_SchlickRoughness(vec3(0.04), vDotN, alphaCoat);
    vec3 specularCoat =
        specularColorCoat * (FCoat * envBRDF.x + envBRDF.y) * clearcoat;

    // Blend between diffuse and specular to conserve energy
    // Use Fresnel term as the fraction of specular reflectance
    vec3 irradiance =
        envLight.intensity *
        ((specular + diffuse * (vec3(1.0) - F)) * (vec3(1.0) - FCoat) +
         specularCoat);
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
                     vec3 baseColor,
                     float roughness,
                     float metalness)
{
    vec3 cLinear = vec3(0.0);

    vec3 worldView = normalize(eyePosition - worldPosition);

    // Remap roughness for a perceptually more linear correspondence
    float alpha = remapRoughness(roughness);

    // Approaximate F0 using dielectric and metalness
    vec3 dielectricColor = vec3(remapSpecular(specular));
    vec3 F0 = mix(dielectricColor, baseColor, metalness);

    // Image-based lighting
    cLinear += pbrIblModel(
        worldNormal, worldView, baseColor, metalness, roughness, alpha, F0);

    // Punctual lighting
    for (int i = 0; i < lightCount; ++i) {
        cLinear += shadowMapping_PCSS(lightSpacePosition, worldNormal) *
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

    // Apply tonemap transform to get into LDR range [0, 1]
    vec3 cToneMapped = toneMap_ACES(cLinear);

    // Apply gamma correction prior to display
    vec3 cGamma = gammaCorrect(cToneMapped);

    return cGamma;
}
