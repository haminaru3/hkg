#include "c_ui_widgets.hpp"
#include "base64.hpp"
#include "../../c_config.hpp"
#include "../c_fonts.h"
#include "../interface_variant.hpp"

void c_ui_widgets::s_ui_misc::window_decorations() {
    auto draw = ImGui::GetWindowDrawList();
    auto pos = ImGui::GetWindowPos();
    auto size = ImGui::GetWindowSize();
    ImGuiStyle* s = &ImGui::GetStyle();

    ImColor header_col = ImColor(15 / 255.f, 15 / 255.f, 15 / 255.f, s->Alpha);
    ImColor side_col = ImColor(16 / 255.f, 16 / 255.f, 16 / 255.f, s->Alpha);
    ImColor accent_col = ImColor(1.0f, 1.0f, 1.0f, 0.04f * s->Alpha);
    ImColor title_col = ImColor(1.f, 1.f, 1.f, s->Alpha);

#if CORTEX_UI_VARIANT == 1
    accent_col = ImColor(76 / 255.f, 210 / 255.f, 194 / 255.f, 0.55f * s->Alpha);
    draw->AddRectFilled(ImVec2(pos.x + 18, pos.y + 18), ImVec2(pos.x + size.x - 18, pos.y + 52),
        ImColor(8 / 255.f, 18 / 255.f, 22 / 255.f, 0.88f * s->Alpha), 9.f);
    draw->AddRect(ImVec2(pos.x + 18, pos.y + 18), ImVec2(pos.x + size.x - 18, pos.y + 52),
        ImColor(76 / 255.f, 210 / 255.f, 194 / 255.f, 0.16f * s->Alpha), 9.f);
    const char* variant_title_text = (g_config && !g_config->misc.menu_title.empty()) ? g_config->misc.menu_title.c_str() : "Cortex";
    draw->AddText(ui_fonts[e_fonts::BOLD], 20.0f, ImVec2(pos.x + 34, pos.y + 25),
        ImColor(227 / 255.f, 1.f, 246 / 255.f, s->Alpha), variant_title_text);
    draw->AddText(ui_fonts[e_fonts::MEDIUM], 13.0f, ImVec2(pos.x + size.x - 188, pos.y + 28),
        ImColor(153 / 255.f, 182 / 255.f, 188 / 255.f, 0.86f * s->Alpha), "control deck");
    draw->AddLine(ImVec2(pos.x + 28, pos.y + 76), ImVec2(pos.x + size.x - 28, pos.y + 76), accent_col);
    draw->AddRect(pos + ImVec2(1, 1), pos + size - ImVec2(1, 1), ImColor(76 / 255.f, 210 / 255.f, 194 / 255.f, 0.12f * s->Alpha), 9.f);
    return;
#elif CORTEX_UI_VARIANT == 2
    accent_col = ImColor(242 / 255.f, 188 / 255.f, 67 / 255.f, 0.68f * s->Alpha);
    draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 78, pos.y + size.y),
        ImColor(14 / 255.f, 15 / 255.f, 17 / 255.f, 0.96f * s->Alpha), 6.f, ImDrawFlags_RoundCornersLeft);
    draw->AddRectFilled(ImVec2(pos.x + 94, pos.y + 18), ImVec2(pos.x + size.x - 22, pos.y + 54),
        ImColor(1.f, 1.f, 1.f, 0.035f * s->Alpha), 6.f);
    const char* variant_title_text = (g_config && !g_config->misc.menu_title.empty()) ? g_config->misc.menu_title.c_str() : "Cortex";
    draw->AddText(ui_fonts[e_fonts::BOLD], 20.0f, ImVec2(pos.x + 110, pos.y + 25),
        ImColor(245 / 255.f, 246 / 255.f, 240 / 255.f, s->Alpha), variant_title_text);
    draw->AddRectFilled(ImVec2(pos.x + 18, pos.y + 18), ImVec2(pos.x + 58, pos.y + 54),
        ImColor(242 / 255.f, 188 / 255.f, 67 / 255.f, 0.18f * s->Alpha), 6.f);
    draw->AddText(ui_fonts[e_fonts::BOLD], 18.0f, ImVec2(pos.x + 28, pos.y + 26),
        ImColor(242 / 255.f, 188 / 255.f, 67 / 255.f, s->Alpha), "CX");
    draw->AddLine(ImVec2(pos.x + 78, pos.y + 18), ImVec2(pos.x + 78, pos.y + size.y - 18), accent_col);
    draw->AddRect(pos + ImVec2(1, 1), pos + size - ImVec2(1, 1), ImColor(242 / 255.f, 188 / 255.f, 67 / 255.f, 0.12f * s->Alpha), 6.f);
    return;
