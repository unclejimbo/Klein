#version 430 core

#define MAX_LIGHTS 4

struct Material
{
	float roughness;
	float metallic;
	float ao;
};

const float pi = 3.14159265359;

uniform Material material;
uniform vec3 viewPosition_w;
uniform vec3 lightPosition_w[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform float alpha;

in vec3 vertPosition_w;
in vec3 vertNormal_w;
in vec3 vertColor;

out vec4 fragColor;

// Normal distribution function for surface color
float distributionGGX(vec3 normal, vec3 halfway, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float nDotH = max(dot(normal, halfway), 0.0f);
	float nDotH2 = nDotH * nDotH;

	float denomitor = (nDotH2 * (a2 - 1.0f) + 1.0f);
	denomitor = pi * denomitor * denomitor;

	return a2 / denomitor;
}

// Geometry function for self-shadowing
float geometrySchlickGGX(float angle, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r * r) / 8.0f; // Used for direct lighting

	return angle / (angle * (1.0f - k) + k);
}

// Geometry function combining both lighting and viewing obstruction
float geometrySmith(vec3 normal, vec3 view, vec3 light, float roughness)
{
	float nDotV = max(dot(normal, view), 0.0f);
	float nDotL = max(dot(normal, light), 0.0f);
	float ggxV = geometrySchlickGGX(nDotV, roughness);
	float ggxL = geometrySchlickGGX(nDotL, roughness);

	return ggxV * ggxL;
}

// Fresnel function for fresnel effect
vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
	return f0 + (1.0f - f0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 viewDir_w = normalize(viewPosition_w - vertPosition_w);

	vec3 f0 = vec3(0.04f); // Base reflection
	f0 = mix(f0, vertColor, material.metallic);

	vec3 lo = vec3(0.0f);
	for (int i = 0; i < MAX_LIGHTS; ++i) {
		vec3 lightDir_w = normalize(lightPosition_w[i] - vertPosition_w);
		vec3 halfDir_w = normalize(viewDir_w + lightDir_w);
		float distance = length(lightPosition_w[i] - vertPosition_w);
		float attenuation = 1.0f / (distance * distance);
		vec3 radiance = lightColor[i] * attenuation;

		float ndf = distributionGGX(vertNormal_w, halfDir_w, material.roughness);
		float geometry = geometrySmith(vertNormal_w, viewDir_w, lightDir_w, material.roughness);
		vec3 fresnel = fresnelSchlick(max(dot(halfDir_w, viewDir_w), 0.0f), f0);

		vec3 ks = fresnel;
		vec3 kd = vec3(1.0f) - ks;
		kd *= 1.0f - material.metallic;

		vec3 nominator = ndf * geometry * fresnel;
		float denominator = 4 * max(dot(vertNormal_w, viewDir_w), 0.0f) *
			max(dot(vertNormal_w, lightDir_w), 0.0f) + 0.001f; // Prevent dividing by 0
		vec3 brdf = nominator / denominator;

		// ks is left out because it equals to fresnel and already appears in the brdf
		lo += (kd * vertColor / pi + brdf) * radiance * max(dot(vertNormal_w, lightDir_w), 0.0f);
	}

	vec3 ambient = vec3(0.03f) * vertColor * material.ao; // Add some ambient lighting

	vec3 color = lo + ambient;

	color = color / (color + vec3(1.0f)); // HDR tone mapping
	color = pow(color, vec3(1.0f / 2.2f)); // Gamma correction

	fragColor = vec4(color, alpha);
}