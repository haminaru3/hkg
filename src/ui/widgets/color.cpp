#include "c_ui_widgets.hpp"
#include "../c_fonts.h"

ImColor colorpicker_buffer = ImColor(255, 255, 255, 255);

struct s_colorbtn_animation { float outline_op = 0.f; };

struct s_colorbtn_element {
    s_colorbtn_animation anim;
    element_state_t state;
};

bool color_btn(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags)
{
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_colorbtn_element, 255> _element;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(desc_id);
    ImGuiStyle* s = &ImGui::GetStyle();

    const ImVec2 pos = window->DC.CursorPos;
    const float width = ImGui::GetContentRegionMax().x - ImGui::GetStyle().WindowPadding.x + 2;
    const ImRect rect(pos, pos + ImVec2(width, 16));
    const ImRect clickable(pos + ImVec2(width - 24, 3), pos + ImVec2(width, 18));
    ImGui::ItemSize(ImRect(rect.Min, rect.Max + ImVec2(0, 3)));
    if (!ImGui::ItemAdd(rect, id)) return false;

    _element.try_emplace(id, s_colorbtn_element());
    auto this_element = _element.find(id);
    s_colorbtn_animation* this_anim = &this_element->second.anim;
    element_state_t* this_state = &this_element->second.state;

    this_state->pressed = ImGui::ButtonBehavior(clickable, id, &this_state->hovered, &this_state->held);

    if (flags & ImGuiColorEditFlags_NoAlpha)
        flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

    ImVec4 col_rgb = col;
    if (flags & ImGuiColorEditFlags_InputHSV)
        ImGui::ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

    ImColor main_col = ImGui::GetColorU32(col_rgb);
    ImColor halftone_col = IM_COL32(
        static_cast<int>(col_rgb.x * 255.0f / 2.f),
        static_cast<int>(col_rgb.y * 255.0f / 2.f),
        static_cast<int>(col_rgb.z * 255.0f / 2.f),
        static_cast<int>(col_rgb.w * 255.0f)
    );
    ImColor halftone_col_2 = IM_COL32(
        static_cast<int>(col_rgb.x * 255.0f / 2.5f),
        static_cast<int>(col_rgb.y * 255.0f / 2.5f),
        static_cast<int>(col_rgb.z * 255.0f / 2.5f),
        static_cast<int>(col_rgb.w * 255.0f)
    );

    // ontline
    window->DrawList->AddRect(clickable.Min, clickable.Max, ImColor(19 / 255.f, 19 / 255.f, 19 / 255.f, 1.f * s->Alpha), 3.0f);
    // checkerboard bg
    ImGui::RenderColorRectWithAlphaCheckerboard(window->DrawList, clickable.Min + ImVec2(3, 3), clickable.Max - ImVec2(3, 3), ImColor(0, 0, 0, 0), 3, ImVec2(0.f, 0.f), 2);
    // main col
    window->DrawList->AddRectFilledMultiColorRounded(clickable.Min + ImVec2(3, 3), clickable.Max - ImVec2(3, 3), ImColor(19, 19, 19),
        main_col, main_col, halftone_col_2, halftone_col, 2.f, ImDrawCornerFlags_All);

    // idk
    window->DrawList->AddRect(clickable.Min, clickable.Max, ImColor(1.f, 1.f, 1.f, 30 / 255.f * s->Alpha), 3.0f);

    ui_cursor.change_cursor(this_state->hovered ? e_cursors::hand : e_cursors::default_c, (this_state->hovered));
    return this_state->pressed;
}

struct s_colorpicker_animation {
    float sv_circle_rad; ImVec2 sv_circle_move;
    float hue_circle_move; float hue_circle_rad;
    float alpha_circle_move; float alpha_circle_rad;
};

struct s_colorpicker_element {
    s_colorpicker_animation anim;
    element_state_t state;
};

