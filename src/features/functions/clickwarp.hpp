#pragma once
#include "func_utils.hpp"

namespace functions {
	namespace clickwarp {
		void get_coords_for_mark() {
			// Get camera position and direction
			Vector3 camPos = Game.getCam()->m_cam_pos;
			PVector3 camRot = native::cam::get_gameplay_cam_rot(2);

			PVector3 direction = utils::rot_to_dir(camRot);
			float rayLength = 1000.0f;

			PVector3 endCoords = {
				camPos.x + direction.x * rayLength,
				camPos.y + direction.y * rayLength,
				camPos.z + direction.z * rayLength
			};

			int playerPed = pointer_to_handle((uintptr_t)local.player);

			// First ray: cast a ray forward from the camera
			int ray = native::worldprobe::_start_shape_test_ray(camPos.x, camPos.y, camPos.z, endCoords.x, endCoords.y, endCoords.z, -1, playerPed, 17);
			bool hit;
			PVector3 hitCoords;
			PVector3 surfaceNormal;
			native::type::entity hitEntity;
			native::worldprobe::get_shape_test_result(ray, &hit, &hitCoords, &surfaceNormal, &hitEntity);

			// If the ray hits something
			if (hit) {
				// If the surface is mostly flat (horizontal), we assume it's a roof or ground
				if (surfaceNormal.z > 0.5f) {
					Game.cw_coords = hitCoords;
				}
				else {
					// Cast a second ray vertically downward from a higher point above the collision
					PVector3 highPoint = { hitCoords.x, hitCoords.y, hitCoords.z + 300.0f };
					int rayDown = native::worldprobe::_start_shape_test_ray(highPoint.x, highPoint.y, highPoint.z, hitCoords.x, hitCoords.y, hitCoords.z - 0.3f, -1, playerPed, 17);
					bool hitGround;
					PVector3 groundCoords;
					native::worldprobe::get_shape_test_result(rayDown, &hitGround, &groundCoords, &groundCoords, &hitEntity);

					// If we find a valid ground, teleport there
					if (hitGround) {
						Game.cw_coords = groundCoords;
					}
					else {
						Game.cw_coords = hitCoords;
					}
				}
			}
			else {
				// No hit, teleport to the end of the ray (maximum distance)
				Game.cw_coords = endCoords;
			}
		}

