#pragma once
#include "../func_utils.hpp"

namespace functions {
	namespace player {
		void inf_stamina() {
			if (g_config->player.movement.player_mgmt.inf_stamina) {
				native::player::reset_player_stamina(pointer_to_handle((uintptr_t)local.player));
			}
		}
		void sprintboost() {
			if (g_config->player.movement.player_mgmt.sh_enabled) {
				int player_id = native::player::player_id();
				native::player::set_run_sprint_multiplier_for_player(player_id, 1.49f);
				cheat_base->funcs.single.player.sprintboost = true;
			}

			if (!g_config->player.movement.player_mgmt.sh_enabled && cheat_base->funcs.single.player.sprintboost) {
				int player_id = native::player::player_id();
				if (cheat_base->funcs.single.player.sprintboost) native::player::set_run_sprint_multiplier_for_player(player_id, 1.f),
					cheat_base->funcs.single.player.sprintboost = false; //-V547 //-V571
			}
		}
	}
}