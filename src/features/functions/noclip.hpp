#pragma once
#include "func_utils.hpp"

namespace functions {
	void noclip() {
		if (!IsValidPtr(local.player)) return;

		static unsigned int aa_yaw = 0;
		static unsigned int aa_roll = 55;
		static unsigned int aa_pitch = 119;
		static bool t_pose_applied = false;

		auto l_player = pointer_to_handle_safe((uintptr_t)local.player);
		if (l_player == 0) return;

		auto pos = local.player->fPosition;
		native::type::any entity;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float speed;

		if (g_config->player.movement.noclip.speed <= 0.f) {
			g_config->player.movement.noclip.speed = 1.f;
		}

		if (g_config->player.movement.noclip.boost_key > 0 && is_key_down_safe(g_config->player.movement.noclip.boost_key) && g_config->player.movement.noclip.boost) {
			speed = g_config->player.movement.noclip.speed * 2;
			cheat_base->funcs.noclip.boost = true;
		}
		else {
			speed = g_config->player.movement.noclip.speed;
			cheat_base->funcs.noclip.boost = false;
		}

		if (!Game.is_lp_in_vehicle) {
			entity = pointer_to_handle_safe((uintptr_t)local.player);
			if (entity == 0) return;
			x = pos.x;
			y = pos.y;
			z = pos.z;

			if (g_config->player.movement.noclip.t_pose) {
				native::entity::set_entity_collision(entity, false, false);
				native::entity::freeze_entity_position(entity, true);
				t_pose_applied = true;
			}
			else if (t_pose_applied) {
				native::entity::set_entity_collision(entity, true, true);
				native::entity::freeze_entity_position(entity, false);
				t_pose_applied = false;
			}
		}
		else {
			if (t_pose_applied) {
				native::entity::set_entity_collision(l_player, true, true);
				native::entity::freeze_entity_position(l_player, false);
				t_pose_applied = false;
			}

			if (Game.is_lp_in_vehicle) {
				CVehicle* veh = local.player->vehicle();
				if (!IsValidPtr(veh)) return;
				pos = veh->fPosition;
				entity = pointer_to_handle_safe((uintptr_t)veh);
				if (entity == 0) return;
			}


			x = pos.x;
			y = pos.y;
			z = pos.z;

			native::entity::set_entity_collision(entity, false, false);
			native::entity::freeze_entity_position(entity, true);
		}
		// TODO: angles from CPlayerAngles
		auto cam_direction = utils::get_cam_directions();

		float dx = cam_direction.x;
		float dy = cam_direction.y;
		float dz = cam_direction.z;
		float heading = native::cam::get_gameplay_cam_relative_heading() + native::entity::get_entity_heading(entity);
		float pitch = native::cam::get_gameplay_cam_relative_pitch();

		if (is_key_down_safe(0x57)) {
			x = x + speed * dx;
			y = y + speed * dy;
			z = z + speed * dz;

		}

		if (is_key_down_safe(0x53)) {
			x = x - speed * dx;
			y = y - speed * dy;
			z = z - speed * dz;
		}

		if (is_key_down_safe(0x41)) {
			float temp = dx;
			dx = -dy;
			dy = temp;
			x = x + speed * dx;
			y = y + speed * dy;
		}

		if (is_key_down_safe(0x44)) {
			float temp = dx;
			dx = dy;
			dy = -temp;
			x = x + speed * dx;
			y = y + speed * dy;
		}

		if (is_key_down_safe(0x20)) {
			z = z + speed;
		}

		if (is_key_down_safe(0xA2)) {
			z = z - speed;
		}



		if (entity == 0 || !is_safe_float(x) || !is_safe_float(y) || !is_safe_float(z)) return;
		native::entity::set_entity_coords_no_offset(entity, x, y, z, true, true, true);

		// ROTATION VECTOR
		// x = YAW
		// y = PITCH
		// z = ROLL

		Vector3 rotation = Vector3(heading, 0, 0);

		//if (g_config->player.movement.noclip.invert_yaw) {
		//	rotation.x = (360.0 - rotation.x, 360.0); // wtf
		//}

		if (g_config->player.movement.noclip.go_stupid) {
			//rotation.y = pitch;
			if (aa_pitch == 360) {
				aa_pitch = 0;
			}

			if (aa_roll == 360) {
				aa_roll = 0;
			}

			if (aa_yaw == 360) {
				aa_yaw = 0;
			}

			rotation = Vector3(aa_yaw, aa_pitch, aa_roll);

			aa_pitch += g_config->player.movement.noclip.stupid_speed;
			aa_roll += g_config->player.movement.noclip.stupid_speed;
			aa_yaw += g_config->player.movement.noclip.stupid_speed;
		}
		else {
			if (!is_key_down_safe(VK_RBUTTON) && !Game.is_lp_in_vehicle && !g_config->player.movement.noclip.t_pose) {
				tick::thread_invoker::queue([] {
					if (IsValidPtr(local.player)) {
						CWeaponManager* wep = local.player->weapon();
						if (IsValidPtr(wep)) {
							if (IsValidPtr(wep->_WeaponInfo)) {
								auto aWep = wep->_WeaponInfo;
								auto name = aWep->GetWeaponHash();
								if (IsValidPtr(aWep)) {
									const auto ped_handle = pointer_to_handle_safe((uintptr_t)local.player);
									if (ped_handle != 0) native::brain::clear_ped_tasks_immediately(ped_handle);
								}
							}
						}
					}
					});
			}

			if (g_config->player.movement.noclip.t_pose && !Game.is_lp_in_vehicle && !is_key_down_safe(VK_RBUTTON)) {
				tick::thread_invoker::queue([] {
					if (IsValidPtr(local.player)) {
						const auto ped_handle = pointer_to_handle_safe((uintptr_t)local.player);
						if (ped_handle != 0) native::brain::clear_ped_tasks_immediately(ped_handle);
					}
					});
			}
		}

		if (false /*g_config->player.movement.noclip.enable_roll*/) {
			rotation.z = 90;
		}

		if (Game.is_lp_in_vehicle) {
			if (!g_config->player.movement.noclip.go_stupid) {
				native::entity::set_entity_rotation(entity, rotation.y, rotation.z, rotation.x, 0, true);
			}
			else {
				native::entity::set_entity_rotation(entity, pitch, 0, rotation.z, 0, true); // helicopter mode
			}
		}
		else {
			native::entity::set_entity_rotation(entity, rotation.y, rotation.z, rotation.x, 0, true);
		}

	}

