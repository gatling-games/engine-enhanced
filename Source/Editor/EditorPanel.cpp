#include "EditorPanel.h"

#include "Math/Rect.h"

Rect EditorPanel::size() const
{
    return size_;
}


void EditorPanel::setSize(const Rect &rect)
{
    size_ = rect;

    // On resize callback for modifying displayed texture size
    onResize(rect.width, rect.height);
}
