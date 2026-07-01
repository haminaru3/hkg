#include "c_ui_tabs.hpp"

struct s_player_sub {
    enum e_sub {
        HEALTH,
        MOVEMENT,
        EXPERIMENTAL
    };

    e_sub current{ HEALTH };

    std::vector<std::tuple<const char*, e_sub>> sub_arr = {
        {m_crypt("Health"), HEALTH},
        {m_crypt("Movement"), MOVEMENT},
        {m_crypt("Experimental"), EXPERIMENTAL},
    };
} player_sub;

void c_ui_tabs::player_tab() {
    ui_widgets.misc.subtab_frame();
    ImGui::BeginGroup(); {
        for (const auto& [name, _id] : player_sub.sub_arr) {
            if (ui_widgets.tab.sub(name, player_sub.current == _id)) { player_sub.current = _id; }
        }
    } ImGui::EndGroup();

    switch (player_sub.current)
    {
    case player_sub.e_sub::HEALTH:
        //g_ui_misc.hint(m_crypt("Hint: Healing"), hints::healing_tab, hints::pos);

        _column_left("cheal_1"); {
            ui_widgets.child.begin(m_crypt("General"), ImVec2(0, 95)); {
                ui_widgets.checkbox(m_crypt("GodMode"), &g_config->player.health.godmode);
                if (ui_widgets.popup.begin_settings(m_crypt("GodMode Settings"), 230)) {
                    // idk wtf is this, dont even ask
                    ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.7f), m_crypt("GodMode Key"));
                    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(-2, 26));
                    ui_widgets.keybind(m_crypt("gmode-bind"), &g_config->player.health.godmode_key);
                    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, 4));

                    ui_widgets.slider_int(m_crypt("Freeze Value"), &g_config->player.health.constant_settings.value, 0, 100, "%d% HP");
                    ui_widgets.checkbox(m_crypt("Disable if Admins Near"), &g_config->player.health.disable_on_admin);

                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Heal Hit"), &g_config->player.health.healing.enabled);
                if (ui_widgets.popup.begin_settings(m_crypt("Heal Hit Settings"), 200)) {
                    const char* gm_types[2] = { "Heal At", "Set Health" };
                    ui_widgets.combo(m_crypt("Heal Type"), &g_config->player.health.healing.heal_type, gm_types, IM_ARRAYSIZE(gm_types));
                    switch (g_config->player.health.healing.heal_type)
                    {
                    case 0:
                        ui_widgets.slider_int(m_crypt("Heal Value"), &g_config->player.health.healing.heal_value, 0, 100, "+%d% HP");
                        ui_widgets.button(m_crypt("Heal Player"), ImVec2(0, 30));
                        break;
                    case 1:
                        ui_widgets.slider_int(m_crypt("Health Value"), &g_config->player.health.healing.set_value, 0, 100, "%d% HP");
                        ui_widgets.button(m_crypt("Set Health"), ImVec2(0, 30));
                        break;
                    }
                    ui_widgets.popup_key(m_crypt("#hp_fill"), m_crypt("Heal Key"), &g_config->player.health.healing.heal_key_state, &g_config->player.health.healing.heal_key);
                    ui_widgets.popup.end_settings();
                }

            } ui_widgets.child.end();
        }; _end_column;

        _column_right("cheal_2"); {
            ui_widgets.child.begin("Buttons", ImVec2(0, 215)); {
                if (ui_widgets.button(m_crypt("Set Full Health"), ImVec2(0, 30))) {
                    g_singleuse->sethealth = true;
                }
                ui_widgets.popup_key(m_crypt("#hp_fill"), m_crypt("Full Health Hotkey"), &g_config->player.health.btn_hotkeys.full_hp, &g_config->player.health.btn_hotkeys.full_hp_key);

                if (ui_widgets.button(m_crypt("Set Full Armor"), ImVec2(0, 30))) {
                    g_singleuse->setarmor = true;
                }
                ui_widgets.popup_key(m_crypt("#arm_fill"), m_crypt("Full Armor Hotkey"), &g_config->player.health.btn_hotkeys.full_arm, &g_config->player.health.btn_hotkeys.full_arm_key);

                if (ui_widgets.button(m_crypt("Commit Suicide"), ImVec2(0, 30))) {
                    g_singleuse->suicide = true;
                }
                ui_widgets.popup_key(m_crypt("#roskomnadzor_key"), m_crypt("Suicide Hotkey"), &g_config->player.health.btn_hotkeys.suicide, &g_config->player.health.btn_hotkeys.suicide_key);

                if (ui_widgets.button(m_crypt("Clear Visible Damage"), ImVec2(0, 30))) {
                    g_singleuse->clearvisibledamage = true;
                }
                ui_widgets.popup_key(m_crypt("#clr-vis-dmg"), m_crypt("Clear Hotkey"), &g_config->player.health.btn_hotkeys.clr_visible_dmg, &g_config->player.health.btn_hotkeys.cvd_key);

            } ui_widgets.child.end();
        }; _end_column;

        break;

    case player_sub.e_sub::MOVEMENT:
        //g_ui_misc.hint(m_crypt("Hint: Movement"), hints::mv_player_tab, hints::pos);

        _column_left("cmove_1"); {
            ui_widgets.child.begin(m_crypt("General"), { 0, 200 }); {
                ui_widgets.checkbox(m_crypt("Disable Ragdoll"), &g_config->player.movement.player_mgmt.disable_ragdoll);
                ui_widgets.checkbox(m_crypt("Disable Collision"), &g_config->player.movement.player_mgmt.disable_collision);
                if (ui_widgets.popup.begin_settings("Player Collision Settings", 190)) {
                    ui_widgets.checkbox("Players", &g_config->player.movement.player_mgmt.col_filters.players);
                    ui_widgets.checkbox("Vehicles", &g_config->player.movement.player_mgmt.col_filters.vehicles);
                    ui_widgets.checkbox("Objects", &g_config->player.movement.player_mgmt.col_filters.objects);
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Infinity Stamina"), &g_config->player.movement.player_mgmt.inf_stamina);

                ui_widgets.checkbox(m_crypt("Fall Break"), &g_config->player.movement.player_mgmt.fallbreak);
                if (ui_widgets.popup.begin_settings(m_crypt("Fall Break Settings"), 220)) {
                    ui_widgets.slider_float(m_crypt("Threshold"), &g_config->player.movement.player_mgmt.fb_treshold, 0.f, 3.f, "+%.1f m");
                    ui_widgets.popup.end_settings();
                }

                ui_widgets.checkbox(m_crypt("Sprint Boost"), &g_config->player.movement.player_mgmt.sh_enabled);

                ui_widgets.checkbox(m_crypt("Skip Ladder"), &g_config->player.movement.player_mgmt.skip_ladder);
                if (ui_widgets.popup.begin_settings(m_crypt("=)"), 220, false)) {
                    ui_widgets.checkbox(m_crypt("ladder thru the moon"), &g_config->player.movement.player_mgmt.ladder_to_moon);
                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();

            ui_widgets.child.begin(m_crypt("Cortex Airlines"), { 0, 95 }); {
                ui_widgets.checkbox(m_crypt("Noclip"), &g_config->player.movement.noclip.enabled);
                if (ui_widgets.popup.begin_settings(m_crypt("Noclip Settings"), 200)) {
                    ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.7f), m_crypt("Noclip Key"));
                    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(-2, 28));
                    ui_widgets.keybind(m_crypt("nclip-bind"), &g_config->player.movement.noclip.keybind);
                    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, 4));
                    ui_widgets.slider_float(m_crypt("Speed"), &g_config->player.movement.noclip.speed, 0.f, 10.f);
                    ui_widgets.checkbox_bind(m_crypt("Boost"), &g_config->player.movement.noclip.boost, &g_config->player.movement.noclip.boost_key);
                    ui_widgets.checkbox(m_crypt("Warp On Ground"), &g_config->player.movement.noclip.warp_on_ground);
                    ui_widgets.popup.end_settings();
                }

                if (ui_widgets.popup.begin_settings(m_crypt("Additional Settings"), 220, true)) {
                    ui_widgets.checkbox(m_crypt("Invert Player Yaw"), &g_config->player.movement.noclip.invert_yaw);
                    ui_widgets.checkbox(m_crypt("Enable Player Lean"), &g_config->player.movement.noclip.enable_lean);
                    ui_widgets.checkbox(m_crypt("T-Pose"), &g_config->player.movement.noclip.t_pose);
                    ui_widgets.checkbox(m_crypt("Roll Anim"), &g_config->player.movement.noclip.go_stupid);
                    ui_widgets.slider_int(m_crypt("Roll Speed"), &g_config->player.movement.noclip.stupid_speed, 1, 30, "%d%");
                    //ui_widgets.checkbox(m_crypt("Catch Vehicles"), &g_config->player.movement.noclip.catch_vehicles);
                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();
        }; _end_column;

        _column_right("cmove_2"); {
            ui_widgets.child.begin(m_crypt("Teleportation"), { 0, 80 }); {
                if (ui_widgets.button(m_crypt("Teleport To Blip Waypoint"), ImVec2(0, 30))) {
                    g_singleuse->tptowp = true;
                }
                ui_widgets.popup_key(m_crypt("#tp_k_wp"), m_crypt("Teleport Key"), &g_config->player.movement.teleport.teleport_on_key,
                    &g_config->player.movement.teleport.teleport_key);

            } ui_widgets.child.end();

            ui_widgets.child.begin(m_crypt("Camera"), { 0, 110 }); {
                ui_widgets.checkbox(m_crypt("FreeCam"), &g_config->player.movement.freecam.enabled);
                if (ui_widgets.popup.begin_settings(m_crypt("FreeCam Settings"), 200)) {
                    ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.7f), m_crypt("FreeCam Key"));
                    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(-2, 28));
                    ui_widgets.keybind(m_crypt("freecum-bind"), &g_config->player.movement.freecam.keybind);
                    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, 4));

                    ui_widgets.slider_float(m_crypt("Speed"), &g_config->player.movement.freecam.speed, 0, 10);
                    ui_widgets.checkbox_bind(m_crypt("Boost"), &g_config->player.movement.freecam.boost, &g_config->player.movement.freecam.boost_key);

                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();

            ui_widgets.child.begin("Additional", { 0, 80 }); {
                if (ui_widgets.button(m_crypt("Clear Ped Tasks"), ImVec2(0, 30))) {
                    g_singleuse->clearpedtasks = true;
                }
                ui_widgets.popup_key(m_crypt("#tp_k_wp"), m_crypt("Clear Ped Tasks Key"), &g_config->player.movement.player_mgmt.clr_ped_tasks_on_key,
                    &g_config->player.movement.player_mgmt.clr_ped_tasks_key);

            } ui_widgets.child.end();
        }; _end_column;

        break;

    case player_sub.e_sub::EXPERIMENTAL:
        //g_ui_misc.hint(m_crypt("Hint: Movement"), hints::mv_player_tab, hints::pos);
        _column_left("cplayer_expl_1"); {
            ui_widgets.child.begin("Anti Anti-Cheat", { 0, 165 }); {
                ui_widgets.checkbox("GetGroundZFor3DCoord", &g_config->player.native_hooks.get_ground_z_for_3d_coord);
                ui_widgets.checkbox("FreezeEntityPosition", &g_config->player.native_hooks.freeze_entity_position);
                ui_widgets.checkbox("TaskGoStraightToCoord", &g_config->player.native_hooks.task_go_straight_to_coord);
                ui_widgets.checkbox("TaskStandStill", &g_config->player.native_hooks.task_stand_still);
            } ui_widgets.child.end();
        }; _end_column;
        break;
    }
}
