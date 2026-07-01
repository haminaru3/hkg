#include "c_ui_widgets.hpp"
#include "../c_fonts.h"

#include <map>
#include <Windows.h>
#include <format>
#include <iostream>

struct s_selectable_animation {
    float label_op = 0.f, mark_op = 0.f;
};

struct s_itemselectable_animation {
    float label_op = 0.f, mark_op = 0.f, hash_pos = 0.f, hash_op = 0.f;
};

struct s_selectable_element {
    s_selectable_animation anim;
    element_state_t state;
};

struct s_itemselectable_element {
    s_itemselectable_animation anim;
    element_state_t state;
};

bool c_ui_widgets::selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_selectable_element, 512> _element;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(label);
    ImGuiStyle* s = &ImGui::GetStyle();
    ImGuiContext& g = *GImGui;

    ImVec2 label_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, label);
    ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
    ImVec2 pos = window->DC.CursorPos;
    pos.y += window->DC.CurrLineTextBaseOffset;
    ImGui::ItemSize(size, 0.0f);

    const float min_x = pos.x;
    const float max_x = window->WorkRect.Max.x;
    if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
        size.x = ImMax(label_size.x, max_x - min_x);

    const ImVec2 text_min = pos;
    const ImVec2 text_max(min_x + size.x, pos.y + size.y);

    ImRect bb(min_x, pos.y, text_max.x, text_max.y);
    if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
    {
        const float spacing_x = s->ItemSpacing.x;
        const float spacing_y = s->ItemSpacing.y;
        const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
        const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
        bb.Min.x -= spacing_L;
        bb.Min.y -= spacing_U;
        bb.Max.x += (spacing_x - spacing_L);
        bb.Max.y += (spacing_y - spacing_U);
    }

    const float backup_clip_rect_min_x = window->ClipRect.Min.x;
    const float backup_clip_rect_max_x = window->ClipRect.Max.x;

    const bool disabled_item = (flags & ImGuiSelectableFlags_Disabled) != 0;
    if (!ImGui::ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None))
        return false;

    _element.try_emplace(id, s_selectable_element());
    auto this_element = _element.find(id);
    s_selectable_animation* this_anim = &this_element->second.anim;
    element_state_t* this_state = &this_element->second.state;

    const bool disabled_global = (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
    if (disabled_item && !disabled_global)
        ImGui::BeginDisabled();

    const bool was_selected = selected;

    this_state->pressed = ImGui::ButtonBehavior(bb, id, &this_state->hovered, &this_state->held, 0);

    if (this_state->pressed || (this_state->hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
    {
        if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
        {
            ImGui::SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent, ImGui::WindowRectAbsToRel(window, bb));
            g.NavDisableHighlight = true;
        }
    }
    if (this_state->pressed)
        ImGui::MarkItemEdited(id);

    if (flags & ImGuiSelectableFlags_AllowItemOverlap)
        ImGui::SetItemAllowOverlap();

    if (selected != was_selected) //-V547
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

    // Render
    if (this_state->held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
        this_state->hovered = true;

    ImGui::RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);

    this_anim->label_op = ImLerp(this_anim->label_op, (selected ? 0.7f : this_state->hovered ? 0.5f : 0.2f), ImGui::GetIO().DeltaTime * 15.f);
    this_anim->mark_op = ImLerp(this_anim->mark_op, (selected ? 1.f : 0.f), ImGui::GetIO().DeltaTime * 12.f);

    //ImVec2 label_pos((selected) ? (text_min.x + (12.f * this_anim->mark_op)) : text_min.x, text_min.y);
    ImVec2 label_pos((text_min.x + (16.f * this_anim->mark_op)), text_min.y);

    window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 14, label_pos,
        ImColor(1.0f, 1.0f, 1.0f, this_anim->label_op * s->Alpha), label);

    if (selected) {
        ImGui::GetWindowDrawList()->AddText(ui_fonts[e_fonts::ICONS14], 8, text_min + ImVec2(2, 4), 
            ImColor(1.0f, 1.0f, 1.0f, this_anim->mark_op * s->Alpha), "M");
    }

    if (disabled_item && !disabled_global)
        ImGui::EndDisabled();

    ui_cursor.change_cursor(this_state->hovered ? e_cursors::hand : e_cursors::default_c, (this_state->hovered));

    return this_state->pressed;
}

