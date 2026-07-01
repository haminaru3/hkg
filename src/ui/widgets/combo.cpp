#include "c_ui_widgets.hpp"
#include "imgui_helpers.hpp"
#include "../c_fonts.h"

struct s_combo_animation {
    float text_op = 0.f, arrow_op = 0.f,
        popup_size = 0.f, frame_size = 0.f;
};

struct s_combo_element {
    s_combo_animation anim;
    element_state_t state;
};

bool c_ui_widgets::begin_combo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_combo_element, 512> _element;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(label);
    ImGuiStyle* s = &ImGui::GetStyle();
    ImGuiContext& g = *GImGui;

    ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
    g.NextWindowData.ClearFlags();

    _element.try_emplace(id, s_combo_element());
    auto this_element = _element.find(id);
    s_combo_animation* this_anim = &this_element->second.anim;
    element_state_t* this_state = &this_element->second.state;

    const float size = ImGui::GetWindowWidth() - 30;
    const ImVec2 label_size = ImGui::CalcTextSize(label);
    const ImVec2 prev_val_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 12, preview_value);
    this_anim->frame_size = ImLerp(this_anim->frame_size, prev_val_size.x, ImGui::GetIO().DeltaTime * 10.f);

    const ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(size, 18));
    const ImRect clickable(window->DC.CursorPos + ImVec2(size - this_anim->frame_size - 38, 0), window->DC.CursorPos + ImVec2(size, 18));

    const ImVec2 arrow_size = _calc_text_size(ui_fonts[e_fonts::ARROW], 8, "O");

    ImGui::ItemSize(rect, s->FramePadding.y);
    if (!ImGui::ItemAdd(clickable, id, &rect))
        return false;

    this_state->pressed = ImGui::ButtonBehavior(clickable, id, &this_state->hovered, &this_state->held);
    const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
    bool popup_open = ImGui::IsPopupOpen(popup_id, ImGuiPopupFlags_None);
    if (this_state->pressed && !popup_open)
    {
        ImGui::OpenPopupEx(popup_id, ImGuiPopupFlags_None);
        popup_open = true;
    }

    this_anim->popup_size = ImLerp(this_anim->popup_size, popup_open ? 1.0f : 0.0f, 0.04f * (1.0f - ImGui::GetIO().DeltaTime));
    this_anim->text_op = ImLerp(this_anim->text_op, popup_open ? 0.8f : 0.7f, ImGui::GetIO().DeltaTime * 15.f);
    this_anim->arrow_op = ImLerp(this_anim->arrow_op, popup_open ? 1.f : 0.7f, ImGui::GetIO().DeltaTime * 15.f);

    window->DrawList->AddRect(clickable.Min, clickable.Max, ImColor(1.0f, 1.0f, 1.0f, 0.03f * s->Alpha), 5.0f);
    window->DrawList->AddText(rect.Min + ImVec2(0, 9 - label_size.y / 2), ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.7f * s->Alpha), label);

    window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 12.f, clickable.Min + ImVec2(9, 8 - prev_val_size.y / 2), 
        ImColor(1.0f, 1.0f, 1.0f, this_anim->text_op * s->Alpha), preview_value);
    window->DrawList->AddText(ui_fonts[e_fonts::ARROW], 8.f, ImVec2(clickable.Max.x - 18, ((clickable.Min.y + clickable.Max.y) / 2 - arrow_size.y / 2) - 1.f),
        ImColor(1.0f, 1.0f, 1.0f, this_anim->arrow_op * s->Alpha), "O");

    if (!popup_open)
        return false;

    g.NextWindowData.Flags = backup_next_window_data_flags;
    if (!ImGui::IsPopupOpen(popup_id, ImGuiPopupFlags_None))
    {
        g.NextWindowData.ClearFlags();
        return false;
    }

    imgui_helpers::update_width();
    imgui_helpers::set_target_width(clickable.GetWidth() + 12);

    float w = imgui_helpers::combo_cur_w;

    {
        if ((flags & ImGuiComboFlags_HeightMask_) == 0)
            flags |= ImGuiComboFlags_HeightRegular;
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_)); // Only one
        int popup_max_height_in_items = -1;
        if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
        else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
        else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
        ImGui::SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, imgui_helpers::calc_max_popup_height(popup_max_height_in_items) * this_anim->popup_size));
    }

    char name[16];
    ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

    if (ImGuiWindow* popup_window = ImGui::FindWindowByName(name)) {
        if (popup_window->WasActive)
        {
            ImVec2 size_expected = ImGui::CalcWindowNextAutoFitSize(popup_window);
            popup_window->AutoPosLastDirection = ImGuiDir_Left;
            ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup_window);
            ImVec2 pos = ImGui::FindBestWindowPosForPopupEx(clickable.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, clickable, ImGuiPopupPositionPolicy_ComboBox);
            ImGui::SetNextWindowPos(pos + ImVec2(0, 3));
        }
    }

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.03f));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(ImColor(19, 19, 19, 255)));
    bool ret = ImGui::Begin(name, NULL, window_flags | ImGuiWindowFlags_NoScrollbar);
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
    if (!ret)
    {
        ImGui::EndPopup();
        return false;
    }

    ui_cursor.change_cursor(this_state->hovered ? e_cursors::hand : e_cursors::default_c, (this_state->hovered));

    return true;
}

bool c_ui_widgets::combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
{
    ImGuiContext& g = *GImGui;
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        items_getter(data, *current_item, &preview_value);
    else if (items_count > 0) {
        *current_item = 0;
        items_getter(data, *current_item, &preview_value);
    }

    if (!preview_value)
        preview_value = "???";

    if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, imgui_helpers::calc_max_popup_height(popup_max_height_in_items)));

    if (!begin_combo(label, preview_value, ImGuiComboFlags_None))
        return false;

    bool value_changed = false;
    for (int i = 0; i < items_count; i++)
    {
        ImGui::PushID(i);
        const bool item_selected = (i == *current_item);
        const char* item_text;
        if (!items_getter(data, i, &item_text))
            item_text = "???";
        if (selectable(item_text, item_selected))
        {
            value_changed = true;
            *current_item = i;
        }
        if (item_selected)
            ImGui::SetItemDefaultFocus();
        ImGui::PopID();
    }

    ImGui::EndPopup();

    if (value_changed)
        ImGui::MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

bool c_ui_widgets::combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
    const bool _v = combo(label, current_item, imgui_helpers::items_array_getter, (void*)items, items_count, height_in_items);
    return _v;
}
