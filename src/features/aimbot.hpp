#pragma once
#include "../imports.h"



namespace game {
	extern vector<pair<CObject*, DataPed>> ped_list;

	extern bool isValidPlayer(DWORD hash, CObject* player);
	extern bool is_ped_visible(CObject* this_ped);
}


namespace aimbot {
	CObject* current_vector_target;
	DataPed current_vector_target_data;
	CObject* current_silent_target;
	DataPed current_silent_target_data;

	static Vector3 lastTargetVector = Vector3(0, 0, 0);
	static auto targetTimeout = std::chrono::high_resolution_clock::now();
	static float aimTargetDist = 99999.0f;
	static Vector3 target_aim_angle;

	Vector3 current_silent_target_coord;

	bool is_zalupa_patched = false;

	bool has_clear_silent_line(CObject* target) {
		if (!target || !local.player) return false;

		__try {
			if (!IsValidPtr(target) || !IsValidPtr(local.player) || target->HP <= 0) return false;
			return game::is_ped_visible(target);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return false;
		}
	}

	//TODO:
	// ЗАЛУПА ПАТЧ ХУЙНИ https://www.unknowncheats.me/forum/4065073-post24.html
	// 0x90 0x90 0x90 0x90
	// ПЕНИСЫ
	/*__int64 on_gta_bullet_shot(__int64 CWeapon, Vector3 vend, Vector3 vend2) {

		//std::cout << "bullet shot hook test" << std::endl;
		//logs::add("weapon_shot_test");

		//std::cout << "--------------" << std::endl << "target: " << current_silent_target_coord.x << " " << current_silent_target_coord.y << " " << current_silent_target_coord.z << std::endl;
		//if (current_silent_target != nullptr && (((rand()%100) + 1) <= g_config->aimbot.silent.hitchance)) {
		//	vend = current_silent_target_coord;
		//	vend2 = current_silent_target_coord;
		//
		//	//is_zalupa_patched = true;
		//	local.player->weapon()->weaponobj()->get_cweapon()->set_muzzle_start(
		//		Vector3(
		//			current_silent_target_coord.x,
		//			current_silent_target_coord.y,
		//			current_silent_target_coord.z + 1)
		//	);
		//}
		//Vector3 vstart = local.player->weapon()->weaponobj()->get_cweapon()->get_muzzle_start();

		//std::cout << "pos: " << vend.x << " " << vend.y << " " << vend.z << std::endl;
		//std::cout << "muzzle: " << vstart.x << " " << vstart.y << " " << vstart.z << std::endl;

		return original_bullet_shot(CWeapon, vend, vend2);
	}
	*/
	
	///  <summary>
	///  Called when a bullet is spawned. This is the function that will be called by the player every time a bullet is fired
	///  </summary>
	///  <param name="sBulletInstance_1">The bullet instance object</param>
	///  <param name="pWeaponInfo">WeaponInfo</param>
	///  <param name="vStart">The start position of the bullet instance ( if any )</param>
	///  <param name="vEnd">The end position of the bullet instance ( if any</param>
	///  <param name="fVelocity">Velocity of the bullet</param>
    ///  <param name="weaponHash">weapon hash from which the bullet was fired</param>
	///  <param name="createsTrace">is there a tracer?</param>
	///  <param name="isAccurate">is it accurate (??????)</param> 
	__int64 on_bullet_instance(__int64* sBulletInstance_1, const __int64* pWeaponInfo, Vector3* vStart, Vector3* vEnd, float fVelocity, unsigned int weaponHash, bool createsTrace, bool isAccurate) {

		if (!cfg_manager::is_io_active() && g_config && g_config->aimbot.silent.enabled && current_silent_target != nullptr && IsValidPtr(vStart) && IsValidPtr(vEnd)) {
			if (!has_clear_silent_line(current_silent_target)) {
				current_silent_target = nullptr;
				current_silent_target_coord = Vector3(-1, -1, -1);
			}
			else if ((((rand() % 100) + 1) <= g_config->aimbot.silent.hitchance)) {
				// Keep the real bullet origin so GTA owns wall/object collision.
				vEnd->x = current_silent_target_coord.x;
				vEnd->y = current_silent_target_coord.y;
				vEnd->z = current_silent_target_coord.z;
			}
		}

		return original_bullet_instance(sBulletInstance_1, pWeaponInfo, vStart, vEnd, fVelocity, weaponHash, createsTrace, isAccurate);
	}
	
	
	///  <summary>
	///  Returns the position of the closest bones. This is used to determine where is the closest bone from DataPed
	///  </summary>
	///  <param name="current">The position of the current aimpos</param>
	///  <param name="data">The data that contains the bones we are looking for</param>
	///  <returns>The position of the closest bone </returns>
	Vector3 get_closest_bone(Vector3 current, DataPed data) {
		Vector3 aim = current;
		PlayerBones bones = data.bones;
		Vector3 bone_positions[] = {
					bones.HEAD, bones.NECK,
					bones.RIGHT_FOREARM,
					bones.RIGHT_UPPER_ARM, bones.RIGHT_CLAVICLE,
					bones.LEFT_FOREARM,
					bones.LEFT_UPPER_ARM, bones.LEFT_CLAVICLE,
					bones.PELVIS, bones.SPINE_ROOT,
					bones.SPINE0, bones.SPINE1,
					bones.SPINE2, bones.SPINE3,
					bones.RIGHT_THIGH,
					bones.LEFT_THIGH
		};
		float closest_distance = 999.f;
		int arrSize = sizeof(bone_positions) / sizeof(bone_positions[0]);

		for (size_t i = 0; i < arrSize; i++) {
			ImVec2 screen_point;
			if (!WorldToScreen2(bone_positions[i], &screen_point)) continue;
			float dist = screen_distance(Game.screen.x / 2, Game.screen.y / 2, screen_point.x, screen_point.y);
			if (closest_distance > dist) {
				closest_distance = dist;
				aim = bone_positions[i];
			}
		}
		return aim;
	}

