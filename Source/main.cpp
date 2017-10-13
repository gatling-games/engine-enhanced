#include <iostream>
#include <GLFW/glfw3.h>

GLFWwindow* window;

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

	// Create a window and its OpenGL context - exit with failure if window not initialised
	window = glfwCreateWindow(1280, 720, "hello gaem", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make window's context current
	glfwMakeContextCurrent(window);

    printf("Initializing OpenGL %s \n", glGetString(GL_VERSION));

	// Run game loop while window not closed
	while (!glfwWindowShouldClose(window))
	{
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

		// Swap front & back framebuffers
        glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}