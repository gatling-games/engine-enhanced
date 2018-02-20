#include "ResourcesPanel.h"

#include <imgui.h>

#include "EditableObject.h"
#include "PropertiesPanel.h"
#include "ResourceManager.h"

#include <filesystem>
namespace fs = std::experimental::filesystem::v1;

ResourcesPanel::ResourcesPanel()
    : resourceTreeRoot_()
{

}

void ResourcesPanel::draw()
{
    // Recreate the tree when needed
    if (resourceTreeRoot_.childNodes.empty())
    {
        updateTree();
    }

    // Draw the root nodes in the view
    drawNodes(resourceTreeRoot_.childNodes);
}

void ResourcesPanel::clearTree()
{
    resourceTreeRoot_.childNodes.clear();
}

void ResourcesPanel::updateTree()
{
    // Delete the current tree
    resourceTreeRoot_.childNodes.clear();

    // (Re)add every source file to the tree
    for (Resource* resource : ResourceManager::instance()->loadedResourcesOfType<Resource>())
    {
        addToTree(resource);
    }
}

void ResourcesPanel::drawNodes(const std::vector<TreeNode> &nodes)
{
    for (auto node = nodes.begin(); node != nodes.end(); ++node)
    {
        // Draw a bullet for nodes with no children
        if (node->childNodes.empty())
        {
            bool selected = node->resource != nullptr && PropertiesPanel::instance()->current() == dynamic_cast<IEditableObject*>(node->resource);

            ImGui::Bullet();
            if (ImGui::Selectable(node->name.c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick))
            {
                resourceSelected(node->sourcePath);
            }
        }
        else
        {
            // Draw an expandable box for nodes with > 0 children
            if (ImGui::TreeNode(node._Ptr, node->name.c_str()))
            {
                drawNodes(node->childNodes);
                ImGui::TreePop();
            }
        }
    }
}

void ResourcesPanel::resourceSelected(const std::string &sourcePath)
{
    // Load the resource
    Resource* resource = ResourceManager::instance()->load<Resource>(sourcePath);

    IEditableObject* ieo = dynamic_cast<IEditableObject*>(resource);
    if (ieo != nullptr)
    {
        // If the object was double clicked, open it
        if (ImGui::IsMouseDoubleClicked(0))
        {
            ieo->onOpenAction();
        }
        else
        {
            // If the resource can be edited, display it.
            PropertiesPanel::instance()->inspect(ieo);
        }
    }
}

void ResourcesPanel::addToTree(Resource* resource)
{
    // Create the node for this value.
    TreeNode node;
    node.resource = resource;
    node.name = resource->resourceName();
    node.sourcePath = resource->resourcePath();

    // Add it to the correct place in the tree.
    getParentNode(node.sourcePath)->childNodes.push_back(node);
}

ResourcesPanel::TreeNode* ResourcesPanel::getParentNode(const std::string & sourcePath)
{
    // Get the path of the parent node
    fs::path parentPath = fs::path(sourcePath).parent_path();

    // If there is no parent for the parent, return the tree root
    if (!parentPath.has_parent_path())
    {
        return &resourceTreeRoot_;
    }

    // Otherwise get the parent node for the parent and look
    // for an existing node that matches the one we are looking for.
    std::string parentName = parentPath.filename().string();
    TreeNode* parentParent = getParentNode(parentPath.string());
    for (unsigned int i = 0; i < parentParent->childNodes.size(); ++i)
    {
        if (parentParent->childNodes[i].name == parentName)
        {
            return &parentParent->childNodes[i];
        }
    }

    // No existing node. Create a new one
    TreeNode node;
    node.name = parentName;
    node.sourcePath = parentPath.string();
    parentParent->childNodes.push_back(node);
    return &parentParent->childNodes.back();
}