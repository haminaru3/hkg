#include "c_ui_widgets.hpp"
#include "../c_fonts.h"

struct s_setspop_animation {
    float bg_op = 0.f, gear_op = 0.f,
        size = 0.f /* y axis */, gear_roll = 0.f;
    int rotation_idx;
};

struct s_setspop_element {
    s_setspop_animation anim;
    element_state_t state;
};

void imrotate_start(s_setspop_animation* animbase);
ImVec2 imrotate_center(s_setspop_animation* animbase);
void imrotate_end(float rad, ImVec2 center, s_setspop_animation* animbase);

bool c_ui_widgets::s_ui_popups::begin_settings(const char* popup_label, float x_size, bool label, ImVec2 custom_size_arg) {
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_setspop_element, 512> _element;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(popup_label);
    ImGuiStyle* s = &ImGui::GetStyle();

    if (label) { ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.7f * s->Alpha), popup_label); };
    bool alt_draw = (custom_size_arg.x != 24 && custom_size_arg.y != 16) ? true : false;
    ImVec2 icon_size = _calc_text_size(ui_fonts[e_fonts::ICONS18], 18, "H");
    const float width = (ImGui::GetContentRegionMax().x - ImGui::GetStyle().WindowPadding.x - 24);
    ImVec2 pos = window->DC.CursorPos + ImVec2(width, 0) - ImVec2(label ? 0 : 34, 26);

    bool opened = ImGui::IsPopupOpen(popup_label);

    const ImRect rect(pos, ImVec2(pos.x + custom_size_arg.x, pos.y + custom_size_arg.y));
    ImGui::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x, rect.Max.y), 6);
    if (!ImGui::ItemAdd(rect, id))
        return false;

    _element.try_emplace(id, s_setspop_element());
    auto this_element = _element.find(id);
    s_setspop_animation* this_anim = &this_element->second.anim;
    element_state_t* this_state = &this_element->second.state;

    this_state->pressed = ImGui::ButtonBehavior(rect, id, &this_state->hovered, &this_state->held, NULL);

    this_anim->size = ImLerp(this_anim->size, (opened ? 1.0f : 0.0f), ImGui::GetIO().DeltaTime * 12.f);
    this_anim->bg_op = ImLerp(this_anim->bg_op, (this_anim->size > 0.5f) ? (opened ? 1.0f : this_state->hovered ? 0.5f : 0.2f) : 0.f, 0.2f * (1.0f - ImGui::GetIO().DeltaTime));
    this_anim->gear_op = ImLerp(this_anim->gear_op, (opened ? 0.8f : this_state->hovered ? 0.4f : 0.2f), ImGui::GetIO().DeltaTime * 15.f);
    this_anim->gear_roll = ImLerp(this_anim->gear_roll, opened ? 1.f : this_state->hovered ? -0.75f : -1.f, ImGui::GetIO().DeltaTime * 15.f);

    window->DrawList->AddRectFilled(rect.Min, rect.Max, ImColor(19 / 255.f, 19 / 255.f, 19 / 255.f, 1.f * s->Alpha), 3.0f);

    window->DrawList->PushClipRect(rect.Min + ImVec2(3, 3), rect.Max - ImVec2(3, 3), true);
    ///
    imrotate_start(this_anim);

    if (!alt_draw) window->DrawList->AddText(ui_fonts[e_fonts::ICONS18], 18.0f, ImVec2(rect.Min.x + 7, rect.Min.y + 2), ImColor(1.f, 1.f, 1.f, this_anim->gear_op * s->Alpha), "H");
    else window->DrawList->AddText(ui_fonts[e_fonts::ICONS18], 18.0f, ImVec2(rect.Min.x + (custom_size_arg.x / 2) - (icon_size.x / 2),
        rect.Min.y + (custom_size_arg.y / 2) - (icon_size.y / 2)), ImColor(1.f, 1.f, 1.f, this_anim->gear_op * s->Alpha), "H");

    imrotate_end(1.57f * this_anim->gear_roll, imrotate_center(this_anim), this_anim);
    ///
    window->DrawList->PopClipRect();

    window->DrawList->AddRect(rect.Min, rect.Max, ImColor(19 / 255.f, 19 / 255.f, 19 / 255.f, 1.f * s->Alpha), 3.0f);
    window->DrawList->AddRect(rect.Min - ImVec2(1, 1), rect.Max - ImVec2(1, 1), ImColor(19 / 255.f, 19 / 255.f, 19 / 255.f, 1.f * s->Alpha), 3.0f);
    if (!alt_draw) window->DrawList->AddRect(rect.Min, rect.Max, ImColor(1.f, 1.f, 1.f, 30 / 255.f * s->Alpha), 3.0f);
    else window->DrawList->AddRect(rect.Min, rect.Max, ImColor(1.f, 1.f, 1.f, 6 / 255.f * s->Alpha), 3.0f);
    if (!alt_draw) window->DrawList->AddRect(rect.Min + ImVec2(2, 2), rect.Max - ImVec2(2, 2), ImColor(1.f, 1.f, 1.f, 30 / 255.f * s->Alpha), 3.0f);

    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, 26));

    if (ImGui::IsItemClicked()) {
        ImGui::OpenPopup(popup_label);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.03f * this_anim->bg_op));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.7f * this_anim->bg_op));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(ImColor(19 / 255.f, 19 / 255.f, 19 / 255.f, 1.f * this_anim->bg_op)));

    ImGui::SetNextWindowSize(ImVec2(x_size * this_anim->size, 0), ImGuiCond_Always);

    bool popup_opened = false;
    if (ImGui::BeginPopup(popup_label)) {
        auto draw = ImGui::GetWindowDrawList();
        auto pos = ImGui::GetWindowPos();
        auto size = ImGui::GetWindowSize();

        draw->AddText(pos + ImVec2(16, 12), ImColor(1.f, 1.f, 1.f, 0.4f), popup_label);
        draw->AddLine(ImVec2(pos.x + 0, pos.y + 34), ImVec2(pos.x + size.x - 0, pos.y + 34), ImColor(255, 255, 255, 15));

        ImGui::SetCursorPosY(44);
        popup_opened = true;
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);

    if (!opened) {
        ui_cursor.change_cursor(this_state->hovered ? e_cursors::hand : e_cursors::default_c, (this_state->hovered));
    }


    return popup_opened;
}

void c_ui_widgets::s_ui_popups::end_settings() {
    // just wrapper
    ImGui::EndPopup();
}

//
//
//

void imrotate_start(s_setspop_animation* animbase)
{
    animbase->rotation_idx = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}
ImVec2 imrotate_center(s_setspop_animation* animbase)
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = animbase->rotation_idx; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
}
void imrotate_end(float rad, ImVec2 center, s_setspop_animation* animbase)
{
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = animbase->rotation_idx; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}
