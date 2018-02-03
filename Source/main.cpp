
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <exception>
#include <iostream>

#include "Application.h"

// Contain the main application code in a class.
Application* application;

// glfw callback
// Triggered when the window is resized.
void windowSizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
    application->resize(newWidth, newHeight);
}

// glfw callback
// Triggered when the window gains or loses focus
void windowFocusCallback(GLFWwindow* window, int focused)
{
    if (focused && application != nullptr)
    {
        // Notify the application class when it regains focus
        application->windowFocused();
    }
}

// opengl callback
// Implementation of glDebugMessageCallback 
void glErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const char* message, const void* userParam)
{
    // Ignore callbacks with a severity of "notification"
    // from the gl spec: "Anything that isn't an error or performance issue."
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        return;
    }

#ifdef NDEBUG
    // Release mode - dont throw exceptions, just print an error.
    printf("GLError: %s\n", message);
#else
    // Debug mode - throw an exception.
    throw std::exception(("GLError: " + std::string(message)).c_str());
#endif
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
    
    // We use linear space rendering, so ensure the backbuffer is srgb
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

#ifndef NDEBUG
    // Create an opengl debug context when *not* in release mode.
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    // Create a window and its OpenGL context - exit with failure if window not initialised
    int windowWidth = 1920;
    int windowHeight = 1080;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Gatling Engine - Cardboard Copters", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gl3wInit();

    // Register a callback for opengl errors.
    glDebugMessageCallback(&glErrorCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT); // Required to fire error callbacks

    // Register glfw callbacks
    glfwSetWindowSizeCallback(window, &windowSizeCallback);
    glfwSetWindowFocusCallback(window, &windowFocusCallback);

    // Create the main application class.
    application = new Application(window);
    application->resize(windowWidth, windowHeight);

    // Run game loop while window not closed
    while (!glfwWindowShouldClose(window) && application->running())
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