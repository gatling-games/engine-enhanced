#include "InputManager.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <imgui.h>

#include "Utils\Clock.h"

InputManager::InputManager(GLFWwindow* window)
{
    // Set reference to window
    window_ = window;

    // Initialise vectors for holding key states
    pressedKeys_ = {};
    previouslyPressedKeys_ = {};

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
    // Store vector of previously pressed keys
    previouslyPressedKeys_ = pressedKeys_;

    // Poll keyboard and joystick inputs
    pollKeyboard();
    pollJoystick();
    pollMouse();

    // Create object holding input data to send to server
    InputCmd inputs;

    // Set input object parameters
    inputs.deltaTime = clock->deltaTime();
    inputs.pressedKeys = pressedKeys_;
    inputs.joystickButtons = joystickButtons_;
    inputs.joystickAxes = joystickAxes_;

    // Store array of previously pressed joypad inputs
    previousFrameJoystickButtons_ = joystickButtons_;
    previousFrameJoystickAxes_ = joystickAxes_;
}

void InputManager::drawDebugMenu()
{
    ImGui::Text("Input Manager");

    for (int i = 0; i < (int)pressedKeys_.size(); i++)
    {
        ImGui::Text("Key %i pressed", pressedKeys_[i]);
    }
}

// Returns true if input key is found to be pressed, false if not
bool InputManager::isKeyDown(InputKey key) const
{
    auto iterator = std::find(pressedKeys_.begin(), pressedKeys_.end(), key);

    if (iterator != pressedKeys_.end())
    {
        return true;
    }
    return false;
}

// Returns true if key is not being pressed
bool InputManager::isKeyUp(InputKey key) const
{
    return !(isKeyDown(key));
}

// Adds key input code to vector of currently pressed keys
bool InputManager::keyPressed(InputKey key)
{
    if (!isKeyDown(key))
    {
        pressedKeys_.push_back(key);
        return true;
    }
    return false;
}

// Removes key input code from vector of currently pressed keys (DO THESE NEED TO BE BOOL?)
bool InputManager::keyReleased(InputKey key)
{
    pressedKeys_.erase(std::remove(pressedKeys_.begin(), pressedKeys_.end(), key), pressedKeys_.end());
    return true;
}

// Returns true if input key was down in previous frame
bool InputManager::wasKeyDown(InputKey key) const
{
    auto iterator = std::find(previouslyPressedKeys_.begin(), previouslyPressedKeys_.end(), key);

    if (iterator != previouslyPressedKeys_.end())
    {
        return true;
    }
    return false;
}

// Returns true if input key was not down in previous frame
bool InputManager::wasKeyUp(InputKey key) const
{
    return !(wasKeyDown(key));
}

