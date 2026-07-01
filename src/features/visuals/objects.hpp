#pragma once
#include "esp_utils.hpp"

namespace esp {
	void objects_frame() {
		static float floating_anim;
		const auto& object_list = g_config->esp.objects.tech.objects_list;

		for (CObject* entity : game::object_list) {
			if (!IsValidPtr(entity)) continue;

			float dist = get_distance(local.player->fPosition, entity->fPosition);
			DWORD hash = entity->ModelInfo()->GetHash();

			g_config->esp.objects.text_preset.adaptive_scale = 13.f - dist / g_config->esp.objects.text_preset.adsc_strength;
			float text_step = g_config->esp.objects.text_preset.adaptive_scale_state ?
				g_config->esp.vehicles.text_preset.adaptive_scale * g_config->esp.objects.text_preset.text_scale
				: 13.5f * g_config->esp.objects.text_preset.text_scale;

			bool is_any_obj_esp = g_config->esp.objects.distance || g_config->esp.objects.names || g_config->esp.objects.snaplines || g_config->esp.objects.render_all_stream;

			auto it = object_list.find(hash);
			if (g_config->esp.objects.render_all_stream || it != object_list.end()) {
				std::string name = (it != object_list.end()) ? it->second : std::format("0x{:X}", hash);

				ImVec2 object2d;
				if (!WorldToScreen(entity->fPosition, &object2d) || !isW2SValid(object2d)) continue;

				if (g_config->esp.objects.snaplines) {
					ImVec2 s_local_bottom;
					Vector3 startpos = GetBonePosition(local.player, SPINE_ROOT);
					if (!WorldToScreen(startpos, &s_local_bottom)) {
						float ScreenWidth = ImGui::GetIO().DisplaySize.x;
						float ScreenHeight = ImGui::GetIO().DisplaySize.y;
						s_local_bottom = { ScreenWidth / 2.f, ScreenHeight };
					}
					renderer.RenderLine(s_local_bottom, object2d, RGBA(g_config->esp.objects.snaplines_col), 1.0f);
				}

				if (g_config->esp.objects.names || g_config->esp.objects.render_all_stream) {
					renderer.RenderText(name, object2d, g_config->esp.objects.names_col, g_config->esp.objects.text_preset);
				}

				if (g_config->esp.objects.distance) {
					char buf[10];
					sprintf(buf, "%.1f m", dist);
					std::string l_distance(buf);
					renderer.RenderText(l_distance.c_str(), ImVec2(object2d.x, object2d.y + text_step), g_config->esp.objects.distance_col, g_config->esp.objects.text_preset);
				}

				if (it == object_list.end()) {
					ImVec2 mPos = ImGui::GetMousePos();
					float screenDist = screen_distance(object2d.x, object2d.y, mPos.x, mPos.y);

					if (ImGui::IsMouseClicked(0, false) && screenDist < 20) {

						sprintf(cheat_base->ui_bufs.obj_hash, "0x%X\n", hash);
					}
					else if (screenDist < 10) {
						ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
						floating_anim = 2.5f * (1.0f + sin(ImGui::GetTime() * 2.8f));
						ImVec2 arrow_pos(object2d.x, object2d.y - floating_anim);
						ImVec2 arrow_halfsz(5, 10);
						static ImGuiWindow* window = ImGui::GetCurrentWindow();
						ImColor arrow_col = ImColor(g_config->esp.objects.names_col[0], g_config->esp.objects.names_col[1], g_config->esp.objects.names_col[2], 1.f);
						window->DrawList->AddTriangle(
							ImVec2(arrow_pos.x - arrow_halfsz.x, arrow_pos.y - arrow_halfsz.y),
							ImVec2(arrow_pos.x + arrow_halfsz.x, arrow_pos.y - arrow_halfsz.y),
							arrow_pos, arrow_col, 1.4f);
					}
				}
			}
		}
	}
}