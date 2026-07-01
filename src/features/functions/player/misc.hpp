#pragma once
#include "../func_utils.hpp"

namespace functions {
	namespace player {
		void fallbreak() {
			int player_ped = pointer_to_handle((uintptr_t)local.player);

			if (native::ped::is_ped_falling(player_ped))
			{
				PVector3 player_coords = native::entity::get_entity_coords(player_ped, true);
				PVector3 ray_end_coords = { player_coords.x, player_coords.y, player_coords.z - 1000.0f };
				int ray = native::worldprobe::_start_shape_test_ray(
					player_coords.x, player_coords.y, player_coords.z,
					ray_end_coords.x, ray_end_coords.y, ray_end_coords.z,
					1, player_ped, 7
				);

				bool hit;
				PVector3 hit_coords;
				PVector3 surface_normal;
				int entity_hit;
				native::worldprobe::get_shape_test_result(ray, &hit, &hit_coords, &surface_normal, &entity_hit);

				if (hit)
				{
					float distance_to_ground = player_coords.z - hit_coords.z;
					float threshold = (g_config->player.movement.player_mgmt.disable_ragdoll) ? 2.1f : 1.1f;
					if (distance_to_ground < threshold + g_config->player.movement.player_mgmt.fb_treshold)
					{
						tick::thread_invoker::queue([] { native::brain::clear_ped_tasks_immediately(pointer_to_handle((uintptr_t)local.player)); });
					}
				}
			}
		}

		void skip_ladder() {
			auto player_ped = pointer_to_handle((uintptr_t)local.player);

			if (native::ped::get_ped_config_flag(player_ped, 388, 1))
			{
				PVector3 ladder_top_coords = utils::get_ladder_top_coords(player_ped);
				native::entity::set_entity_coords_no_offset(player_ped, ladder_top_coords.x, ladder_top_coords.y, ladder_top_coords.z, true, true, true);

				if (!g_config->player.movement.player_mgmt.ladder_to_moon)
					tick::thread_invoker::queue([] { native::brain::clear_ped_tasks_immediately(pointer_to_handle((uintptr_t)local.player)); });
			}
		}

	}
}