// Returns true if any key is down
bool InputManager::anyKeyDown() const
{
    if (pressedKeys_.size() != 0)
    {
        return true;
    }
    return false;
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

bool InputManager::mouseButtonDown(MouseButton button) const
{
    return glfwGetMouseButton(window_, (int)button);
}

// Method which takes input GLFW key code and outputs in our own format
InputKey InputManager::convertGLFWKey(int key) const
{
    switch (key)
    {
    case GLFW_KEY_A: return InputKey::A;
    case GLFW_KEY_B: return InputKey::B;
    case GLFW_KEY_C: return InputKey::C;
    case GLFW_KEY_D: return InputKey::D;
    case GLFW_KEY_E: return InputKey::E;
    case GLFW_KEY_F: return InputKey::F;
    case GLFW_KEY_G: return InputKey::G;
    case GLFW_KEY_H: return InputKey::H;
    case GLFW_KEY_I: return InputKey::I;
    case GLFW_KEY_J: return InputKey::J;
    case GLFW_KEY_K: return InputKey::K;
    case GLFW_KEY_L: return InputKey::L;
    case GLFW_KEY_M: return InputKey::M;
    case GLFW_KEY_N: return InputKey::N;
    case GLFW_KEY_O: return InputKey::O;
    case GLFW_KEY_P: return InputKey::P;
    case GLFW_KEY_Q: return InputKey::Q;
    case GLFW_KEY_R: return InputKey::R;
    case GLFW_KEY_S: return InputKey::S;
    case GLFW_KEY_T: return InputKey::T;
    case GLFW_KEY_U: return InputKey::U;
    case GLFW_KEY_V: return InputKey::V;
    case GLFW_KEY_W: return InputKey::W;
    case GLFW_KEY_X: return InputKey::X;
    case GLFW_KEY_Y: return InputKey::Y;
    case GLFW_KEY_Z: return InputKey::Z;
    case GLFW_KEY_0: return InputKey::Keyboard0;
    case GLFW_KEY_1: return InputKey::Keyboard1;
    case GLFW_KEY_2: return InputKey::Keyboard2;
    case GLFW_KEY_3: return InputKey::Keyboard3;
    case GLFW_KEY_4: return InputKey::Keyboard4;
    case GLFW_KEY_5: return InputKey::Keyboard5;
    case GLFW_KEY_6: return InputKey::Keyboard6;
    case GLFW_KEY_7: return InputKey::Keyboard7;
    case GLFW_KEY_8: return InputKey::Keyboard8;
    case GLFW_KEY_9: return InputKey::Keyboard9;
    case GLFW_KEY_ESCAPE: return InputKey::Esc;
    case GLFW_KEY_F1: return InputKey::F1;
    case GLFW_KEY_F2: return InputKey::F2;
    case GLFW_KEY_F3: return InputKey::F3;
    case GLFW_KEY_F4: return InputKey::F4;
    case GLFW_KEY_F5: return InputKey::F5;
    case GLFW_KEY_F6: return InputKey::F6;
    case GLFW_KEY_F7: return InputKey::F7;
    case GLFW_KEY_F8: return InputKey::F8;
    case GLFW_KEY_F9: return InputKey::F9;
    case GLFW_KEY_F10: return InputKey::F10;
    case GLFW_KEY_F11: return InputKey::F11;
    case GLFW_KEY_F12: return InputKey::F12;
    case GLFW_KEY_LEFT_SHIFT: return InputKey::LShift;
    case GLFW_KEY_RIGHT_SHIFT: return InputKey::RShift;
    case GLFW_KEY_TAB: return InputKey::Tab;
    case GLFW_KEY_CAPS_LOCK: return InputKey::CapsLock;
    case GLFW_KEY_LEFT_CONTROL: return InputKey::LCtrl;
    case GLFW_KEY_RIGHT_CONTROL: return InputKey::RCtrl;
    case GLFW_KEY_LEFT_ALT: return InputKey::LAlt;
    case GLFW_KEY_RIGHT_ALT: return InputKey::RAlt;
    case GLFW_KEY_UP: return InputKey::Up;
    case GLFW_KEY_LEFT: return InputKey::Left;
    case GLFW_KEY_DOWN: return InputKey::Down;
    case GLFW_KEY_RIGHT: return InputKey::Right;
    case GLFW_KEY_BACKSPACE: return InputKey::Backspace;
    case GLFW_KEY_ENTER: return InputKey::Enter;
    case GLFW_KEY_LEFT_BRACKET: return InputKey::LBracket;
    case GLFW_KEY_RIGHT_BRACKET: return InputKey::RBracket;
    case GLFW_KEY_SEMICOLON: return InputKey::Semicolon;
    case GLFW_KEY_APOSTROPHE: return InputKey::Apostrophe;
    case GLFW_KEY_BACKSLASH: return InputKey::Hash;
    case GLFW_KEY_COMMA: return InputKey::Comma;
    case GLFW_KEY_PERIOD: return InputKey::Fullstop;
    case GLFW_KEY_SLASH: return InputKey::FSlash;
    case GLFW_KEY_WORLD_2: return InputKey::BSlash;
    case GLFW_KEY_GRAVE_ACCENT: return InputKey::Grave;
    case GLFW_KEY_KP_0: return InputKey::Num0;
    case GLFW_KEY_KP_1: return InputKey::Num1;
    case GLFW_KEY_KP_2: return InputKey::Num2;
    case GLFW_KEY_KP_3: return InputKey::Num3;
    case GLFW_KEY_KP_4: return InputKey::Num4;
    case GLFW_KEY_KP_5: return InputKey::Num5;
    case GLFW_KEY_KP_6: return InputKey::Num6;
    case GLFW_KEY_KP_7: return InputKey::Num7;
    case GLFW_KEY_KP_8: return InputKey::Num8;
    case GLFW_KEY_KP_9: return InputKey::Num9;
    case GLFW_KEY_KP_DIVIDE: return InputKey::NumSlash;
    case GLFW_KEY_KP_MULTIPLY: return InputKey::NumAsterisk;
    case GLFW_KEY_KP_SUBTRACT: return InputKey::NumMinus;
    case GLFW_KEY_KP_ADD: return InputKey::NumPlus;
    case GLFW_KEY_KP_ENTER: return InputKey::NumEnter;
    case GLFW_KEY_INSERT: return InputKey::Insert;
    case GLFW_KEY_HOME: return InputKey::Home;
    case GLFW_KEY_PAGE_UP: return InputKey::PageUp;
    case GLFW_KEY_PAGE_DOWN: return InputKey::PageDown;
    case GLFW_KEY_DELETE: return InputKey::Delete;
    case GLFW_KEY_END: return InputKey::End;
    default: return InputKey::None;
    }
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

void InputManager::pollKeyboard()
{
    // For each key
    for (int i = 0; i < 150; ++i)
    {
        // If key is being pressed
        if (glfwGetKey(window_, i))
        {
            // Add to pressed key vector
            keyPressed(convertGLFWKey(i));
        }
        else
        {
            // Remove from pressed key vector
            keyReleased(convertGLFWKey(i));
        }
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
