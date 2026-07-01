#pragma once
#include "../../imports.h"

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

namespace game {
	extern vector<pair<CObject*, DataPed>> ped_list;
	extern vector<CObject*> object_list;
	extern vector<pair<CVehicle*, DataVehicle>> vehicle_list;

	extern bool isValidPlayer(DWORD hash, CObject* player);
}

std::vector<uint64_t> heavyWeapons = {
	0xA914799, // heavy mk 2
	0xC472FE2, // heavy mk 1
	0xDBBD7280, // combat mg mk2
	0x7FD62962, // combat mg mk1
	0xC734385A, // marksman mk1
	0x6A6C02E0, // marksman mk2
	0x6E7DDDEC // Precision Rifle
};

namespace esp {
	struct s_espanim {
		struct { float opacity = 0.f; } skeleton;
		struct { float opacity = 0.f; } rect_box;
		struct {
			float opacity = 0.f,
				size = 4.f / (g_config->esp.players.corners_size / 10.f);
		} corners;
		struct { float opacity = 0.f; } fill_box;
		struct { float opacity = 0.f; } snaplines;
		struct {
			float health_percent = 0.f, armor_percent = 0.f,
				opacity_hp = 0.f, opacity_a = 0.f;
		} health_bar;

		struct {
			float weapon_op = 0.f, distance_op = 0.f;
		} text_flags;
	};

	namespace utils {
		ImVec2 world_to_radar(Vector3 position, float radar_x, float radar_y, ImVec2 radar_wnd_size, float zoom) {
			ImVec2 return_pos(0, 0);

			CPlayerAngles* cam = Game.getCam();
			if (IsValidPtr(cam) && IsValidPtr(local.player)) {
				float rot = atan2(cam->m_fps_angles.y, cam->m_fps_angles.x);

				Vector2 forward_vec(cos(rot), sin(rot));

				float delta_x = position.x - local.player->fPosition.x;
				float delta_y = position.y - local.player->fPosition.y;

				float position_x = (delta_y * forward_vec.x - delta_x * forward_vec.y) / zoom;
				float position_y = (delta_x * forward_vec.x + delta_y * forward_vec.y) / zoom;

				if (fabs(position_x) > radar_wnd_size.x / 2.0f || fabs(position_y) > radar_wnd_size.y / 2.0f) {
					return ImVec2(0, 0);
				}

				position_x = std::clamp(position_x, -(radar_wnd_size.x / 2.0f - 2.5f), radar_wnd_size.x / 2.0f - 2.5f);
				position_y = std::clamp(position_y, -(radar_wnd_size.y / 2.0f - 2.5f), radar_wnd_size.y / 2.0f - 2.5f);

				return_pos.x = radar_x - position_x;
				return_pos.y = radar_y - position_y;
			}

			return return_pos;
		}

		PVector3 transform_point(const PVector3& center, float offsetX, float offsetY, float offsetZ, const PVector3& rightVector, const PVector3& forwardVector, const PVector3& upVector)
		{
			return PVector3(
				center.x + rightVector.x * offsetX + forwardVector.x * offsetY + upVector.x * offsetZ,
				center.y + rightVector.y * offsetX + forwardVector.y * offsetY + upVector.y * offsetZ,
				center.z + rightVector.z * offsetX + forwardVector.z * offsetY + upVector.z * offsetZ
			);
		}

		enum e_entity_type { ped, vehicle, object };