	Vector3 get_hitbox_position(const DataPed& data, int hitbox) {
		switch (hitbox) {
		case 0: return data.bones.HEAD;
		case 1: return data.bones.NECK;
		case 2: return data.bones.SPINE1;
		case 3: return data.bones.SPINE2;
		case 4: return get_closest_bone(Vector3(), data);
		default: return Vector3(); // Return a default position if invalid
		}
	}

	
	


	
	///  <summary>
	///  Get a new vector target.
	///  </summary>
	///  <param name="cam"> The camera's position</param>
	///  <param name="hitbox">selected hitbox</param>
	///  <param name="only_visible">if true will find only visible players</param>
	///  <param name="fov">self explanatory</param>
	void get_new_vector_target(CPlayerAngles* cam, int hitbox, bool only_visible, int fov) {
		if (!IsValidPtr(cam) || !IsValidPtr(local.player)) {
			return; // Early exit if pointers are not valid
		}

		Vector3 CrosshairPos = cam->m_cam_pos;
		CObject* closest = nullptr;
		DataPed closest_data = { 0 };
		bool target_acquired = false;

		for (const auto& entity : game::ped_list) {
			CObject* ped = entity.first;
			DataPed data = entity.second;

			// Skip invalid or irrelevant entities
			if (!IsValidPtr(ped) || local.player == ped) {
				continue;
			}

			DWORD hash = 0;
			__try {
				if (ped->HP <= 0) continue;
				if (!IsValidPtr(ped->ModelInfo())) continue;
				hash = ped->ModelInfo()->GetHash();
			}
			__except (EXCEPTION_EXECUTE_HANDLER) { continue; }

			if (!game::isValidPlayer(hash, ped) && !g_config->aimbot.vector.filters.target_npcs) {
				continue;
			}
			if (data.isAdmin && !g_config->aimbot.vector.filters.target_adms) {
				continue;
			}

			Vector3 aimpos = get_hitbox_position(data, hitbox);
			if (!data.visible && only_visible) {
				continue; // Skip if the target is not visible and only_visible is true
			}

			ImVec2 aim2d;
			if (!WorldToScreen2(aimpos, &aim2d)) {
				continue; // Skip if aim position cannot be projected to screen
			}

			float Distance2d = screen_distance(Game.screen.x / 2, Game.screen.y / 2, aim2d.x, aim2d.y);
			if (fov > Distance2d && (aimTargetDist > Distance2d || current_vector_target == ped)) {
				aimTargetDist = Distance2d;
				target_acquired = true;
				closest = ped;
				closest_data = data;
			}
		}

		
		if (closest) {
			// Only update if the closest target is different or if there is no current target
			if (closest != current_vector_target || current_vector_target == nullptr) { //-V560
				current_vector_target = closest;
				current_vector_target_data = closest_data;
				dbg::vector_aimbot_target.updateArg(closest);
			}
		}
		else {
			current_vector_target = nullptr;
			dbg::vector_aimbot_target.updateArg("None");
			aimTargetDist = 9999.f;
			target_aim_angle = { 0 };
		}
	}