bool c_ui_widgets::item_selectable(const char* label, const char* hash, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_itemselectable_element, 512> _element;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    char str_id[128];
    snprintf(str_id, sizeof(str_id), "%s%s", label, hash);
    const ImGuiID id = window->GetID(str_id);
    ImGuiStyle* s = &ImGui::GetStyle();
    ImGuiContext& g = *GImGui;

    ImVec2 label_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, label);
    ImVec2 hash_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, hash);
    ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
    ImVec2 pos = window->DC.CursorPos;
    pos.y += window->DC.CurrLineTextBaseOffset;
    ImGui::ItemSize(size, 0.0f);

    const float min_x = pos.x;
    const float max_x = window->WorkRect.Max.x;
    if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
        size.x = ImMax(label_size.x, max_x - min_x);

    const ImVec2 text_min = pos;
    const ImVec2 text_max(min_x + size.x, pos.y + size.y);

    ImRect bb(min_x, pos.y, text_max.x, text_max.y);
    if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
    {
        const float spacing_x = s->ItemSpacing.x;
        const float spacing_y = s->ItemSpacing.y;
        const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
        const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
        bb.Min.x -= spacing_L;
        bb.Min.y -= spacing_U;
        bb.Max.x += (spacing_x - spacing_L);
        bb.Max.y += (spacing_y - spacing_U);
    }

    const float backup_clip_rect_min_x = window->ClipRect.Min.x;
    const float backup_clip_rect_max_x = window->ClipRect.Max.x;

    const bool disabled_item = (flags & ImGuiSelectableFlags_Disabled) != 0;
    if (!ImGui::ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None))
        return false;

    _element.try_emplace(id, s_itemselectable_element());
    auto this_element = _element.find(id);
    s_itemselectable_animation* this_anim = &this_element->second.anim;
    element_state_t* this_state = &this_element->second.state;

    const bool disabled_global = (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
    if (disabled_item && !disabled_global)
        ImGui::BeginDisabled();

    const bool was_selected = selected;

    this_state->pressed = ImGui::ButtonBehavior(bb, id, &this_state->hovered, &this_state->held, 0);

    if (this_state->pressed || (this_state->hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
    {
        if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
        {
            ImGui::SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent, ImGui::WindowRectAbsToRel(window, bb));
            g.NavDisableHighlight = true;
        }
    }
    if (this_state->pressed)
        ImGui::MarkItemEdited(id);

    if (flags & ImGuiSelectableFlags_AllowItemOverlap)
        ImGui::SetItemAllowOverlap();

    if (selected != was_selected) //-V547
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

    // Render
    if (this_state->held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
        this_state->hovered = true;

    ImGui::RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);

    this_anim->label_op = ImLerp(this_anim->label_op, (selected ? 0.7f : this_state->hovered ? 0.5f : 0.2f), ImGui::GetIO().DeltaTime * 15.f);
    this_anim->mark_op = ImLerp(this_anim->mark_op, (selected ? 1.f : 0.f), ImGui::GetIO().DeltaTime * 12.f);

    this_anim->hash_pos = ImLerp(this_anim->hash_pos, (selected ? label_size.x : 0.f), ImGui::GetIO().DeltaTime * (selected ? 12.f : 16.f));
    this_anim->hash_op = ImLerp(this_anim->hash_op, (selected ? 0.5f : 0.0f), ImGui::GetIO().DeltaTime * (selected ? 12.f : 16.f));

    //ImVec2 label_pos((selected) ? (text_min.x + (12.f * this_anim->mark_op)) : text_min.x, text_min.y);
    ImVec2 label_pos((text_min.x + (16.f * this_anim->mark_op)), text_min.y);

    window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 14, label_pos,
        ImColor(1.0f, 1.0f, 1.0f, this_anim->label_op * s->Alpha), label);

    if (selected) {
        window->DrawList->AddText(ui_fonts[e_fonts::ICONS14], 8, text_min + ImVec2(2, 4),
            ImColor(1.0f, 1.0f, 1.0f, this_anim->mark_op * s->Alpha), "M");
    }

    ImGui::PushClipRect(label_pos + ImVec2(label_size.x + 6, 0), label_pos + label_size + ImVec2(6, 0) + hash_size, false);
    window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 14, label_pos + ImVec2(this_anim->hash_pos + 6, 0),
        ImColor(1.0f, 1.0f, 1.0f, this_anim->hash_op * s->Alpha), hash);
    ImGui::PopClipRect();

    if (disabled_item && !disabled_global)
        ImGui::EndDisabled();

    ui_cursor.change_cursor(this_state->hovered ? e_cursors::hand : e_cursors::default_c, (this_state->hovered));

    return this_state->pressed;
}


bool c_ui_widgets::selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
    if (selectable(label, *p_selected, flags, size_arg))
    {
        *p_selected = !*p_selected;
        return true;
    }
    return false;
}

bool c_ui_widgets::item_selectable(const char* label, const char* hash, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
    if (item_selectable(label, hash, *p_selected, flags, size_arg))
    {
        *p_selected = !*p_selected;
        return true;
    }
    return false;
}