#elif CORTEX_UI_VARIANT == 3
    accent_col = ImColor(1.f, 91 / 255.f, 41 / 255.f, 0.62f * s->Alpha);
    draw->AddRectFilled(ImVec2(pos.x + 20, pos.y + 18), ImVec2(pos.x + size.x - 20, pos.y + 72),
        ImColor(8 / 255.f, 7 / 255.f, 9 / 255.f, 0.52f * s->Alpha), 12.f);
    draw->AddRectFilled(ImVec2(pos.x + 24, pos.y + size.y - 62), ImVec2(pos.x + size.x - 24, pos.y + size.y - 16),
        ImColor(10 / 255.f, 8 / 255.f, 10 / 255.f, 0.74f * s->Alpha), 12.f);
    const char* variant_title_text = (g_config && !g_config->misc.menu_title.empty()) ? g_config->misc.menu_title.c_str() : "Cortex";
    auto variant_title_size = _calc_text_size(ui_fonts[e_fonts::BOLD], 22, variant_title_text);
    draw->AddText(ui_fonts[e_fonts::BOLD], 22.0f, ImVec2(pos.x + ((size.x / 2) - (variant_title_size.x / 2)), pos.y + 34),
        ImColor(255 / 255.f, 230 / 255.f, 217 / 255.f, s->Alpha), variant_title_text);
    draw->AddLine(ImVec2(pos.x + 42, pos.y + 80), ImVec2(pos.x + size.x - 42, pos.y + 80), accent_col);
    draw->AddRect(pos + ImVec2(1, 1), pos + size - ImVec2(1, 1), ImColor(1.f, 91 / 255.f, 41 / 255.f, 0.14f * s->Alpha), 9.f);
    return;
#endif

    switch (g_config->misc.menu_style) {
    case 1:
        header_col = ImColor(43 / 255.f, 82 / 255.f, 124 / 255.f, s->Alpha);
        side_col = ImColor(14 / 255.f, 42 / 255.f, 74 / 255.f, s->Alpha);
        accent_col = ImColor(1.f, 140 / 255.f, 0.f, 0.55f * s->Alpha);
        title_col = ImColor(1.f, 178 / 255.f, 37 / 255.f, s->Alpha);
        break;
    case 2:
        header_col = ImColor(102 / 255.f, 28 / 255.f, 38 / 255.f, s->Alpha);
        side_col = ImColor(55 / 255.f, 18 / 255.f, 25 / 255.f, s->Alpha);
        accent_col = ImColor(1.f, 70 / 255.f, 90 / 255.f, 0.45f * s->Alpha);
        break;
    case 3:
        header_col = ImColor(31 / 255.f, 93 / 255.f, 70 / 255.f, s->Alpha);
        side_col = ImColor(18 / 255.f, 54 / 255.f, 42 / 255.f, s->Alpha);
        accent_col = ImColor(74 / 255.f, 222 / 255.f, 128 / 255.f, 0.42f * s->Alpha);
        break;
    case 4:
        header_col = ImColor(31 / 255.f, 30 / 255.f, 29 / 255.f, 0.78f * s->Alpha);
        side_col = ImColor(18 / 255.f, 18 / 255.f, 17 / 255.f, 0.82f * s->Alpha);
        accent_col = ImColor(238 / 255.f, 108 / 255.f, 35 / 255.f, 0.48f * s->Alpha);
        title_col = ImColor(255 / 255.f, 236 / 255.f, 210 / 255.f, s->Alpha);
        break;
    }

    draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 48), header_col, s->WindowRounding, ImDrawFlags_RoundCornersTop); // upper bar
    if (g_config->misc.menu_style != 0) draw->AddRectFilled(ImVec2(pos.x, pos.y + 48), ImVec2(pos.x + 162, pos.y + size.y), side_col, s->WindowRounding, ImDrawFlags_RoundCornersBottomLeft);
    draw->AddLine(ImVec2(pos.x + 162, pos.y + 48), ImVec2(pos.x + 162, pos.y + size.y), accent_col); // line upside-down
    draw->AddLine(ImVec2(pos.x + 2, pos.y + 48), ImVec2(pos.x + size.x, pos.y + 48), accent_col); // vertical line

    const char* title_text = (g_config && !g_config->misc.menu_title.empty()) ? g_config->misc.menu_title.c_str() : "Cortex";
    auto title_size = _calc_text_size(ui_fonts[e_fonts::BOLD], 20, title_text);

    draw->AddText(ui_fonts[e_fonts::BOLD], 20.0f, ImVec2(pos.x + ((size.x / 2) - (title_size.x / 2)), pos.y + 15), title_col, title_text);

    draw->AddRect(pos + ImVec2(1, 1), pos + size - ImVec2(1, 1), ImColor(1.0f, 1.0f, 1.0f, 0.03f * s->Alpha), s->WindowRounding);
}