		void draw_entity_3dbox(int32_t target, int32_t model_hash, Vector3 center_pos, std::array<float, 4> color, e_entity_type matrix_type) {
			PVector3 minDim, maxDim;
			native::gameplay::get_model_dimensions(model_hash, &minDim, &maxDim); // model size

			PVector3 rightVector, forwardVector, upVector, position;
			float halfSizeX, halfSizeY, halfSizeZ;

			auto calculate_half_sizes = [&](float factorX, float factorY, float factorZ) {
				halfSizeX = (maxDim.x - minDim.x) / factorX;
				halfSizeY = (maxDim.y - minDim.y) / factorY;
				halfSizeZ = (maxDim.z - minDim.z) / factorZ;
				native::entity::get_entity_matrix(target, &forwardVector, &rightVector, &upVector, &position);
				};

			switch (matrix_type) {
			case esp::utils::ped:
				calculate_half_sizes(2.7f, 2.5f, 2.0f);
				break;
			case esp::utils::vehicle:
				calculate_half_sizes(1.7f, 2.0f, 2.0f);
				break;
			case esp::utils::object:
				return;
			}

			PVector3 center(center_pos.x, center_pos.y, center_pos.z);
			PVector3 corners[8];
			//PVector3(upVector.x, upVector.y, upVector.z - 0.6f)

			corners[0] = utils::transform_point(center, -halfSizeX, -halfSizeY, -halfSizeZ, rightVector, forwardVector, upVector);
			corners[1] = utils::transform_point(center, -halfSizeX, -halfSizeY, halfSizeZ, rightVector, forwardVector,
				(matrix_type == ped) ? PVector3(upVector.x, upVector.y, upVector.z - 0.15f) : upVector);

			corners[2] = utils::transform_point(center, -halfSizeX, halfSizeY, -halfSizeZ, rightVector, forwardVector, upVector);
			corners[3] = utils::transform_point(center, -halfSizeX, halfSizeY, halfSizeZ, rightVector, forwardVector,
				(matrix_type == ped) ? PVector3(upVector.x, upVector.y, upVector.z - 0.15f) : upVector);

			corners[4] = utils::transform_point(center, halfSizeX, -halfSizeY, -halfSizeZ, rightVector, forwardVector, upVector);
			corners[5] = utils::transform_point(center, halfSizeX, -halfSizeY, halfSizeZ, rightVector, forwardVector,
				(matrix_type == ped) ? PVector3(upVector.x, upVector.y, upVector.z - 0.15f) : upVector);

			corners[6] = utils::transform_point(center, halfSizeX, halfSizeY, -halfSizeZ, rightVector, forwardVector, upVector);
			corners[7] = utils::transform_point(center, halfSizeX, halfSizeY, halfSizeZ, rightVector, forwardVector,
				(matrix_type == ped) ? PVector3(upVector.x, upVector.y, upVector.z - 0.15f) : upVector);

			ImVec2 screenCorners[8];
			for (int i = 0; i < 8; ++i) {
				if (!WorldToScreen(Vector3(corners[i].x, corners[i].y, corners[i].z), &screenCorners[i])) return;
			}

			int lines[][2] = {
				{0, 1}, {0, 2}, {0, 4}, {1, 3}, {1, 5}, {2, 3}, {2, 6}, {3, 7},
				{4, 5}, {4, 6}, {5, 7}, {6, 7}
			};

			for (auto& line : lines) {
				renderer.RenderLine(screenCorners[line[0]], screenCorners[line[1]], color);
			}
		}

		void draw_hp_bar(float x, float y, float w, float h, float health, float max, bool armor, bool horizontal = false, float alpha = 1.f) {
			if (!max)
				return;

			if (health < 0)
				health = 0;

			float ratio = health / max;
			RGBA solid_color = RGBA(armor ? g_config->esp.players.bars.solid_color.armor_bar_col : g_config->esp.players.bars.solid_color.health_bar_col, alpha);

			RGBA grd_perv = RGBA(armor ? g_config->esp.players.bars.grd_color.armor_bar_col_perv : g_config->esp.players.bars.grd_color.health_bar_col_perv, alpha);
			RGBA grd_sec = RGBA(armor ? g_config->esp.players.bars.grd_color.armor_bar_col_sec : g_config->esp.players.bars.grd_color.health_bar_col_sec, alpha);

			if (!horizontal) {
				float step = (w / max);
				float draw = (step * health);
				renderer.RenderRectFilled(ImVec2(x - 2, y - 2), ImVec2(x + w + 2, y + h + 2), RGBA(g_config->esp.players.bars.bars_bg_color, alpha), 2.f);

				if (g_config->esp.players.bars.bars_color_type == 0) {
					renderer.RenderRectFilled(ImVec2(x, y), ImVec2(x + draw, y + h), solid_color, 0.0f, 0);
				}
				else if (g_config->esp.players.bars.bars_color_type == 1) {
					renderer.RenderRectFilledMulti(ImVec2(x, y), ImVec2(x + draw, y + h), grd_perv, grd_sec);
				}
			}
			else {
				float step = (h / max);
				float draw = (step * health);

				renderer.RenderRectFilled(ImVec2(x - 2, y - 2), ImVec2(x + w + 2, y + h + 2), RGBA(g_config->esp.players.bars.bars_bg_color, alpha), 2.f);

				if (g_config->esp.players.bars.bars_color_type == 0) {
					renderer.RenderRectFilled(ImVec2(x, y + h), ImVec2(x + w, y + h - draw), solid_color, 0.0f, 0);
				}
				else if (g_config->esp.players.bars.bars_color_type == 1) {
					renderer.RenderRectFilledMulti(ImVec2(x, y + h), ImVec2(x + w, y + h - draw), grd_perv, grd_sec, true);
				}

				if (g_config->esp.players.health_text && (g_config->esp.players.health_text_style == 1)) {
					static char hp_buf[3] = "";
					sprintf(hp_buf, "%d", (int)health);
					//renderer.RenderText(hp_buf, ImVec2(x + ((g_config->esp.players.bars.bars_position == 3) ? (-2) : 3), y + h - draw - 2), 12, g_config->esp.players.health_text_col, 1, true);
				}
			}
		}

