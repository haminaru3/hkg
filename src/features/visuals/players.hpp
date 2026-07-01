#pragma once
#include "esp_utils.hpp"

#define is_any_animesp_render_enabled (g_config->esp.players.skeleton | g_config->esp.players.rect_box || g_config->esp.players.corners_box || g_config->esp.players.fill_box || g_config->esp.players.bars.health_bar || g_config->esp.players.bars.armor_bar)

namespace esp {
	void players_frame() {
		cheat_base->admin_counter = 0;
		cheat_base->closest_dist = 0;
		for (pair<CObject*, DataPed>& entity : game::ped_list) {
		  __try {
			CObject* ped = entity.first;
			DataPed data = entity.second;
			if (!IsValidPtr(ped) || !IsValidPtr(local.player)) continue;

			if (!IsValidPtr(ped->ModelInfo())) continue;
			DWORD hash = ped->ModelInfo()->GetHash();

			if (data.isAdmin) cheat_base->admin_counter++; // for widget
			if (!game::isValidPlayer(hash, ped)) continue; // is ped player check

			float w_distance = get_distance(local.player->fPosition, ped->fPosition);
			if (w_distance > g_config->esp.players.max_dist) continue;

			// initializationn of shitty anim system
			static fixed_containers::FixedUnorderedMap<int, s_espanim, 256> _playeranim;
			_playeranim.try_emplace(data.index, s_espanim());
			auto this_panim = _playeranim.find(data.index);
			s_espanim* this_anim = &this_panim->second;

			Vector3 w_top, w_bottom;

			if (data.isAdmin && (cheat_base->closest_dist == 0 || cheat_base->closest_dist > w_distance)) {
				cheat_base->closest_dist = w_distance;
			}

			// calculating box size by z axis of player coords in wrld (static and shitty method)
			w_top = Vector3(ped->fPosition.x, ped->fPosition.y, ped->fPosition.z + 0.85f);
			w_bottom = Vector3(ped->fPosition.x, ped->fPosition.y, ped->fPosition.z - 1.0f);

			ImVec2 s_top, s_bottom;	ImVec2 s_local_bottom;
			// checking is player on screen or not (validating w2s)
			if (!WorldToScreen(w_top, &s_top) || !WorldToScreen(w_bottom, &s_bottom)) continue;

			float s_box_height = abs(s_top.y - s_bottom.y);
			float s_box_width = s_box_height / 2.0f;

			float TextStartY = s_top.y + s_box_height;
			int textRow = 0;
			// for proper text spacing on adaptive scale + default scale
			float text_step = g_config->esp.players.text_preset.adaptive_scale_state ?
				g_config->esp.players.text_preset.adaptive_scale * g_config->esp.players.text_preset.text_scale
				: 13.5f * g_config->esp.players.text_preset.text_scale;

			float thickness_box = 0;
			ImVec2 rect_start = { s_top.x - (s_box_width / 2.0f), s_top.y };
			ImVec2 rect_end = { s_top.x - (s_box_width / 2.0f) + s_box_width, s_top.y + s_box_height };

			// render player as admin
			if (data.isAdmin) {
				if (g_config->esp.admins.snaplines) {
					Vector3 startpos = GetBonePosition(local.player, SPINE_ROOT);

					if (!WorldToScreen(startpos, &s_local_bottom)) {
						float ScreenWidth = ImGui::GetIO().DisplaySize.x;
						float ScreenHeight = ImGui::GetIO().DisplaySize.y;
						s_local_bottom = { ScreenWidth / 2.f, ScreenHeight };
					}
					renderer.RenderLine(s_local_bottom, s_bottom, RGBA(g_config->esp.admins.snaplines_col), 1.0f);
				}
				if (g_config->esp.admins.skeleton) {
					draw_entity_bones(data.bones, g_config->esp.admins.skeleton_col, 1.f, false);
				}

				if (g_config->esp.admins.rect_box) {
					if (g_config->esp.admins.rect_box_type == 0) {
						renderer.RenderRect(rect_start, rect_end, RGBA(g_config->esp.admins.rect_box_col), 0.0f, 0, 1.f);
					}
					else if (g_config->esp.admins.rect_box_type == 1) {
						utils::draw_entity_3dbox(pointer_to_handle((uintptr_t)ped), hash, ped->fPosition, g_config->esp.admins.rect_box_col, utils::ped);
					}
				}

				continue;
			}

			if (g_config->esp.players.skeleton) {
				_make_anim(this_anim->skeleton.opacity, g_config->esp.players.skeleton, 14.f);
				RGBA skeleton_color = RGBA((data.visible) ?
					g_config->esp.players.skeleton_visible_col :
					g_config->esp.players.skeleton_invisible_col,
					this_anim->skeleton.opacity);

				draw_entity_bones(data.bones, skeleton_color, g_config->esp.players.skeleton_thickness,
					(w_distance < 30.f && g_config->esp.players.skel_hands) ? true : false);
			}

			if (!g_config->esp.players.enabled) continue;

			if (g_config->esp.players.snaplines) {
				Vector3 startpos = GetBonePosition(local.player, SPINE_ROOT);

				if (!WorldToScreen(startpos, &s_local_bottom)) {
					// if local player start pos of line not on screen - rendering start pos from btm center of screen
					float ScreenWidth = ImGui::GetIO().DisplaySize.x;
					float ScreenHeight = ImGui::GetIO().DisplaySize.y;
					s_local_bottom = { ScreenWidth / 2.f, ScreenHeight };
				}
				_make_anim(this_anim->snaplines.opacity, g_config->esp.players.snaplines, 14.f);
				renderer.RenderLine(s_local_bottom, s_bottom, RGBA(g_config->esp.players.snaplines_col, this_anim->snaplines.opacity), g_config->esp.players.snaplines_thickness);
			}

			if (g_config->esp.players.fill_box) {
				_make_anim(this_anim->fill_box.opacity, g_config->esp.players.fill_box, 14.f);

				if (g_config->esp.players.fill_box_col_type == 0) {
					if (ped->HP - 100.f > g_config->esp.players.lethal_health) {
						renderer.RenderRectFilled(rect_start, rect_end, RGBA(g_config->esp.players.fill_box_solid_col, this_anim->fill_box.opacity), 1.f);
					}
					else if (ped->HP - 100.f <= g_config->esp.players.lethal_health && g_config->esp.players.lethal_fill) {
						renderer.RenderRectFilled(rect_start, rect_end, RGBA(g_config->esp.players.lethal_fill_col, this_anim->fill_box.opacity), 1.f);
					}
				}
				else if (g_config->esp.players.fill_box_col_type == 1) {
					renderer.RenderRectFilledMulti(rect_start, rect_end,
						RGBA(g_config->esp.players.fillbox_box_grd.col_upr, this_anim->fill_box.opacity),
						RGBA(g_config->esp.players.fillbox_box_grd.col_bot, this_anim->fill_box.opacity),
						true);
				}
			}

			if (g_config->esp.players.rect_box) {
				_make_anim(this_anim->rect_box.opacity, g_config->esp.players.rect_box, 14.f);
				renderer.RenderRect(rect_start, rect_end, RGBA(g_config->esp.players.rect_box_col, this_anim->rect_box.opacity), 1.0f);
				// todo: optimize and add this in furture
				//utils::draw_entity_3dbox(pointer_to_handle((uintptr_t)ped), hash, ped->fPosition, g_config->esp.vehicles.names_col, utils::ped);
			}

			if (g_config->esp.players.corners_box) {
				auto top_left = ImVec2(s_top.x - (s_box_width / 2.0f), s_top.y);
				auto bottom_right = ImVec2(s_top.x + (s_box_width / 2.0f), s_top.y + s_box_height);

				// sizing anim
				_make_anim(this_anim->corners.opacity, g_config->esp.players.corners_box, 14.f);
				this_anim->corners.size = ImLerp(this_anim->corners.size, 4.f / (g_config->esp.players.corners_size / 10.f), ImGui::GetIO().DeltaTime * 13.f);

				float corner_offset_w = s_box_width / this_anim->corners.size;
				float corner_offset_h = s_box_height / this_anim->corners.size;
				// local shorter var
				auto color = RGBA(g_config->esp.players.corners_box_col, this_anim->corners.opacity);

				// func shorter
				auto draw_corner = [&](ImVec2 p1, ImVec2 p2, bool horizontal) {
					ImVec2 offset = horizontal ? ImVec2(corner_offset_w, 0) : ImVec2(0, corner_offset_h);
					//renderer.RenderLine(p1, { p1.x + offset.x, p1.y + offset.y }, color, thickness_box);
					renderer.RenderLine(p1, p2, color, thickness_box);
					};

				// left upper corner
				draw_corner(top_left, ImVec2(top_left.x, top_left.y + corner_offset_h), false);
				draw_corner(top_left, ImVec2(top_left.x + corner_offset_w, top_left.y), true);

				// left btm corner
				draw_corner(ImVec2(top_left.x, top_left.y + s_box_height), ImVec2(top_left.x + corner_offset_w, top_left.y + s_box_height), true);
				draw_corner(ImVec2(top_left.x, top_left.y + s_box_height), ImVec2(top_left.x, top_left.y + s_box_height - corner_offset_h), false);

				// right upper corner
				draw_corner(ImVec2(bottom_right.x, bottom_right.y - s_box_height), ImVec2(bottom_right.x - corner_offset_w, bottom_right.y - s_box_height), true);
				draw_corner(ImVec2(bottom_right.x, bottom_right.y - s_box_height), ImVec2(bottom_right.x, bottom_right.y - s_box_height + corner_offset_h), false);

				// right btm corner
				draw_corner(bottom_right, ImVec2(bottom_right.x - corner_offset_w, bottom_right.y), true);
				draw_corner(bottom_right, ImVec2(bottom_right.x, bottom_right.y - corner_offset_h), false);
			}


			float out_of_box_spacing = (s_box_width / 8 > 6.f) ? 6.f : ((s_box_width / 8 < 2.f) ? 4.f : s_box_width / 8);

			if (g_config->esp.players.bars.health_bar) {
				this_anim->health_bar.health_percent = ImLerp(this_anim->health_bar.health_percent, ped->HP - 100.f, ImGui::GetIO().DeltaTime * 14.f);
				this_anim->health_bar.armor_percent = ImLerp(this_anim->health_bar.health_percent, ped->GetArmor(), ImGui::GetIO().DeltaTime * 14.f);

				float w_health = 20.f;
				float bar_width = (s_box_width > w_health) ? s_box_width : w_health;
				int bar_mode = g_config->esp.players.bars.bars_position;

				bool draw_armor = ped->GetArmor() > 1.0f && g_config->esp.players.bars.armor_bar;

				_make_anim(this_anim->health_bar.opacity_hp, g_config->esp.players.bars.health_bar, 14.f);
				_make_anim(this_anim->health_bar.opacity_a, draw_armor, 14.f);

				switch (bar_mode) {
				case 0: // top
					utils::draw_hp_bar(s_top.x - bar_width / 2, s_top.y - out_of_box_spacing, bar_width, 2, this_anim->health_bar.health_percent, ped->MaxHP - 100.0f, false, false, this_anim->health_bar.opacity_hp);
					if (draw_armor) {
						utils::draw_hp_bar(s_top.x - bar_width / 2, s_top.y - (out_of_box_spacing * 2), bar_width, 2, this_anim->health_bar.armor_percent, 100.0f, true, false, this_anim->health_bar.opacity_a);
					}
					break;
				case 1: // bottom
					utils::draw_hp_bar(s_top.x - bar_width / 2, s_box_height + s_top.y + out_of_box_spacing, bar_width, 2, this_anim->health_bar.health_percent, ped->MaxHP - 100.0f, false, false, this_anim->health_bar.opacity_hp);
					if (draw_armor) {
						utils::draw_hp_bar(s_top.x - bar_width / 2, s_box_height + s_top.y + (out_of_box_spacing * 2), bar_width, 2, this_anim->health_bar.armor_percent, 100.0f, true, false, this_anim->health_bar.opacity_a);
					}
					break;
				case 2: // left
					utils::draw_hp_bar(s_top.x - s_box_width / 2 - out_of_box_spacing, s_top.y, 2, s_box_height, this_anim->health_bar.health_percent, ped->MaxHP - 100.0f, false, true, this_anim->health_bar.opacity_hp);
					if (draw_armor) {
						utils::draw_hp_bar(s_top.x - s_box_width / 2 - out_of_box_spacing * 2, s_top.y, 2, s_box_height, this_anim->health_bar.armor_percent, 100.0f, true, true, this_anim->health_bar.opacity_a);
					}
					break;
				case 3: // right
					utils::draw_hp_bar(s_top.x + s_box_width / 2 + out_of_box_spacing, s_top.y, 2, s_box_height, this_anim->health_bar.health_percent, ped->MaxHP - 100.0f, false, true, this_anim->health_bar.opacity_hp);
					if (draw_armor) {
						utils::draw_hp_bar(s_top.x + s_box_width / 2 + out_of_box_spacing * 2, s_top.y, 2, s_box_height, this_anim->health_bar.armor_percent, 100.0f, true, true, this_anim->health_bar.opacity_a);
					}
					break;
				}
			}


			ImVec2 s_text_position;
			float hpbar_spacing;

			switch (g_config->esp.players.text_preset.condition)
			{
			case 0:
				hpbar_spacing = (g_config->esp.players.bars.health_bar && g_config->esp.players.bars.bars_position == 3) ?
					(g_config->esp.players.bars.armor_bar && ped->GetArmor() > 1.0f) ?
					out_of_box_spacing * 3 : // hp + armor bar + oob spacing
					out_of_box_spacing * 2 : // only hp bar + spacing
					out_of_box_spacing;		 // just spacing
				s_text_position = ImVec2((s_top.x + hpbar_spacing) - (s_box_width / 2.0f) + s_box_width + thickness_box, s_top.y); // right (algn - left)
				break;

			case 1:
				hpbar_spacing = (g_config->esp.players.bars.health_bar && g_config->esp.players.bars.bars_position == 1) ?
					(g_config->esp.players.bars.armor_bar && ped->GetArmor() > 1.0f) ?
					out_of_box_spacing * 3 : // hp + armor bar + oob spacing
					out_of_box_spacing * 2 : // only hp bar + spacing
					out_of_box_spacing;		 // just spacing
				s_text_position = ImVec2(s_top.x, TextStartY + hpbar_spacing); // btm (centered)
				break;

			case 2:
				hpbar_spacing = (g_config->esp.players.bars.health_bar && g_config->esp.players.bars.bars_position == 2) ?
					(g_config->esp.players.bars.armor_bar && ped->GetArmor() > 1.0f) ?
					out_of_box_spacing * 3 : // hp + armor bar + oob spacing
					out_of_box_spacing * 2 : // only hp bar + spacing
					out_of_box_spacing;		 // just spacing
				s_text_position = ImVec2((s_top.x - hpbar_spacing) - (s_box_width / 2.0f) - thickness_box, s_top.y); // left (algn - right)
				break;
			}

			g_config->esp.players.text_preset.adaptive_scale = 13.f - w_distance / g_config->esp.players.text_preset.adsc_strength;

			if (g_config->esp.players.eqp_weapon) {
				if (data.weapon != 0xA2719263) { // 0xA2719263 - fist
					_make_anim(this_anim->text_flags.weapon_op, g_config->esp.players.eqp_weapon, 14.f);

					RGBA text_col = g_config->esp.players.eqp_weapon_col;

					if (g_config->esp.players.heavy_wep) {
						if (std::find(heavyWeapons.begin(), heavyWeapons.end(), data.weapon) != heavyWeapons.end()) {
							text_col = g_config->esp.players.heavy_wep_col;
						}
					}
					renderer.RenderText(utils::get_weapon_name(data.weapon),
						ImVec2(s_text_position.x, s_text_position.y + (textRow * text_step)), RGBA(g_config->esp.players.eqp_weapon_col, this_anim->text_flags.weapon_op), g_config->esp.players.text_preset);
					textRow += 1;
				}
			}

			if (g_config->esp.players.distance) {
				char buf[10];
				if (g_config->esp.players.distance_style == 0) {
					sprintf(buf, "%.1f m", w_distance);
				}
				else {
					sprintf(buf, "[%.0f M]", w_distance);
				}

				std::string l_distance(buf);

				_make_anim(this_anim->text_flags.distance_op, g_config->esp.players.distance, 14.f);
				renderer.RenderText(l_distance.c_str(),
					ImVec2(s_text_position.x, s_text_position.y + (textRow * text_step)), RGBA(g_config->esp.players.distance_col, this_anim->text_flags.distance_op), g_config->esp.players.text_preset);
				textRow += 1;
			}


			//for (int i = 0; i < data.flags.size(); i++) {
			//	renderer.RenderText(data.flags[i].text,
			//		ImVec2(s_text_position.x, s_text_position.y + totalTextSpacing),
			//		data.flags[i].size,
			//		RGBA(data.flags[i].color[0],
			//			data.flags[i].color[1],
			//			data.flags[i].color[2],
			//			data.flags[i].color[3]),
			//		g_config->esp.players.text_mgr.esp_font, text_align, true);

			//	textRow += 1;
			//	totalTextSpacing += data.flags[i].size;
			//}
		  }
		  __except (EXCEPTION_EXECUTE_HANDLER) { continue; }
		}

		return;
	} 
}
