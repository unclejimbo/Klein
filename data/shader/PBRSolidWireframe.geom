#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 worldPosition[];
in vec3 worldNormal[];
in vec3 eyePosition[];
in vec3 eyeNormal[];
in vec2 texCoord[];
in vec3 vcolor[];

out WireframeVertex
{
    vec3 worldPosition;
    vec3 worldNormal;
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    vec3 color;
    noperspective vec4 edgeA;
    noperspective vec4 edgeB;
    flat int configuration;
}
gs_out;

#pragma include SolidWireframe.inc.geom

void main()
{
    SolidWireframeData data[3];
    fillWireframeData(data);

    for (int i = 0; i < 3; ++i) {
        gs_out.edgeA = data[i].edgeA;
        gs_out.edgeB = data[i].edgeB;
        gs_out.configuration = data[i].configuration;
        gs_out.normal = eyeNormal[i];
        gs_out.position = eyePosition[i];
        gs_out.texCoord = texCoord[i];
        gs_out.color = vcolor[i];
        gs_out.worldPosition = worldPosition[i];
        gs_out.worldNormal = worldNormal[i];
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
