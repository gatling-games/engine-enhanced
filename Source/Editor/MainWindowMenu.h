#pragma once

#include <string>
#include <functional>
#include <vector>

#include "Utils/Singleton.h"

typedef std::function<void()> MenuItemCallback;
typedef std::function<bool()> MenuItemCheckedCallback;

class MainWindowMenu : public Singleton<MainWindowMenu>
{
    struct MenuItem
    {
        std::string text;
        std::vector<MenuItemCallback> callbacks;
        std::vector<MenuItemCheckedCallback> checkedCallbacks;
        std::vector<MenuItem> children;

        MenuItem(const std::string &text)
            : text(text)
        {

        }
    };

public:
    MainWindowMenu();
    
    // Adds a new menu item at the specified location.
    // Specify the path from the root of the menu, eg Edit/Undo
    // The callback function will be triggered whenever the menu item is clicked.
    void addMenuItem(const std::string& path, MenuItemCallback callback);

    // Adds a new menu item
    // This overload includes a second function, used to determine if the menu item
    // is currently checked or unchecked.
    void addMenuItem(const std::string& path, MenuItemCallback callback, MenuItemCheckedCallback checkedCallback);

    // Draws the menu using imgui
    void draw() const;

private:
    // Place all menu items in a dummy root object
    MenuItem menuRoot_;

    // Finds or creates a menu item with the given path
    MenuItem* findOrCreateMenuItem(const std::string& path);

    // Recursively draws a menu item and its children.
    // Menu callbacks will be triggered if an item is clicked.
    void drawItem(const MenuItem &item) const;

    // Calls a menu item's checked callback to determine if it is currently checked.
    bool isItemChecked(const MenuItem &item) const;

    // Triggers callbacks when a menu item is pressed.
    void itemPressed(const MenuItem &item) const;
};
