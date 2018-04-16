#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "Utils/Singleton.h"
#include "Math\Vector2.h"
#include "Math\Vector3.h"
#include "Math\Quaternion.h"

class Clock;

enum class InputKey
{
    None = -1,
    A = GLFW_KEY_A,
    B = GLFW_KEY_B,
    C = GLFW_KEY_C,
    D = GLFW_KEY_D,
    E = GLFW_KEY_E,
    F = GLFW_KEY_F,
    G = GLFW_KEY_G,
    H = GLFW_KEY_H,
    I = GLFW_KEY_H,
    J = GLFW_KEY_J,
    K = GLFW_KEY_K,
    L = GLFW_KEY_L,
    M = GLFW_KEY_M,
    N = GLFW_KEY_N,
    O = GLFW_KEY_O,
    P = GLFW_KEY_P,
    Q = GLFW_KEY_Q,
    R = GLFW_KEY_R,
    S = GLFW_KEY_S,
    T = GLFW_KEY_T,
    U = GLFW_KEY_U,
    V = GLFW_KEY_V,
    W = GLFW_KEY_W,
    X = GLFW_KEY_X,
    Y = GLFW_KEY_Y,
    Z = GLFW_KEY_Z,
    Keyboard1 = GLFW_KEY_1,
    Keyboard2 = GLFW_KEY_2,
    Keyboard3 = GLFW_KEY_3,
    Keyboard4 = GLFW_KEY_4,
    Keyboard5 = GLFW_KEY_5,
    Keyboard6 = GLFW_KEY_6,
    Keyboard7 = GLFW_KEY_7,
    Keyboard8 = GLFW_KEY_8,
    Keyboard9 = GLFW_KEY_9,
    Keyboard0 = GLFW_KEY_0,
    Esc = GLFW_KEY_ESCAPE,
    F1 = GLFW_KEY_F1,
    F2 = GLFW_KEY_F2,
    F3 = GLFW_KEY_F3,
    F4 = GLFW_KEY_F4,
    F5 = GLFW_KEY_F5,
    F6 = GLFW_KEY_F6,
    F7 = GLFW_KEY_F7,
    F8 = GLFW_KEY_F8,
    F9 = GLFW_KEY_F9,
    F10 = GLFW_KEY_F10,
    F11 = GLFW_KEY_F11,
    F12 = GLFW_KEY_F12,
    LShift = GLFW_KEY_LEFT_SHIFT,
    RShift = GLFW_KEY_RIGHT_SHIFT,
    Tab = GLFW_KEY_TAB,
    CapsLock = GLFW_KEY_CAPS_LOCK,
    LCtrl = GLFW_KEY_LEFT_CONTROL,
    RCtrl = GLFW_KEY_RIGHT_CONTROL,
    LAlt = GLFW_KEY_LEFT_ALT,
    RAlt = GLFW_KEY_RIGHT_ALT,
    Up = GLFW_KEY_UP,
    Left = GLFW_KEY_LEFT,
    Down = GLFW_KEY_DOWN,
    Right = GLFW_KEY_RIGHT,
    Backspace = GLFW_KEY_BACKSPACE,
    Enter = GLFW_KEY_ENTER,
    LBracket = GLFW_KEY_LEFT_BRACKET,
    RBracket = GLFW_KEY_RIGHT_BRACKET,
    Semicolon = GLFW_KEY_SEMICOLON,
    Apostrophe = GLFW_KEY_APOSTROPHE,
    Comma = GLFW_KEY_COMMA,
    Fullstop = GLFW_KEY_PERIOD,
    FSlash = GLFW_KEY_SLASH,
    BSlash = GLFW_KEY_BACKSLASH,
    Grave = GLFW_KEY_GRAVE_ACCENT,
    Num1 = GLFW_KEY_KP_1,
    Num2 = GLFW_KEY_KP_2,
    Num3 = GLFW_KEY_KP_3,
    Num4 = GLFW_KEY_KP_4,
    Num5 = GLFW_KEY_KP_5,
    Num6 = GLFW_KEY_KP_6,
    Num7 = GLFW_KEY_KP_7,
    Num8 = GLFW_KEY_KP_8,
    Num9 = GLFW_KEY_KP_9,
    Num0 = GLFW_KEY_KP_0,
    NumSlash = GLFW_KEY_KP_DIVIDE,
    NumAsterisk = GLFW_KEY_KP_MULTIPLY,
    NumMinus = GLFW_KEY_KP_SUBTRACT,
    NumPlus = GLFW_KEY_KP_ADD,
    NumEnter = GLFW_KEY_KP_ENTER,
    Insert = GLFW_KEY_INSERT,
    Home = GLFW_KEY_HOME,
    PageUp = GLFW_KEY_PAGE_UP,
    PageDown = GLFW_KEY_PAGE_DOWN,
    Delete = GLFW_KEY_DELETE,
    End = GLFW_KEY_END
};

enum class MouseButton
{
    Left = GLFW_MOUSE_BUTTON_LEFT,
    Right = GLFW_MOUSE_BUTTON_RIGHT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE,
};

struct InputCmd
{
    float deltaTime; // Elapsed time since last update
    Quaternion lookRotation; // Current camera orientation relative to helicopter
    Vector3 axes; // Input helicopter velocity
    float deltaPixelsX; // Current helicopter rotational accelleration
    float deltaPixelsY;
};

class InputManager : public Singleton<InputManager>
{
public:
    explicit InputManager(GLFWwindow* window);

    // Called every frame
    void frameStart(const Clock* clock);

    // Allows all input to be enabled and disabled.
    // This should be used when the game is paused, or the game panel does not have focus.
    void enableInput();
    void disableInput();
    void setInputEnabled(bool enabled);

    // Returns true if a key is currently down
    bool isKeyDown(InputKey key) const;

    // Returns true if a key is not currently down
    bool isKeyUp(InputKey key) const;

    // Returns -1, 0, or 1 based on which keys are pressed.
    // Only positive = 1, only negative = -1, both/neither = 0
    float getAxis(InputKey positiveKey, InputKey negativeKey) const;

    float getAxisLerp(InputKey positiveKey, InputKey negativeKey, float timeToMax) const;

    // Returns true if the specified mouse button is currently pressed.
    bool mouseButtonDown(MouseButton button) const;

    // Capture mouse input and hide mouse from viewer
    void captureMouse();

    // Release mouse
    void releaseMouse();

    // The number of pixels that the mouse has moved in the last frame.
    float mouseDeltaX() const { return (float)mouseDeltaX_; }
    float mouseDeltaY() const { return (float)mouseDeltaY_; }

private:
    // Reference to GLFW window object
    GLFWwindow* window_;

    // When true, the input manager ignores all button presses.
    bool ignoringInput_;

    // Arrays for holding pressed joystick buttons and last frame's joystick buttons
    const unsigned char* joystickButtons_;
    const unsigned char* previousFrameJoystickButtons_;

    // Arrays for holding joystick axis data
    const float* joystickAxes_;
    const float* previousFrameJoystickAxes_;

    double mouseDeltaX_;
    double mouseDeltaY_;
    double prevMouseX_;
    double prevMouseY_;

    void pollJoystick();
    void pollMouse();
};