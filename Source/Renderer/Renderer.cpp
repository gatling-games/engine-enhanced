#include "Renderer.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>

Renderer::Renderer()
{
}

void Renderer::renderFrame()
{
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}