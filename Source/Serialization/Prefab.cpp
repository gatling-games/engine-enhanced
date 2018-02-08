#include "Prefab.h"

#include <imgui.h>

#include "SceneManager.h"

Prefab::Prefab(ResourceID resourceID)
    : Resource(resourceID),
    gameObject_("Prefab")
{

}

void Prefab::drawEditor()
{
    gameObject_.drawEditor();
}

void Prefab::serialize(PropertyTable &table)
{
    gameObject_.serialize(table);
}

