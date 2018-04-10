#pragma once

#include <string>

#include "Utils/Singleton.h"

struct GLFWwindow;

class Clock;
class EditorManager;
class InputManager;
class ResourceManager;
class SceneManager;
class RenderManager;

enum class ApplicationMode
{
    Edit,
    Play,
};

class Application : public Singleton<Application>
{
public:
    Application(const std::string &name, GLFWwindow* window);
    ~Application();

    bool running() const { return running_; }
    bool isEditing() const { return (mode_ == ApplicationMode::Edit); }
    bool isPlaying() const { return (mode_ == ApplicationMode::Play); }

    // Switches to playing mode
    // The current scene will be saved immediately before playing
    void enterPlayMode();

    // Switches to editing mode.
    // The current scene will be restored immediately after playing
    void enterEditMode();

    // Called when the window is resized.
    void resize(int newWidth, int newHeight);

    // Called when the window (re)gains focus.
    void windowFocused();

    // Called during the game loop.
    void frameStart();
    void drawFrame();

private:
    const std::string name_;

    ApplicationMode mode_;

    // The main window
    GLFWwindow* window_;

    // Module managers
    EditorManager* editorManager_;
    InputManager* inputManager_;
    ResourceManager* resourceManager_;
    SceneManager* sceneManager_;
    RenderManager* renderManager_;

    // The clock manager
    Clock* clock_;

    // True until quitting
    bool running_;
};