		///  <summary>
		///  Get weapon name from hash. This is used to map a hash to a weapon name.
		///  </summary>
		///  <param name="hash">The hash of the weapon.</param>
		///  <returns>Pointer to the weapon name or NULL if not found</returns>
		const char* get_weapon_name(DWORD hash)
		{
			static const fixed_containers::FixedUnorderedMap<DWORD, const char*, 120> weapon_map = {
				{0x92A27487, ("Dagger")},
				{0x958A4A8F, ("Bat")},
				{0xF9E6AA4B, ("Bottle")},
				{0x84BD7BFD, ("Crow Bar")},
				{0xA2719263, ("\0\0\0")}, // weapon_unarmed
				{0x8BB05FD7, ("Flash Light")},
				{0x440E4788, ("Golf Club")},
				{0x4E875F73, ("Hammer")},
				{0xF9DCBF2D, ("Hatchet")},
				{0xD8DF3C3C, ("Knuckle")},
				{0x99B507EA, ("Knife")},
				{0xDD5DF8D9, ("Machete")},
				{0xDFE37640, ("Switch Blade")},
				{0x678B81B1, ("Night Stick")},
				{0x19044EE0, ("Wrench")},
				{0xCD274149, ("Battle Axe")},
				{0x94117305, ("Pool Cue")},
				{0x1B06D571, ("Pistol")},
				{0xBFE256D4, ("Pistol MK2")},
				{0x5EF9FEC4, ("Combat Pistol")},
				{0x22D8FE39, ("AP Pistol")},
				{0x3656C8C1, ("Taser")},
				{0x99AEEB3B, ("Pistol .50")},
				{0xBFD21232, ("SNS Pistol")},
				{0x88374054, ("SNS Pistol MKII")},
				{0xD205520E, ("Heavy Pistol")},
				{0x83839C4, ("Vintage Pistol")},
				{0x47757124, ("Flare Gun")},
				{0xDC4DB296, ("Marksman Pistol")},
				{0xC1B3C3D1, ("Heavy Revolver")},
				{0xCB96392F, ("Heavy Revolver MKII")},
				{0x97EA20B8, ("DA Revolver")},

				{0xAF3696A1, ("Ray Pistol")},
				{0x2B5EF5EC, ("Ceramic Pistol")},
				{0x917F6C8C, ("Navy Revolver")},
				{0x57A4368C, ("Perico Pistol")},
				{0x45CD9CF3, ("Taser MP")},
				{0x1BC4FDB9, ("WM 29 Pistol")},

				{0x13532244, ("Micro SMG")},
				{0x2BE6766B, ("SMG")},
				{0x78A97CD0, ("SMG MKII")},
				{0xEFE7E2DF, ("Assault SMG")},
				{0xA3D4D34, ("Combat PDW")},
				{0xDB1AA450, ("Machine Pistol")},
				{0xBD248B55, ("Mini SMG")},

				{0x476BF155, ("Ray Carbine")},
				{0x14E5AFD5, ("Tactical SMG")},

				{0x1D073A89, ("Pump Shotgun")},
				{0x555AF99A, ("Pump Shotgun MKII")},
				{0x7846A318, ("Sawed-Off Shotgun")},
				{0xE284C527, ("Assault Shotgun")},
				{0x9D61E50F, ("Bullpup Shotgun")},
				{0xA89CB99E, ("Musket")},
				{0x3AABBBAA, ("Heavy Shotgun")},
				{0xEF951FBB, ("DB Shotgun")},
				{0x12E82D3D, ("Sweeper Shotgun")},

				{0x5A96BA4, ("Combat Shotgun")},

				{0xBFEFFF6D, ("Assault Rifle")},
				{0x394F415C, ("Assault Rifle MKII")},
				{0x83BF0278, ("Carbine Rifle")},
				{0xFAD1F1C9, ("Carbine Rifle MKII")},
				{0xAF113F99, ("Advanced Rifle")},
				{0xC0A3098D, ("Special Carbine")},
				{0x969C3D67, ("Special Carbine MKII")},
				{0x7F229F94, ("Bullpup Rifle")},
				{0x84D6FAFD, ("Bullpup Rifle MKII")},

				{0x9D1F17E6, ("Military Rifle")},
				{0xC78D71B4, ("Heavy Rifle")},
				{0xD1D5F52B, ("Tactical Rifle")},

				{0x624FE830, ("Compact Rifle")},
				{0x9D07F764, ("MG")},
				{0x7FD62962, ("Combat MG")},
				{0xDBBD7280, ("Combat MG MKII")},
				{0x61012683, ("Gusenberg Sweeper")},
				{0x5FC3C11, ("Bolt Sniper")},
				{0xC472FE2, ("Heavy Sniper")},
				{0xA914799, ("Heavy Sniper MKII")},
				{0xC734385A, ("Marksman Rifle")},
				{0x6A6C02E0, ("Marksman Rifle MKII")},

				{0x6E7DDDEC, ("Precision Rifle")},

				{0xB1CA77B1, ("Rocket Launcher")},
				{0xA284510B, ("Grenade Launcher")},
				{0x4DD2DC56, ("Smoke Launcher")},
				{0x42BF8A85, ("MiniGun")},
				{0x7F7497E5, ("Firework Launcher")},
				{0x6D544C99, ("RailGun")},
				{0x63AB0442, ("Homing Launcher")},
				{0x781FE4A, ("Compact Grenade Launcher")},

				{0xB62D1F67, ("Ray MiniGun")},
				{0xDB26713A, ("Compact EMP Launcher") },
				{0xFEA23564, ("RailGun XM3") },

				{0x93E220BD, ("Grenade") },
				{0xA0973D5E, ("BZ Gas") },
				{0xFDBC8A50, ("Tear Gas") },
				{0x497FACC3, ("Flare") },
				{0x24B17070, ("Molotov") },
				{0x2C3731D9, ("Sticky Bomb") },
				{0xAB564B93, ("Proximity Mine") },
				{0x787F0BB, ("Snowball") },
				{0xBA45E8B8, ("Pipe Bomb") },
				{0x23C9F95C, ("Baseball") },

				{0xF7F1E25E, ("Acid Package") },

				{0x34A67B97, ("Jerry Can") },
				{0xBA536372, ("Hazard Can") },
				{0x184140A1, ("Fertilizer Can") },
				{0x60EC506, ("Fire Extinguisher") },
				{0xFBAB5776, ("Parachute")}
			};

			auto it = weapon_map.find(hash);
			if (it != weapon_map.end()) {
				return it->second;
			}
			else {
				return "\0\0\0";
			}
		}
	}

