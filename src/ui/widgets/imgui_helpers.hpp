#pragma once
#include "imgui.h"

namespace imgui_helpers {
    float combo_cur_w = 0.0f;
    float combo_target_w = 0.0f;

    void set_target_width(float newTargetWidth) {
        combo_target_w = newTargetWidth;
    }

    void update_width() {
        float deltaTime = ImGui::GetIO().DeltaTime;
        float interpolationSpeed = 10.0f;
        combo_cur_w = ImLerp(combo_cur_w, combo_target_w, interpolationSpeed * deltaTime);
    }

    static float calc_max_popup_height(int items_count)
    {
        ImGuiContext& g = *GImGui;
        if (items_count <= 0)
            return FLT_MAX;
        return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
    }

    static bool items_array_getter(void* data, int idx, const char** out_text)
    {
        const char* const* items = (const char* const*)data;
        if (out_text)
            *out_text = items[idx];
        return true;
    }
}