bool color_picker(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
{
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_colorpicker_element, 255> _element;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(label);

    ImDrawList* draw_list = window->DrawList;
    ImGuiStyle& style = g.Style;
    ImGuiIO& io = g.IO;

    const float width = ImGui::CalcItemWidth();
    g.NextItemData.ClearFlags();

    ImGui::PushID(label);
    ImGui::BeginGroup();

    flags |= ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_AlphaBar;

    // Setup
    int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
    bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
    ImVec2 picker_pos = window->DC.CursorPos + ImVec2(8, 8);
    ImVec2 bar_pos = window->DC.CursorPos + ImVec2(0, 196);
    float square_sz = ImGui::GetFrameHeight();
    float bars_width = 166.f;
    float sv_rect_size = 166.f;
    float sv_picker_size = ImMax(bars_width * 1, width - (alpha_bar ? 2 : 1) * (bars_width + style.ItemInnerSpacing.x)) + 14; // Saturation/Value picking box
    float sv_bar_size = 18; // Saturation/Value picking box
    float bar0_pos_x = ImGui::GetWindowPos().x + 18;
    float bar1_pos_x = bar0_pos_x;
    float bars_triangles_half_sz = IM_FLOOR(bars_width * 0.20f);

    float backup_initial_col[4];
    memcpy(backup_initial_col, col, components * sizeof(float));

    float H = col[0], S = col[1], V = col[2];
    float R = col[0], G = col[1], B = col[2];
    if (flags & ImGuiColorEditFlags_InputRGB)
    {
        // Hue is lost when converting from greyscale rgb (saturation=0). Restore it.
        ImGui::ColorConvertRGBtoHSV(R, G, B, H, S, V);
    }
    else if (flags & ImGuiColorEditFlags_InputHSV)
    {
        ImGui::ColorConvertHSVtoRGB(H, S, V, R, G, B);
    }

    bool value_changed = false, value_changed_h = false, value_changed_sv = false;

    ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);

    // SV rectangle logic
    ImGui::InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));
    if (ImGui::IsItemActive())
    {
        S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
        V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size));

        // Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
        //if (g.ColorEditSavedColor == ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
        //    H = g.ColorEditSavedHue;
        value_changed = value_changed_sv = true;
    }

    // Hue bar logic
    ImGui::SetCursorScreenPos(ImVec2(bar0_pos_x, bar_pos.y));
    ImGui::InvisibleButton("hue", ImVec2(bars_width, sv_bar_size));
    if (ImGui::IsItemActive())
    {
        H = 1.f - ImSaturate((io.MousePos.x - bar_pos.x) / (bars_width - 1));
        ui_cursor.change_cursor(e_cursors::horizontal_resize, true);
        value_changed = value_changed_h = true;
    }

    // Alpha bar logic
    if (alpha_bar)
    {
        ImGui::SetCursorScreenPos(ImVec2(bar1_pos_x, bar_pos.y + 24));
        ImGui::InvisibleButton("alpha", ImVec2(bars_width, sv_bar_size + 8));
        if (ImGui::IsItemActive())
        {
            col[3] = ImSaturate((io.MousePos.x - bar_pos.x) / (bars_width - 1));
            ui_cursor.change_cursor(e_cursors::horizontal_resize, true);
            value_changed = true;
        }
    }
    ImGui::PopItemFlag(); // ImGuiItemFlags_NoNav

    // Convert back color to RGB
    if (value_changed_h || value_changed_sv)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            ImGui::ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
            //g.ColorEditSavedHue = H;
            //g.ColorEditSavedSat = S;
            //g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            col[0] = H;
            col[1] = S;
            col[2] = V;
        }
    }

    if (value_changed)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            R = col[0];
            G = col[1];
            B = col[2];
            ImGui::ColorConvertRGBtoHSV(R, G, B, H, S, V);
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            H = col[0];
            S = col[1];
            V = col[2];
            ImGui::ColorConvertHSVtoRGB(H, S, V, R, G, B);
        }
    }

    const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
    const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
    const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
    const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
    const ImU32 col_hues[7] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(0,0,255,style_alpha8),IM_COL32(0,255,255,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(255,0,0,style_alpha8) };
    //const ImU32 col_hues[7] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(0,255,255,style_alpha8), IM_COL32(0,0,255,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

    ImVec4 hue_color_f(1, 1, 1, style.Alpha); ImGui::ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
    ImU32 hue_color32 = ImGui::ColorConvertFloat4ToU32(hue_color_f);
    ImU32 user_col32_striped_of_alpha = ImGui::ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha)); // Important: this is still including the main rendering/style alpha!!

    ImVec2 sv_cursor_pos;

    // sv bg
    draw_list->AddRectFilled(picker_pos - ImVec2(8, 8), picker_pos + ImVec2(sv_rect_size + 8, sv_rect_size + 8), ImColor(32, 32, 32), 6);
    // Render SV Square
    draw_list->AddRectFilledMultiColorRounded(picker_pos, picker_pos + ImVec2(sv_rect_size, sv_rect_size), ImColor(32, 32, 32), col_white, hue_color32, hue_color32, col_white, 6, ImDrawCornerFlags_All);
    draw_list->AddRectFilledMultiColorRounded(picker_pos, picker_pos + ImVec2(sv_rect_size, sv_rect_size), ImColor(32, 32, 32), 0, 0, col_black, col_black, 6, ImDrawCornerFlags_All);
    // sv outline
    //draw_list->AddRect(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), ImColor(1.f, 1.f, 1.f, 0.1f), 4, ImDrawCornerFlags_All);

    ImGui::RenderFrameBorder(picker_pos, picker_pos + ImVec2(sv_rect_size, sv_rect_size), 0.0f);
    sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * (sv_rect_size)), picker_pos.x + 2, picker_pos.x + sv_rect_size - 2); // Sneakily prevent the circle to stick out too much
    sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * (sv_rect_size)), picker_pos.y + 2, picker_pos.y + sv_rect_size);

    _element.try_emplace(id, s_colorpicker_element());
    auto this_element = _element.find(id);
    s_colorpicker_animation* this_anim = &this_element->second.anim;
    //element_state_t* this_state = &this_element->second.state;

    // bars bg
    draw_list->AddRectFilled(picker_pos + ImVec2(-8, 182), picker_pos + ImVec2(-8, 182) + ImVec2(182, 58), ImColor(32, 32, 32), 6);

    // Render Hue Bar
    for (int i = 0; i < 6; ++i)
        draw_list->AddRectFilledMultiColor(ImVec2(bar0_pos_x + i * (bars_width / 6), bar_pos.y + 8), ImVec2(bar0_pos_x + (i + 1) * (bars_width / 6), bar_pos.y + sv_bar_size), col_hues[i], col_hues[i + 1], col_hues[i + 1], col_hues[i]);
    draw_list->AddRectFilledMultiColorRounded(ImVec2(bar0_pos_x, bar_pos.y + 8), ImVec2(bar0_pos_x + bars_width, bar_pos.y + sv_bar_size), ImGui::GetColorU32(ImGuiCol_PopupBg), 0, 0, 0, 0, 30.f, ImDrawCornerFlags_All);

    float bar0_line_x = IM_ROUND(bar_pos.x + (1.f - H) * bars_width + 1.f);
    bar0_line_x = ImClamp(bar0_line_x, picker_pos.x + 4.f, picker_pos.x + 209.f);

    this_anim->hue_circle_move = ImLerp(this_anim->hue_circle_move, bar0_line_x - bar_pos.x, g.IO.DeltaTime * 16.f);
    this_anim->hue_circle_rad = ImLerp(this_anim->hue_circle_rad, value_changed_h ? 2.f : 3.f, g.IO.DeltaTime * 15.f);
    //
    draw_list->AddCircleFilled(ImVec2(this_anim->hue_circle_move + bar_pos.x, bar_pos.y + 13), 7, ImColor(255, 255, 255), 40.f); // hue main
    draw_list->AddCircleFilled(ImVec2(this_anim->hue_circle_move + bar_pos.x, bar_pos.y + 13), this_anim->hue_circle_rad, user_col32_striped_of_alpha, 40.f); // hue col

    this_anim->sv_circle_move = ImLerp(this_anim->sv_circle_move, sv_cursor_pos - bar_pos, g.IO.DeltaTime * 22.f);
    this_anim->sv_circle_rad = ImLerp(this_anim->sv_circle_rad, value_changed_sv ? 2.f : 3.f, g.IO.DeltaTime * 15.f);
    //
    draw_list->AddCircleFilled(this_anim->sv_circle_move + bar_pos + ImVec2(0, 1), 6, ImColor(50, 50, 50, 150), 40); // sv shdw
    draw_list->AddCircleFilled(this_anim->sv_circle_move + bar_pos, 6, IM_COL32(255, 255, 255, 255 * style.Alpha), 40); // sv main
    draw_list->AddCircleFilled(this_anim->sv_circle_move + bar_pos, this_anim->sv_circle_rad, user_col32_striped_of_alpha, 40); // sv center

    // Render alpha bar
    if (alpha_bar)
    {
        float alpha = ImSaturate(col[3]);
        ImRect bar1_bb(bar1_pos_x, bar_pos.y + 28, bar1_pos_x + bars_width, bar_pos.y + 20 + sv_bar_size);

        ImGui::RenderColorRectWithAlphaCheckerboard(draw_list, bar1_bb.Min, bar1_bb.Max - ImVec2(4.f, 0.f), ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.f)), 3.f, ImVec2(0.0f, 0.0f), 128.f);
        draw_list->AddRectFilledMultiColor(ImVec2(bar1_bb.Min.x + 4.f, bar1_bb.Min.y), ImVec2(bar1_bb.Max.x - 4.f, bar1_bb.Max.y), user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK);
        draw_list->AddRectFilled(ImVec2(bar1_bb.Max.x - 4.f, bar1_bb.Min.y), bar1_bb.Max, user_col32_striped_of_alpha, 128.f, ImDrawFlags_RoundCornersRight);

        float bar1_line_x = IM_ROUND(bar_pos.x + alpha * bars_width - 4);

        bar1_line_x = ImClamp(bar1_line_x, bar_pos.x + 8.f, picker_pos.x + 209.f);
        this_anim->alpha_circle_move = ImLerp(this_anim->alpha_circle_move, bar1_line_x - bar_pos.x + 5.f, g.IO.DeltaTime * 25.f);

        draw_list->AddCircle(ImVec2(this_anim->alpha_circle_move + bar_pos.x, bar1_bb.Min.y + 5.0f), 5.f, ImColor(255, 255, 255), 40.f, 5.f);

        //ImGui::RenderArrowPointingAt(GetForegroundDrawList(), ImVec2(it_anim->second.alpha_bar + bar_pos.x, bar1_bb.Min.y + 13.0f), ImVec2(2,2), ImGuiDir_Up, IM_COL32(255, 255, 255, 255));

    }

    //if (ImGui::Button("Copy", { 86, 28 })) {
    //    colorpicker_buffer = ImColor(R, G, B, col[3]);
    //}
    //ImGui::SameLine();
    //if (ImGui::Button("Paste", { 86, 28 })) {
    //    ImVec4 color_vec = colorpicker_buffer;
    //    col[0] = color_vec.x;
    //    col[1] = color_vec.y;
    //    col[2] = color_vec.z;
    //    col[3] = color_vec.w;
    //}

    ImGui::EndGroup();

    if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0)
        value_changed = false;
    if (value_changed)
        ImGui::MarkItemEdited(g.LastItemData.ID);

    ImGui::PopID();
    return value_changed;
}

