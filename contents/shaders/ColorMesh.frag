#version 430 core

#define MAX_LIGHTS 4

uniform vec3 lightPosition_w[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform vec3 color;
uniform float alpha;

in vec3 vertPosition_w;
in vec3 vertNormal_w;

out vec4 fragColor;

void main()
{
	const float ambientStrength = 0.003f;
	const float diffuseStrength = 0.007f;
	vec3 result = vec3(0.0f);

	for (int i = 0; i < MAX_LIGHTS; ++i) {
		vec3 ambient = lightColor[i] * ambientStrength;

		vec3 lightDir = normalize(lightPosition_w[i] - vertPosition_w);
		float diff = max(dot(vertNormal_w, lightDir), 0.0f) * diffuseStrength;
		vec3 diffuse = lightColor[i] * (diff * color);

		result += ambient + diffuse;
	}

	result = result / (result + vec3(1.0f));
	fragColor = vec4(result, alpha);
}
