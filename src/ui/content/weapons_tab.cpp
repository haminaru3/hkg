#include "c_ui_tabs.hpp"

//struct s_weapons_sub {
//    enum e_sub {
//        MAIN,
//        MISC
//    };
//
//    e_sub current{ MAIN };
//
//    std::vector<std::tuple<const char*, e_sub>> sub_arr = {
//        {m_crypt("Management"), MAIN},
//        {m_crypt("Misc"), MISC}
//    };
//} weapons_sub;

void c_ui_tabs::weapons_tab() {
    _column_left_no_sub("cwep_mgmt_1"); {
        ui_widgets.child.begin(m_crypt("Manager Type"), { 0, 70 }); {
            const char* mngr_type[2] = { "Modify", "Disable" };
            ui_widgets.combo(m_crypt("Manager Type"), &g_config->weapons.manager_type, mngr_type, IM_ARRAYSIZE(mngr_type));
        } ui_widgets.child.end();

        if (g_config->weapons.manager_type == 0) {
            ui_widgets.child.begin(m_crypt("Accuracy"), { 0, 90 }); {
                ui_widgets.checkbox(m_crypt("Change Recoil"), &g_config->weapons.recoil);
                if (ui_widgets.popup.begin_settings(m_crypt("Recoil Settings"), 250)) {
                    ui_widgets.slider_int(m_crypt("Recoil Percentage"), &g_config->weapons.modify_values.recoil_percent, 0, 100, "%d%%");
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Change Spread"), &g_config->weapons.spread);
                if (ui_widgets.popup.begin_settings(m_crypt("Spread Settings"), 250)) {
                    ui_widgets.slider_int(m_crypt("Spread Percentage"), &g_config->weapons.modify_values.spread_percent, 0, 100, "%d%%");
                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();
        }

        else if (g_config->weapons.manager_type == 1) {
            ui_widgets.child.begin(m_crypt("Accuracy"), { 0, 90 }); {
                ui_widgets.checkbox(m_crypt("Disable Recoil"), &g_config->weapons.recoil);
                ui_widgets.checkbox(m_crypt("Disable Spread"), &g_config->weapons.spread);
            } ui_widgets.child.end();
        }
    }; _end_column;

    _column_right_no_sub("c_wep_mgmt_2"); {
        if (g_config->weapons.manager_type == 0) {
            ui_widgets.child.begin(m_crypt("Other"), { 0, 90 }); {
                ui_widgets.checkbox(m_crypt("Change Reload Speed"), &g_config->weapons.reload);
                if (ui_widgets.popup.begin_settings(m_crypt("Reload Speed Settings"), 250)) {
                    ui_widgets.slider_int(m_crypt("Reload Speed"), &g_config->weapons.modify_values.reload_speed_percent, 0, 100, "+%d%%");
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Change Weapon Range"), &g_config->weapons.weapon_range);
                if (ui_widgets.popup.begin_settings(m_crypt("Weapon Range Settings"), 230)) {
                    ui_widgets.slider_int(m_crypt("Weapon Range"), &g_config->weapons.modify_values.weapon_range_meters, 20, 1000, "%d% M");
                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();
        }

        else if (g_config->weapons.manager_type == 1) {
            ui_widgets.child.begin(m_crypt("Other"), { 0, 90 }); {
                ui_widgets.checkbox(m_crypt("No Weapon Reload"), &g_config->weapons.reload);
                ui_widgets.checkbox(m_crypt("Infinity Weapon Range"), &g_config->weapons.weapon_range);
            } ui_widgets.child.end();
        }

    }; _end_column;
}