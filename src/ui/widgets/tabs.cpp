#include "c_ui_widgets.hpp"
#include "../c_fonts.h"
#include "../../c_config.hpp"
#include "../interface_variant.hpp"

struct s_maintab_animation {
    float rect_op, text_op, line_op = 0.f;
};

struct s_maintab_element {
    s_maintab_animation anim;
    element_state_t state;
};

bool c_ui_widgets::s_ui_tabs::main(const char* icon, const char* label, bool boolean)
{
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_maintab_element, 128> _element;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(label);
    ImGuiStyle* s = &ImGui::GetStyle();

    const ImVec2 label_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, label);
    const ImVec2 icon_size = _calc_text_size(ui_fonts[e_fonts::ICONS14], 14, icon);
    ImVec2 pos = window->DC.CursorPos;

#if CORTEX_UI_VARIANT == 1
    const ImVec2 tab_size(100.f, 34.f);
#elif CORTEX_UI_VARIANT == 2
    const ImVec2 tab_size(54.f, 44.f);
#elif CORTEX_UI_VARIANT == 3
    const ImVec2 tab_size(96.f, 34.f);
#else
    const ImVec2 tab_size(145.f, 30.f);
#endif
    const ImRect rect(pos, ImVec2(pos.x + tab_size.x, pos.y + tab_size.y));
    ImGui::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x, rect.Max.y + 3), s->FramePadding.y);
    if (!ImGui::ItemAdd(rect, id))
        return false;

    _element.try_emplace(id, s_maintab_element());
    auto this_element = _element.find(id);
    s_maintab_animation* this_anim = &this_element->second.anim;
    element_state_t* this_state = &this_element->second.state;

    this_state->pressed = ImGui::ButtonBehavior(rect, id, &this_state->hovered, &this_state->held, NULL);

    this_anim->rect_op = ImLerp(this_anim->rect_op, (boolean ? 0.04f : this_state->hovered ? 0.01f : 0.0f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));
    this_anim->text_op = ImLerp(this_anim->text_op, (boolean ? 1.0f : this_state->hovered ? 0.5f : 0.3f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));
    this_anim->line_op = ImLerp(this_anim->line_op, (boolean ? 1.0f : 0.0f), 0.15f * (1.0f - ImGui::GetIO().DeltaTime));

    const bool styled = g_config->misc.menu_style != 0;
    ImColor active_col = ImColor(1.f, 1.f, 1.f, this_anim->line_op * s->Alpha);
    ImColor rect_col = ImColor(1.0f, 1.0f, 1.0f, this_anim->rect_op * s->Alpha);

    switch (g_config->misc.menu_style) {
    case 1:
        active_col = ImColor(1.f, 140 / 255.f, 0.f, this_anim->line_op * s->Alpha);
        rect_col = ImColor(49 / 255.f, 91 / 255.f, 132 / 255.f, (boolean ? 0.75f : this_anim->rect_op * 3.f) * s->Alpha);
        break;
    case 2:
        active_col = ImColor(1.f, 80 / 255.f, 100 / 255.f, this_anim->line_op * s->Alpha);
        rect_col = ImColor(84 / 255.f, 27 / 255.f, 36 / 255.f, (boolean ? 0.70f : this_anim->rect_op * 3.f) * s->Alpha);
        break;
    case 3:
        active_col = ImColor(74 / 255.f, 222 / 255.f, 128 / 255.f, this_anim->line_op * s->Alpha);
        rect_col = ImColor(31 / 255.f, 91 / 255.f, 68 / 255.f, (boolean ? 0.70f : this_anim->rect_op * 3.f) * s->Alpha);
        break;
    case 4:
        active_col = ImColor(238 / 255.f, 108 / 255.f, 35 / 255.f, this_anim->line_op * s->Alpha);
        rect_col = ImColor(54 / 255.f, 47 / 255.f, 39 / 255.f, (boolean ? 0.72f : this_anim->rect_op * 3.f) * s->Alpha);
        break;
    }

