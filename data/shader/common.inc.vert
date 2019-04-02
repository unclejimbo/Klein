uniform float texCoordScale = 2.0f;
uniform float texCoordOffset = 0.0f;
uniform float shift = 0.0f;

vec2 transformTexCoord(vec2 texCoord)
{
    return vec2(texCoordOffset, texCoordOffset) + texCoord * texCoordScale;
}

mat3 modelNormal(mat4 model)
{
    return mat3(transpose(inverse(model)));
}

vec3 transformPosition(vec3 position, mat4 matrix)
{
    return vec3(matrix * vec4(position, 1.0f));
}

vec3 transformNormal(vec3 normal, mat3 matrix)
{
    return normalize(matrix * normal);
}

vec4 glPosition(vec3 position, mat4 mvp)
{
    return mvp * vec4(position, 1.0f);
}

vec3 normalShift(vec3 position, vec3 normal)
{
    return position + shift * normal;
}
