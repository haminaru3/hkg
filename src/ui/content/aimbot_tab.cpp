#include "c_ui_tabs.hpp"

struct s_aim_sub {
    enum e_sub {
        VECTOR,
        SILENT,
        //DAMAGER
    };

    e_sub current{ VECTOR };

    std::vector<std::tuple<const char*, e_sub>> sub_arr = {
        {m_crypt("Vector"), VECTOR},
        {m_crypt("Slient"), SILENT},
        //{m_crypt("Damager"), DAMAGER}
    };
} aim_sub;

void c_ui_tabs::aimbot_tab() {
    ui_widgets.misc.subtab_frame();
    ImGui::BeginGroup(); {
        for (const auto& [name, _id] : aim_sub.sub_arr) {
            if (ui_widgets.tab.sub(name, aim_sub.current == _id)) { aim_sub.current = _id; }
        }
    } ImGui::EndGroup();

    switch (aim_sub.current)
    {
    case aim_sub.e_sub::VECTOR:
        _column_left("cvector_1"); {
            ui_widgets.child.begin(m_crypt("General"), ImVec2(0, 155)); {
                const char* bones_list[1] = { "Only Body" };
                int body_only = 0;
                g_config->aimbot.vector.target_bones = 2;
                ui_widgets.checkbox_bind(m_crypt("Vector Aimbot"), &g_config->aimbot.vector.enabled, &g_config->aimbot.vector.hotkey);
                ui_widgets.slider_int(m_crypt("Vector FOV"), &g_config->aimbot.vector.field_of_view, 0, 180);
                ui_widgets.combo(m_crypt("Target Hitboxes"), &body_only, bones_list, IM_ARRAYSIZE(bones_list));
                ui_widgets.slider_int(m_crypt("Smoothness"), &g_config->aimbot.vector.smoothness, 1, 100, "%d%%");
            } ui_widgets.child.end();
        }; _end_column;

        _column_right("cvector_2") {
            ui_widgets.child.begin(m_crypt("Additional"), ImVec2(0, 145)); {
                ui_widgets.checkbox(m_crypt("Visualize FOV"), &g_config->aimbot.vector.visual.visualize_fov);
                if (ui_widgets.popup.begin_settings(m_crypt("FOV Visualize Settings"), 220)) {
                    const char* c_stylez[3] = { "Outline", "Filled", "Filled + Outline" };
                    ui_widgets.combo(m_crypt("Style"), &g_config->aimbot.vector.visual.fov_type, c_stylez, IM_ARRAYSIZE(c_stylez));
                    switch (g_config->aimbot.vector.visual.fov_type) {
                    case 0:
                        ui_widgets.color.edit_4(m_crypt("Color"), g_config->aimbot.vector.visual.outlinefov, cp_flags_w_label);
                        break;
                    case 1:
                        ui_widgets.color.edit_4(m_crypt("Color"), g_config->aimbot.vector.visual.filledfov, cp_flags_w_label);
                        break;
                    case 2:
                        ui_widgets.color.edit_4(m_crypt("Outline"), g_config->aimbot.vector.visual.outlinefov, cp_flags_w_label);
                        ui_widgets.color.edit_4(m_crypt("Stroke"), g_config->aimbot.vector.visual.filledfov, cp_flags_w_label);
                        break;
                    }
                    ui_widgets.popup.end_settings();
                }

                ui_widgets.checkbox(m_crypt("Ignore Invisible"), &g_config->aimbot.vector.filters.visibility_check);
                ui_widgets.checkbox(m_crypt("Target NPC"), &g_config->aimbot.vector.filters.target_npcs);
                ui_widgets.checkbox(m_crypt("Target Admins"), &g_config->aimbot.vector.filters.target_adms);
            } ui_widgets.child.end();
        }; _end_column;

        break;

    case aim_sub.e_sub::SILENT:

        _column_left("csilent_1"); {
            ui_widgets.child.begin(m_crypt("General"), ImVec2(0, 155)); {
                const char* bones_list[1] = { "Only Body" };
                int body_only = 0;
                g_config->aimbot.silent.target_bones = 2;
                ui_widgets.checkbox_bind(m_crypt("Silent Aimbot"), &g_config->aimbot.silent.enabled, &g_config->aimbot.silent.hotkey);
                ui_widgets.slider_int(m_crypt("Silent FOV"), &g_config->aimbot.silent.field_of_view, 0, 180);
                ui_widgets.combo(m_crypt("Target Hitboxes"), &body_only, bones_list, IM_ARRAYSIZE(bones_list));
                ui_widgets.slider_int(m_crypt("Hitchance"), &g_config->aimbot.silent.hitchance, 0, 100, "%d%%");
            } ui_widgets.child.end();

            ui_widgets.child.begin(m_crypt("Exploits"), ImVec2(0, 65)); {
                ui_widgets.checkbox(m_crypt("Magic Bullet"), &g_config->aimbot.silent.magicbullet);
            } ui_widgets.child.end();
        }; _end_column;

        _column_right("csilent_2"); {
            ui_widgets.child.begin(m_crypt("Additional"), ImVec2(0, 145)); {
                ui_widgets.checkbox(m_crypt("Visualize FOV"), &g_config->aimbot.silent.visual.visualize_fov);
                if (ui_widgets.popup.begin_settings(m_crypt("FOV Visualize Settings"), 220)) {
                    const char* c_stylez[3] = { "Outline", "Filled", "Filled + Outline" };
                    ui_widgets.combo(m_crypt("Style"), &g_config->aimbot.silent.visual.fov_type, c_stylez, IM_ARRAYSIZE(c_stylez));
                    switch (g_config->aimbot.silent.visual.fov_type) {
                    case 0:
                        ui_widgets.color.edit_4(m_crypt("Color"), g_config->aimbot.silent.visual.outlinefov, cp_flags_w_label);
                        break;
                    case 1:
                        ui_widgets.color.edit_4(m_crypt("Color"), g_config->aimbot.silent.visual.filledfov, cp_flags_w_label);
                        break;
                    case 2:
                        ui_widgets.color.edit_4(m_crypt("Outline"), g_config->aimbot.silent.visual.outlinefov, cp_flags_w_label);
                        ui_widgets.color.edit_4(m_crypt("Stroke"), g_config->aimbot.silent.visual.filledfov, cp_flags_w_label);
                        break;
                    }
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Ignore Invisible"), &g_config->aimbot.silent.filters.visibility_check);
                ui_widgets.checkbox(m_crypt("Target NPC"), &g_config->aimbot.silent.filters.target_npcs);
                ui_widgets.checkbox(m_crypt("Target Admins"), &g_config->aimbot.silent.filters.target_adms);

            } ui_widgets.child.end();
        }; _end_column;

        break;

    //case aim_sub.e_sub::DAMAGER:
    //    break;
    }
}
