#include "Application.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "Utils/Clock.h"
#include "EditorManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "VRManager.h"
#include "PhysicsManager.h"

Application::Application(const std::string &name, GLFWwindow* window)
    : name_(name),
    mode_(ApplicationMode::Edit),
    playType_(ApplicationPlayType::InEditorPreview),
    window_(window),
    fullScreenRenderer_(nullptr),
    fullScreenDepthTexture_(nullptr),
    fullScreenColorTexture_(nullptr),
    fullScreenFramebuffer_(nullptr)
{
    // Create engine modules
    editorManager_ = new EditorManager(window, true);
    inputManager_ = new InputManager(window);
    resourceManager_ = new ResourceManager("Resources/", "Build/CompiledResources");
    sceneManager_ = new SceneManager();
    renderManager_ = new RenderManager();
    vrManager_ = new VRManager();
    physicsManager_ = new PhysicsManager();

    // Create core classes
    clock_ = new Clock();
    clock_->setPaused(true);

    // Create a Quit menu item
    MainWindowMenu::instance()->addMenuItem("File/Exit", [&] { glfwSetWindowShouldClose(window_, GLFW_TRUE); });

    // Create a menu item for toggling between play & edit modes
    MainWindowMenu::instance()->addMenuItem("Game/Toggle Playing", [&]
    {
        if (isEditing())
            enterPlayMode();
        else
            enterEditMode();
    }, [&] { return isPlaying(); });

    // Create a menu item for toggling full screen play mode
    MainWindowMenu::instance()->addMenuItem("Game/Play FullScreen (G+C+L to exit)", [&]
    {
        if (isEditing())
            enterPlayMode();
    
        setPlayType(ApplicationPlayType::FullScreen);
    });

    // In standalone builds, start the game immediately
#ifdef STANDALONE
    enterPlayMode();
    setPlayType(ApplicationPlayType::FullScreen);
#endif
}

Application::~Application()
{
    // Save all source files when the application exits
    resourceManager_->saveAllSourceFiles();

    // Delete modules in opposite order to
    // how they were created.
    delete physicsManager_;
    delete vrManager_;
    delete sceneManager_;
    delete editorManager_;
    delete inputManager_;
    delete clock_;

    destroyFullScreenRenderer();
}

bool Application::running() const
{
    return !glfwWindowShouldClose(window_);
}

void Application::enterPlayMode()
{
    // Do nothing if already in play mode.
    if(isPlaying())
    {
        return;
    }

    // First, save the current scene
    // This allows the original scene to be restored when exiting play mode.
    SceneManager::instance()->saveScene();

    // Now, enter play mode and start the clock
    mode_ = ApplicationMode::Play;
    Clock::instance()->restart();
}

void Application::enterEditMode()
{
    // Do nothing if already in edit mode
    if(isEditing())
    {
        return;
    }

    // Enter edit mode and stop the clock
    mode_ = ApplicationMode::Edit;
    Clock::instance()->stop();

    // When we entered play mode, we saved the scene.
    // We now need to restore the original scene to undo changes made while playing.
    SceneManager::instance()->openScene(SceneManager::instance()->scenePath());
}

void Application::setPlayType(ApplicationPlayType type)
{
    playType_ = type;
}

void Application::resize(int width, int height)
{
    // Each time a resize occurs we need to update the backbuffer size.
    Framebuffer::backbuffer()->setResolution(width, height);

    // Inform relevent modules of the resize
    editorManager_->resize(width, height);

    // Make a new full screen renderer / framebuffer etc
    createFullScreenRenderer();
}

void Application::windowFocused()
{
    // Resources may have been changed via external tools
    // so check for changed resources when focus switches
    // back to the application window.
    resourceManager_->importChangedResources();
}

void Application::frameStart()
{
    // Hide and lock the cursor in full screen playing mode.
    if (isPlaying() && playType_ == ApplicationPlayType::FullScreen)
    {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    // Update each module manager
    clock_->frameStart();
    inputManager_->frameStart();
    sceneManager_->frameStart();
    vrManager_->frameStart();

    // Let a user press G + C + L simultenously to exit full screen play mode
    if (InputManager::instance()->isKeyDown(InputKey::G)
        && InputManager::instance()->isKeyDown(InputKey::C)
        && InputManager::instance()->isKeyDown(InputKey::L))
    {
        setPlayType(ApplicationPlayType::InEditorPreview);
    }

    // When in play mode, dispatch user input
    if(isPlaying())
    {
        inputManager_->dispatchInput(clock_->deltaTime());
    }

    // Put the FPS in the window title.
    // Update every 20 frames (start at frame 1)
    if (clock_->frameCount() % 20 == 1)
    {
        const float deltaTime = clock_->realDeltaTime();
        const float frameRate = 1.0f / deltaTime;
        const float frameTime = deltaTime * 1000.0f;
        std::string titleWithFPS = name_ + " - " + SceneManager::instance()->sceneName() + " [" + std::to_string(frameRate).substr(0, 4) + "FPS] [" + std::to_string(frameTime).substr(0, 5) + "ms]";
        if (isEditing()) titleWithFPS += " [Editing]";
        if (isPlaying()) titleWithFPS += " [Playing]";
        glfwSetWindowTitle(window_, titleWithFPS.c_str());
    }
}

void Application::drawFrame()
{
    // Full-screen play mode just draws the scene to the backbuffer
    if (isPlaying() && playType_ == ApplicationPlayType::FullScreen)
    {
        if (fullScreenRenderer_ == nullptr)
        {
            createFullScreenRenderer();
        }

        // OpenGL doesn't like us grabbing the default framebuffer's depth texture
        // As a workaround, render to a separate framebuffer and blit to the default fbo.
        fullScreenRenderer_->renderFrame(SceneManager::instance()->mainCamera());
        glBlitNamedFramebuffer(fullScreenFramebuffer_->glid(), Framebuffer::backbuffer()->glid(),
            0, 0, Framebuffer::backbuffer()->width(), Framebuffer::backbuffer()->height(),
            0, 0, Framebuffer::backbuffer()->width(), Framebuffer::backbuffer()->height(),
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
        return;
    }

    // Re-draw the editor window
    if (!isPlaying() || playType_ != ApplicationPlayType::FullScreen)
    {
        editorManager_->render();
    }
}

void Application::createFullScreenRenderer()
{
    if(fullScreenRenderer_ != nullptr)
    {
        destroyFullScreenRenderer();
    }

    fullScreenDepthTexture_ = new Texture(TextureFormat::Depth, Framebuffer::backbuffer()->width(), Framebuffer::backbuffer()->height());
    fullScreenColorTexture_ = new Texture(TextureFormat::RGBA8_SRGB, Framebuffer::backbuffer()->width(), Framebuffer::backbuffer()->height());
    fullScreenFramebuffer_ = new Framebuffer();
    fullScreenFramebuffer_->attachDepthTexture(fullScreenDepthTexture_);
    fullScreenFramebuffer_->attachColorTexture(fullScreenColorTexture_);
    fullScreenRenderer_ = new Renderer(fullScreenFramebuffer_);
}

void Application::destroyFullScreenRenderer()
{
    delete fullScreenRenderer_;
    delete fullScreenFramebuffer_;
    delete fullScreenDepthTexture_;
    delete fullScreenColorTexture_;
}