bool c_ui_widgets::s_ui_cpickers::edit_4(const char* label, float col[4], ImGuiColorEditFlags flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float square_sz = ImGui::GetFrameHeight();
    const float w_full = ImGui::CalcItemWidth();
    const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
    const float w_inputs = w_full - w_button;
    const char* label_display_end = ImGui::FindRenderedTextEnd(label);
    g.NextItemData.ClearFlags();

    ImGui::BeginGroup();
    ImGui::PushID(label);
    //const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
    //if (set_current_color_edit_id)
        //g.ColorEditCurrentID = window->IDStack.back();

    // If we're not showing any slider there's no point in doing any HSV conversions
    const ImGuiColorEditFlags flags_untouched = flags;
    if (flags & ImGuiColorEditFlags_NoInputs)
        flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

    // Context menu: display and modify options (before defaults are applied)
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        ImGui::ColorEditOptionsPopup(col, flags);

    // Read stored options
    if (!(flags & ImGuiColorEditFlags_DisplayMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
    if (!(flags & ImGuiColorEditFlags_DataTypeMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_);
    if (!(flags & ImGuiColorEditFlags_PickerMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
    if (!(flags & ImGuiColorEditFlags_InputMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);
    flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_));
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_DisplayMask_)); // Check that only 1 is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));   // Check that only 1 is selected

    const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
    const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
    const int components = alpha ? 4 : 3;

    // Convert to the formats we need
    float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
    if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
        ImGui::ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
    else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
    {
        // Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
        ImGui::ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
    }
    int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

    bool value_changed = false;
    bool value_changed_as_float = false;

    const ImVec2 pos = window->DC.CursorPos - ImVec2(2, 4);
    const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
    window->DC.CursorPos.x = pos.x + inputs_offset_x;

    ImGuiWindow* picker_active_window = NULL;
    if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
    {
        const float button_offset_x = ((flags & ImGuiColorEditFlags_NoInputs) || (style.ColorButtonPosition == ImGuiDir_Left)) ? 0.0f : w_inputs + style.ItemInnerSpacing.x;
        window->DC.CursorPos = ImVec2(pos.x + button_offset_x, pos.y);

        const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);
        if (color_btn("##ColorButton", col_v4, flags))
        {
            if (!(flags & ImGuiColorEditFlags_NoPicker))
            {
                // Store current color and open a picker
                g.ColorPickerRef = col_v4;
                ImGui::OpenPopup("picker");
                ImGui::SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, style.ItemSpacing.y));
            }
        }
        if (!(flags & ImGuiColorEditFlags_NoOptions))
            ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImGui::GetColorU32(ImVec4(22 / 255.f, 22 / 255.f, 22 / 255.f, 1.f)));
        ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 0.07f)));

        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 6);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

        if (ImGui::BeginPopup("picker"))
        {
            if (g.CurrentWindow->BeginCount == 1)
            {
                picker_active_window = g.CurrentWindow;

                ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
                ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
                ImGui::SetNextItemWidth(square_sz * 12.0f); // Use 256 + bar sizes?
                value_changed |= color_picker("##picker", col, picker_flags, &g.ColorPickerRef.x);
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }

    if (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel))
    {
        char buf[64];

        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        window->DC.CursorPos.x = pos.x - w_button + ((flags & ImGuiColorEditFlags_NoInputs) ? w_button : w_full);

        ImGui::GetWindowDrawList()->AddText(window->DC.CursorPos + ImVec2(2, 3), ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 0.7f)), label);

        ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));

    }

    // Convert back
    if (value_changed && picker_active_window == NULL)
    {
        if (!value_changed_as_float)
            for (int n = 0; n < 4; n++)
                f[n] = i[n] / 255.0f;
        if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
        {
            // g.ColorEditSavedHue = f[0];
             //g.ColorEditSavedSat = f[1];
            ImGui::ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
            // g.ColorEditSavedID = g.ColorEditCurrentID;
            // g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(f[0], f[1], f[2], 0));
        }
        if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
            ImGui::ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

        col[0] = f[0];
        col[1] = f[1];
        col[2] = f[2];
        if (alpha)
            col[3] = f[3];
    }

    //if (set_current_color_edit_id)
    //    g.ColorEditCurrentID = 0;
    ImGui::PopID();
    ImGui::EndGroup();

    // Drag and Drop Target
    // NB: The flag test is merely an optional micro-optimization, BeginDragDropTarget() does the same test.
    if ((g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredRect) && !(flags & ImGuiColorEditFlags_NoDragDrop) && ImGui::BeginDragDropTarget())
    {
        bool accepted_drag_drop = false;
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
        {
            memcpy((float*)col, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512 //-V1086
            value_changed = accepted_drag_drop = true;
        }
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
        {
            memcpy((float*)col, payload->Data, sizeof(float) * components);
            value_changed = accepted_drag_drop = true;
        }

        // Drag-drop payloads are always RGB
        if (accepted_drag_drop && (flags & ImGuiColorEditFlags_InputHSV))
            ImGui::ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);
        ImGui::EndDragDropTarget();
    }

    // When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
    if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
        g.LastItemData.ID = g.ActiveId;

    if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
        ImGui::MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

bool c_ui_widgets::s_ui_cpickers::edit_4(const char* label, std::array<float, 4>& col, ImGuiColorEditFlags flags)
{
    return edit_4(label, col.data(), flags);
}
