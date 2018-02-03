#include "MainWindow.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <functional>

#include "RenderManager.h"
#include "MainWindowMenu.h"

// Panel placement config
const float MainWindow::GamePanelWidth = 0.55f;
const float MainWindow::ScenePanelWidth = 0.18f;
const float MainWindow::PropertiesPanelWidth = 1.0f - GamePanelWidth - ScenePanelWidth;
const float MainWindow::ScenePanelHeight = 0.5f;
const float MainWindow::GamePanelAspect = 16.0f / 9.0f;
const float MainWindow::OutputPanelMaxHeight = 0.8f;

MainWindow::MainWindow()
    : drawImGuiTestWindow_(false),
    gamePanel_(),
    outputPanel_(),
    scenePanel_(),
    resourcesPanel_(),
    propertiesPanel_()
{
    MainWindowMenu::instance()->addMenuItem("Tools/Toggle ImGui test", [&] { drawImGuiTestWindow_ = !drawImGuiTestWindow_; });
}

void MainWindow::resize(int width, int height)
{
    windowWidth_ = width;
    windowHeight_ = height;
}

void MainWindow::repaint()
{
    // Draw the top file/edit menu
    mainMenu_.draw();

    // If the user has selected the test window, display that instead.
    if (drawImGuiTestWindow_)
    {
        ImGui::ShowTestWindow();
        return;
    }

    // Draw each editor panel in the correct location
    drawPanel(gamePanel_, gamePanelRect());
    drawPanel(outputPanel_, outputPanelRect());
    drawPanel(scenePanel_, scenePanelRect());
    drawPanel(resourcesPanel_, resourcesPanelRect());
    drawPanel(propertiesPanel_, propertiesPanelRect());
}

Rect MainWindow::fullRect() const
{
    return Rect(0.0f, 0.0f, (float)windowWidth_, (float)windowHeight_);
}

Rect MainWindow::mainMenuRect() const
{
    // Determine the height of the main menu
    ImGuiContext* g = ImGui::GetCurrentContext();
    float mainMenuHeight = g->FontBaseSize + g->Style.FramePadding.y * 2.0f + 1.0f;

    return Rect(0.0f, 0.0f, fullRect().width, mainMenuHeight).roundedToPixels();
}

Rect MainWindow::gamePanelRect() const
{
    // Position below the main menu
    float minx = 0.0f;
    float miny = mainMenuRect().max().y;

    // Determine the game panel height required to keep the aspect ratio.
    float width = fullRect().width * GamePanelWidth;
    float height = width / GamePanelAspect;

    // Add the border padding
    width += 2.0f;
    height += 2.0f;

    // Add the header area  
    ImGuiContext* g = ImGui::GetCurrentContext();
    height += g->FontBaseSize + g->Style.FramePadding.y * 2.0f + 1.0f;

    // Prevent the panel from being too big
    float maxHeight = fullRect().height * OutputPanelMaxHeight;
    if (height > maxHeight)
    {
        height = maxHeight;
    }

    return Rect(minx, miny, width, height).roundedToPixels();
}

Rect MainWindow::outputPanelRect() const
{
    // Position below the game panel
    float minx = gamePanelRect().min().x;
    float miny = gamePanelRect().max().y;

    // Match the game panel width, and fit against the window bottom
    float width = gamePanelRect().width;
    float height = fullRect().height - gamePanelRect().max().y;

    return Rect(minx, miny, width, height).roundedToPixels();
}

Rect MainWindow::scenePanelRect() const
{
    // Position to the right of the game panel and below the main window
    float minx = gamePanelRect().max().x;
    float miny = mainMenuRect().max().y;

    // Fill ScenePanelWidth % of the window horzontally,
    // and ScenePanelHeight % vertically.
    float width = fullRect().width * ScenePanelWidth;
    float height = fullRect().height * ScenePanelHeight;

    return Rect(minx, miny, width, height).roundedToPixels();
}

Rect MainWindow::resourcesPanelRect() const
{
    // Position below the scene panel
    float minx = scenePanelRect().min().x;
    float miny = scenePanelRect().max().y;

    // Match the scene panel width, and fit against the window bottom
    float width = scenePanelRect().width;
    float height = fullRect().height - scenePanelRect().max().y;

    return Rect(minx, miny, width, height).roundedToPixels();
}

Rect MainWindow::propertiesPanelRect() const
{
    // Position to the right of the scene panel and below the main menu
    float minx = scenePanelRect().max().x;
    float miny = mainMenuRect().max().y;

    // Fill the entire column, and fit against the right side of the window
    float width = fullRect().width - minx;
    float height = fullRect().height - miny;

    return Rect(minx, miny, width, height).roundedToPixels();
}

void MainWindow::drawPanel(EditorPanel &panel, const Rect &size)
{
    // Ensure the panel has borders and can't be moved
    // or collapsed by the user.
    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_ShowBorders;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoResize;

    // Set the position and size of the panel.
    ImGui::SetNextWindowPos(ImVec2(size.minx, size.miny));
    ImGui::SetNextWindowSize(ImVec2(size.width, size.height));

    // Draw the panel
    ImGui::Begin(panel.name().c_str(), 0, flags);

    // Set focus for the panel if right clicking within it.
    if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered())
    {
        ImGui::SetWindowFocus();
    }

    panel.draw();
    ImGui::End();
}