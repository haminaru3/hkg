#pragma once
#include "../func_utils.hpp"

namespace functions {
	namespace vehicle {
		void accel_boost() {
			// shit :skull:skull::skull:
			int veh_boost_key = g_config->vehicles.boost.key;
			if ((Game.keyStates[veh_boost_key]) || ((veh_boost_key <= 6) && (GetAsyncKeyState(veh_boost_key & 0x8000)))) { //-V781
				auto lp_veh = pointer_to_handle((uintptr_t)local.player->vehicle());
				float veh_boost_speed = g_config->vehicles.boost.accel_value;
				native::vehicle::set_vehicle_forward_speed(lp_veh, veh_boost_speed / 3.6); // x * 3.6 = accel in kmh
				if (g_config->vehicles.boost.pin_to_ground) {
					PVector3 vehicle_coords = native::entity::get_entity_coords(lp_veh, true);
					PVector3 vehicle_velocity = native::entity::get_entity_velocity(lp_veh);
					float vertical_velocity = vehicle_velocity.z;
					float downward_force = -0.1f;
					if (g_config->vehicles.boost.pin_to_ground) {
						native::entity::apply_force_to_entity_center_of_mass(lp_veh, 1, 0.0f, 0.0f, downward_force, false, true, true, false);
					}
					vehicle_velocity.z = vertical_velocity > 0 ? -0.1f : vertical_velocity;
					native::entity::set_entity_velocity(lp_veh, vehicle_velocity.x, vehicle_velocity.y, vehicle_velocity.z);
				}
			}
		}

		void instant180() {
			// shit :skull:skull::skull:
			if (GetAsyncKeyState(g_config->vehicles.instant_180_key) & 0x01) {
				auto player_ped = pointer_to_handle((uintptr_t)local.player);
				auto vehicle = pointer_to_handle((uintptr_t)local.player->vehicle());
				float velocity = native::entity::get_entity_speed(vehicle);
				float heading = native::entity::get_entity_heading(vehicle);
				float new_heading = fmod(heading + 180.0f, 360.0f);

				native::entity::set_entity_heading(vehicle, new_heading);
				native::vehicle::set_vehicle_forward_speed(vehicle, 0);
				native::vehicle::set_vehicle_forward_speed(vehicle, velocity);
			}
		}
	}
}