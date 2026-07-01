#include "c_ui_widgets.hpp"
#include "../c_fonts.h"
#include "../../c_config.hpp"

struct s_button_animation {
    float bg = 0.f, text_op = 0.f;
};

struct s_button_element {
    s_button_animation anim;
    element_state_t state;
};

bool c_ui_widgets::button(const char* label, const ImVec2& size_arg)
{
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_button_element, 512> _element;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(label);
    ImGuiStyle* s = &ImGui::GetStyle();

    const ImVec2 label_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, label);
    const ImVec2 pos = window->DC.CursorPos;
    ImGuiContext& g = *GImGui;

    float x_width = ImGui::GetContentRegionMax().x - s->WindowPadding.x;
    ImVec2 size = ImGui::CalcItemSize((size_arg.x == 0) ? ImVec2(x_width, size_arg.y) : size_arg, label_size.x, label_size.y);

    const ImRect bb(pos, pos + size);

    ImGui::ItemSize(size, 0.f);
    if (!ImGui::ItemAdd(bb, id)) return false;

    _element.try_emplace(id, s_button_element());
    auto this_element = _element.find(id);
    s_button_animation* this_anim = &this_element->second.anim;
    element_state_t* this_state = &this_element->second.state;

    this_state->pressed = ImGui::ButtonBehavior(bb, id, &this_state->hovered, &this_state->pressed);

    this_anim->bg = ImLerp(this_anim->bg, ImGui::IsItemActive() ? 25.f : 19.f, g.IO.DeltaTime * 15.f);
    this_anim->text_op = ImLerp(this_anim->text_op, ImGui::IsItemActive() ? 0.4f : 0.7f, g.IO.DeltaTime * 15.f);

    float rect_acc = this_anim->bg / 255.f;

    const bool hokage_style = g_config && g_config->misc.menu_style == 4;
    window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(rect_acc, rect_acc, rect_acc, (hokage_style ? 0.58f : 1.f) * s->Alpha), 4);
    window->DrawList->AddRect(bb.Min, bb.Max, hokage_style
        ? ImColor(238 / 255.f, 108 / 255.f, 35 / 255.f, 0.34f * s->Alpha)
        : ImColor(1.f, 1.f, 1.f, 6 / 255.f * s->Alpha), 4);

    window->DrawList->AddText(ImVec2(bb.Min.x + (size.x - label_size.x) / 2,
        bb.Max.y - label_size.y - (size.y - label_size.y) / 2), ImColor(1.f, 1.f, 1.f, this_anim->text_op * s->Alpha), label);

    ui_cursor.change_cursor(this_state->hovered ? e_cursors::hand : e_cursors::default_c, (this_state->hovered));

    return this_state->pressed;
}

void c_ui_widgets::popup_key(const char* popul_label, const char* checkbox_label, bool* value_checkbox, int* value_keybind) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(popul_label);

    bool opened = ImGui::IsPopupOpen(popul_label);
    //itthis_anim->popup_opacity = ImLerp(itthis_anim->popup_opacity, (opened ? 1.f : 0.0f), 0.2f * (1.0f - ImGui::GetIO().DeltaTime));

    const ImVec2 label_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, checkbox_label);

    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right) && !opened)
    {
        ImGui::OpenPopup(popul_label);
    }

    ImGui::SetNextWindowSize(ImVec2(175, 52), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 16, 16 });
    if (ImGui::BeginPopup(popul_label))
    {
        keybind("Hotkey\0\0", value_keybind, true);
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(3);
}
