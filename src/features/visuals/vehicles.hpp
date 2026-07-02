#pragma once
#include "esp_utils.hpp"

namespace esp {
	void vehicles_frame() {
		for (auto& this_vehicle : game::vehicle_list) {
			CVehicle* veh = this_vehicle.first;
			DataVehicle& data = this_vehicle.second;
			if (!IsValidPtr(veh) || !IsValidPtr(local.player)) continue;
			auto player_h = pointer_to_handle((uintptr_t)local.player);

			float dist = get_distance(local.player->fPosition, veh->fPosition);
			if (dist > g_config->esp.vehicles.max_dist) continue;

			g_config->esp.vehicles.text_preset.adaptive_scale = 13.f - dist / g_config->esp.vehicles.text_preset.adsc_strength;

			ImVec2 veh2d;
			if (!WorldToScreen(veh->fPosition, &veh2d)) continue;

			if (g_config->esp.vehicles.ignore_local && Game.is_lp_in_vehicle) {
				if (native::ped::get_vehicle_ped_is_in(player_h, false) == pointer_to_handle((uintptr_t)veh)) continue;
			}

			int textRow = 0;
			float text_step = g_config->esp.vehicles.text_preset.adaptive_scale_state ?
				g_config->esp.vehicles.text_preset.adaptive_scale * g_config->esp.vehicles.text_preset.text_scale
				: 13.5f * g_config->esp.vehicles.text_preset.text_scale;

			if (g_config->esp.vehicles.snaplines) {
				ImVec2 s_local_bottom;
				if (!WorldToScreen(GetBonePosition(local.player, SPINE_ROOT), &s_local_bottom)) {
					float ScreenWidth = ImGui::GetIO().DisplaySize.x;
					float ScreenHeight = ImGui::GetIO().DisplaySize.y;
					s_local_bottom = { ScreenWidth / 2.f, ScreenHeight };
				}
				renderer.RenderLine(s_local_bottom, veh2d, g_config->esp.vehicles.snaplines_col, 1.0f);
			}

			if (g_config->esp.vehicles.box) {
				utils::draw_entity_3dbox(pointer_to_handle((uintptr_t)veh), veh->ModelInfo()->GetHash(), veh->fPosition, g_config->esp.vehicles.box_col, utils::vehicle);
			}

			if (g_config->esp.vehicles.names) {
				std::string name = g_config->esp.vehicles.manufacture ?
					std::format("{} {}", veh->ModelInfo()->GetManufacturer(), veh->ModelInfo()->GetName()) :
					veh->ModelInfo()->GetName();
				renderer.RenderText(name, veh2d, g_config->esp.vehicles.names_col, g_config->esp.vehicles.text_preset);
				textRow++;
			}

			if (g_config->esp.vehicles.distance) {
				char buf[10];
				sprintf(buf, "%.1f m", dist);
				renderer.RenderText(buf, ImVec2(veh2d.x, veh2d.y + (text_step * textRow)), g_config->esp.vehicles.distance_col, g_config->esp.vehicles.text_preset);
				textRow++;
			}

			if (g_config->esp.vehicles.lock_state || g_config->esp.vehicles.engine_state) {
				float letters_spacing = g_config->esp.vehicles.text_preset.adaptive_scale_state ?
					6.f * (g_config->esp.vehicles.text_preset.adaptive_scale * g_config->esp.vehicles.text_preset.text_scale) :
					6.f * (g_config->esp.vehicles.text_preset.text_scale);

				ImVec2 lock_pos = (!g_config->esp.vehicles.engine_state) ? ImVec2(veh2d.x, veh2d.y + (text_step * textRow))
					: ImVec2(veh2d.x - 5, veh2d.y + (text_step * textRow));

				ImVec2 engine_pos = (!g_config->esp.vehicles.lock_state) ? ImVec2(veh2d.x, veh2d.y + (text_step * textRow))
					: ImVec2(veh2d.x + 5, veh2d.y + (text_step * textRow));

				if (g_config->esp.vehicles.lock_state) {
					renderer.RenderText("L", lock_pos,
						data.is_locked ? g_config->esp.vehicles.locked_col : g_config->esp.vehicles.unlocked_col,
						g_config->esp.vehicles.text_preset);
				}

				if (g_config->esp.vehicles.engine_state) {
					renderer.RenderText("E", engine_pos,
						data.is_engine_started ? g_config->esp.vehicles.engine_on_col : g_config->esp.vehicles.engine_off_col,
						g_config->esp.vehicles.text_preset);
				}

				if (!g_config->esp.vehicles.lock_state || !g_config->esp.vehicles.engine_state) {
					textRow++;
				}
			}
		}
	}
}