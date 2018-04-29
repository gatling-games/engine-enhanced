#pragma once

#include <string>

#include "Utils/Singleton.h"
#include "Renderer/Renderer.h"

struct GLFWwindow;

class Clock;
class EditorManager;
class InputManager;
class ResourceManager;
class SceneManager;
class RenderManager;
class VRManager;
class PhysicsManager;

enum class ApplicationMode
{
    Edit,
    Play,
};

enum class ApplicationPlayType
{
    // Editor interface visible
    InEditorPreview,

    // No editor interface visible
    FullScreen,
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

    // Sets the type of play mode currently being used
    void setPlayType(ApplicationPlayType type);

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
    ApplicationPlayType playType_;

    // The main window
    GLFWwindow* window_;

    // Module managers
    EditorManager* editorManager_;
    InputManager* inputManager_;
    ResourceManager* resourceManager_;
    SceneManager* sceneManager_;
    RenderManager* renderManager_;
    VRManager* vrManager_;
    PhysicsManager* physicsManager_;

    // The clock manager
    Clock* clock_;

    // A renderer used for full screen play mode
    Renderer* fullScreenRenderer_;
    Texture* fullScreenDepthTexture_;
    Texture* fullScreenColorTexture_;
    Framebuffer* fullScreenFramebuffer_;

    // True until quitting
    bool running_;

    void createFullScreenRenderer();
    void destroyFullScreenRenderer();
};