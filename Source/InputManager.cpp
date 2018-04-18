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
void InputManager::frameStart()
{
    // Poll joystick inputs
    if(!ignoringInput_)
    {
        pollJoystick();
    }

    pollMouse();
}

void InputManager::dispatchInput(float deltaTime) const
{
    InputCmd inputs;
    inputs.deltaTime = deltaTime;
    inputs.forwardsMovement = getAxis(InputKey::W, InputKey::S);
    inputs.sidewaysMovement = getAxis(InputKey::D, InputKey::A);
    inputs.verticalMovement = getAxis(InputKey::Space, InputKey::LCtrl);
    inputs.horizontalRotation = mouseDeltaX();
    inputs.verticalRotation = mouseDeltaY();
    inputs.lookRotation = Quaternion::identity();

    // The scene manager passes the input to all components
    // They can use the input by implementing the handleInput callback.
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
    float axis = 0.0f;

    if (isKeyDown(positiveKey))
    {
        axis += 1.0f;
    }

    if (isKeyDown(negativeKey))
    {
        axis -= 1.0f;
    }

    return axis;
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
