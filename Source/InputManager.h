#pragma once

#include "Application.h"
#include "Utils/Singleton.h"
#include "Math\Vector2.h"
#include "Math\Vector3.h"

#include <GLFW/glfw3.h>
#include <vector>

class Clock;

enum class InputKey
{
    None = -1,
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6,
    H = 7,
    I = 8,
    J = 9,
    K = 10,
    L = 11,
    M = 12,
    N = 13,
    O = 14,
    P = 15,
    Q = 16,
    R = 17,
    S = 18,
    T = 19,
    U = 20,
    V = 21,
    W = 22,
    X = 23,
    Y = 24,
    Z = 25,
    Keyboard1 = 26,
    Keyboard2 = 27,
    Keyboard3 = 28,
    Keyboard4 = 29,
    Keyboard5 = 30,
    Keyboard6 = 31,
    Keyboard7 = 32,
    Keyboard8 = 33,
    Keyboard9 = 34,
    Keyboard0 = 35,
    Esc = 36,
    F1 = 37,
    F2 = 38,
    F3 = 39,
    F4 = 40,
    F5 = 41,
    F6 = 42,
    F7 = 43,
    F8 = 44,
    F9 = 45,
    F10 = 46,
    F11 = 47,
    F12 = 48,
    LShift = 49,
    RShift = 50,
    Tab = 51,
    CapsLock = 52,
    LCtrl = 53,
    RCtrl = 54,
    LAlt = 55,
    RAlt = 56,
    Up = 57,
    Left = 58,
    Down = 59,
    Right = 60,
    Backspace = 61,
    Enter = 62,
    LBracket = 63,
    RBracket = 64,
    Semicolon = 65,
    Apostrophe = 66,
    Hash = 67,
    Comma = 68,
    Fullstop = 69,
    FSlash = 70,
    BSlash = 71,
    Grave = 72,
    Num1 = 73,
    Num2 = 74,
    Num3 = 75,
    Num4 = 76,
    Num5 = 77,
    Num6 = 78,
    Num7 = 79,
    Num8 = 80,
    Num9 = 81,
    Num0 = 82,
    NumSlash = 83,
    NumAsterisk = 84,
    NumMinus = 85,
    NumPlus = 86,
    NumEnter = 87,
    Insert = 88,
    Home = 89,
    PageUp = 90,
    PageDown = 91,
    Delete = 92,
    End = 93,
    GamepadDUp = 94,
    GamepadDDown = 95,
    GamepadDLeft = 96,
    GamepadDRight = 97,
    GamepadStart = 98,
    GamepadSelect = 99,
    GamepadLStick = 100,
    GamepadRStick = 101,
    GamepadLTrigger = 102,
    GamepadRTrigger = 103,
    GamepadLBumper = 104,
    GamepadRBumper = 105,
    GamepadA = 106,
    GamepadB = 107,
    GamepadX = 108,
    GamepadY = 109
};

struct InputCmd
{
    float deltaTime;
    Vector2 rotationVelocity;
    Vector3 movementVelocity;

    std::vector<InputKey> pressedKeys;
    const unsigned char* joystickButtons;
    const float* joystickAxes;

    // Add commands for shooting etc
};

class InputManager : public ApplicationModule, public Singleton<InputManager>
{
public:
    explicit InputManager(GLFWwindow* window);

    // Called every frame
    void frameStart(const Clock* clock);

    // ApplicationModule callbacks
    std::string name() const { return "Input Manager"; }
    void drawDebugMenu() override;

    // Returns true if a key is currently down
    bool isKeyDown(InputKey key) const;

    // Returns true if a key is not currently down
    bool isKeyUp(InputKey key) const;

    // Returns true the first frame that a key is down
    bool keyPressed(InputKey key);

    // Returns true the first frame that a key is released
    bool keyReleased(InputKey key);

    // Returns true if key was pressed on previous frame
    bool wasKeyDown(InputKey key) const;

    // Returns true if key was released on previous frame
    bool wasKeyUp(InputKey key) const;

    // Returns true on any frame where any key is down
    bool anyKeyDown() const;

    float getAxis(InputKey key1, InputKey key2);

    float mouseDeltaX();

    float mouseDeltaY();

private:
    // Reference to GLFW window object
    GLFWwindow* window_;

    // Private function for converting between GLFW input codes and our own input codes
    InputKey convertGLFWKey(int key) const;

    // Vectors for holding pressed keys and last frame's pressed keys
    std::vector<InputKey> pressedKeys_;
    std::vector<InputKey> previouslyPressedKeys_;

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
    void pollKeyboard();
    void pollMouse();
};