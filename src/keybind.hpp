#pragma once
#include "imports.h"


namespace hotkeys {
    enum HotKeyMode
    {
        MODE_PRESS,
        MODE_TOGGLE,
        MODE_HOLD
    };

    struct hotkey_struct
    {
        int* key = 0;
        int mode = MODE_PRESS;

        bool* value = nullptr;
        bool statetemp = false;

        ///  <param name="_value">Config var</param>
        hotkey_struct(bool* _value)
        {
            key = 0;
            value = _value;
        }

        ///  <param name="_value">Config var</param>
        ///  <param name="_key">Key for a keybind</param>
        hotkey_struct(bool* _value, int* _key)
        {
            key = _key;
            value = _value;
        }

        hotkey_struct(bool* _value, int* _key, HotKeyMode _mode)
        {
            key = _key;
            value = _value;
            mode = _mode;
        }
    };

    std::vector<hotkey_struct> hotkeys;


    void keybind_runner() {
        if (g_config->controls.menu_state) return;

        for (auto& hk : hotkeys) {
            if (*hk.key == NULL) continue;
            if (hk.mode == MODE_PRESS)
            {
                if (GetAsyncKeyState(*hk.key) < 0) {
                    //down
                    if (!hk.statetemp) {
                        *hk.value = true;
                        hk.statetemp = true;
                    }
                }
                else {
                    hk.statetemp = false;
                }

            }
            else if (hk.mode == MODE_TOGGLE) {
                if (GetAsyncKeyState(*hk.key) < 0) {
                    //down
                    if (!hk.statetemp) {
                        *hk.value = !(*hk.value);
                        hk.statetemp = true;
                    }
                }
                else {
                    hk.statetemp = false;
                }
            }
            else if (hk.mode == MODE_HOLD)
            {
                if (GetAsyncKeyState(*hk.key) < 0) {
                    *hk.value = true;

                }
                else {
                    *hk.value = false;
                }
            }
        }
    }

    void init_hotkeys() {
        hotkeys.push_back({ &g_singleuse->clearpedtasks, &g_config->player.movement.player_mgmt.clr_ped_tasks_key });
		hotkeys.push_back({ &g_singleuse->sethealth, &g_config->player.health.btn_hotkeys.full_hp_key });
		hotkeys.push_back({ &g_singleuse->setarmor, &g_config->player.health.btn_hotkeys.full_arm_key });
		//hotkeys.push_back({ &g_singleuse->tptowp, &g_config->player.movement.player_mgmt.tp_to_wp_key });
		hotkeys.push_back({ &g_singleuse->suicide, &g_config->player.health.btn_hotkeys.suicide_key });
        hotkeys.push_back({ &g_singleuse->flipvehicle, &g_config->vehicles.flip_key });
    }

}