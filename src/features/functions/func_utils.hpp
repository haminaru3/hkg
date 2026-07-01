#pragma once
#include "../../imports.h"

namespace game {
	extern vector<pair<CObject*, DataPed>> ped_list;
	extern vector<CObject*> object_list;
	extern vector<pair<CVehicle*, DataVehicle>> vehicle_list;

	extern bool isValidPlayer(DWORD hash, CObject* player);

	extern DWORD freemode_f;
	extern DWORD freemode_m;
}

namespace functions {
	namespace utils {
		///  <summary>
		///  Get the direction of the camera. This is based on the heading and pitch of the player.
		///  </summary>
		///  <returns>A 3 - vector that contains the x y and z directions</returns>
		Vector3 get_cam_directions() {
			auto l_player = pointer_to_handle((uintptr_t)local.player);

			float heading = native::cam::get_gameplay_cam_relative_heading() + native::entity::get_entity_heading(l_player);
			float pitch = native::cam::get_gameplay_cam_relative_pitch();

			float x = -sin(heading * PI / 180.0f);
			float y = cos(heading * PI / 180.0f);
			float z = sin(pitch * PI / 180.0f);

			double len = sqrt(x * x + y * y + z * z);
			if (len != 0) {
				x = x / len;
				y = y / len;
				z = z / len;
			}

			return Vector3(x, y, z);
		}

		///  <summary>
		///  Teleports the player to the ground. This is used to make it easier to play the game in the case of multiple vehicles
		///  </summary>
		///  <param name="vehicle">Index of the vehicle to teleport (if needed)</param>
		void teleport_player_to_ground(int vehicle)
		{
			auto l_player = pointer_to_handle((uintptr_t)local.player);
			PVector3 player_coords = native::entity::get_entity_coords(l_player, true);

			float ground_z;
			// Get the ground z coordinate of the player s position.
			if (native::gameplay::get_ground_z_for_3d_coord(player_coords.x, player_coords.y, player_coords.z + 600, &ground_z, false))
			{
				// Set the entity coordinates for the vehicle and player.
				if (vehicle != 0) {
					if (native::entity::does_entity_exist(vehicle))
						native::entity::set_entity_coords_no_offset(vehicle, player_coords.x, player_coords.y, ground_z, true, true, true);
				}

				native::entity::set_entity_coords_no_offset(l_player, player_coords.x, player_coords.y, ground_z, true, true, true);
			}
		}

		///  <summary>
		///  Get ladder top coordinates for a ped. This is based on the position of the ground.
		///  Needed for "Skip Ladder" function.
		///  </summary>
		///  <param name="player_ped">Ped to get coordinates for</param>
		///  <returns>ladder top coordinate</returns>
		PVector3 get_ladder_top_coords(int player_ped)
		{
			PVector3 player_coords = native::entity::get_entity_coords(player_ped, true);
			PVector3 forward_vector = native::entity::get_entity_forward_vector(player_ped);

			PVector3 ladder_top_coords;
			ladder_top_coords.x = player_coords.x + forward_vector.x * 2.0f;
			ladder_top_coords.y = player_coords.y + forward_vector.y * 2.0f;
			ladder_top_coords.z = player_coords.z + 2.0f;

			float ground_z;
			if (native::gameplay::get_ground_z_for_3d_coord(ladder_top_coords.x, ladder_top_coords.y, ladder_top_coords.z + 50.0f, &ground_z, false))
			{
				if (!g_config->player.movement.player_mgmt.ladder_to_moon)
					ladder_top_coords.z = ground_z;
				else
					ladder_top_coords.z = player_coords.z + forward_vector.y * 3.0f;
			}
			else
			{
				ladder_top_coords.z = player_coords.z + 2.0f;
			}

			return ladder_top_coords;
		}

		///  <summary>
		///  Setting weather override by ID (from list in combo)
		///  </summary>
		///  <param name="index">Weather ID</param>
		void set_weather_by_index(int index) {
			const char* weather_types[] = { "EXTRASUNNY", "CLEAR", "CLOUDS", "SMOG", "FOGGY", "OVERCAST", "RAIN", "THUNDER", "CLEARING", "NEUTRAL", "SNOW", "BLIZZARD", "SNOWLIGHT", "XMAS" };
			const int weather_types_count = sizeof(weather_types) / sizeof(weather_types[0]);
			if (index >= 0 && index < weather_types_count) {
				native::gameplay::set_weather_type_now_persist(weather_types[index]);
				native::gameplay::clear_weather_type_persist();
			}
		}

		///  <summary>
		///  idk tf is this (четоумное 4 cw)
		///  </summary>
		///  <param name="index">Vector of camrot.</param>
		PVector3 rot_to_dir(PVector3 rotation) {
			float radiansZ = rotation.z * (3.1415 / 180.0f);
			float radiansX = rotation.x * (3.1415 / 180.0f);
			float cosX = cos(radiansX);
			return PVector3{
				-sin(radiansZ) * cosX,
				cos(radiansZ) * cosX,
				sin(radiansX)
			};
		}
	}
}