		void clickwarp() {
			const s_esptext_render_params cw_text_render{ 0,1,1,1 };

			CVehicle* target_veh = nullptr; const float tveh_catch_dim = 2.5f;
			if (GetAsyncKeyState(g_config->world.clickwarp.key) & 0x8000) {
				PVector3 worldPos = Game.cw_coords;
				Vector3 cursor_pos_interpr{ worldPos.x, worldPos.y, worldPos.z };
				auto local_player = pointer_to_handle((uintptr_t)local.player);

				if (get_distance(cursor_pos_interpr, local.player->fPosition) > 500.f) return;

				ImVec2 s_clickwarp;
				if (!WorldToScreen(Vector3(worldPos.x, worldPos.y, worldPos.z - 1.f), &s_clickwarp)) return;

				ImVec4 color = ImVec4(g_config->world.clickwarp.marker_col[0] * 255,
					g_config->world.clickwarp.marker_col[1] * 255,
					g_config->world.clickwarp.marker_col[2] * 255,
					g_config->world.clickwarp.marker_col[3] * 255);

				for (int i = 0; i < Game.ReplayInterface->vehicle_interface->max_vehicles; i++) {
					CVehicle* veh = reinterpret_cast<CVehicle*>(Game.ReplayInterface->vehicle_interface->get_vehicle(i));
					if (IsValidPtr(veh)) {
						float cursor_n_veh = get_distance(veh->fPosition, cursor_pos_interpr);

						if (cursor_n_veh <= tveh_catch_dim) {
							target_veh = veh;
						}
					}
				}

				if (target_veh == nullptr) {
					if (g_config->world.clickwarp.marker_type == 0) {
						native::graphics::draw_marker(0, worldPos.x, worldPos.y, worldPos.z + 1.f, 0, 0, 0, 0, 0, 0, 1.0f, 1.0f, 1.0f,
							color.x, color.y, color.z, color.w, false, true, 2, false, nullptr, nullptr, false);
					}
					else {
						Vector3 center{ worldPos.x, worldPos.y, worldPos.z };
						// 8 vertices of the box
						std::vector<Vector3> vertices(8);

						// Calculate half dimensions
						float hw = 0.2;
						float hh = 0.2;
						float hd = 0.4;

						// Define the vertices relative to the center
						vertices[0] = Vector3(center.x - hw, center.y - hh, center.z - hd); // Bottom-front-left
						vertices[1] = Vector3(center.x + hw, center.y - hh, center.z - hd); // Bottom-front-right
						vertices[2] = Vector3(center.x - hw, center.y + hh, center.z - hd); // Top-front-left
						vertices[3] = Vector3(center.x + hw, center.y + hh, center.z - hd); // Top-front-right
						vertices[4] = Vector3(center.x - hw, center.y - hh, center.z + hd); // Bottom-back-left
						vertices[5] = Vector3(center.x + hw, center.y - hh, center.z + hd); // Bottom-back-right
						vertices[6] = Vector3(center.x - hw, center.y + hh, center.z + hd); // Top-back-left
						vertices[7] = Vector3(center.x + hw, center.y + hh, center.z + hd); // Top-back-right

						// Convert to screen coordinates
						std::vector<ImVec2> screenCoords(8);
						for (int i = 0; i < 8; ++i) {
							WorldToScreen(vertices[i], &screenCoords[i]);
						}

						// Define the edges of the box
						int edges[12][2] = {
							{0, 1}, {1, 3}, {3, 2}, {2, 0}, // Bottom rectangle
							{4, 5}, {5, 7}, {7, 6}, {6, 4}, // Top rectangle
							{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical lines
						};

						// Draw the edges of the box
						for (int i = 0; i < 12; ++i) {
							renderer.RenderLine(screenCoords[edges[i][0]], screenCoords[edges[i][1]], g_config->world.clickwarp.marker_col, 1.f);
						}
					}
					if (g_config->world.clickwarp.draw_distance) {
						float dist = get_distance(local.player->fPosition, Vector3(worldPos.x, worldPos.y, worldPos.z));
						char buf[10]; sprintf(buf, "%.1f m", dist);
						renderer.RenderText(buf, s_clickwarp, g_config->world.clickwarp.distance_col, cw_text_render);
					}
				}
				else {
					esp::utils::draw_entity_3dbox(pointer_to_handle((uintptr_t)target_veh), target_veh->ModelInfo()->GetHash(),
						target_veh->fPosition, g_config->world.clickwarp.marker_col, esp::utils::vehicle);

					ImVec2 s_desk_pos;
					if (!WorldToScreen(target_veh->fPosition, &s_desk_pos)) return;

					if (g_config->esp.vehicles.enabled && (g_config->esp.vehicles.names || g_config->esp.vehicles.distance || g_config->esp.vehicles.lock_state || g_config->esp.vehicles.engine_state))
						s_desk_pos = ImVec2(s_desk_pos.x, s_desk_pos.y - 30);

					renderer.RenderText(std::format("Warp Into {}", target_veh->ModelInfo()->GetName()), s_desk_pos, RGBACOL_WHITE, cw_text_render);

					if (g_config->world.clickwarp.draw_distance) {
						char buf[10]; sprintf(buf, "%.1f m", get_distance(local.player->fPosition, target_veh->fPosition));
						renderer.RenderText(buf, ImVec2(s_desk_pos.x, s_desk_pos.y + 14), g_config->world.clickwarp.distance_col, cw_text_render);
					}
				}
				cheat_base->funcs.clickwarp_state = true;

				if (GetAsyncKeyState(g_config->world.clickwarp.cancel_key) & 0x8000) {
					renderer.RenderText("Cancel TP...", ImVec2(s_clickwarp.x, s_clickwarp.y - 14), RGBA(196, 255, 196, 255), cw_text_render);
					cheat_base->funcs.clickwarp_state = false;
				}

			}
			else {
				if (cheat_base->funcs.clickwarp_state) {
					auto worldPos = Game.cw_coords;
					Vector3 cursor_pos_interpr{ worldPos.x, worldPos.y, worldPos.z };
					auto local_player = pointer_to_handle((uintptr_t)local.player);

					if (get_distance(cursor_pos_interpr, local.player->fPosition) > 500.f) return;

					ImVec2 s_clickwarp;
					if (!WorldToScreen(Vector3(worldPos.x, worldPos.y, worldPos.z - 1.f), &s_clickwarp)) return;

					for (int i = 0; i < Game.ReplayInterface->vehicle_interface->max_vehicles; i++) {
						CVehicle* veh = reinterpret_cast<CVehicle*>(Game.ReplayInterface->vehicle_interface->get_vehicle(i));
						if (IsValidPtr(veh)) {
							float cursor_n_veh = get_distance(veh->fPosition, cursor_pos_interpr);

							if (cursor_n_veh <= tveh_catch_dim) {
								target_veh = veh;
							}
						}
					}

					if (target_veh != nullptr) {
						auto veh_handle = pointer_to_handle((uintptr_t)target_veh);
						//native::entity::set_entity_coords_no_offset(local_player,
						//	target_veh->fPosition.x, target_veh->fPosition.y, target_veh->fPosition.z + 1.f, true, true, true);
						//tick::thread_invoker::queue([] { native::brain::clear_ped_tasks_immediately(pointer_to_handle((uintptr_t)local.player)); });
						native::ped::set_ped_into_vehicle(local_player, veh_handle, -1);
						cheat_base->funcs.clickwarp_state = false;
					}
					else {
						native::entity::set_entity_coords_no_offset(local_player, worldPos.x, worldPos.y, worldPos.z + 0.5f, true, true, true);
						//if (native::ped::is_ped_falling(local_player) || native::ped::is_ped_in_any_vehicle(local_player, false) || native::ped::is_ped_ragdoll(local_player)) {
						tick::thread_invoker::queue([] { native::brain::clear_ped_tasks_immediately(pointer_to_handle((uintptr_t)local.player)); });
						//}
						cheat_base->funcs.clickwarp_state = false;
					}

					cheat_base->funcs.clickwarp_state = false;
				}
			}
		}
	}
}