	void noclip_disable() {
		if (!IsValidPtr(local.player)) {
			cheat_base->funcs.noclip.state = false;
			cheat_base->funcs.noclip.boost = false;
			return;
		}

		const auto local_handle = pointer_to_handle_safe((uintptr_t)local.player);
		native::type::entity entity = Game.is_lp_in_vehicle && local_handle != 0
			? native::ped::get_vehicle_ped_is_in(local_handle, false)
			: local_handle;

		if (entity != 0) {
			native::entity::set_entity_collision(entity, true, true);
			native::entity::freeze_entity_position(entity, false);
			if (!Game.is_lp_in_vehicle) {
				native::brain::clear_ped_tasks_immediately(entity);
			}
			if (g_config->player.movement.noclip.go_stupid) {
				native::entity::set_entity_rotation(entity, 0, 0, 0, 0, true);
			}
			if (g_config->player.movement.noclip.warp_on_ground) {
				utils::teleport_player_to_ground(entity);
			}
		}

		cheat_base->funcs.noclip.state = false;
		cheat_base->funcs.noclip.boost = false;
	}

	void noclip_switch() {
		// shit :skull:skull::skull:
		int key = g_config->player.movement.noclip.keybind;
		static bool old_state = false;
		if (key <= 0) {
			cheat_base->funcs.noclip.state = g_config->player.movement.noclip.enabled;
			old_state = false;
			return;
		}

		const bool pressed = is_key_down_safe(key);
		if (pressed && !old_state) {
			cheat_base->funcs.noclip.state = !cheat_base->funcs.noclip.state;
			if (!cheat_base->funcs.noclip.state) noclip_disable();
		}
		old_state = pressed;
	}
}
