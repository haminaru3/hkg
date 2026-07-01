#include "c_ui_tabs.hpp"

struct s_vehicle_sub{
    enum e_sub {
        MOVEMENT,
        HEALTH,
        EXPLOITS
    };

    e_sub current{ MOVEMENT };

    std::vector<std::tuple<const char*, e_sub>> sub_arr = {
        {m_crypt("Movement"), MOVEMENT},
        {m_crypt("Health"), HEALTH},
        {m_crypt("Experimental"), EXPLOITS}
    };
} vehicle_sub;


void c_ui_tabs::vehicle_tab() {
    ui_widgets.misc.subtab_frame();
    ImGui::BeginGroup(); {
        for (const auto& [name, _id] : vehicle_sub.sub_arr) {
            if (ui_widgets.tab.sub(name, vehicle_sub.current == _id)) { vehicle_sub.current = _id; }
        }
    } ImGui::EndGroup();

    switch (vehicle_sub.current)
    {
    case vehicle_sub.e_sub::MOVEMENT:
        _column_left("cveh_mov_1"); {
            ui_widgets.child.begin(m_crypt("General"), { 0, 120 }); {
                ui_widgets.checkbox(m_crypt("Disable Collision"), &g_config->vehicles.disable_collision);
                if (ui_widgets.popup.begin_settings(m_crypt("Vehicle Collision Settings"), 190)) {
                    ui_widgets.checkbox(m_crypt("Players"), &g_config->vehicles.col_filters.players);
                    ui_widgets.checkbox(m_crypt("Vehicles"), &g_config->vehicles.col_filters.vehicles);
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Acceleration"), &g_config->vehicles.boost.enabled);
                if (ui_widgets.popup.begin_settings(m_crypt("Acceleration Settings"), 220)) {
                    ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.7f), m_crypt("Use Key"));
                    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(-2, 28));
                    ui_widgets.keybind(m_crypt("vehaccel-bind"), &g_config->vehicles.boost.key);
                    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, 4));

                    ui_widgets.slider_float(m_crypt("Value"), &g_config->vehicles.boost.accel_value, 60.f, 360.f, "%.1f km/h");
                    ui_widgets.checkbox(m_crypt("Smooth Increase"), &g_config->vehicles.boost.smooth_increase);
                    ui_widgets.checkbox(m_crypt("Downward Force"), &g_config->vehicles.boost.pin_to_ground);
                    ui_widgets.popup.end_settings();
                }

                ui_widgets.checkbox("Seat Warp", &g_config->vehicles.seatwarp.enabled);
                if (ui_widgets.popup.begin_settings("Seat Warp Settings", 190)) {
                    ui_widgets.checkbox("Warp Into", &g_config->vehicles.seatwarp.into);
                    ui_widgets.checkbox("Exit Warp", &g_config->vehicles.seatwarp.exit);
                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();


        }; _end_column;
            
        _column_right("cveh_mov_2"); {
            ui_widgets.child.begin(m_crypt("Additional"), { 0, 110 }); {
                ui_widgets.checkbox_bind(m_crypt("Instant 180\xC2\xB0"), &g_config->vehicles.instant_180, &g_config->vehicles.instant_180_key);
                if (ui_widgets.button(m_crypt("Flip Vehicle"), ImVec2(0, 30))) {
                    g_singleuse->flipvehicle = true;
                }
                ui_widgets.popup_key(m_crypt("#monkeflip"), m_crypt("Flip Key"), &g_config->vehicles.flip_on_key, &g_config->vehicles.flip_key);
            } ui_widgets.child.end();
            //ui_widgets.child.begin(m_crypt("elon musk suck my dick"), { 0, 150 }); {
            //    ui_widgets.checkbox(m_crypt("Auto-Pilot"), &g_config->vehicles.autopilot.enabled);
            //    if (ui_widgets.popup.begin_settings(m_crypt("Auto-Pilot Manager"), 220)) {
            //        const char* drivemode_list[2] = { "Obedient Boy", "Vin Diesel" };
            //        ui_widgets.combo(m_crypt("Drive Mode"), &g_config->vehicles.autopilot.drive_mode, drivemode_list, IM_ARRAYSIZE(drivemode_list));

            //        ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.7f), m_crypt("Fast Stop"));
            //        ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(-2, 28));
            //        ui_widgets.keybind(m_crypt("at-stop-bind"), &g_config->vehicles.autopilot.fast_stop_key);
            //        ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, 4));

            //        ui_widgets.popup.end_settings();
            //    }
            //    if (ui_widgets.button(g_singleuse->autopilot_inuse ? "Stop Auto-Pilot" : "Start Auto-Pilot", ImVec2(0, 30))) {
            //        if (!g_singleuse->autopilot_inuse)
            //            g_singleuse->startautopilot = true, g_singleuse->autopilot_inuse = true;
            //        else
            //            g_singleuse->stopautopilot = true, g_singleuse->autopilot_inuse = false;
            //    }
            //} ui_widgets.child.end();
        }; _end_column;
        break;

    case vehicle_sub.e_sub::HEALTH:
        _column_left("cveh_hp_1"); {
            ui_widgets.child.begin(m_crypt("General"), { 0, 120 }); {
                ui_widgets.checkbox(m_crypt("GodMode"), &g_config->vehicles.health.gmode);
                ui_widgets.checkbox(m_crypt("No Deformations"), &g_config->vehicles.health.no_deform);
                ui_widgets.checkbox(m_crypt("Bulletproof Tyres"), &g_config->vehicles.health.bulletproof_tyres);
                //ui_widgets.checkbox(m_crypt("Seat Belt"), &g_config->vehicles.health.seatbelt);
            } ui_widgets.child.end();

            ui_widgets.child.begin("idk", { 0, 165 }); {
                if (ui_widgets.button(m_crypt("Kill Engine"), ImVec2(0, 30))) {
                    g_singleuse->killvehengine = true;
                }

                if (ui_widgets.button(m_crypt("Kill GasTank"), ImVec2(0, 30))) {
                    g_singleuse->killvehgastank = true;
                }

                if (ui_widgets.button(m_crypt("Blow Up Tyres"), ImVec2(0, 30))) {
                    g_singleuse->popvehicletyres = true;
                }
            } ui_widgets.child.end();

            //ui_widgets.child.begin(m_crypt("Additional"), { 0, 95 }); {
            //    ui_widgets.checkbox(m_crypt("Always Clean"), &g_config->vehicles.health.always_clean);
            //} ui_widgets.child.end();
        }; _end_column;

        _column_right("cveh_hp_2"); {
            ui_widgets.child.begin(m_crypt("Healing"), { 0, 130 }); { // 165
                if (ui_widgets.button(m_crypt("Fix Vehicle"), ImVec2(0, 30))) {
                    g_singleuse->fixvehicle = true;
                }

                if (ui_widgets.button(m_crypt("Fix Deformations"), ImVec2(0, 30))) {
                    g_singleuse->fixvehdeformations = true;
                }

                //if (ui_widgets.button(m_crypt("Clean Vehicle"), ImVec2(0, 30))) {
                //    g_singleuse->cleanvehicle = true;
                //}
            } ui_widgets.child.end();
        }; _end_column;

        break;
    case vehicle_sub.e_sub::EXPLOITS:

        _column_left("cveh_expl_1"); {
            ui_widgets.child.begin("General", { 0, 105 }); {
                ui_widgets.checkbox("Return Hijacking", &g_config->vehicles.exploits.return_stealing);
                if (ui_widgets.button(m_crypt("Start Engine"), ImVec2(0, 30))) {
                    g_singleuse->startvehengine = true;
                }
            } ui_widgets.child.end();
        }; _end_column;
        _column_right("cveh_expl_2"); {
            ui_widgets.child.begin("Anti-Cheat Hooks", { 0, 165 }); {
                ui_widgets.checkbox("GetEntitySpeed", &g_config->vehicles.native_hooks.get_entity_speed);
                ui_widgets.checkbox("GetEntityVelocity", &g_config->vehicles.native_hooks.get_entity_velocity);
                ui_widgets.checkbox("GetIsVehicleEngineRunning", &g_config->vehicles.native_hooks.get_is_vehicle_engine_running);
                ui_widgets.checkbox("SetVehicleEngineOn", &g_config->vehicles.native_hooks.set_vehicle_engine_on);
                ui_widgets.checkbox("GetVehicleUndriveable", &g_config->vehicles.native_hooks.set_vehicle_undriveable);
            } ui_widgets.child.end();
        }; _end_column;
        break;
    }
}