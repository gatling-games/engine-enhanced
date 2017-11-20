#pragma once

#include "Editor/EditorPanel.h"

#include "ResourceManager.h"

class ResourcesPanel : public EditorPanel
{
public:
    ResourcesPanel();

    // EditorPanel overrides
    virtual std::string name() const { return "Resources Panel"; }
    virtual void draw();

private:

    // Store the values to display in a tree structure
    struct TreeNode
    {
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
    void addToTree(const std::string &sourcePath);

    // Returns the parent node for the specified source path.
    TreeNode* getParentNode(const std::string &sourcePath);
};