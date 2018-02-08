#pragma once

#include "Editor/EditorPanel.h"

#include "ResourceManager.h"
#include "Utils/Singleton.h"

class ResourcesPanel : public EditorPanel, public Singleton<ResourcesPanel>
{
public:
    ResourcesPanel();

    // EditorPanel overrides
    virtual std::string name() const { return "Resources Panel"; }
    virtual void draw();

    // Causes the tree of resources to be rebuilt on next draw.
    void clearTree();

private:

    // Store the values to display in a tree structure
    struct TreeNode
    {
        Resource* resource;
        std::string name;
        std::string sourcePath;
        std::vector<TreeNode> childNodes;
    };

    TreeNode resourceTreeRoot_;

    // Recreates the tree of resources in the project.
    void updateTree();

    // Draws the specified tree nodes, including child nodes.
    void drawNodes(const std::vector<TreeNode> &nodes);

    // Called when the user selects a resource displayed in the panel.
    void resourceSelected(const std::string &sourcePath);
    
    // Adds the specified source path to the tree.
    void addToTree(Resource* resource);

    // Returns the parent node for the specified source path.
    TreeNode* getParentNode(const std::string &sourcePath);
};