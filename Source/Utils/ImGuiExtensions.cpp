#include "ImGuiExtensions.h"

#include "Math/Vector3.h"

namespace ImGui
{
    bool QuaternionEdit(const char* label, Quaternion* quat)
    {
        Vector3 eulerAngles = Vector3::quat(*quat);

        // To avoid introducing extra error, only write to 
        // the quaternion when imgui says the value was changed
        bool changed = ImGui::DragFloat3(label, &eulerAngles.x, 0.2f, -360.0f, 360.0f);
        if (changed)
        {
            *quat = Quaternion::euler(eulerAngles);
        }

        return changed;
    }
}