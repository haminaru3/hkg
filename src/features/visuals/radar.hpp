#pragma once
#include "esp_utils.hpp"

namespace esp {
	void radar_wnd() {
		if (!g_config->esp.radar.enabled) return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, g_config->esp.radar.rounding + 0.5f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });

		//ImVec4 col_bg = ImColor((19 / 255.f) * g_config->esp.radar.rad_accent[0],
		//	(19 / 255.f) * g_config->esp.radar.rad_accent[1],
		//	(19 / 255.f) * g_config->esp.radar.rad_accent[2],
		//	g_config->esp.radar.opacity / 100.f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 100.f,100.f }); ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(400, 400));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, (0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Border, (0, 0, 0, 0));

		auto rad_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
		if (!g_config->controls.menu_state) { rad_flags |= ImGuiWindowFlags_NoResize; };

		ImGui::Begin("##radar", nullptr, rad_flags);

		auto draw = ImGui::GetWindowDrawList();
		auto pos = ImGui::GetWindowPos();
		auto size = ImGui::GetWindowSize();

		// fuck it
		//if (cheat_base->on_config_load) ImGui::SetWindowSize(ImVec2(g_config->widgets.radar_size[0], g_config->widgets.radar_size[1])),
		//	ImGui::SetWindowPos(ImVec2(g_config->widgets.radar_pos[0], g_config->widgets.radar_pos[1]));
		//else if (!cheat_base->on_config_load) g_config->widgets.radar_pos = { pos.x, pos.y }, g_config->widgets.radar_size = { size.x, size.y };

		ImVec2 regn_min = ImGui::GetWindowContentRegionMin(); ImVec2 regn_max = ImGui::GetWindowContentRegionMax();

		ImVec2 vMin = ImVec2(regn_min.x + 4, regn_min.y + 4);
		ImVec2 vMax = ImVec2(regn_max.x + 4, regn_max.y + 4);
		ImVec2 vSizes = ImGui::GetWindowSize();
		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;
		ImVec2 Center = ImVec2(vMin.x + (vSizes.x / 2), vMin.y + (vSizes.y / 2));

		// (19, 19, 19, (g_config->esp.radar.opacity / 100.f)*255.f)
		draw->AddRectFilled(pos, { pos.x + size.x, pos.y + size.y }, ImColor(0.07f, 0.07f, 0.07f, g_config->esp.radar.opacity / 100.f), g_config->esp.radar.rounding + 0.5f);

		for (pair<CObject*, DataPed>& entity : game::ped_list) {
			CObject* ped = entity.first;
			DataPed data = entity.second;
			if (local.player == ped) continue;
			if (get_distance(local.player->fPosition, ped->fPosition) > 1000.f) continue;
			DWORD hash = ped->ModelInfo()->GetHash();
			if (!game::isValidPlayer(hash, ped)) continue;

			RGBA current_color = g_config->esp.radar.filters.players_col;
			int cur_mark = g_config->esp.radar.marks.players;
			float mark_scale = g_config->esp.radar.marks.players_scale / 100;

			if (data.isAdmin) {
				if (!g_config->esp.radar.filters.admins) continue;
				current_color = g_config->esp.radar.filters.admins_col;
				cur_mark = g_config->esp.radar.marks.admins;
				mark_scale = g_config->esp.radar.marks.admins_scale / 100;
			}
			else if (!g_config->esp.radar.filters.players) continue;

			ImVec2 RadarPos = utils::world_to_radar(ped->fPosition, Center.x, Center.y, vSizes, g_config->esp.radar.zoom);

			switch (cur_mark)
			{
			case 0:
				draw->AddCircleFilled(RadarPos, 7.f * mark_scale, ImColor(0, 0, 0, 255), 16);
				renderer.RenderCircleFilled(RadarPos, 5.f * mark_scale, current_color, 16.f);
				break;
			case 1:
				draw->AddRectFilled(ImVec2(RadarPos.x - 4 * mark_scale, RadarPos.y - 4 * mark_scale), ImVec2(RadarPos.x + 4 * mark_scale, RadarPos.y + 4 * mark_scale), ImColor(0, 0, 0, 255));
				renderer.RenderRectFilled(ImVec2(RadarPos.x - 3.5f * mark_scale, RadarPos.y - 3.5f * mark_scale), ImVec2(RadarPos.x + 3.5f * mark_scale, RadarPos.y + 3.5f * mark_scale), current_color);
				break;
			}
		}

		draw->AddRect(pos, { pos.x + size.x, pos.y + size.y }, ImColor(255, 255, 255, 20), g_config->esp.radar.rounding + 0.5f);

		draw->AddLine(ImVec2(pos.x + size.x / 2, pos.y + 2), ImVec2(pos.x + size.x / 2, pos.y + size.y - 2), ImColor(255, 255, 255, 30));// vert // def (255, 255, 255, 30)
		draw->AddLine(ImVec2(pos.x + 2, pos.y + size.y / 2), ImVec2(pos.x + size.x - 2, pos.y + size.y / 2 - 1), ImColor(255, 255, 255, 30)); // horiz // def (255, 255, 255, 30)

		ImGui::End();
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(2);
	}
}