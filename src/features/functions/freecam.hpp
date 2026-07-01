#pragma once
#include "func_utils.hpp"

namespace functions {
	namespace freecam {
		native::type::cam cam = 0;
		PVector3 rot{}, coord{}, p_coord{};
		bool script_cam_active = false;

		void create();
		void destroy();
		bool start();
		void stop();

		void disable_safely() {
			destroy();
			cheat_base->funcs.freecam.state = false;
			cheat_base->funcs.freecam.teleport = false;
			g_config->player.movement.freecam.enabled = false;
		}

		bool has_cam() {
			return cam != 0;
		}

		PVector3 get_start_coord() {
			if (Game.is_lp_in_vehicle && IsValidPtr(local.player->vehicle())) {
				Vector3 pos = local.player->vehicle()->fPosition;
				return PVector3{ pos.x, pos.y, pos.z + 1.2f };
			}

			Vector3 pos = local.player->fPosition;
			return PVector3{ pos.x, pos.y, pos.z + 0.8f };
		}

		PVector3 get_view_rot() {
			PVector3 view_rot = rot;

			__try {
				const auto local_handle = pointer_to_handle_safe((uintptr_t)local.player);
				if (local_handle != 0) {
					const float heading = native::cam::get_gameplay_cam_relative_heading() + native::entity::get_entity_heading(local_handle);
					const float pitch = native::cam::get_gameplay_cam_relative_pitch();
					view_rot = PVector3{ pitch, 0.f, heading };
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
			}

			if (!is_valid_world_pos(view_rot)) {
				view_rot = PVector3{ 0.f, 0.f, 0.f };
			}
			return view_rot;
		}

		void tick() {
			__try {
				if (!cheat_base->funcs.freecam.state) return;
				if (!has_cam() || !script_cam_active) {
					disable_safely();
					return;
				}

				rot = get_view_rot();
				if (!is_valid_world_pos(coord)) {
					disable_safely();
					return;
				}

				float speed = g_config->player.movement.freecam.speed;
				if (speed <= 0.f) speed = 1.f;
				if (speed > 10.f) speed = 10.f;
				if (g_config->player.movement.freecam.boost && g_config->player.movement.freecam.boost_key > 0 &&
					is_key_down_safe(g_config->player.movement.freecam.boost_key)) {
					speed *= 2.f;
				}

				PVector3 dir = utils::rot_to_dir(rot);
				PVector3 right{ dir.y, -dir.x, 0.f };

				if (is_key_down_safe(0x57)) {
					coord.x += dir.x * speed;
					coord.y += dir.y * speed;
					coord.z += dir.z * speed;
				}
				if (is_key_down_safe(0x53)) {
					coord.x -= dir.x * speed;
					coord.y -= dir.y * speed;
					coord.z -= dir.z * speed;
				}
				if (is_key_down_safe(0x41)) {
					coord.x -= right.x * speed;
					coord.y -= right.y * speed;
				}
				if (is_key_down_safe(0x44)) {
					coord.x += right.x * speed;
					coord.y += right.y * speed;
				}
				if (is_key_down_safe(VK_SPACE)) {
					coord.z += speed;
				}
				if (is_key_down_safe(VK_LCONTROL)) {
					coord.z -= speed;
				}

				if (!is_valid_world_pos(coord)) {
					disable_safely();
					return;
				}

				native::cam::set_cam_coord(cam, coord.x, coord.y, coord.z);
				native::cam::set_cam_rot(cam, rot.x, rot.y, rot.z, 2);

				if (cheat_base->funcs.freecam.teleport) {
					cheat_base->funcs.freecam.teleport = false;
				}

				if (g_config->player.movement.freecam.teleport_on_key &&
					g_config->player.movement.freecam.teleport_key > 0 &&
					is_key_down_safe(g_config->player.movement.freecam.teleport_key)) {
					cheat_base->funcs.freecam.teleport = false;
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				disable_safely();
			}
		}

		void create() {
			if (!IsValidPtr(local.player)) return;

			__try {
				rot = get_view_rot();
				coord = get_start_coord();
				if (!is_valid_world_pos(coord)) {
					cam = 0;
					return;
				}

				if (!has_cam()) {
					cam = native::cam::create_cam_with_params("DEFAULT_SCRIPTED_CAMERA",
						coord.x, coord.y, coord.z,
						rot.x, rot.y, rot.z,
						50.f, true, 2);

					if (cam == 0) {
						cam = native::cam::create_cam("DEFAULT_SCRIPTED_CAMERA", true);
					}

					if (cam == 0) {
						const auto cam_hash = rage_joaat("DEFAULT_SCRIPTED_CAMERA");
						cam = native::cam::create_camera_with_params(cam_hash,
							coord.x, coord.y, coord.z,
							rot.x, rot.y, rot.z,
							50.f, true, 2);
					}

					if (cam == 0) {
						const auto cam_hash = rage_joaat("DEFAULT_SCRIPTED_CAMERA");
						cam = native::cam::create_camera(cam_hash, true);
					}

					if (cam != 0) {
						native::cam::set_cam_rot(cam, rot.x, rot.y, rot.z, 2);
						native::cam::set_cam_coord(cam, coord.x, coord.y, coord.z);
					}
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				cam = 0;
				script_cam_active = false;
			}
		}

		void destroy() {
			__try {
				if (script_cam_active) {
					native::cam::render_script_cams(false, true, 300, true, true);
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
			}

			__try {
				if (cam != NULL) {
					native::cam::set_cam_active(cam, false);
					native::cam::destroy_cam(cam, false);
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
			}

			cam = NULL;
			script_cam_active = false;
		}

		bool activate() {
			if (!has_cam()) return false;

			__try {
				native::cam::set_cam_rot(cam, rot.x, rot.y, rot.z, 2);
				native::cam::set_cam_coord(cam, coord.x, coord.y, coord.z);
				native::cam::set_cam_active(cam, true);
				native::cam::render_script_cams(true, false, 0, true, true);
				script_cam_active = true;
				return true;
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				destroy();
				return false;
			}
		}

		bool start() {
			if (!IsValidPtr(local.player)) return false;

			if (!has_cam()) {
				create();
			}

			if (!has_cam()) {
				add_log("freecam: camera create failed");
				return false;
			}

			if (!script_cam_active && !activate()) {
				add_log("freecam: camera activate failed");
				return false;
			}

			cheat_base->funcs.freecam.state = true;
			return true;
		}

		void stop() {
			destroy();
			cheat_base->funcs.freecam.state = false;
			cheat_base->funcs.freecam.teleport = false;
		}

		void _switch() {
			int key = g_config->player.movement.freecam.keybind;
			static bool old_state = false;

			if (!g_config->player.movement.freecam.enabled) {
				if (cheat_base->funcs.freecam.state) stop();
				old_state = false;
				return;
			}

			if (key <= 0) {
				if (!cheat_base->funcs.freecam.state && !start()) {
					g_config->player.movement.freecam.enabled = false;
					stop();
				}
				old_state = false;
				return;
			}

			const bool pressed = is_key_down_safe(key);
			if (pressed && !old_state) {
				if (!cheat_base->funcs.freecam.state) {
					if (!start()) {
						g_config->player.movement.freecam.enabled = false;
						stop();
					}
				}
				else {
					stop();
				}
			}
			old_state = pressed;
		}
	}
}
