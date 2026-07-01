#include "c_ui_widgets.hpp"
#include "../c_fonts.h"

static const char* slider_string_format(const char* fmt)
{
    if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0)
        return "%d";
    const char* fmt_start = ImParseFormatFindStart(fmt);
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);
    if (fmt_end > fmt_start && fmt_end[-1] == 'f')
    {
        if (fmt_start == fmt && fmt_end[0] == 0)
            return "%d";
        const char* tmp_format;
        ImFormatStringToTempBuffer(&tmp_format, NULL, "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end);
        return tmp_format;
    }
    return fmt;
}

struct s_slider_animation {
    float text_op = 0.f, x_pos = 0.f;
};

struct s_slider_element {
    s_slider_animation anim;
    element_state_t state;
};

bool c_ui_widgets::slider_scalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) {
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_slider_element, 512> _element;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(label);
    ImGuiStyle* s = &ImGui::GetStyle(); ImGuiContext& g = *GImGui;

    const float w = ImGui::GetWindowWidth() - 30;
    const ImVec2 label_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, label);
    const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y));
    const ImRect frame_bb(total_bb.Min + ImVec2(label_size.x + 30, 0), total_bb.Max);

    ImGui::ItemSize(total_bb, s->FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
        return false;

    _element.try_emplace(id, s_slider_element());
    auto this_element = _element.find(id);
    s_slider_animation* this_anim = &this_element->second.anim;
    element_state_t* this_state = &this_element->second.state;

    if (format == NULL)
        format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
    else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0)
        format = slider_string_format(format);

    this_state->hovered = ImGui::ItemHoverable(frame_bb, id);
    this_state->pressed = (this_state->hovered && g.IO.MouseClicked[0]);
    this_state->held = (ImGui::GetActiveID() == id && ImGui::IsAnyMouseDown());

    if (this_state->pressed)
    {
        ImGui::SetActiveID(id, window);
        ImGui::SetFocusID(id, window);
        ImGui::FocusWindow(window);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
    }

    this_anim->text_op = ImLerp(this_anim->text_op, this_state->held ? 1.0f : this_state->hovered ? 1.f : 0.7f, ImGui::GetIO().DeltaTime * 15.f);

    ImRect grab_bb;
    const bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
    if (value_changed)
        ImGui::MarkItemEdited(id);

    char value_buf[64];
    const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

    this_anim->x_pos = ((ImGui::IsItemActive() || this_state->hovered) && this_anim->x_pos != grab_bb.Min.x) ?
        ImLerp(this_anim->x_pos, grab_bb.Min.x, ImGui::GetIO().DeltaTime * 12.f) : grab_bb.Min.x;

    window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(3, 3), ImVec2(this_anim->x_pos, frame_bb.Max.y - 3), ImColor(25 / 255.f, 25 / 255.f, 25 / 255.f, 1.f * s->Alpha), 5.0f);
    window->DrawList->AddRect(frame_bb.Min - ImVec2(2, 2), frame_bb.Max + ImVec2(2, 2), ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.04f * s->Alpha), 5.0f);
    window->DrawList->AddRect(frame_bb.Min + ImVec2(1, 1), ImVec2(this_anim->x_pos, frame_bb.Max.y - 1), ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.04f * s->Alpha), 5.0f);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.7f)); {
        ImGui::RenderText(total_bb.Min, label);
    } ImGui::PopStyleColor();

    auto buf_size_px = ui_fonts[e_fonts::MEDIUM]->CalcTextSizeA(12.0f, FLT_MAX, 0.0f, value_buf, value_buf_end);
    auto buf_text_pos = (this_anim->x_pos + buf_size_px.x > total_bb.Max.x) ? ImVec2(total_bb.Max.x - buf_size_px.x - 4, total_bb.Min.y) : ImVec2(this_anim->x_pos, total_bb.Min.y);

    window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 12.0f, buf_text_pos, ImColor(1.0f, 1.0f, 1.0f, this_anim->text_op * s->Alpha), value_buf, value_buf_end);

    ui_cursor.change_cursor(this_state->held ? e_cursors::horizontal_resize : this_state->hovered ? e_cursors::hand : e_cursors::default_c,
        (this_state->held || this_state->hovered));

    return value_changed;
}

bool c_ui_widgets::slider_int(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return slider_scalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}
bool c_ui_widgets::slider_float(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return slider_scalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}
