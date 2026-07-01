#include "c_ui_widgets.hpp"
#include "../c_fonts.h"
#include "../../c_config.hpp"
#include "../interface_variant.hpp"
#include <map>
#include <fixed_containers/fixed_map.hpp>

struct s_ui_child {
    float avail_x, avail_y; int place_idx;
};

static fixed_containers::FixedMap<ImGuiID, s_ui_child, 256> ui_childs;

bool c_ui_widgets::s_ui_child::begin_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, ImGuiWindowFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* parent_window = g.CurrentWindow;
    ImGuiStyle* s = &ImGui::GetStyle();

    flags |= ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_ChildWindow | 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysUseWindowPadding;

    const ImVec2 content_avail = ImGui::GetContentRegionAvail();
    ImVec2 size = ImFloor(size_arg);
    const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
    if (size.x <= 0.0f)
        size.x = ImMax(content_avail.x + size.x, 4.0f);
    if (size.y <= 0.0f)
        size.y = ImMax(content_avail.y + size.y, 4.0f);

    ImGui::SetNextWindowPos(ImVec2(parent_window->DC.CursorPos.x, parent_window->DC.CursorPos.y + 20.0f));
    ImGui::SetNextWindowSize(size - ImVec2(0, 20));

    ImColor child_bg = ImColor(19 / 255.f, 19 / 255.f, 19 / 255.f, s->Alpha);
    ImColor child_head = child_bg;
    ImColor child_border = ImColor(1.0f, 1.0f, 1.0f, 0.04f * s->Alpha);
    ImColor child_title = ImColor(77 / 255.f, 77 / 255.f, 77 / 255.f, s->Alpha);

#if CORTEX_UI_VARIANT == 1
    child_bg = ImColor(8 / 255.f, 18 / 255.f, 22 / 255.f, 0.78f * s->Alpha);
    child_head = ImColor(16 / 255.f, 31 / 255.f, 36 / 255.f, 0.88f * s->Alpha);
    child_border = ImColor(76 / 255.f, 210 / 255.f, 194 / 255.f, 0.18f * s->Alpha);
    child_title = ImColor(220 / 255.f, 255 / 255.f, 248 / 255.f, 0.86f * s->Alpha);
#elif CORTEX_UI_VARIANT == 2
    child_bg = ImColor(18 / 255.f, 19 / 255.f, 21 / 255.f, 0.92f * s->Alpha);
    child_head = ImColor(28 / 255.f, 30 / 255.f, 32 / 255.f, 0.96f * s->Alpha);
    child_border = ImColor(242 / 255.f, 188 / 255.f, 67 / 255.f, 0.16f * s->Alpha);
    child_title = ImColor(248 / 255.f, 224 / 255.f, 171 / 255.f, 0.84f * s->Alpha);
#elif CORTEX_UI_VARIANT == 3
    child_bg = ImColor(16 / 255.f, 12 / 255.f, 15 / 255.f, 0.58f * s->Alpha);
    child_head = ImColor(28 / 255.f, 16 / 255.f, 17 / 255.f, 0.62f * s->Alpha);
    child_border = ImColor(1.f, 91 / 255.f, 41 / 255.f, 0.22f * s->Alpha);
    child_title = ImColor(255 / 255.f, 224 / 255.f, 210 / 255.f, 0.88f * s->Alpha);