	void get_new_silent_target(CPlayerAngles* cam, int hitbox, int fov) {
		if (!IsValidPtr(cam) || !IsValidPtr(local.player)) {
			return; // early exit
		}

		Vector3 aimpos;
		CObject* closest = nullptr;
		DataPed closest_data = { 0 };
		Vector3 CrosshairPos = cam->m_cam_pos;

		for (const auto& entity : game::ped_list) {
			CObject* ped = entity.first;
			DataPed data = entity.second;

			
			if (!IsValidPtr(ped) || local.player == ped) {
				continue;
			}

			DWORD hash = 0;
			__try {
				if (ped->HP <= 0) continue;
				if (!IsValidPtr(ped->ModelInfo())) continue;
				hash = ped->ModelInfo()->GetHash();
			}
			__except (EXCEPTION_EXECUTE_HANDLER) { continue; }

			if (!game::isValidPlayer(hash, ped) && !g_config->aimbot.silent.filters.target_npcs) {
				continue;
			}
			if (data.isAdmin && !g_config->aimbot.vector.filters.target_adms) {
				continue;
			}

			
			switch (hitbox) {
			case 0: aimpos = data.bones.HEAD; break;
			case 1: aimpos = data.bones.NECK; break;
			case 2: aimpos = data.bones.SPINE1; break;
			case 3: aimpos = data.bones.SPINE2; break;
			case 4: aimpos = get_closest_bone(aimpos, data); break;
			default: continue;
			}

			ImVec2 aim2d;
			if (!WorldToScreen2(aimpos, &aim2d)) {
				logs::add("Err: something went wrong, vector target is somehow off screen");
				continue; 
			}

			float Distance2d = screen_distance(Game.screen.x / 2, Game.screen.y / 2, aim2d.x, aim2d.y);
			if (Distance2d < fov) {
				closest = ped;
				closest_data = data;
			}
		}

		// Update target data if a closest target was found
		if (closest) {
			current_silent_target = closest;
			current_silent_target_data = closest_data;
			current_silent_target_coord = get_hitbox_position(closest_data, hitbox);
			dbg::silent_aimbot_target.updateArg(closest);
		}
		else {
			current_silent_target = nullptr;
			current_silent_target_coord = Vector3(-1, -1, -1);
			dbg::silent_aimbot_target.updateArg("NONE");
		}
	}



	void vector_aimbot_tick() {
		float max_aim_radius = g_config->aimbot.vector.field_of_view;
		float threshold = 0.5f / g_config->aimbot.vector.smoothness;
		CPlayerAngles* cam = Game.getCam();

		// Check for valid ptrs and if aimbot is enabled
		if (!IsValidPtr(cam) || !IsValidPtr(local.player) || !g_config->aimbot.vector.enabled) {
			return;
		}

		CWeaponManager* wep = local.player->weapon();
		if (IsValidPtr(wep) && IsValidPtr(wep->_WeaponInfo)) {
			if (wep->_WeaponInfo->GetWeaponHash() == 0xA2719263)
				return;
		}

		auto old_target = current_vector_target;
		get_new_vector_target(cam, g_config->aimbot.vector.target_bones, g_config->aimbot.vector.filters.visibility_check, max_aim_radius);

		if (current_vector_target == old_target) {
			auto match = std::find_if(game::ped_list.begin(), game::ped_list.end(),
				[](const std::pair<CObject*, DataPed>& v) { return v.first == current_vector_target; });

			if (match != game::ped_list.end()) {
				int index = std::distance(game::ped_list.begin(), match);
				DataPed data = game::ped_list[index].second;

				Vector3 aim3d;
				switch (g_config->aimbot.vector.target_bones) {
				case 0: aim3d = data.bones.HEAD; break;
				case 1: aim3d = data.bones.NECK; break;
				case 2: aim3d = data.bones.SPINE1; break;
				case 3: aim3d = data.bones.SPINE2; break;
				case 4: aim3d = get_closest_bone(aim3d, data); break;
				}

				ImVec2 aim2d;
				if (!WorldToScreen2(aim3d, &aim2d)) {
					logs::add("Err: something went wrong, vector target is somehow off screen");
					return;
				}

				float Distance2d = screen_distance(Game.screen.x / 2, Game.screen.y / 2, aim2d.x, aim2d.y);
				Vector3 Crosshair3D = (g_config->aimbot.vector.field_of_view > Distance2d) ? cam->m_cam_pos : Vector3{};

				float dist = get_distance(Crosshair3D, current_vector_target->fPosition);
				Vector3 Out((aim3d.x - Crosshair3D.x) / dist, (aim3d.y - Crosshair3D.y) / dist, (aim3d.z - Crosshair3D.z) / dist);
				Vector3 viewAngles = cam->m_tps_angles;

				// Update angles only when a bind is pressed
				if (GetAsyncKeyState(g_config->aimbot.vector.hotkey) & 0x8000) {
					Vector3 delta = Out - cam->m_tps_angles;
					float newX = 0;
					float newY = 0;
					float newZ = 0;

					float delta_formula = (g_config->aimbot.vector.smoothness / 1.5f);

					newX = viewAngles.x + (delta.x / delta_formula);
					newY = viewAngles.y + (delta.y / delta_formula);
					newZ = viewAngles.z + (delta.z / delta_formula);

					if (newX != 0) {
						cam->m_fps_angles.x = newX;
						cam->m_tps_angles.x = newX;
					}
					if (newY != 0) {
						cam->m_fps_angles.y = newY;
						cam->m_tps_angles.y = newY;
					}
					if (newZ != 0) {
						cam->m_fps_angles.z = newZ;
						cam->m_tps_angles.z = newZ;
					}
				
				}
			}
		}
	}

