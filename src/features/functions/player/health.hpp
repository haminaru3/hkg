#pragma once
#include "../func_utils.hpp"

namespace functions {
	namespace player {
		void godmode() {
			if (g_config->player.health.godmode) {
				if (g_config->player.health.disable_on_admin && cheat_base->admin_counter != 0) return;
				if (local.player->HP < g_config->player.health.constant_settings.value + 100)
					local.player->HP = g_config->player.health.constant_settings.value + 100;
			}
		}
	}
}