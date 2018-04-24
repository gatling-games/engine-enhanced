#include "PhysicsManager.h"

#include "Editor/MainWindowMenu.h"

PhysicsManager::PhysicsManager()
    : physicsDebugEnabled_(false)
{
    // Make a menu item for toggling physics debugging
    MainWindowMenu::instance()->addMenuItem(
        "View/Physics Debugging",
        [&] { physicsDebugEnabled_ = !physicsDebugEnabled_; },
        [&] { return physicsDebugEnabled_; }
    );
}
