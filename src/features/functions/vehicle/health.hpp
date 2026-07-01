#pragma once
#include "../func_utils.hpp"

namespace functions {
	namespace vehicle {
		void godmode() {
			int player_ped = pointer_to_handle((uintptr_t)local.player);
			int vehicle = pointer_to_handle((uintptr_t)local.player->vehicle());
			if (native::vehicle::get_vehicle_engine_health(vehicle) < 980) {
				native::vehicle::set_vehicle_engine_health(vehicle, 1000.0f);
			}
			if (native::vehicle::get_vehicle_body_health(vehicle) < 1000) {
				native::vehicle::set_vehicle_body_health(vehicle, 1000.0f);
				native::vehicle::set_vehicle_deformation_fixed(vehicle);
				native::vehicle::set_vehicle_fixed(vehicle);
			}
			if (native::vehicle::get_vehicle_petrol_tank_health(vehicle) < 980) {
				native::vehicle::set_vehicle_petrol_tank_health(vehicle, 1000.0f);
			}
		}

		void seatbelt() {
			if (!cheat_base->funcs.single.vehicle.seatbelt && g_config->vehicles.health.seatbelt)
			{
				int player_ped = pointer_to_handle((uintptr_t)local.player);
				native::ped::set_ped_config_flag(player_ped, 32, false); // PED_FLAG_CAN_FLY_THRU_WINDSCREEN = 32
				native::ped::set_ped_can_ragdoll_from_player_impact(player_ped, false);
			}
			else if (!g_config->vehicles.health.seatbelt && cheat_base->funcs.single.vehicle.seatbelt)
			{
				int player_ped = pointer_to_handle((uintptr_t)local.player);
				native::ped::set_ped_config_flag(player_ped, 32, true); // PED_FLAG_CAN_FLY_THRU_WINDSCREEN = 32
				native::ped::set_ped_can_ragdoll_from_player_impact(player_ped, true);
			}
		}

		void bulletproof_tyres() {
			//if (g_config->vehicles.health.bulletproof_tyres)
			//{
			//	int vehicle = pointer_to_handle((uintptr_t)local.player->vehicle());

			//	if (native::entity::does_entity_exist(vehicle))
			//	{
			//		native::vehicle::set_vehicle_tyres_can_burst(vehicle, false);
			//	}
			//}
			//else if (!g_config->vehicles.health.bulletproof_tyres && cheat_base->funcs.single.vehicle.bulletproof_tyres)
			//{
			//	int vehicle = pointer_to_handle((uintptr_t)local.player->vehicle());
			//	native::vehicle::set_vehicle_tyres_can_burst(vehicle, true);
			//}

			// бред ебаный
			// переписать
		}
	}
}