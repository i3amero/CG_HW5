#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <vector>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

// -------------------------------------------------
// Global Variables
// -------------------------------------------------
int Width = 512;
int Height = 512;
std::vector<float> OutputImage;
// -------------------------------------------------

std::vector<float> DepthBuffer;

#include "sphere_scene.cpp"
#include <limits>


void render()
{
	OutputImage.clear();
	OutputImage.resize(Width * Height * 3, 0.5f); // gray background
	DepthBuffer.resize(Width * Height, std::numeric_limits<float>::infinity());

	create_scene(); // call to fill gVertexBuffer, gIndexBuffer

	// --- Setup transforms ---
	glm::mat4 model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)), glm::vec3(0, 0, -7));
	glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	glm::mat4 proj = glm::frustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 1000.0f);

	glm::mat4 mvp = proj * view * model;

	// --- Transform vertices ---
	std::vector<glm::vec4> screenVertices(gNumVertices);
	for (int i = 0; i < gNumVertices; ++i) {
		glm::vec4 v(gVertexBuffer[3 * i + 0], gVertexBuffer[3 * i + 1], gVertexBuffer[3 * i + 2], 1.0f);
		glm::vec4 clip = mvp * v;
		clip /= clip.w; // perspective divide
		int sx = (int)((clip.x * 0.5f + 0.5f) * Width);
		int sy = (int)((1.0f - (clip.y * 0.5f + 0.5f)) * Height);
		float sz = clip.z;
		screenVertices[i] = glm::vec4(sx, sy, sz, 1.0f);
	}

	// --- Rasterize triangles ---
	for (int i = 0; i < gNumTriangles; ++i) {
		int k0 = gIndexBuffer[3 * i + 0];
		int k1 = gIndexBuffer[3 * i + 1];
		int k2 = gIndexBuffer[3 * i + 2];

		glm::vec4 v0 = screenVertices[k0];
		glm::vec4 v1 = screenVertices[k1];
		glm::vec4 v2 = screenVertices[k2];

		// bounding box
		int minX = std::max(0, (int)std::floor(std::min({ v0.x, v1.x, v2.x })));
		int maxX = std::min(Width - 1, (int)std::ceil(std::max({ v0.x, v1.x, v2.x })));
		int minY = std::max(0, (int)std::floor(std::min({ v0.y, v1.y, v2.y })));
		int maxY = std::min(Height - 1, (int)std::ceil(std::max({ v0.y, v1.y, v2.y })));

		float area = (v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y);
		if (std::abs(area) < 1e-6f) continue;

		// per pixel
		for (int y = minY; y <= maxY; ++y) {
			for (int x = minX; x <= maxX; ++x) {
				float w0 = ((v1.x - v0.x) * (y - v0.y) - (v1.y - v0.y) * (x - v0.x)) / area;
				float w1 = ((v2.x - v1.x) * (y - v1.y) - (v2.y - v1.y) * (x - v1.x)) / area;
				float w2 = ((v0.x - v2.x) * (y - v2.y) - (v0.y - v2.y) * (x - v2.x)) / area;

				if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
					float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;
					int idx = (Height - 1 - y) * Width + x;
					if (z < DepthBuffer[idx]) {
						DepthBuffer[idx] = z;
						OutputImage[idx * 3 + 0] = 1.0f; // red
						OutputImage[idx * 3 + 1] = 1.0f; // green
						OutputImage[idx * 3 + 2] = 1.0f; // blue
					}
				}
			}
		}
	}
}



void resize_callback(GLFWwindow*, int nw, int nh) 
{
	//This is called in response to the window resizing.
	//The new width and height are passed in so we make 
	//any necessary changes:
	Width = nw;
	Height = nh;
	//Tell the viewport to use all of our screen estate
	glViewport(0, 0, nw, nh);

	//This is not necessary, we're just working in 2d so
	//why not let our spaces reflect it?
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, static_cast<double>(Width)
		, 0.0, static_cast<double>(Height)
		, 1.0, -1.0);

	//Reserve memory for our render so that we don't do 
	//excessive allocations and render the image
	OutputImage.assign(Width * Height * 3, 0.5f);
	DepthBuffer.assign(Width * Height, std::numeric_limits<float>::infinity());

	render();
}


int main(int argc, char* argv[])
{
	// -------------------------------------------------
	// Initialize Window
	// -------------------------------------------------

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(Width, Height, "OpenGL Viewer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//We have an opengl context now. Everything from here on out 
	//is just managing our window or opengl directly.

	//Tell the opengl state machine we don't want it to make 
	//any assumptions about how pixels are aligned in memory 
	//during transfers between host and device (like glDrawPixels(...) )
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	//We call our resize function once to set everything up initially
	//after registering it as a callback with glfw
	glfwSetFramebufferSizeCallback(window, resize_callback);
	resize_callback(NULL, Width, Height);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// -------------------------------------------------------------
		//Rendering begins!
		glDrawPixels(Width, Height, GL_RGB, GL_FLOAT, &OutputImage[0]);
		//and ends.
		// -------------------------------------------------------------

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		//Close when the user hits 'q' or escape
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
			|| glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
