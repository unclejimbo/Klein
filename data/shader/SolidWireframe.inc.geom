const int infoA[] = int[](0, 0, 0, 0, 1, 1, 2);
const int infoB[] = int[](1, 1, 2, 0, 2, 1, 2);
const int infoAd[] = int[](2, 2, 1, 1, 0, 0, 0);
const int infoBd[] = int[](2, 2, 1, 2, 0, 2, 1);

uniform mat4 viewportMatrix;

vec2 transformToViewport(const in vec4 p)
{
    return vec2(viewportMatrix * (p / p.w));
};

struct SolidWireframeData
{
    vec4 edgeA;
    vec4 edgeB;
    int configuration;
};

void fillWireframeData(out SolidWireframeData[3] data)
{
    int configuration = int(gl_in[0].gl_Position.z < 0) * int(4) +
                        int(gl_in[1].gl_Position.z < 0) * int(2) +
                        int(gl_in[2].gl_Position.z < 0);

    // If all vertices are behind us, cull the primitive
    if (configuration == 7) return;

    // Transform each vertex into viewport space
    vec2 p[3];
    p[0] = transformToViewport(gl_in[0].gl_Position);
    p[1] = transformToViewport(gl_in[1].gl_Position);
    p[2] = transformToViewport(gl_in[2].gl_Position);

    if (configuration == 0) {
        // Common configuration where all vertices are within the viewport
        gs_out.edgeA = vec4(0.0);
        gs_out.edgeB = vec4(0.0);

        // Calculate lengths of 3 edges of triangle
        float a = length(p[1] - p[2]);
        float b = length(p[2] - p[0]);
        float c = length(p[1] - p[0]);

        // Calculate internal angles using the cosine rule
        float alpha = acos((b * b + c * c - a * a) / (2.0 * b * c));
        float beta = acos((a * a + c * c - b * b) / (2.0 * a * c));

        // Calculate the perpendicular distance of each vertex from the opposing
        // edge
        float ha = abs(c * sin(beta));
        float hb = abs(c * sin(alpha));
        float hc = abs(b * sin(alpha));

        // Fill data
        data[0].configuration = configuration;
        data[0].edgeA = vec4(ha, 0.0, 0.0, 0.0);
        data[1].configuration = configuration;
        data[1].edgeA = vec4(0.0, hb, 0.0, 0.0);
        data[2].configuration = configuration;
        data[2].edgeA = vec4(0.0, 0.0, hc, 0.0);
    }
    else {
        // Viewport projection breaks down for one or two vertices.
        // Caclulate what we can here and defer rest to fragment shader.
        // Since this is coherent for the entire primitive the conditional
        // in the fragment shader is still cheap as all concurrent
        // fragment shader invocations will take the same code path.
        for (int i = 0; i < 3; ++i) {
            // Copy across the viewport-space points for the (up to) two
            // vertices in the viewport
            data[i].edgeA.xy = p[infoA[gs_out.configuration]];
            data[i].edgeB.xy = p[infoB[gs_out.configuration]];

            // Copy across the viewport-space edge vectors for the (up to) two
            // vertices in the viewport
            data[i].edgeA.zw =
                normalize(gs_out.edgeA.xy - p[infoAd[gs_out.configuration]]);
            data[i].edgeB.zw =
                normalize(gs_out.edgeB.xy - p[infoBd[gs_out.configuration]]);
        }
    }
}
