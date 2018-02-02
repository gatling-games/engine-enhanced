#include "MainWindowMenu.h"

#include <imgui.h>
#include <vector>

// Used for path handling
#include <filesystem>
namespace fs = std::experimental::filesystem::v1;

MainWindowMenu::MainWindowMenu()
    : menuRoot_("")
{

}

void MainWindowMenu::addMenuItem(const std::string& path, MenuItemCallback callback)
{
    findOrCreateMenuItem(path)->callbacks.push_back(callback);
}

void MainWindowMenu::draw() const
{
    ImGui::BeginMainMenuBar();

    // Draw every menu in the menu bar.
    for (const MenuItem &menuItem : menuRoot_.children)
    {
        drawItem(menuItem);
    }

    ImGui::EndMainMenuBar();
}

MainWindowMenu::MenuItem* MainWindowMenu::findOrCreateMenuItem(const std::string& path)
{
    // First, find the parent node of the menu item we are looking for
    MenuItem* parentNode = &menuRoot_;
    if (fs::path(path).has_parent_path())
    {
        parentNode = findOrCreateMenuItem(fs::path(path).parent_path().string());
    }

    // Look for an existing node
    for (MenuItem &child : parentNode->children)
    {
        if (child.text == path)
        {
            return &child;
        }
    }

    // No existing node. Create a new one
    parentNode->children.push_back(MenuItem(fs::path(path).filename().string()));
    return &parentNode->children.back();
}

void MainWindowMenu::drawItem(const MainWindowMenu::MenuItem &item) const
{
    // If the item has no children, draw a simple menu item.
    if (item.children.empty() && ImGui::MenuItem(item.text.c_str()))
    {
        itemPressed(item);
    }

    // If the item has children, draw a full menu.
    if (!item.children.empty() && ImGui::BeginMenu(item.text.c_str()))
    {
        for (const MenuItem &child : item.children)
        {
            drawItem(child);
        }

        ImGui::EndMenu();
    }
}

void MainWindowMenu::itemPressed(const MainWindowMenu::MenuItem &item) const
{
    // Call all callbacks registered on the menu item.
    for (const MenuItemCallback &callback : item.callbacks)
    {
        callback();
    }
}