void c_ui_widgets::s_ui_misc::subtab_frame() {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    auto draw = ImGui::GetWindowDrawList();
    ImGuiStyle* s = &ImGui::GetStyle();

    ImGui::SetCursorPos({ CORTEX_SUBTAB_X, CORTEX_SUBTAB_Y });
    const float frame_width = CORTEX_SUBTAB_W;
    const float frame_alpha =
#if CORTEX_UI_VARIANT == 1 || CORTEX_UI_VARIANT == 2 || CORTEX_UI_VARIANT == 3
        0.48f;
#else
        (g_config->misc.menu_style == 4) ? 0.50f : 1.0f;
#endif
    const ImColor frame_col =
#if CORTEX_UI_VARIANT == 1
        ImColor(9 / 255.f, 19 / 255.f, 22 / 255.f, frame_alpha * s->Alpha);
#elif CORTEX_UI_VARIANT == 2
        ImColor(24 / 255.f, 26 / 255.f, 28 / 255.f, frame_alpha * s->Alpha);
#elif CORTEX_UI_VARIANT == 3
        ImColor(13 / 255.f, 10 / 255.f, 13 / 255.f, frame_alpha * s->Alpha);
#else
        (g_config->misc.menu_style == 4)
        ? ImColor(19 / 255.f, 19 / 255.f, 19 / 255.f, frame_alpha * s->Alpha)
        : ImColor(19 / 255.f, 19 / 255.f, 19 / 255.f, 255 / 255.f * s->Alpha);
#endif
    const ImColor border_col =
#if CORTEX_UI_VARIANT == 1
        ImColor(76 / 255.f, 210 / 255.f, 194 / 255.f, 0.22f * s->Alpha);
#elif CORTEX_UI_VARIANT == 2
        ImColor(242 / 255.f, 188 / 255.f, 67 / 255.f, 0.18f * s->Alpha);
#elif CORTEX_UI_VARIANT == 3
        ImColor(1.f, 91 / 255.f, 41 / 255.f, 0.20f * s->Alpha);
#else
        (g_config->misc.menu_style == 4)
        ? ImColor(238 / 255.f, 108 / 255.f, 35 / 255.f, 0.24f * s->Alpha)
        : ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, (6 / 255.f) * s->Alpha);
#endif
    draw->AddRectFilled(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y), ImVec2(window->DC.CursorPos.x + frame_width, window->DC.CursorPos.y + 50), frame_col, 5.f);
    draw->AddRect(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y), ImVec2(window->DC.CursorPos.x + frame_width, window->DC.CursorPos.y + 50), border_col, 5.f);
    ImGui::SetCursorPos({ CORTEX_SUBTAB_X + 10.f, CORTEX_SUBTAB_Y + 10.f });
}

void c_ui_widgets::s_ui_misc::cfg_ex_render(std::string label, std::string f_upd_date) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiStyle* s = &ImGui::GetStyle();

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.c_str());
    const ImVec2 label_size = _calc_text_size(ui_fonts[e_fonts::BOLD], 16, label.c_str());

    const float w = ImGui::GetWindowWidth() - 10;
    const float square_sz = 64;
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect frame_bb(pos, window->DC.CursorPos + ImVec2(w, square_sz));
    const ImRect total_bb(pos, pos + ImVec2(w, square_sz + 8));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    ImGui::ItemAdd(total_bb, id);

    window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, ImColor(18 / 255.f, 18 / 255.f, 18 / 255.f, 1.f * s->Alpha), 6.0f);
    window->DrawList->AddRect(frame_bb.Min, frame_bb.Max, ImColor(1.0f, 1.0f, 1.0f, 0.03f * s->Alpha), 6.0f);
    window->DrawList->AddText(ui_fonts[e_fonts::BOLD], 16, pos + ImVec2(12, 16), ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.7f * s->Alpha), label.c_str());
    window->DrawList->AddText(ui_fonts[e_fonts::BOLD], 14, pos + ImVec2(12, 34), ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.7f * s->Alpha), "Last Update: ");
    window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 14, pos + ImVec2(12 + ui_fonts[e_fonts::BOLD]->CalcTextSizeA(14.f, FLT_MAX, 0, "Last Update: ").x, 34),
        ImColor(77 / 255.f, 77 / 255.f, 77 / 255.f, 1.f * s->Alpha), f_upd_date.c_str());
}

