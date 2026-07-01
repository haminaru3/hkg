#include "c_ui_tabs.hpp"

struct s_world_sub {
    enum e_sub {
        VISUAL,
        OBJECTS,
        VEHICLES,
        WAYPOINTS
    };

    e_sub current{ VISUAL };

    std::vector<std::tuple<const char*, e_sub>> sub_arr = {
        {m_crypt("Main"), VISUAL},
        //{m_crypt("Objects"), OBJECTS},
        //{m_crypt("Vehicles"), VEHICLES},
        //{m_crypt("Waypoints"), WAYPOINTS}
    };
} world_sub;

bool slider_int_12hr(const char* label, int* v, int* hours, int* minutes, int v_min, int v_max);

void c_ui_tabs::world_tab() {
    ui_widgets.misc.subtab_frame();
    ImGui::BeginGroup(); {
        for (const auto& [name, _id] : world_sub.sub_arr) {
            if (ui_widgets.tab.sub(name, world_sub.current == _id)) { world_sub.current = _id; }
        }
    } ImGui::EndGroup();

    switch (world_sub.current)
    {
    case world_sub.e_sub::VISUAL:
        _column_left("cwrld_vis_1"); {
            ui_widgets.child.begin(m_crypt("Additional"), { 0, 97 }); {
                ui_widgets.checkbox("Time Changer", &g_config->world.visuals.timechanger);
                if (ui_widgets.popup.begin_settings("Time Settings", 220, false)) {
                    slider_int_12hr("Value", &g_config->world.visuals.nformated_time, &g_config->world.visuals.time_hour, &g_config->world.visuals.time_minute, 0, 2359);
                    ui_widgets.popup.end_settings();
                }

                ui_widgets.checkbox("Weather Changer", &g_config->world.visuals.weatherchanger);
                if (ui_widgets.popup.begin_settings("Weather Settings", 190, false)) {
                    const char* weather_types[14] = {
                    "Extra Sunny", "Clear", "Cloudy", "Smoggy", "Foggy", "Overcast",
                    "Rain", "Thunder", "Clearing", "Neutral", "Snow", "Blizzard",
                    "Snowlight", "Christmas" };

                    ui_widgets.combo("Value", &g_config->world.visuals.weather_type, weather_types, IM_ARRAYSIZE(weather_types));
                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();
        }; _end_column;

        _column_right("cwrld_vis_2"); {
            ui_widgets.child.begin(m_crypt("General"), { 0, 97 }); {
                ui_widgets.checkbox(m_crypt("ClickWarp"), &g_config->world.clickwarp.enabled);
                if (ui_widgets.popup.begin_settings("ClickWarp Settings", 220, false)) {
                    ui_widgets.keybind("Keybind", &g_config->world.clickwarp.key, true);
                    ui_widgets.keybind("Cancel Bind", &g_config->world.clickwarp.cancel_key, true);
                    //const char* marker_styles[2] = { "Cone", "Cube" };
                    //ui_widgets.combo("Marker Style", &g_config->world.clickwarp.marker_type, marker_styles, IM_ARRAYSIZE(marker_styles));
                    ui_widgets.color.edit_4(m_crypt("Marker Color"), g_config->world.clickwarp.marker_col, cp_flags_w_label);
                    ui_widgets.checkbox_cpicker("Marker Distance", &g_config->world.clickwarp.draw_distance, "dstclwp", g_config->world.clickwarp.distance_col);
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox("Camera FOV", &g_config->world.visuals.cam_fov);
                if (ui_widgets.popup.begin_settings("Cam FOV Settings", 190, false)) {
                    ui_widgets.slider_float("Value", &g_config->world.visuals.cam_fov_v, 50.f, 120.f, "%.0f\xC2\xB0");

                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();
        }; _end_column;
        break;
    }
}

bool slider_int_12hr(const char* label, int* v, int* hours, int* minutes, int v_min, int v_max)
{
    static c_ui_widgets local_ui_w;

    if (*v < v_min) *v = v_min;
    if (*v > v_max) *v = v_max;

    *hours = *v / 100;
    *minutes = *v % 100;

    if (*minutes >= 60) *minutes = 59;

    int display_hours = *hours % 12;
    if (display_hours == 0) display_hours = 12;

    const char* am_pm = (*hours < 12) ? "AM" : "PM";

    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d %s", display_hours, *minutes, am_pm);

    bool value_changed = local_ui_w.slider_int(label, v, v_min, v_max, timeStr);

    return value_changed;
}