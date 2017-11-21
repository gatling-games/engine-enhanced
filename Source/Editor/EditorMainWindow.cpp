#include "EditorMainWindow.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <functional>

#include "Math\Vector3.h"

// Panel placement config
const float EditorMainWindow::GamePanelWidth = 0.55f;
const float EditorMainWindow::ScenePanelWidth = 0.18f;
const float EditorMainWindow::PropertiesPanelWidth = 1.0f - GamePanelWidth - ScenePanelWidth;
const float EditorMainWindow::ScenePanelHeight = 0.5f;
const float EditorMainWindow::GamePanelAspect = 16.0f / 9.0f;
const float EditorMainWindow::OutputPanelMaxHeight = 0.8f;

EditorMainWindow::EditorMainWindow()
    : drawImGuiTestWindow_(false),
    gamePanel_(resourceManager),
    outputPanel_(),
    debugPanel_(),
    scenePanel_(),
    resourcesPanel_(),
    propertiesPanel_()
{

}

void EditorMainWindow::resize(int width, int height)
{
    windowWidth_ = width;
    windowHeight_ = height;

    // Recompute size for each panel on editor main window
    gamePanel_.setSize(gamePanelRect());
    outputPanel_.setSize(outputPanelRect());
    debugPanel_.setSize(debugPanelRect());
    scenePanel_.setSize(scenePanelRect());
    resourcesPanel_.setSize(resourcesPanelRect());
    propertiesPanel_.setSize(propertiesPanelRect());
}

void EditorMainWindow::repaint()
{
    // Draw the top file/edit menu
    drawMainMenu();

    // If the user has selected the test window, display that instead.
    if (drawImGuiTestWindow_)
    {
        ImGui::ShowTestWindow();
        return;
    }

    // Draw each editor panel in the correct location
    drawPanel(gamePanel_);
    drawPanel(outputPanel_);
    drawPanel(debugPanel_);
    drawPanel(scenePanel_);
    drawPanel(resourcesPanel_);
    drawPanel(propertiesPanel_);
}

void EditorMainWindow::drawMainMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("Close");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::MenuItem("Close");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            resize(windowWidth_, windowHeight_);
            ImGui::MenuItem("Close");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Game"))
        {
            ImGui::MenuItem("Close");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Resources"))
        {
            ImGui::MenuItem("Close");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene"))
        {
            ImGui::MenuItem("Close");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Toggle ImGui Test"))
            {
                drawImGuiTestWindow_ = !drawImGuiTestWindow_;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

Rect EditorMainWindow::fullRect() const
{
    return Rect(0.0f, 0.0f, (float)windowWidth_, (float)windowHeight_);
}

Rect EditorMainWindow::mainMenuRect() const
{
    // Determine the height of the main menu
    ImGuiContext* g = ImGui::GetCurrentContext();
    float mainMenuHeight = g->FontBaseSize + g->Style.FramePadding.y * 2.0f + 1.0f;

    return Rect(0.0f, 0.0f, fullRect().width, mainMenuHeight).roundedToPixels();
}

Rect EditorMainWindow::gamePanelRect() const
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

Rect EditorMainWindow::outputPanelRect() const
{
    // Position below the game panel
    float minx = gamePanelRect().min().x;
    float miny = gamePanelRect().max().y;

    // Match half of the game panel width, and fit against the window bottom
    float width = gamePanelRect().width * 0.5f;
    float height = fullRect().height - gamePanelRect().max().y;

    return Rect(minx, miny, width, height).roundedToPixels();
}

Rect EditorMainWindow::debugPanelRect() const
{
    // Position to the right of the output panel
    float minx = outputPanelRect().max().x;
    float miny = outputPanelRect().min().y;

    // Fill the rest of the game panel width, and fit against the window bottom
    float width = gamePanelRect().max().x - outputPanelRect().max().x;
    float height = outputPanelRect().height;

    return Rect(minx, miny, width, height).roundedToPixels();
}

Rect EditorMainWindow::scenePanelRect() const
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

Rect EditorMainWindow::resourcesPanelRect() const
{
    // Position below the scene panel
    float minx = scenePanelRect().min().x;
    float miny = scenePanelRect().max().y;

    // Match the scene panel width, and fit against the window bottom
    float width = scenePanelRect().width;
    float height = fullRect().height - scenePanelRect().max().y;

    return Rect(minx, miny, width, height).roundedToPixels();
}

Rect EditorMainWindow::propertiesPanelRect() const
{
    // Position to the right of the scene panel and below the main menu
    float minx = scenePanelRect().max().x;
    float miny = mainMenuRect().max().y;

    // Fill the entire column, and fit against the right side of the window
    float width = fullRect().width - minx;
    float height = fullRect().height - miny;

    return Rect(minx, miny, width, height).roundedToPixels();
}

void EditorMainWindow::drawPanel(EditorPanel &panel)
{
    // Ensure the panel has borders and can't be moved
    // or collapsed by the user.
    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_ShowBorders;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoResize;

    // Get desired panel size
    const Rect size = panel.size();

    // Set the position and size of the panel.
    ImGui::SetNextWindowPos(ImVec2(size.minx, size.miny));
    ImGui::SetNextWindowSize(ImVec2(size.width, size.height));

    // Draw the panel
    ImGui::Begin(panel.name().c_str(), 0, flags);
    panel.draw();
    ImGui::End();
}