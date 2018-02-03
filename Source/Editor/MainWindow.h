#pragma once

#include "Math/Rect.h"

#include "MainWindowMenu.h"
#include "EditorPanel.h"
#include "GamePanel.h"
#include "OutputPanel.h"
#include "ScenePanel.h"
#include "ResourcesPanel.h"
#include "PropertiesPanel.h"

class MainWindow
{
private:
    static const float GamePanelWidth;
    static const float ScenePanelWidth;
    static const float PropertiesPanelWidth;
    static const float ScenePanelHeight;
    static const float GamePanelAspect;
    static const float OutputPanelMaxHeight;

public:
    MainWindow();

    // Pointers to each editor panel
    GamePanel* gamePanel() { return &gamePanel_; }
    OutputPanel* outputPanel() { return &outputPanel_; }
    ScenePanel* scenePanel() { return &scenePanel_; }
    ResourcesPanel* resourcesPanel() { return &resourcesPanel_; }
    PropertiesPanel* propertiesPanel() { return &propertiesPanel_; }

    // Rearranges windows when the window size changes.
    void resize(int width, int height);

    // Redraws the window.
    void repaint();

private:
    // Window details
    int windowWidth_;
    int windowHeight_;

    // Toggles between drawing the window and the imgui test window.
    bool drawImGuiTestWindow_;

    // The menu at the top of the window.
    MainWindowMenu mainMenu_;

    // Panels in the window
    GamePanel gamePanel_;
    OutputPanel outputPanel_;
    ScenePanel scenePanel_;
    ResourcesPanel resourcesPanel_;
    PropertiesPanel propertiesPanel_;

    // Regions for each editor panel.
    Rect fullRect() const;
    Rect mainMenuRect() const;
    Rect gamePanelRect() const;
    Rect outputPanelRect() const;
    Rect scenePanelRect() const;
    Rect resourcesPanelRect() const;
    Rect propertiesPanelRect() const;

    // Draws a panel in the specified screen location.
    void drawPanel(EditorPanel &panel, const Rect &rect);
};