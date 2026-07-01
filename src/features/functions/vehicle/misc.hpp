#pragma once
#include "../func_utils.hpp"

namespace functions {
	namespace vehicle {
		void always_clean() {
			int vehicle = pointer_to_handle((uintptr_t)local.player->vehicle());
			if (native::entity::does_entity_exist(vehicle))
				native::vehicle::set_vehicle_dirt_level(vehicle, 0.0f);
		}

		void seat_warp() {
			auto player_ped = pointer_to_handle((uintptr_t)local.player);
			if (g_config->vehicles.seatwarp.into) {
				if (native::brain::get_script_task_status(player_ped, 0x950B6492)) { // 0x950B6492 -- SCRIPT_TASK_ENTER_VEHICLE
					auto veh_to_enter = native::ped::get_vehicle_ped_is_trying_to_enter(player_ped);
					if (native::entity::does_entity_exist(veh_to_enter)) {
						PVector3 veh_pos = native::entity::get_entity_coords(veh_to_enter, true);
						if (get_distance(local.player->fPosition, Vector3(veh_pos.x, veh_pos.y, veh_pos.z)) < 2.5f)
							native::vehicle::set_vehicle_doors_locked_for_player(veh_to_enter, player_ped, false);
							native::ped::set_ped_into_vehicle(player_ped, veh_to_enter, -1);
					}
				}
			}
		}

		void car_hijacking() {
			if (g_config->vehicles.exploits.return_stealing && !cheat_base->funcs.single.vehicle.hijack_flags) {
				int player_ped = native::player::player_ped_id();
				native::ped::set_ped_config_flag(player_ped, 342, false);
				native::ped::set_ped_config_flag(player_ped, 252, true);
				native::ped::set_ped_config_flag(player_ped, 141, true);

				cheat_base->funcs.single.vehicle.hijack_flags = true;
			}
			else if (cheat_base->funcs.single.vehicle.hijack_flags && !g_config->vehicles.exploits.return_stealing) {
				int player_ped = native::player::player_ped_id();
				native::ped::set_ped_config_flag(player_ped, 342, true);
				native::ped::set_ped_config_flag(player_ped, 252, false);
				native::ped::set_ped_config_flag(player_ped, 141, false);

				cheat_base->funcs.single.vehicle.hijack_flags = false;
			}
			//if (g_config->vehicles.exploits.return_stealing) {
			//	for (CVehicle*& veh : game::vehicle_list) {
			//		veh->setUnlocked(0x0);
			//	}
			//}
		}
	}
}