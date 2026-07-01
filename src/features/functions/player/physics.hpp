#pragma once
#include "../func_utils.hpp"

namespace functions {
	namespace player {
		void no_ragdoll() {
			if (g_config->player.movement.player_mgmt.disable_ragdoll) {
				local.player->setRagdoll(0x1);
				cheat_base->funcs.single.player.ragdoll_removed = true;
			}
			else if (!g_config->player.movement.player_mgmt.disable_ragdoll && cheat_base->funcs.single.player.ragdoll_removed) {
				cheat_base->funcs.single.player.ragdoll_removed = false;
				local.player->setRagdoll(0x20);
			}
		}

		void no_collision() {
			if (g_config->player.movement.player_mgmt.disable_collision || cheat_base->funcs.noclip.state) {
				local.player->setCollision(-1.0f);
				cheat_base->funcs.single.player.ped_coll_removed = true;
			}
			else if ((!g_config->player.movement.player_mgmt.disable_collision || !cheat_base->funcs.noclip.state) && cheat_base->funcs.single.player.ped_coll_removed) {

				local.player->setCollision(0.25f);
				cheat_base->funcs.single.player.ped_coll_removed = false;
			}
		}
	}
}