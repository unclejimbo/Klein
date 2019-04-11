uniform float texCoordScale = 2.0f;
uniform float texCoordOffset = 0.0f;
uniform float shift = 0.0f;

vec4 glPosition(vec3 position, mat4 mvp)
{
    return mvp * vec4(position, 1.0f);
}

mat3 modelNormal(mat4 model)
{
    return mat3(transpose(inverse(model)));
}

vec2 transformTexCoord(vec2 texCoord)
{
    return vec2(texCoordOffset, texCoordOffset) + texCoord * texCoordScale;
}

vec3 transformPosition(vec3 position, mat4 matrix)
{
    return vec3(matrix * vec4(position, 1.0f));
}

vec3 transformNormal(vec3 normal, mat3 matrix)
{
    return normalize(matrix * normal);
}

vec3 normalShift(vec3 position, vec3 normal)
{
    return position + shift * normal;
}
