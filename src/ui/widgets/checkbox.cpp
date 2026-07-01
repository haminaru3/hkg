#include "c_ui_widgets.hpp"
#include "../c_fonts.h"

struct s_checkbox_animation { float main = 0.f; };

struct s_checkbox_element {
    s_checkbox_animation anim;
    element_state_t state;
};

bool c_ui_widgets::checkbox(const char* label, bool* v) {
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_checkbox_element, 512> _element;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(label);
    ImGuiStyle* s = &ImGui::GetStyle();

    const ImVec2 label_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, label);

    const float w = ImGui::GetWindowWidth() - 30;
    const float square_sz = 17;
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect frame_bb(pos + ImVec2(w - square_sz - 13, 0), pos + ImVec2(w, square_sz - 1));
    const ImRect total_bb(pos, pos + ImVec2(square_sz + s->ItemInnerSpacing.x + label_size.x + 105, label_size.y)); //+ style.FramePadding.y * 2.0f

    ImGui::ItemSize(total_bb, s->FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id)) return false;

    _element.try_emplace(id, s_checkbox_element());
    auto this_element = _element.find(id);
    s_checkbox_animation* this_anim = &this_element->second.anim;
    element_state_t* this_state = &this_element->second.state;

    this_state->pressed = ImGui::ButtonBehavior(frame_bb, id, &this_state->hovered, &this_state->held);
    if (this_state->pressed) {
        *v = !(*v);
        ImGui::MarkItemEdited(id);
    }

    this_anim->main = ImLerp(this_anim->main, *v ? 1.0f : 0.0f, 0.10f * (1.0f - ImGui::GetIO().DeltaTime));

    ImGui::RenderNavHighlight(total_bb, id);

    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImColor(33 / 255.0f, 33 / 255.0f, 33 / 255.0f, this_anim->main * s->Alpha), false, 9.0f);
    window->DrawList->AddCircleFilled(ImVec2(frame_bb.Min.x + 9 + 13 * this_anim->main, frame_bb.Min.y + 8), 4.5f, ImColor(1.0f, 1.0f, 1.0f, 1.f * s->Alpha), 360);

    window->DrawList->AddRect(frame_bb.Min, frame_bb.Max, ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, 30 / 255.f * s->Alpha), 9.0f);

    if (label_size.x > 0.0f)
        window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 14.f, pos, ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.7f * s->Alpha), label);

    ui_cursor.change_cursor(this_state->hovered ? e_cursors::hand : e_cursors::default_c, (this_state->hovered));

    return this_state->pressed;
}