	void draw_entity_bones(PlayerBones bones, RGBA current_color, float thickness, bool hands_addtnl) {
		thickness = std::clamp(thickness, 0.5f, 6.0f);

		auto is_real_bone = [](const Vector3& p) {
			return is_valid_world_pos(p) &&
				(std::fabs(p.x) > 0.001f || std::fabs(p.y) > 0.001f || std::fabs(p.z) > 0.001f);
		};

		auto bone_to_screen = [&](const Vector3& p, ImVec2& out) {
			if (!is_real_bone(p)) return false;
			if (!WorldToScreen(p, &out) && !WorldToScreen2(p, &out)) return false;
			return isW2SValid(out);
		};

		auto draw_segment = [&](const Vector3& from, const Vector3& to) {
			const float len = get_distance(from, to);
			if (!std::isfinite(len) || len < 0.015f || len > 5.0f) return;

			ImVec2 start_pos;
			ImVec2 end_pos;
			if (!bone_to_screen(from, start_pos) || !bone_to_screen(to, end_pos)) return;
			if (std::fabs(start_pos.x - end_pos.x) + std::fabs(start_pos.y - end_pos.y) < 1.0f) return;

			renderer.RenderLine(start_pos, end_pos, current_color, thickness);
		};

		Vector3 bones_coords[][2] = {
			{ bones.HEAD, bones.NECK },
			{ bones.NECK, bones.RIGHT_CLAVICLE },
			{ bones.RIGHT_CLAVICLE, bones.RIGHT_UPPER_ARM },
			{ bones.RIGHT_UPPER_ARM, bones.RIGHT_FOREARM },
			{ bones.RIGHT_FOREARM, bones.RIGHT_HAND },

			{ bones.NECK, bones.LEFT_CLAVICLE },
			{ bones.LEFT_CLAVICLE, bones.LEFT_UPPER_ARM },
			{ bones.LEFT_UPPER_ARM, bones.LEFT_FOREARM },
			{ bones.LEFT_FOREARM, bones.LEFT_HAND },

			{ bones.NECK, bones.SPINE3 },
			{ bones.SPINE3, bones.SPINE2 },
			{ bones.SPINE2, bones.SPINE1 },
			{ bones.SPINE1, bones.SPINE_ROOT },

			{ bones.SPINE_ROOT, bones.RIGHT_THIGH },
			{ bones.RIGHT_THIGH, bones.RIGHT_CALF },
			{ bones.RIGHT_CALF, bones.RIGHT_FOOT },

			{ bones.SPINE_ROOT, bones.LEFT_THIGH },
			{ bones.LEFT_THIGH, bones.LEFT_CALF },
			{ bones.LEFT_CALF, bones.LEFT_FOOT },
		};

		Vector3 handsskel_coords[][2] = {
			{ bones.LEFT_HAND, bones.LEFT_FINGER1 },
			{ bones.LEFT_FINGER1, bones.LEFT_FINGER2 },
			{ bones.LEFT_HAND, bones.LEFT_FINGER10 },
			{ bones.LEFT_FINGER10, bones.LEFT_FINGER11 },
			{ bones.LEFT_FINGER11, bones.LEFT_FINGER12 },
			{ bones.LEFT_HAND, bones.LEFT_FINGER20 },
			{ bones.LEFT_FINGER20, bones.LEFT_FINGER21 },
			{ bones.LEFT_FINGER21, bones.LEFT_FINGER22 },
			{ bones.LEFT_HAND, bones.LEFT_FINGER30 },
			{ bones.LEFT_FINGER30, bones.LEFT_FINGER31 },
			{ bones.LEFT_FINGER31, bones.LEFT_FINGER32 },
			{ bones.LEFT_HAND, bones.LEFT_FINGER40 },
			{ bones.LEFT_FINGER40, bones.LEFT_FINGER41 },
			{ bones.LEFT_FINGER41, bones.LEFT_FINGER42 },

			{ bones.RIGHT_HAND, bones.RIGHT_FINGER1 },
			{ bones.RIGHT_FINGER1, bones.RIGHT_FINGER2 },
			{ bones.RIGHT_HAND, bones.RIGHT_FINGER10 },
			{ bones.RIGHT_FINGER10, bones.RIGHT_FINGER11 },
			{ bones.RIGHT_FINGER11, bones.RIGHT_FINGER12 },
			{ bones.RIGHT_HAND, bones.RIGHT_FINGER20 },
			{ bones.RIGHT_FINGER20, bones.RIGHT_FINGER21 },
			{ bones.RIGHT_FINGER21, bones.RIGHT_FINGER22 },
			{ bones.RIGHT_HAND, bones.RIGHT_FINGER30 },
			{ bones.RIGHT_FINGER30, bones.RIGHT_FINGER31 },
			{ bones.RIGHT_FINGER31, bones.RIGHT_FINGER32 },
			{ bones.RIGHT_HAND, bones.RIGHT_FINGER40 },
			{ bones.RIGHT_FINGER40, bones.RIGHT_FINGER41 },
			{ bones.RIGHT_FINGER41, bones.RIGHT_FINGER42 },
		};

		for (size_t i = 0; i < sizeof(bones_coords) / sizeof(bones_coords[0]); i++) {
			draw_segment(bones_coords[i][0], bones_coords[i][1]);
		}

		if (hands_addtnl) {
			for (size_t i = 0; i < sizeof(handsskel_coords) / sizeof(handsskel_coords[0]); i++) {
				draw_segment(handsskel_coords[i][0], handsskel_coords[i][1]);
			}
		}
	}
}

#define _make_anim(anim_var, v, speed) anim_var = ImLerp(anim_var, v ? 1.f : 0.f, ImGui::GetIO().DeltaTime * speed)