//static char rename_cfg_buf[42] = "";
 
void c_ui_widgets::s_ui_misc::cfg_element(std::string filename, std::string edit_date, int cfg_id) {
#ifndef NO_CONFIGS
    static int current_cfg_id = 0;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    std::string cfg_title;
    if (!cfg_manager::decode_config_filename(filename, cfg_title)) return;

    ImGui::PushID(filename.c_str());
    cfg_ex_render(cfg_title, edit_date);
    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(-384, 48 + 14));
    ImGui::BeginChild(window->GetID(cfg_title.c_str()) + cfg_id, {170, 35}); {
        ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 3));
        ImGui::BeginGroup(); {
            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(46, 0));

            if (current_cfg_id == cfg_id) {
                if (c_ui_widgets::button("Save", { 50, 32 })) {
                    if (!cfg_manager::save_config(cfg_title)) {
                        MessageBoxA(0, "failed to save config.", "=(", MB_OK | MB_ICONERROR);
                    }
                    //g_notify.spawn("saved " + cfg_title + " configuration.", 1200);
                } 
            }
            else {
                if (c_ui_widgets::button("Load", { 50, 32 })) {
                    // load cfg
                    if (!cfg_manager::load_config(filename)) {
                        MessageBoxA(0, "failed to load config.", "=(", MB_OK | MB_ICONERROR);
                    }
                    else current_cfg_id = cfg_id;
                    //g_notify.spawn("loaded " + cfg_title + " configuration.", 1200);
                }
            }

            ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, 9));

            if (c_ui_widgets::s_ui_popups::begin_settings("Management", 154, false, { 32,31 })) {
                if (c_ui_widgets::button("Delete", { 120, 30 })) {
                    if (!cfg_manager::delete_config(filename)) {
                        MessageBoxA(0, "failed to delete config.", "=(", MB_OK | MB_ICONERROR);
                    }
                    else if (current_cfg_id == cfg_id) {
                        current_cfg_id = 0;
                    }
                    c_ui_widgets::s_ui_popups::end_settings();
                    ImGui::EndGroup();
                    ImGui::EndChild();
                    ImGui::PopID();
                    return;
                }

                if (current_cfg_id == cfg_id) {
                    if (c_ui_widgets::button("Reload", { 120, 30 })) {
                        if (!cfg_manager::load_config(filename)) {
                            MessageBoxA(0, "failed to reload config.", "=(", MB_OK | MB_ICONERROR);
                        }
                        //g_notify.spawn("re-loaded " + cfg_title + " configuration.", 1500);
                    }
                }
                else {
                    if (c_ui_widgets::button("Save", { 120, 30 })) {
                        if (!cfg_manager::save_config(cfg_title)) {
                            MessageBoxA(0, "failed to save config.", "=(", MB_OK | MB_ICONERROR);
                        }
                        //g_notify.spawn("saved current settings to " + cfg_title + " configuration.", 1500);
                    }
                }

                //ImGui::InputText("New Name...", rename_cfg_buf, sizeof(rename_cfg_buf));
                //if (ImGui::Button("Rename", { 120, 30 })) {
                //    std::filesystem::rename("C:\\testcfgs\\" + base64::to_base64(cfg_title), "C:\\testcfgs\\" + base64::to_base64(rename_cfg_buf));
                //}
                c_ui_widgets::s_ui_popups::end_settings();
            }
        } ImGui::EndGroup();

    }; ImGui::EndChild();
    ImGui::PopID();
    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 6 + 14));
#endif
}

void c_ui_widgets::s_ui_misc::in_dev_frame() {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiStyle* s = &ImGui::GetStyle();
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();

    window->DrawList->AddText(ui_fonts[e_fonts::BOLD], 18.f, pos + ImVec2(4, 0),
        ImColor(1.f, 1.f, 1.f, 0.7f), "Uh.. Sorry. This page is empty now.");
    window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 14.f, pos + ImVec2(4, 8 + 18 + 4),
        ImColor(1.f, 1.f, 1.f, 0.5f), "It is supposedly currently in development. Follow the changelog!");
}
