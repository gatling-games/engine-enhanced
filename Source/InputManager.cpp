#include "InputManager.h"
#include "SceneManager.h"

#include <GLFW/glfw3.h>

#include "Utils\Clock.h"

InputManager::InputManager(GLFWwindow* window)
    : ignoringInput_(false)
{
    // Set reference to window
    window_ = window;

    // Initialise the mouse position
    glfwGetCursorPos(window, &prevMouseX_, &prevMouseY_);
    mouseDeltaX_ = 0.0;
    mouseDeltaY_ = 0.0;
}

// Called every time a new frame starts, storing previous frame's input data
void InputManager::frameStart()
{
    pollMouse();
}

void InputManager::dispatchInput(float deltaTime) const
{
    InputCmd inputs;
    inputs.deltaTime = deltaTime;

    // Keyboard & mouse inputs
    inputs.forwardsMovement = getAxis(InputKey::W, InputKey::S);
    inputs.sidewaysMovement = getAxis(InputKey::D, InputKey::A);
    inputs.verticalMovement = getAxis(InputKey::Space, InputKey::LCtrl);
    inputs.horizontalRotation = mouseDeltaX();
    inputs.verticalRotation = mouseDeltaY();

    // Joystick inputs
    // They override any keyboard/mouse inputs set to zero.
    if (fabs(inputs.forwardsMovement) < 0.001f) inputs.forwardsMovement = getJoystickAxis(JoystickAxis::LeftStickVertical);
    if (fabs(inputs.sidewaysMovement) < 0.001f) inputs.sidewaysMovement = getJoystickAxis(JoystickAxis::LeftStickHorizontal);
    if (fabs(inputs.verticalMovement) < 0.001f) inputs.verticalMovement = getJoystickAxis(JoystickButton::RightBumper, JoystickButton::LeftBumper);
    if (fabs(inputs.horizontalRotation) < 0.001f) inputs.horizontalRotation = getJoystickAxis(JoystickAxis::RightStickHorizontal) * 5.0f;
    if (fabs(inputs.verticalRotation) < 0.001f) inputs.verticalRotation = getJoystickAxis(JoystickAxis::RightStickVertical) * -5.0f;

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

bool InputManager::isJoystickButtonDown(JoystickButton button) const
{
    for (int joy = 0; joy < GLFW_JOYSTICK_LAST; ++joy)
    {
        // Check if the joystick is disabled
        if (glfwJoystickPresent(joy) == false)
        {
            continue;
        }

        // Get the values for each joystick axis
        int buttonsCount;
        const unsigned char* buttonValues = glfwGetJoystickButtons(joy, &buttonsCount);
        if (buttonsCount <= (int)button)
        {
            // Button not found
            continue;
        }

        // Read the button value and see if it is down
        if(buttonValues[(int)button] == GLFW_PRESS)
        {
            return true;
        }
    }

    // No joystick has the button down
    return false;
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

float InputManager::getJoystickAxis(JoystickAxis axis) const
{
    // The biggest magnitude joystick axis for any controller
    // that is currently connected.
    float max = 0.0f;

    for(int joy = 0; joy < GLFW_JOYSTICK_LAST; ++joy)
    {
        // Check if the joystick is disabled
        if(glfwJoystickPresent(joy) == false)
        {
            continue;
        }

        // Get the values for each joystick axis
        int axesCount;
        const float* axesValues = glfwGetJoystickAxes(joy, &axesCount);
        if(axesCount <= (int)axis)
        {
            // Axis not found
            continue;
        }

        // Read the axis value, and check if it is the biggest magnitude
        // one for any controller that has been polled.
        const float value = axesValues[(int)axis];
        if(fabs(value) > fabs(max))
        {
            max = value;
        }
    }

    return max;
}

float InputManager::getJoystickAxis(JoystickButton positiveButton, JoystickButton negativeButton) const
{
    float axis = 0.0f;

    if (isJoystickButtonDown(positiveButton))
    {
        axis += 1.0f;
    }

    if (isJoystickButtonDown(negativeButton))
    {
        axis -= 1.0f;
    }

    return axis;
}

bool InputManager::mouseButtonDown(MouseButton button) const
{
    return !ignoringInput_ && glfwGetMouseButton(window_, (int)button);
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