#if CORTEX_UI_VARIANT == 1
    active_col = ImColor(76 / 255.f, 210 / 255.f, 194 / 255.f, this_anim->line_op * s->Alpha);
    rect_col = ImColor(18 / 255.f, 34 / 255.f, 39 / 255.f, (boolean ? 0.78f : this_anim->rect_op * 4.f) * s->Alpha);
    window->DrawList->AddRectFilled(rect.Min, rect.Max, rect_col, 8.0f);
    window->DrawList->AddText(ui_fonts[e_fonts::ICONS14], 14.0f, ImVec2(rect.Min.x + 12, (rect.Min.y + rect.Max.y) / 2 - icon_size.y / 2),
        boolean ? active_col : ImColor(180 / 255.f, 210 / 255.f, 210 / 255.f, this_anim->text_op * s->Alpha), icon);
    window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 14.0f, ImVec2(rect.Min.x + 32, (rect.Min.y + rect.Max.y) / 2 - label_size.y / 2),
        ImColor(232 / 255.f, 248 / 255.f, 246 / 255.f, this_anim->text_op * s->Alpha), label);
    window->DrawList->AddRectFilled(ImVec2(rect.Min.x + 12, rect.Max.y - 3), ImVec2(rect.Max.x - 12, rect.Max.y - 1), active_col, 4.0f);
#elif CORTEX_UI_VARIANT == 2
    active_col = ImColor(242 / 255.f, 188 / 255.f, 67 / 255.f, this_anim->line_op * s->Alpha);
    rect_col = ImColor(34 / 255.f, 36 / 255.f, 39 / 255.f, (boolean ? 0.78f : this_anim->rect_op * 5.f) * s->Alpha);
    window->DrawList->AddRectFilled(rect.Min, rect.Max, rect_col, 8.0f);
    window->DrawList->AddText(ui_fonts[e_fonts::ICONS14], 15.0f, (rect.Min + rect.Max - icon_size) / 2,
        boolean ? active_col : ImColor(1.f, 1.f, 1.f, this_anim->text_op * s->Alpha), icon);
    window->DrawList->AddRectFilled(ImVec2(rect.Min.x, rect.Min.y + 8), ImVec2(rect.Min.x + 3, rect.Max.y - 8), active_col, 3.0f);
#elif CORTEX_UI_VARIANT == 3
    active_col = ImColor(1.f, 91 / 255.f, 41 / 255.f, this_anim->line_op * s->Alpha);
    rect_col = ImColor(42 / 255.f, 25 / 255.f, 24 / 255.f, (boolean ? 0.78f : this_anim->rect_op * 5.f) * s->Alpha);
    window->DrawList->AddRectFilled(rect.Min, rect.Max, rect_col, 10.0f);
    window->DrawList->AddText(ui_fonts[e_fonts::ICONS14], 14.0f, ImVec2(rect.Min.x + 10, (rect.Min.y + rect.Max.y) / 2 - icon_size.y / 2),
        boolean ? active_col : ImColor(1.f, 1.f, 1.f, this_anim->text_op * s->Alpha), icon);
    window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 14.0f, ImVec2(rect.Min.x + 30, (rect.Min.y + rect.Max.y) / 2 - label_size.y / 2),
        ImColor(1.f, 236 / 255.f, 224 / 255.f, this_anim->text_op * s->Alpha), label);
    window->DrawList->AddCircleFilled(ImVec2(rect.Max.x - 11, rect.Min.y + 11), 3.f, active_col, 12);
#else
    window->DrawList->AddRectFilled(rect.Min, rect.Max, rect_col, 4.0f); // bg rect
    window->DrawList->AddText(ui_fonts[e_fonts::ICONS14], 14.0f, ImVec2(rect.Min.x + 6, (rect.Min.y + rect.Max.y) / 2 - icon_size.y / 2),
        styled && boolean ? active_col : ImColor(1.0f, 1.0f, 1.0f, this_anim->text_op * s->Alpha), icon); // tab ico
    window->DrawList->AddText(ImVec2(rect.Min.x + 28, (rect.Min.y + rect.Max.y) / 2 - label_size.y / 2),
        ImColor(1.0f, 1.0f, 1.0f, this_anim->text_op * s->Alpha), label); // tab label
    window->DrawList->AddRectFilled(ImVec2(rect.Max.x - 2, rect.Min.y), ImVec2(rect.Max.x, rect.Max.y),
        active_col, 3.0f, 160); // ind line
#endif

    ui_cursor.change_cursor((this_state->hovered && boolean) ? e_cursors::default_c : this_state->hovered ? e_cursors::hand : e_cursors::default_c,
        (this_state->hovered));

    return this_state->pressed;
}

struct s_subtab_animation {
    float rect_op, text_op, line_op, rect_stroke_op = 0.f;
};

struct s_subtab_element {
    s_subtab_animation anim;
    element_state_t state;
};

