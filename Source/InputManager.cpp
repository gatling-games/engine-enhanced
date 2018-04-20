#include "InputManager.h"
#include "SceneManager.h"

#include <GLFW/glfw3.h>

#include "Utils/Clock.h"

InputManager::InputManager(GLFWwindow* window)
    : ignoringInput_(false)
{
    // Set reference to window
    window_ = window;

    // Initialise the mouse position
    glfwGetCursorPos(window, &prevMouseX_, &prevMouseY_);
    mouseDeltaX_ = 0.0;
    mouseDeltaY_ = 0.0;

    // Setup Xbox Controller
    JoystickMapping mapping;
    mapping.name = "Xbox 360 Controller";
    mapping.axes[(int)JoystickAxis::LeftStickHorizontal] = 0;
    mapping.axes[(int)JoystickAxis::LeftStickVertical] = 1;
    mapping.axes[(int)JoystickAxis::RightStickHorizontal] = 2;
    mapping.axes[(int)JoystickAxis::RightStickVertical] = 3;

    mapping.axesSensitivity[(int)JoystickAxis::LeftStickHorizontal] = 1.0f;
    mapping.axesSensitivity[(int)JoystickAxis::LeftStickVertical] = 1.0f;
    mapping.axesSensitivity[(int)JoystickAxis::RightStickHorizontal] = 5.0f;
    mapping.axesSensitivity[(int)JoystickAxis::RightStickVertical] = -5.0f;

    mapping.buttons[(int)JoystickButton::A] = 0;
    mapping.buttons[(int)JoystickButton::B] = 1;
    mapping.buttons[(int)JoystickButton::X] = 2;
    mapping.buttons[(int)JoystickButton::Y] = 3;
    mapping.buttons[(int)JoystickButton::LeftBumper] = 4;
    mapping.buttons[(int)JoystickButton::RightBumper] = 5;

    mappings_.push_back(mapping);

    // Setup Hotas Joystick
    mapping.name = "T.Flight Hotas X";
    mapping.axes[(int)JoystickAxis::LeftStickHorizontal] = 0;
    mapping.axes[(int)JoystickAxis::LeftStickVertical] = 1;
    mapping.axes[(int)JoystickAxis::RightStickVertical] = 2;
    mapping.axes[(int)JoystickAxis::RightStickHorizontal] = 3;

    mapping.axesSensitivity[(int)JoystickAxis::LeftStickHorizontal] = 1.0f;
    mapping.axesSensitivity[(int)JoystickAxis::LeftStickVertical] = -1.0f;
    mapping.axesSensitivity[(int)JoystickAxis::RightStickHorizontal] = 3.0f;
    mapping.axesSensitivity[(int)JoystickAxis::RightStickVertical] = -1.5f;

    mapping.buttons[(int)JoystickButton::A] = 0;
    mapping.buttons[(int)JoystickButton::B] = 1;
    mapping.buttons[(int)JoystickButton::X] = 2;
    mapping.buttons[(int)JoystickButton::Y] = 3;
    mapping.buttons[(int)JoystickButton::LeftBumper] = 5;
    mapping.buttons[(int)JoystickButton::RightBumper] = 4;

    mappings_.push_back(mapping);
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
	if (fabs(inputs.horizontalRotation) < 0.001f) inputs.horizontalRotation = getJoystickAxis(JoystickAxis::RightStickHorizontal);
	if (fabs(inputs.verticalRotation) < 0.001f) inputs.verticalRotation = getJoystickAxis(JoystickAxis::RightStickVertical);

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

        int value = 0;

        for (JoystickMapping mapping : mappings_)
        {
            if (glfwGetJoystickName(joy) == mapping.name)
            {
                value = mapping.buttons[(int)button];
            }
        }

        // Read the button value and see if it is down
        if(buttonValues[value] == GLFW_PRESS)
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
        float value = 0.0f;

        for (JoystickMapping mapping : mappings_)
        {
            if (glfwGetJoystickName(joy) == mapping.name)
            {
                value = axesValues[mapping.axes[(int)axis]] * mapping.axesSensitivity[(int)axis];
            }
        }       
        
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