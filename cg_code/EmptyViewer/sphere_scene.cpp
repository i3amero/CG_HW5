#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int     gNumVertices = 0;    // Number of 3D vertices.
static int     gNumTriangles = 0;    // Number of triangles.t
static int* gIndexBuffer = NULL; // Vertex indices for the triangles.
static float* gVertexBuffer = NULL; // Vertex positions (x, y, z triples).

static void create_scene()
{
    int width = 32;
    int height = 16;

    float theta, phi;
    int t;

    gNumVertices = (height - 2) * width + 2;    // inner vertices + 2 poles
    gNumTriangles = (height - 3) * (width - 1) * 2 + 2 * (width - 1); // middle + top/bottom caps

    // Allocate arrays
    gVertexBuffer = new float[3 * gNumVertices]; // x, y, z per vertex
    gIndexBuffer = new int[3 * gNumTriangles];  // 3 indices per triangle

    t = 0;
    // Generate inner vertices (excluding poles)
    for (int j = 1; j < height - 1; ++j)
    {
        theta = (float)j / (height - 1) * M_PI;
        for (int i = 0; i < width; ++i)
        {
            phi = (float)i / (width - 1) * M_PI * 2;

            float x = sinf(theta) * cosf(phi);
            float y = cosf(theta);
            float z = -sinf(theta) * sinf(phi);

            gVertexBuffer[3 * t + 0] = x;
            gVertexBuffer[3 * t + 1] = y;
            gVertexBuffer[3 * t + 2] = z;

            t++;
        }
    }

    // Top pole
    gVertexBuffer[3 * t + 0] = 0.0f;
    gVertexBuffer[3 * t + 1] = 1.0f;
    gVertexBuffer[3 * t + 2] = 0.0f;
    int topPoleIndex = t;
    t++;

    // Bottom pole
    gVertexBuffer[3 * t + 0] = 0.0f;
    gVertexBuffer[3 * t + 1] = -1.0f;
    gVertexBuffer[3 * t + 2] = 0.0f;
    int bottomPoleIndex = t;
    t++;

    // Generate middle triangles
    t = 0;
    for (int j = 0; j < height - 3; ++j)
    {
        for (int i = 0; i < width - 1; ++i)
        {
            int k0 = j * width + i;
            int k1 = (j + 1) * width + i + 1;
            int k2 = j * width + i + 1;
            int k3 = j * width + i;
            int k4 = (j + 1) * width + i;
            int k5 = (j + 1) * width + i + 1;

            gIndexBuffer[t++] = k0;
            gIndexBuffer[t++] = k1;
            gIndexBuffer[t++] = k2;

            gIndexBuffer[t++] = k3;
            gIndexBuffer[t++] = k4;
            gIndexBuffer[t++] = k5;
        }
    }

    // Generate top and bottom cap triangles
    for (int i = 0; i < width - 1; ++i)
    {
        // Top cap
        gIndexBuffer[t++] = topPoleIndex;
        gIndexBuffer[t++] = i;
        gIndexBuffer[t++] = i + 1;

        // Bottom cap
        gIndexBuffer[t++] = bottomPoleIndex;
        gIndexBuffer[t++] = (height - 3) * width + i + 1;
        gIndexBuffer[t++] = (height - 3) * width + i;
    }
}
