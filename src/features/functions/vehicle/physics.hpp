#pragma once
#include "../func_utils.hpp"

namespace functions {
	namespace vehicle {
		void no_collision() {
			auto l_player = pointer_to_handle((uintptr_t)local.player);
			auto lp_veh = pointer_to_handle((uintptr_t)local.player->vehicle());
			if (g_config->vehicles.disable_collision) {

				// remove coll between cur veh and other vehicles in stream
				if (g_config->vehicles.col_filters.vehicles) {
					for (int i = 0; i < Game.ReplayInterface->vehicle_interface->max_vehicles; i++) {
						CVehicle* veh = reinterpret_cast<CVehicle*>(Game.ReplayInterface->vehicle_interface->get_vehicle(i));
						if (IsValidPtr(veh)) {
							if (get_distance(local.player->fPosition, veh->fPosition) > 50.f) continue;
							auto veh_handle = pointer_to_handle((uintptr_t)veh);
							if (veh_handle) {
								native::entity::set_entity_no_collision_entity(veh_handle, lp_veh, false);
							}
						}
					}
				}

				// remove coll between cur veh and players peds in stream
				if (g_config->vehicles.col_filters.players) {
					for (int i = 0; i < Game.ReplayInterface->ped_interface->max_peds; i++) {
						CVehicle* ped = reinterpret_cast<CVehicle*>(Game.ReplayInterface->ped_interface->get_ped(i));
						if (IsValidPtr(ped)) {
							if (get_distance(local.player->fPosition, ped->fPosition) > 50.f) continue;
							auto ped_handle = pointer_to_handle((uintptr_t)ped);
							if (ped_handle) {
								native::entity::set_entity_no_collision_entity(ped_handle, lp_veh, false);
							}
						}
					}
				}
				cheat_base->funcs.single.vehicle.coll_removed = true;
			}
			else if (cheat_base->funcs.single.vehicle.coll_removed || !g_config->vehicles.disable_collision || !g_config->vehicles.col_filters.vehicles || !g_config->vehicles.col_filters.players) {
				// returning coll between our veh and all other vehicles
				if (!g_config->vehicles.col_filters.vehicles || !g_config->vehicles.disable_collision) {
					for (int i = 0; i < Game.ReplayInterface->vehicle_interface->max_vehicles; i++) {
						CVehicle* veh = reinterpret_cast<CVehicle*>(Game.ReplayInterface->vehicle_interface->get_vehicle(i));
						if (IsValidPtr(veh)) {
							if (get_distance(local.player->fPosition, veh->fPosition) > 150.f) continue;
							auto veh_handle = pointer_to_handle((uintptr_t)veh);
							if (veh_handle) {
								native::entity::set_entity_no_collision_entity(veh_handle, lp_veh, true);
							}
						}
					}
				}
				// returning coll between our veh and player peds in stream
				if (!g_config->vehicles.col_filters.players || !g_config->vehicles.disable_collision) {
					for (int i = 0; i < Game.ReplayInterface->ped_interface->max_peds; i++) {
						CVehicle* ped = reinterpret_cast<CVehicle*>(Game.ReplayInterface->ped_interface->get_ped(i));
						if (IsValidPtr(ped)) {
							if (get_distance(local.player->fPosition, ped->fPosition) > 150.f) continue;
							auto ped_handle = pointer_to_handle((uintptr_t)ped);
							if (ped_handle) {
								native::entity::set_entity_no_collision_entity(ped_handle, lp_veh, true);
							}
						}
					}
				}
				cheat_base->funcs.single.vehicle.coll_removed = false;
			}
		}
	}
}