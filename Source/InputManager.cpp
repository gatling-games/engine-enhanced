#include "InputManager.h"
#include "SceneManager.h"

#include <GLFW/glfw3.h>

#include "Utils\Clock.h"

InputManager::InputManager(GLFWwindow* window)
    : ignoringInput_(false)
{
    // Set reference to window
    window_ = window;

    //Initialise arrays for holding joypad input states
    joystickButtons_ = nullptr;
    joystickAxes_ = nullptr;
    previousFrameJoystickButtons_ = nullptr;
    previousFrameJoystickAxes_ = nullptr;

    // Initialise the mouse position
    glfwGetCursorPos(window, &prevMouseX_, &prevMouseY_);
    mouseDeltaX_ = 0.0;
    mouseDeltaY_ = 0.0;
}

// Called every time a new frame starts, storing previous frame's input data
void InputManager::frameStart(const Clock* clock)
{
    // Poll joystick inputs
    if(!ignoringInput_)
    {
        pollJoystick();
    }

    pollMouse();

    Vector3 axes;
    axes.z = getAxis(InputKey::W, InputKey::S); // Get forward input
    axes.x = getAxis(InputKey::D, InputKey::A); // Get strafing input
    axes.y = getAxis(InputKey::Q, InputKey::E); // Get vertical input

    InputCmd inputs;

    // Set input object parameters
    inputs.deltaTime = clock->deltaTime();
    inputs.lookRotation = Quaternion::identity();
    inputs.axes = axes;
    inputs.deltaPixelsX = mouseDeltaX();
    inputs.deltaPixelsY = mouseDeltaY();

    // Store array of previously pressed joypad inputs
    previousFrameJoystickButtons_ = joystickButtons_;
    previousFrameJoystickAxes_ = joystickAxes_;

    SceneManager::instance()->handleInput(inputs);
}

void InputManager::enableInput()
{
    ignoringInput_ = false;
}

void InputManager::disableInput()
{
    ignoringInput_ = true;
}

void InputManager::setInputEnabled(bool enabled)
{
    ignoringInput_ = !enabled;
}

// Returns true if input key is found to be pressed, false if not
bool InputManager::isKeyDown(InputKey key) const
{
    return glfwGetKey(window_, (int)key);
}

// Returns true if key is not being pressed
bool InputManager::isKeyUp(InputKey key) const
{
    return !(isKeyDown(key));
}

float InputManager::getAxis(InputKey positiveKey, InputKey negativeKey) const
{
    float axisPlus = 0.0f;
    float axisMinus = 0.0f;

    if (isKeyDown(positiveKey))
    {
        axisPlus += 1.0f;
    }

    if (isKeyDown(negativeKey))
    {
        axisMinus += 1.0f;
    }

    return axisPlus - axisMinus;
}

float InputManager::getAxisLerp(InputKey positiveKey, InputKey negativeKey, float timeToMax) const
{
    float axisPlus = 0.0f;
    float axisMinus = 0.0f;

    if (isKeyDown(positiveKey))
    {
        axisPlus += 1.0f;
    }

    if (isKeyDown(negativeKey))
    {
        axisMinus += 1.0f;
    }

    return axisPlus - axisMinus;
}

bool InputManager::mouseButtonDown(MouseButton button) const
{
    return !ignoringInput_ && glfwGetMouseButton(window_, (int)button);
}

void InputManager::pollJoystick()
{
    // Get joystick connection status
    int present = glfwJoystickPresent(GLFW_JOYSTICK_1);

    // If joystick is present
    if (present == 1)
    {
        // Get joypad axis states
        int axisCount;
        joystickAxes_ = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);

        // Get joypad button states
        int buttonCount;
        joystickButtons_ = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
    }
}

void InputManager::pollMouse()
{
    double mouseX;
    double mouseY;
    glfwGetCursorPos(window_, &mouseX, &mouseY);

    mouseDeltaX_ = mouseX - prevMouseX_;
    mouseDeltaY_ = mouseY - prevMouseY_;

    prevMouseX_ = mouseX;
    prevMouseY_ = mouseY;
}
