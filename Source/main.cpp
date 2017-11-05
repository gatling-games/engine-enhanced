#include <iostream>

// GL3W/GLFW
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

#include "Application.h"

int main(int argc, const char* argv[])
{
	// Initialise GLFW library
	if (!glfwInit())
	{
		return -1;
	}

	// Ensure OpenGL4.5 is supported - exit if not
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window and its OpenGL context - exit with failure if window not initialised
    int windowWidth = 1920;
    int windowHeight = 1080;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "hello gaem", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	gl3wInit();

    // Create manager classes
    Application* application = new Application(window, windowWidth, windowHeight);
    
	// Run game loop while window not closed
	while (!glfwWindowShouldClose(window))
	{
        // Poll for GLFW events
        glfwPollEvents();

        // Check if the window has changed size.
        int newWidth;
        int newHeight;
        glfwGetWindowSize(window, &newWidth, &newHeight);
        if (newWidth != windowWidth || newHeight != windowHeight)
        {
            windowWidth = newWidth;
            windowHeight = newHeight;
            application->resize(newWidth, newHeight);
        }

        // Run frame start methods
        application->frameStart();

        // Processing finished. Render a new frame.
        application->drawFrame();

		// Swap front & back framebuffers
        glfwSwapBuffers(window);
	}

    delete application;

	glfwTerminate();
	return 0;
}