	void silent_aimbot_tick() {
		if (!g_config->aimbot.silent.enabled) {
			current_silent_target = nullptr;
			current_silent_target_coord = Vector3(-1, -1, -1);
			return;
		}

		CPlayerAngles* cam = Game.getCam();
		if (IsValidPtr(cam) && IsValidPtr(local.player)) {
			CWeaponManager* wep = local.player->weapon();
			if (IsValidPtr(wep) && IsValidPtr(wep->_WeaponInfo)) {
				if (wep->_WeaponInfo->GetWeaponHash() == 0xA2719263) {
					current_silent_target = nullptr;
					current_silent_target_coord = Vector3(-1, -1, -1);
					return;
				}
			}

			get_new_silent_target(cam, g_config->aimbot.silent.target_bones, g_config->aimbot.silent.field_of_view);
		}
	}

	void trigger_tick() {
		if (IsValidPtr(local.player)) {
			if (g_config->aimbot.trigger.enabled) {
				CWeaponManager* wep = local.player->weapon();
				if (IsValidPtr(wep)) {
					if (IsValidPtr(wep->_WeaponInfo)) {
						auto str = wep->_WeaponInfo->GetSzWeaponName();
						string lstr(str, str + strlen(str));
						if (lstr == "UNARMED") return;

						for (pair<CObject*, DataPed>& entity : game::ped_list) {
							CObject* ped = entity.first;
							DataPed data = entity.second;
							if (IsValidPtr(ped)) {
								if (local.player == ped) continue;
								if (ped->HP <= 0) continue;
								DWORD hash = ped->ModelInfo()->GetHash();
								if (!game::isValidPlayer(hash, ped)) continue;

								native::type::any player = native::player::get_player_ped(-1);

								if (native::player::is_player_free_aiming_at_entity(player, pointer_to_handle((uintptr_t)ped))) {
									Vector3 pos = data.bones.HEAD;
									native::ped::set_ped_shoots_at_coord(player, pos.x, pos.y, pos.z, true);

								}
							}
						}
					}
				}
			}
		}
	}

	void tickall() {
		vector_aimbot_tick();
		silent_aimbot_tick();
		//trigger_tick();
	}

	DWORD WINAPI init(LPVOID) {
		cout << "aimbot init" << endl;

		if (gta_bullet_instance) {
			MH_CreateHook(gta_bullet_instance, on_bullet_instance, reinterpret_cast<void**>(&original_bullet_instance));
			if (MH_EnableHook(gta_bullet_instance) == MH_OK) {
				add_log("hook bullet instance ok");
				std::cout << "hooked bullet instance" << std::endl;
			}
		}


		while (Game.running) {
			//vector_aimbot_tick();
			//silent_aimbot_tick();
			//trigger_tick();

			std::this_thread::sleep_for(1ms);
		}
		cout << "exit aimbot" << endl;
		return 0;
	}
}
