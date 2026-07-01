#include "../widgets/c_ui_widgets.hpp"
#include <vector>
#include "../../protection/prot_defs.h"
#include "../../c_config.hpp"
#include "../interface_variant.hpp"

#define _column_left(name) ImGui::SetCursorPos({ CORTEX_CONTENT_X, CORTEX_CONTENT_Y }), ImGui::BeginChild(str_hash_id(name), { CORTEX_COL_W,CORTEX_COL_H }, false);
#define _column_right(name) ImGui::SetCursorPos({ CORTEX_CONTENT_RIGHT_X, CORTEX_CONTENT_Y }), ImGui::BeginChild(str_hash_id(name), { CORTEX_COL_W,CORTEX_COL_H }, false);

#define _column_left_no_sub(name) ImGui::SetCursorPos({ CORTEX_CONTENT_X, CORTEX_CONTENT_Y_NO_SUB }), ImGui::BeginChild(str_hash_id(name), { CORTEX_COL_W,CORTEX_COL_H_NO_SUB }, false);
#define _column_right_no_sub(name) ImGui::SetCursorPos({ CORTEX_CONTENT_RIGHT_X, CORTEX_CONTENT_Y_NO_SUB }), ImGui::BeginChild(str_hash_id(name), { CORTEX_COL_W,CORTEX_COL_H_NO_SUB }, false);

#define _end_column ImGui::EndChild();

class c_ui_tabs {
private:
    enum e_main_tabs { 
        AIMBOT, 
        VISUALS,
        PLAYER,
        WEAPONS,
        VEHICLE,
        WORLD,
        CONFIGS,
        SETTINGS
    };

    struct s_main_tabs {
        e_main_tabs current{ AIMBOT };

        std::vector<std::tuple<const char*, const char*, e_main_tabs>> tabs_arr = {
            {m_crypt("B"), m_crypt("Aimbot"), AIMBOT},
            {m_crypt("C"), m_crypt("Visuals"), VISUALS},
            {m_crypt("D"), m_crypt("Player"), PLAYER},
            {m_crypt("E"), m_crypt("Weapons"), WEAPONS},
            {m_crypt("F"), m_crypt("Vehicle"), VEHICLE},
            {m_crypt("P"), m_crypt("World"), WORLD},
            {m_crypt("G"), m_crypt("Configs"), CONFIGS},
            {m_crypt("I"), m_crypt("Settings"), SETTINGS}
        };
    } main;

    c_ui_widgets ui_widgets;

    ImGuiColorEditFlags cp_flags = ImGuiColorEditFlags_NoSidePreview
        | ImGuiColorEditFlags_AlphaBar
        | ImGuiColorEditFlags_NoInputs
        | ImGuiColorEditFlags_AlphaPreview
        | ImGuiColorEditFlags_NoLabel;
    ImGuiColorEditFlags cp_flags_w_label = ImGuiColorEditFlags_NoSidePreview
        | ImGuiColorEditFlags_AlphaBar
        | ImGuiColorEditFlags_NoInputs
        | ImGuiColorEditFlags_AlphaPreview;

public:
    void aimbot_tab();
    void visuals_tab();
    void player_tab();
    void weapons_tab();
    void vehicle_tab();
    void world_tab();
    void configs_tab();
    void settings_tab();

    void render_main() {
        ImGui::SetCursorPos({ CORTEX_NAV_X, CORTEX_NAV_Y });
        ImGui::BeginGroup(); {
            int tab_idx = 0;
            for (const auto& [key, name, tab] : main.tabs_arr) {
                if (ui_widgets.tab.main(key, name, main.current == tab)) {
                    main.current = tab;
                }
#if CORTEX_UI_VARIANT == 1 || CORTEX_UI_VARIANT == 3
                if (++tab_idx < static_cast<int>(main.tabs_arr.size())) {
                    ImGui::SameLine(0, 8);
                }
#else
                ++tab_idx;
#endif
            }
        } ImGui::EndGroup();

        switch (main.current)
        {
        case AIMBOT: aimbot_tab(); break;
        case VISUALS: visuals_tab(); break;
        case PLAYER: player_tab(); break;
        case WEAPONS: weapons_tab(); break;
        case VEHICLE: vehicle_tab(); break;
        case WORLD: world_tab(); break;
        case CONFIGS: configs_tab(); break;
        case SETTINGS: settings_tab(); break;
        }
    }
};

//namespace ui_tabs {
//    // gen tabs
//    enum e_tabs { aimbot, visuals, player, weapons, vehicles, configs, world, settings };
//    static e_tabs main{ aimbot };
//
//    // subtabs
//    enum e_aim_subtab { vector, silent, damager };
//    enum e_visuals_subtab { players, admins, objects, veh_esp, radar };
//    enum e_player_subtab { health, movement, exploits };
//    enum e_weapons_subtab { wep_mngr, misc_wep };
//    enum e_vehicles_subtab { movement_veh, health_veh };
//    enum e_world_subtab { visual, objects_stream, vehicles_stream, waypoints };
//
//    static e_aim_subtab subtabs_aim{ vector };
//    static e_visuals_subtab subtabs_visual{ players };
//    static e_player_subtab subtabs_player{ health };
//    static e_weapons_subtab subtabs_weapon{ wep_mngr };
//    static e_vehicles_subtab subtabs_vehicle{ movement_veh };
//    static e_world_subtab subtabs_world{ visual };
//}
