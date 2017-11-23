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

// Contain the main application code in a class.
Application* application;

//glfw callback
// Triggered when the window is resized.
void windowSizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
    application->resize(newWidth, newHeight);
}

// glfw callback
// Triggered when the window gains or loses focus
void windowFocusCallback(GLFWwindow* window, int focused)
{
    if (focused)
    {
        // Notify the application class when it regains focus
        application->windowFocused();
    }
}

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

    // Register glfw callbacks
    glfwSetWindowSizeCallback(window, &windowSizeCallback);
    glfwSetWindowFocusCallback(window, &windowFocusCallback);

    // Create the main application class.
    application = new Application(window, windowWidth, windowHeight);
    application->resize(windowWidth, windowHeight);
    
	// Run game loop while window not closed
	while (!glfwWindowShouldClose(window))
	{
        // Poll for GLFW events
        glfwPollEvents();

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