#include "c_ui_tabs.hpp"
#include "../../menu_background.hpp"
#include <commdlg.h>

#pragma comment(lib, "Comdlg32.lib")

static void sync_text_buffer(char* buffer, size_t size, const std::string& value, std::string& cached_value) {
    if (cached_value == value) return;

    std::snprintf(buffer, size, "%s", value.c_str());
    cached_value = value;
}

static bool pick_menu_background_photo(std::string& selected_path) {
    char file_path[MAX_PATH] = "";
    if (!selected_path.empty()) {
        std::snprintf(file_path, sizeof(file_path), "%s", selected_path.c_str());
    }

    OPENFILENAMEA ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetForegroundWindow();
    ofn.lpstrFile = file_path;
    ofn.nMaxFile = sizeof(file_path);
    ofn.lpstrFilter = "Image files\0*.jpg;*.jpeg;*.png;*.bmp;*.dds\0All files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    ofn.lpstrTitle = "Choose Menu Background";

    if (!GetOpenFileNameA(&ofn)) {
        return false;
    }

    selected_path = file_path;
    return !selected_path.empty();
}

void c_ui_tabs::settings_tab() {
    _column_right_no_sub("csets_1"); {
        ui_widgets.child.begin(m_crypt("General"), { 0, 105 }); {
            ui_widgets.checkbox_bind(m_crypt("Change Menu Key"), &g_config->controls.menu_custom_key, &g_config->controls.menu_key);

            if (ui_widgets.button(m_crypt("Unhook Cheat"), ImVec2(0, 30))) {
                g_singleuse->unloadcheat = true;
            }
            ui_widgets.popup_key(m_crypt("#unhokk"), m_crypt("Unhook On Key"), &g_config->controls.unload_on_key, &g_config->controls.unload_key);
        } ui_widgets.child.end();

    }; _end_column;


    _column_left_no_sub("csets_2"); {
        ui_widgets.child.begin(m_crypt("Visual"), { 0, 390 }); {
            static char menu_title_buf[64] = "";
            static std::string cached_menu_title;

            sync_text_buffer(menu_title_buf, sizeof(menu_title_buf), g_config->misc.menu_title, cached_menu_title);

            const char* menu_styles[5] = { "Classic", "Blue Orange", "Red", "Green", "Hokage" };
            ui_widgets.combo(m_crypt("Menu Style"), &g_config->misc.menu_style, menu_styles, IM_ARRAYSIZE(menu_styles));

            if (ui_widgets.input_text(m_crypt("Menu Name"), menu_title_buf, sizeof(menu_title_buf))) {
                g_config->misc.menu_title = std::strlen(menu_title_buf) > 0 ? menu_title_buf : "Cortex";
                cached_menu_title = g_config->misc.menu_title;
            }

            ui_widgets.checkbox(m_crypt("Photo Background"), &g_config->misc.menu_background_photo);
            if (ui_widgets.button(m_crypt("Choose Photo"), ImVec2(150, 28))) {
                std::string selected_path = g_config->misc.menu_background_path;
                if (pick_menu_background_photo(selected_path)) {
                    g_config->misc.menu_background_path = selected_path;
                    g_config->misc.menu_background_photo = true;
                    refresh_menu_background_texture(true);
                }
            }
            ImGui::SameLine();
            if (ui_widgets.button(m_crypt("Reset Photo"), ImVec2(150, 28))) {
                g_config->misc.menu_background_path.clear();
                g_config->misc.menu_background_photo = true;
                refresh_menu_background_texture(true);
            }

            ui_widgets.checkbox(m_crypt("Clean Recording Mode"), &g_config->misc.clean_recording_mode);

            ui_widgets.checkbox(m_crypt("Watermark"), &g_config->misc.watermark);
            if (ui_widgets.popup.begin_settings(m_crypt("Watermark Settings"), 220, false)) {
                const char* wm_style[2] = { "Modern", "Legacy" };
                ui_widgets.combo(m_crypt("Style"), &g_config->misc.wm_style, wm_style, IM_ARRAYSIZE(wm_style));

                if (g_config->misc.wm_style == 0) {
                    ui_widgets.checkbox(m_crypt("Background"), &g_config->misc.wm_bg);
                } if (g_config->misc.wm_style == 1) {
                    ui_widgets.color.edit_4(m_crypt("Accent Color"), g_config->misc.wm_accent, cp_flags_w_label);
                }

                ui_widgets.popup.end_settings();
            }

            ui_widgets.checkbox(m_crypt("Menu Background"), &g_config->misc.background.bg_fill);
            if (ui_widgets.popup.begin_settings(m_crypt("Menu Background Settings"), 220, false)) {
                ui_widgets.slider_float(m_crypt("Opacity"), &g_config->misc.background.bg_alpha, 1, 90, "%.0f%%");
                ui_widgets.popup.end_settings();
            }
        } ui_widgets.child.end();

    }; _end_column;
}
