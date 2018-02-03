#include "MainWindowMenu.h"

#include <imgui.h>
#include <vector>

// Used for path handling
#include <filesystem>
namespace fs = std::experimental::filesystem::v1;

MainWindowMenu::MainWindowMenu()
    : menuRoot_("")
{
    // Create the default root menu items to ensure they are in the right order
    findOrCreateMenuItem("File");
    findOrCreateMenuItem("Edit");
    findOrCreateMenuItem("View");
    findOrCreateMenuItem("Game");
    findOrCreateMenuItem("Resources");
    findOrCreateMenuItem("Scene");
}

void MainWindowMenu::addMenuItem(const std::string& path, MenuItemCallback callback)
{
    findOrCreateMenuItem(path)->callbacks.push_back(callback);
}

// Adds a new menu item
// This overload includes a second function, used to determine if the menu item
// is currently checked or unchecked.
void MainWindowMenu::addMenuItem(const std::string& path, MenuItemCallback callback, MenuItemCheckedCallback checkedCallback)
{
    findOrCreateMenuItem(path)->callbacks.push_back(callback);
    findOrCreateMenuItem(path)->checkedCallbacks.push_back(checkedCallback);
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

    // Determine the text that should appear on the menu item
    const std::string text = fs::path(path).filename().string();

    // Look for an existing node
    for (MenuItem &child : parentNode->children)
    {
        if (child.text == text)
        {
            return &child;
        }
    }

    // No existing node. Create a new one
    parentNode->children.push_back(MenuItem(text));
    return &parentNode->children.back();
}

void MainWindowMenu::drawItem(const MainWindowMenu::MenuItem &item) const
{
    // Determine if this is a single menu item, or a full menu root.
    bool isSingleItem = (item.children.empty() && !item.callbacks.empty());

    // If the item has no children, draw a simple menu item.
    bool checked = isItemChecked(item);
    if (isSingleItem && ImGui::MenuItem(item.text.c_str(), (const char*)0, checked))
    {
        itemPressed(item);
    }

    // If the item has children, draw a full menu.
    if (!isSingleItem && ImGui::BeginMenu(item.text.c_str()))
    {
        for (const MenuItem &child : item.children)
        {
            drawItem(child);
        }

        ImGui::EndMenu();
    }
}

bool MainWindowMenu::isItemChecked(const MainWindowMenu::MenuItem &item) const
{
    // Check if any checked callback is true
    for (const MenuItemCheckedCallback &callback : item.checkedCallbacks)
    {
        if (callback())
        {
            // The callback indicated the item is checked.
            return true;
        }
    }

    // Either no checked callback returned true, or there are no callbacks
    return false;
}

void MainWindowMenu::itemPressed(const MainWindowMenu::MenuItem &item) const
{
    // Call all callbacks registered on the menu item.
    for (const MenuItemCallback &callback : item.callbacks)
    {
        callback();
    }
}