bool c_ui_widgets::s_ui_tabs::sub(const char* label, bool boolean)
{
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_subtab_element, 128> _element;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(label);
    ImGuiStyle* s = &ImGui::GetStyle();

    const ImVec2 label_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, label);
    ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, ImVec2(pos.x + 20 + label_size.x, pos.y + 30));
    ImGui::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x, rect.Max.y + 5), s->FramePadding.y);
    if (!ImGui::ItemAdd(rect, id))
        return false;

    _element.try_emplace(id, s_subtab_element());
    auto this_element = _element.find(id);
    s_subtab_animation* this_anim = &this_element->second.anim;
    element_state_t* this_state = &this_element->second.state;

    this_state->pressed = ImGui::ButtonBehavior(rect, id, &this_state->hovered, &this_state->held, NULL);

    this_anim->rect_op = ImLerp(this_anim->rect_op, (boolean ? 0.04f : this_state->hovered ? 0.01f : 0.0f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));
    this_anim->line_op = ImLerp(this_anim->line_op, (boolean ? 1.0f : 0.0f), 0.15f * (1.0f - ImGui::GetIO().DeltaTime));
    this_anim->text_op = ImLerp(this_anim->text_op, (boolean ? 1.0f : this_state->hovered ? 0.5f : 0.3f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));
    this_anim->rect_stroke_op = ImLerp(this_anim->rect_stroke_op, (boolean ? 0.01f : this_state->hovered ? 0.05f : 0.02f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));

    ImColor active_col = ImColor(1.f, 1.f, 1.f, this_anim->line_op * s->Alpha);
    ImColor rect_col = ImColor(1.0f, 1.0f, 1.0f, this_anim->rect_op * s->Alpha);
    ImColor stroke_col = ImColor(1.0f, 1.0f, 1.0f, this_anim->rect_stroke_op * s->Alpha);

    switch (g_config->misc.menu_style) {
    case 1:
        active_col = ImColor(1.f, 140 / 255.f, 0.f, this_anim->line_op * s->Alpha);
        rect_col = ImColor(49 / 255.f, 91 / 255.f, 132 / 255.f, (boolean ? 0.70f : this_anim->rect_op * 3.f) * s->Alpha);
        stroke_col = ImColor(1.f, 140 / 255.f, 0.f, this_anim->rect_stroke_op * 3.f * s->Alpha);
        break;
    case 2:
        active_col = ImColor(1.f, 80 / 255.f, 100 / 255.f, this_anim->line_op * s->Alpha);
        rect_col = ImColor(84 / 255.f, 27 / 255.f, 36 / 255.f, (boolean ? 0.65f : this_anim->rect_op * 3.f) * s->Alpha);
        stroke_col = ImColor(1.f, 80 / 255.f, 100 / 255.f, this_anim->rect_stroke_op * 3.f * s->Alpha);
        break;
    case 3:
        active_col = ImColor(74 / 255.f, 222 / 255.f, 128 / 255.f, this_anim->line_op * s->Alpha);
        rect_col = ImColor(31 / 255.f, 91 / 255.f, 68 / 255.f, (boolean ? 0.65f : this_anim->rect_op * 3.f) * s->Alpha);
        stroke_col = ImColor(74 / 255.f, 222 / 255.f, 128 / 255.f, this_anim->rect_stroke_op * 3.f * s->Alpha);
        break;
    case 4:
        active_col = ImColor(238 / 255.f, 108 / 255.f, 35 / 255.f, this_anim->line_op * s->Alpha);
        rect_col = ImColor(54 / 255.f, 47 / 255.f, 39 / 255.f, (boolean ? 0.68f : this_anim->rect_op * 3.f) * s->Alpha);
        stroke_col = ImColor(238 / 255.f, 108 / 255.f, 35 / 255.f, this_anim->rect_stroke_op * 3.f * s->Alpha);
        break;
    }

    window->DrawList->AddRectFilled(rect.Min, rect.Max, 
        rect_col, 3.0f); // subtab bg
    window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 14.0f, (rect.Min + rect.Max - label_size) / 2,
        ImColor(1.0f, 1.0f, 1.0f, this_anim->text_op * s->Alpha), label); // subtab label
    window->DrawList->AddRect(rect.Min, rect.Max, 
        stroke_col, 3.0f); // rect outline

    window->DrawList->AddRectFilled(ImVec2((rect.Min.x + rect.Max.x - label_size.x) / 2, rect.Max.y - 2), ImVec2((rect.Min.x + rect.Max.x + label_size.x) / 2, rect.Max.y), 
        active_col, 7.0f, 48); // ind line

    ui_cursor.change_cursor((this_state->hovered && boolean) ? e_cursors::default_c : this_state->hovered ? e_cursors::hand : e_cursors::default_c,
        (this_state->hovered));

    ImGui::SameLine();
    return this_state->pressed;
}