#else
    switch (g_config->misc.menu_style) {
    case 1:
        child_bg = ImColor(18 / 255.f, 51 / 255.f, 88 / 255.f, s->Alpha);
        child_head = ImColor(35 / 255.f, 75 / 255.f, 115 / 255.f, s->Alpha);
        child_border = ImColor(1.f, 140 / 255.f, 0.f, 0.22f * s->Alpha);
        child_title = ImColor(220 / 255.f, 235 / 255.f, 1.f, 0.85f * s->Alpha);
        break;
    case 2:
        child_bg = ImColor(48 / 255.f, 18 / 255.f, 25 / 255.f, s->Alpha);
        child_head = ImColor(80 / 255.f, 25 / 255.f, 35 / 255.f, s->Alpha);
        child_border = ImColor(1.f, 90 / 255.f, 110 / 255.f, 0.20f * s->Alpha);
        child_title = ImColor(1.f, 210 / 255.f, 218 / 255.f, 0.80f * s->Alpha);
        break;
    case 3:
        child_bg = ImColor(17 / 255.f, 47 / 255.f, 38 / 255.f, s->Alpha);
        child_head = ImColor(26 / 255.f, 74 / 255.f, 56 / 255.f, s->Alpha);
        child_border = ImColor(74 / 255.f, 222 / 255.f, 128 / 255.f, 0.20f * s->Alpha);
        child_title = ImColor(190 / 255.f, 245 / 255.f, 210 / 255.f, 0.82f * s->Alpha);
        break;
    case 4:
        child_bg = ImColor(25 / 255.f, 24 / 255.f, 23 / 255.f, 0.74f * s->Alpha);
        child_head = ImColor(38 / 255.f, 36 / 255.f, 34 / 255.f, 0.72f * s->Alpha);
        child_border = ImColor(238 / 255.f, 108 / 255.f, 35 / 255.f, 0.30f * s->Alpha);
        child_title = ImColor(255 / 255.f, 232 / 255.f, 199 / 255.f, 0.84f * s->Alpha);
        break;
    }
#endif

    const float child_rounding =
#if CORTEX_UI_VARIANT == 1
        10.0f;
#elif CORTEX_UI_VARIANT == 2
        3.0f;
#elif CORTEX_UI_VARIANT == 3
        12.0f;
#else
        4.0f;
#endif

    parent_window->DrawList->AddRectFilled(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, size.y), child_bg, child_rounding);
    parent_window->DrawList->AddRect(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, size.y), child_border, child_rounding);
    parent_window->DrawList->AddRectFilled(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, 24), child_head, child_rounding, ImDrawCornerFlags_Top);
    parent_window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 14.0f, parent_window->DC.CursorPos + ImVec2(16, 4), child_title, name); // 8x

    auto label_size_px = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, name);

    parent_window->DrawList->AddRect(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, 24), child_border, child_rounding, ImDrawCornerFlags_Top);

    const char* temp_window_name;
    ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);

    bool ret = ImGui::Begin(temp_window_name, NULL, flags);
    ImGuiWindow* child_window = g.CurrentWindow;
    child_window->ChildId = id;

    if (child_window->BeginCount == 1)
        parent_window->DC.CursorPos = child_window->Pos;

    if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavHasScroll))
    {
        ImGui::FocusWindow(child_window);
        ImGui::NavInitWindow(child_window, false);
        ImGui::SetActiveID(id + 1, child_window);
        g.ActiveIdSource = ImGuiInputSource_Nav;
    }
    return ret;
}

bool c_ui_widgets::s_ui_child::begin(const char* str_id, const ImVec2& size_arg, ImGuiWindowFlags extra_flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 11));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.f);

    return begin_child_ex(str_id, window->GetID(str_id), (size_arg.x == 0) ? ImVec2(CORTEX_COL_W, size_arg.y) : size_arg, extra_flags);
    // in one day ill fix it, but not now
    // *upd 12.08.24: still shit, id have sanity rn to fix it
}

void c_ui_widgets::s_ui_child::end()
{
    // just fully pastd 4rom imgui cuz idk
    // todo: add spacing on end (child auto-sort)

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    g.WithinEndChild = true;
    if (window->BeginCount > 1)
    {
        ImGui::End();
    }
    else
    {
        ImVec2 sz = window->Size;
        if (window->AutoFitChildAxises & (1 << ImGuiAxis_X))
            sz.x = ImMax(4.0f, sz.x);
        if (window->AutoFitChildAxises & (1 << ImGuiAxis_Y))
            sz.y = ImMax(4.0f, sz.y);

        ImGui::End();

        ImGuiWindow* parent_window = g.CurrentWindow;
        ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
        ImGui::ItemSize(sz);
        if ((window->DC.NavLayersActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & ImGuiWindowFlags_NavFlattened))
        {
            ImGui::ItemAdd(bb, window->ChildId);
            ImGui::RenderNavHighlight(bb, window->ChildId);

            if (window->DC.NavLayersActiveMask == 0 && window == g.NavWindow)
                ImGui::RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_TypeThin);
        }
        else { ImGui::ItemAdd(bb, 0); }
        if (g.HoveredWindow == window)
            g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
    }

    ImGui::PopStyleVar(3);
    g.WithinEndChild = false;
    g.LogLinePosY = -FLT_MAX;


}
