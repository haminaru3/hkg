#pragma once
#include "imports.h"
#include "database.hpp"
#include "protection/prot_defs.h"
#include "features/functions/clickwarp.hpp"

#define M_PI 3.14159265358979323846
#define M_PI_F   ((float)M_PI)

#define RAD2DEG(x)	( (float)(x) * (float)(180.f / M_PI_F) )
#define DEG2RAD(x)	( (float)(x) * (float)(M_PI_F / 180.f) )


namespace game {
	int menu_key = VK_END;
	int game_ticks = 0;
	float tick_ms = 0.f;

	DWORD freemode_f = 0x9C9EFFD8;
	DWORD freemode_m = 0x705E61F2;

	template <typename T>
	bool read_model_hash(T* entity, DWORD& hash) {
		hash = 0;
		if (!IsValidPtr(entity)) return false;

		CModelInfo* model = nullptr;
		__try {
			model = entity->ModelInfo();
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return false;
		}

		if (!IsValidPtr(model)) return false;

		__try {
			hash = model->GetHash();
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			hash = 0;
			return false;
		}

		return hash != 0;
	}

	//TODO: RESTORE SHIT
	bool isValidPlayer(DWORD hash, CObject* player) {
		if (!IsValidPtr(player)) return false;
		return hash == freemode_f || hash == freemode_m;
	}

	bool safe_admin_state(CObject* ped, int32_t ped_handle) {
		if (!IsValidPtr(ped) || ped_handle == 0) return false;

		__try {
			return ped->GetAlpha() < 10.f
				&& native::entity::_get_entity_collison_disabled(ped_handle)
				&& native::player::get_player_invincible(ped_handle);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return false;
		}
	}

	bool safe_weapon_hash(CObject* ped, DWORD& weapon_hash) {
		weapon_hash = 0;
		if (!IsValidPtr(ped)) return false;

		__try {
			CWeaponManager* wep = ped->weapon();
			if (!IsValidPtr(wep) || !IsValidPtr(wep->_WeaponInfo)) return false;

			weapon_hash = wep->_WeaponInfo->GetWeaponHash();
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			weapon_hash = 0;
			return false;
		}

		return weapon_hash != 0;
	}

	bool safe_is_ped_in_vehicle(int32_t ped_handle, int32_t vehicle_handle) {
		if (ped_handle == 0 || vehicle_handle == 0) return false;

		__try {
			return native::ped::is_ped_in_vehicle(ped_handle, vehicle_handle, false);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return false;
		}
	}

	bool is_ped_visible(CObject* this_ped) {
		if (IsValidPtr(this_ped)) {
			if (!IsValidPtr(_START_SHAPE_TEST_CAPSULE) || !IsValidPtr(_GET_RAYCAST_RESULT) || !IsValidPtr(local.player)) {
				return false;
			}

			ImVec2 screen;
			if (!WorldToScreen2(this_ped->fPosition, &screen) && !WorldToScreen(this_ped->fPosition, &screen)) return false;

			PVector3 EmtVecMap(0, 0, 0);
			RawRaycastResult RaycastResultMap;
			PVector3 HitCordMap(0, 0, 0);

			auto ped_hit_pos = GetBonePosition(this_ped, HEAD);
			auto lp_ray_start_pos = GetBonePosition(local.player, RIGHT_HAND);
			if (!is_valid_world_pos(ped_hit_pos) || !is_valid_world_pos(lp_ray_start_pos)) return false;

			DWORD flags = IntersectMap | IntersectVegetation | IntersectVehicles | IntersectObjects;

			DWORD Raycast = _START_SHAPE_TEST_CAPSULE(PVector3(lp_ray_start_pos.x, lp_ray_start_pos.y, lp_ray_start_pos.z),
				PVector3(ped_hit_pos.x, ped_hit_pos.y, ped_hit_pos.z),
				0.0f, (IntersectOptions)flags, (DWORD)0, 7);

			if (Raycast) {
				_GET_RAYCAST_RESULT(Raycast, &RaycastResultMap.DidHitAnything, &HitCordMap, &EmtVecMap, &RaycastResultMap.HitEntity);

				if (RaycastResultMap.DidHitAnything) {
					return false;
				}
				else {
					return true;
				}
			}
		}
		return false;
	}

	bool is_ped_visible_safe(CObject* this_ped) {
		__try {
			return is_ped_visible(this_ped);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return false;
		}
	}

	void setFOV(float fov) {
		CPlayerAngles* cam = Game.getCam();
		if(IsValidPtr(cam)) {
			cam->m_cam_data->Fov_Zoom = fov;
		}
	}

	vector<pair<CObject*, DataPed>> ped_list;
	vector<CObject*> object_list;
	vector<pair<CVehicle*, DataVehicle>> vehicle_list;

	void update_vehicles() {
		if (!IsValidPtr(Game.ReplayInterface) || !IsValidPtr(Game.ReplayInterface->vehicle_interface) || !IsValidPtr(local.player)) {
			return;
		}

		vector<pair<CVehicle*, DataVehicle>> new_vehicle_list;
		new_vehicle_list.reserve(Game.ReplayInterface->vehicle_interface->max_vehicles); // Предвыделяем память

		for (int i = 0; i < Game.ReplayInterface->vehicle_interface->max_vehicles; i++) {
			CVehicle* veh = reinterpret_cast<CVehicle*>(Game.ReplayInterface->vehicle_interface->get_vehicle(i));

			if (!IsValidPtr(veh)) {
				continue;
			}

			DataVehicle this_data(false, false);
			if (g_config->esp.vehicles.lock_state || g_config->esp.vehicles.engine_state) {
				int32_t veh_h = pointer_to_handle_safe((uintptr_t)veh);
				if (veh_h == 0) {
					continue;
				}
				if (!native::entity::does_entity_exist(veh_h)) {
					continue;
				}

				if (g_config->esp.vehicles.lock_state) {
					this_data.is_locked = native::vehicle::get_vehicle_door_lock_status(veh_h) != 1;
				}

				if (g_config->esp.vehicles.engine_state) {
					this_data.is_engine_started = native::vehicle::get_is_vehicle_engine_running(veh_h);
				}
			}

			new_vehicle_list.push_back({ veh, this_data });
		}

		std::swap(vehicle_list, new_vehicle_list);
	}


	void update_objects() {
		if (IsValidPtr(Game.ReplayInterface) && IsValidPtr(Game.ReplayInterface->object_interface)) {
			if (IsValidPtr(local.player)) {
				const auto& object_findlist = g_config->esp.objects.tech.objects_list;
				vector<CObject*> new_obj_list;

				Vector3 player_pos = local.player->fPosition;

				for (int i = 0; i < Game.ReplayInterface->object_interface->iMaxObjects; i++) {
					CObject* obj = reinterpret_cast<CObject*>(Game.ReplayInterface->object_interface->get_object(i));
					if (!IsValidPtr(obj)) continue;

					float dist = get_distance(player_pos, obj->fPosition);
					if (dist > g_config->esp.objects.max_dist) continue;

					DWORD hash = 0;
					const bool has_hash = read_model_hash(obj, hash);

					if (g_config->esp.objects.enabled || (has_hash && object_findlist.find(hash) != object_findlist.end())) {
						new_obj_list.push_back(obj);
					}
				}

				object_list = std::move(new_obj_list);
			}
		}
	}



	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;
	/*
	void update_peds() {
		if(IsValidPtr(Game.ReplayInterface) && IsValidPtr(Game.ReplayInterface->ped_interface)) {
			if(IsValidPtr(local.player)) {
				vector<pair<CObject*, DataPed>> new_list;
				for(int i = 0; i < Game.ReplayInterface->ped_interface->max_peds; i++) {
					CObject* ped = reinterpret_cast<CObject*>(Game.ReplayInterface->ped_interface->get_ped(i));
					if(IsValidPtr(ped)) {
						DataPed PedData;
						auto ped_handle = pointer_to_handle((uintptr_t)ped);
						//bool ped_gm = native::player::get_player_invincible(ped_handle);

						if (!g_config->esp.local.skeleton && local.player == ped) continue; // local ignore

						DWORD hash = ped->ModelInfo()->GetHash(); // ped check
						if(!game::isValidPlayer(hash, ped)) continue;

						if (g_config->esp.admins.enabled) { // admin check
							if (ped->GetAlpha() < 10.f && native::entity::_get_entity_collison_disabled(ped_handle)) {
								PedData.isAdmin = true;
							}
							else {
								PedData.isAdmin = false;
							}
						}
							
							
						if (g_config->esp.players.eqp_weapon) {
							CWeaponManager* wep = ped->weapon();

							if (IsValidPtr(wep)) {
								if (IsValidPtr(wep->_WeaponInfo)) {
									auto hash = wep->_WeaponInfo->GetWeaponHash();
									if (hash != NULL) {
										PedData.weapon = get_weapon_name(hash);
									}
								}
							}
						}

						//if (g_config->esp.players.health_text && (g_config->esp.players.health_text_style == 0)) {
						//	static char hp_buf[4] = "";
						//	sprintf(hp_buf, "%d", (int)(ped->HP - 100.f));
						//	PedData.flags.push_back(esp_flag{ hp_buf, g_config->esp.players.health_text_col, 14 });
						//}
						//PedData.flags.push_back(esp_flag{ "TESTING", {255,0,0,255}, 14 });
						
						PedData.index = i;
						PedData.visible = visible_peds[i] || false;
						

						//Fetch Group
						string group_name = "\0";

						if(is_any_aimbot_enabled || g_config->esp.players.skeleton || g_config->esp.local.skeleton || g_config->esp.admins.skeleton) {
							ImVec2 screen;

							if(WorldToScreen(ped->fPosition, &screen)) {
								if(isW2SValid(screen)) {
									PedData.bones.HEAD = GetBonePosition(ped, HEAD);
									PedData.bones.NECK = GetBonePosition(ped, NECK);

									PedData.bones.RIGHT_CLAVICLE = GetBonePosition(ped, RIGHT_CLAVICLE);
									PedData.bones.LEFT_CLAVICLE = GetBonePosition(ped, LEFT_CLAVICLE);

									PedData.bones.RIGHT_UPPER_ARM = GetBonePosition(ped, RIGHT_UPPER_ARM);
									PedData.bones.RIGHT_FOREARM = GetBonePosition(ped, RIGHT_FOREARM);
									PedData.bones.RIGHT_HAND = GetBonePosition(ped, RIGHT_HAND);


									PedData.bones.LEFT_UPPER_ARM = GetBonePosition(ped, LEFT_UPPER_ARM);
									PedData.bones.LEFT_FOREARM = GetBonePosition(ped, LEFT_FOREARM);
									PedData.bones.LEFT_HAND = GetBonePosition(ped, LEFT_HAND);


									PedData.bones.SPINE3 = GetBonePosition(ped, SPINE3);
									PedData.bones.SPINE2 = GetBonePosition(ped, SPINE2);
									PedData.bones.SPINE1 = GetBonePosition(ped, SPINE1);
									PedData.bones.SPINE_ROOT = GetBonePosition(ped, SPINE_ROOT);


									PedData.bones.RIGHT_THIGH = GetBonePosition(ped, RIGHT_THIGH);
									PedData.bones.RIGHT_CALF = GetBonePosition(ped, RIGHT_CALF);
									PedData.bones.RIGHT_FOOT = GetBonePosition(ped, RIGHT_FOOT);


									PedData.bones.LEFT_THIGH = GetBonePosition(ped, LEFT_THIGH);
									PedData.bones.LEFT_CALF = GetBonePosition(ped, LEFT_CALF);
									PedData.bones.LEFT_FOOT = GetBonePosition(ped, LEFT_FOOT);

									float dist = get_distance(local.player->fPosition, ped->fPosition);

									if (g_config->esp.players.skel_hands && (dist < 30.f)) {
										PedData.bones.LEFT_FINGER0 =  GetBonePosition(ped, 0x67F2);
										PedData.bones.LEFT_FINGER1 =  GetBonePosition(ped, 0x0FF9);
										PedData.bones.LEFT_FINGER2 =  GetBonePosition(ped, 0x0FFA);

										PedData.bones.LEFT_FINGER10 =  GetBonePosition(ped, 0x67F3);
										PedData.bones.LEFT_FINGER11 =  GetBonePosition(ped, 0x1049);
										PedData.bones.LEFT_FINGER12 =  GetBonePosition(ped, 0x104A);

										PedData.bones.LEFT_FINGER20 =  GetBonePosition(ped, 0x67F4);
										PedData.bones.LEFT_FINGER21 =  GetBonePosition(ped, 0x1059);
										PedData.bones.LEFT_FINGER22 =  GetBonePosition(ped, 0x105A);

										PedData.bones.LEFT_FINGER30 =  GetBonePosition(ped, 0x67F5);
										PedData.bones.LEFT_FINGER31 =  GetBonePosition(ped, 0x1029);
										PedData.bones.LEFT_FINGER32 =  GetBonePosition(ped, 0x102A);

										PedData.bones.LEFT_FINGER40 =  GetBonePosition(ped, 0x67F6);
										PedData.bones.LEFT_FINGER41 =  GetBonePosition(ped, 0x1039);
										PedData.bones.LEFT_FINGER42 =  GetBonePosition(ped, 0x103A);

										PedData.bones.RIGHT_FINGER0 =  GetBonePosition(ped, 0xE5F2);
										PedData.bones.RIGHT_FINGER1 =  GetBonePosition(ped, 0xFA10);
										PedData.bones.RIGHT_FINGER2 =  GetBonePosition(ped, 0xFA11);

										PedData.bones.RIGHT_FINGER10 =  GetBonePosition(ped, 0xE5F3);
										PedData.bones.RIGHT_FINGER11 =  GetBonePosition(ped, 0xFA60);
										PedData.bones.RIGHT_FINGER12 =  GetBonePosition(ped, 0xFA61);

										PedData.bones.RIGHT_FINGER20 =  GetBonePosition(ped, 0xE5F4);
										PedData.bones.RIGHT_FINGER21 =  GetBonePosition(ped, 0xFA70);
										PedData.bones.RIGHT_FINGER22 =  GetBonePosition(ped, 0xFA71);

										PedData.bones.RIGHT_FINGER30 =  GetBonePosition(ped, 0xE5F5);
										PedData.bones.RIGHT_FINGER31 =  GetBonePosition(ped, 0xFA40);
										PedData.bones.RIGHT_FINGER32 =  GetBonePosition(ped, 0xFA41);

										PedData.bones.RIGHT_FINGER40 =  GetBonePosition(ped, 0xE5F6);
										PedData.bones.RIGHT_FINGER41 =  GetBonePosition(ped, 0xFA50);
										PedData.bones.RIGHT_FINGER42 =  GetBonePosition(ped, 0xFA51);
									}
								}
							}
						}

						new_list.push_back(pair(ped, PedData));
					}
				}
				ped_list.clear();
				ped_list = new_list;
			}
		}

	}
	*/

	bool has_usable_bone_layout(const PlayerBones& bones) {
		if (!is_valid_world_pos(bones.HEAD) || !is_valid_world_pos(bones.NECK) || !is_valid_world_pos(bones.SPINE_ROOT)) {
			return false;
		}

		const float head_to_root = get_distance(bones.HEAD, bones.SPINE_ROOT);
		const float left_leg = get_distance(bones.SPINE_ROOT, bones.LEFT_FOOT);
		const float right_leg = get_distance(bones.SPINE_ROOT, bones.RIGHT_FOOT);
		return std::isfinite(head_to_root) && std::isfinite(left_leg) && std::isfinite(right_leg) &&
			head_to_root > 0.15f && head_to_root < 2.5f &&
			(left_leg > 0.15f || right_leg > 0.15f);
	}

	void fill_fallback_bones(CObject* ped, DataPed& PedData) {
		if (!IsValidPtr(ped) || !is_valid_world_pos(ped->fPosition)) return;

		const Vector3 base = ped->fPosition;
		PedData.bones.HEAD = Vector3(base.x, base.y, base.z + 0.82f);
		PedData.bones.NECK = Vector3(base.x, base.y, base.z + 0.62f);
		PedData.bones.SPINE3 = Vector3(base.x, base.y, base.z + 0.42f);
		PedData.bones.SPINE2 = Vector3(base.x, base.y, base.z + 0.22f);
		PedData.bones.SPINE1 = Vector3(base.x, base.y, base.z + 0.02f);
		PedData.bones.SPINE_ROOT = Vector3(base.x, base.y, base.z - 0.22f);

		PedData.bones.RIGHT_CLAVICLE = Vector3(base.x + 0.18f, base.y, base.z + 0.55f);
		PedData.bones.RIGHT_UPPER_ARM = Vector3(base.x + 0.34f, base.y, base.z + 0.32f);
		PedData.bones.RIGHT_FOREARM = Vector3(base.x + 0.38f, base.y, base.z + 0.04f);
		PedData.bones.RIGHT_HAND = Vector3(base.x + 0.34f, base.y, base.z - 0.18f);

		PedData.bones.LEFT_CLAVICLE = Vector3(base.x - 0.18f, base.y, base.z + 0.55f);
		PedData.bones.LEFT_UPPER_ARM = Vector3(base.x - 0.34f, base.y, base.z + 0.32f);
		PedData.bones.LEFT_FOREARM = Vector3(base.x - 0.38f, base.y, base.z + 0.04f);
		PedData.bones.LEFT_HAND = Vector3(base.x - 0.34f, base.y, base.z - 0.18f);

		PedData.bones.RIGHT_THIGH = Vector3(base.x + 0.14f, base.y, base.z - 0.50f);
		PedData.bones.RIGHT_CALF = Vector3(base.x + 0.16f, base.y, base.z - 0.78f);
		PedData.bones.RIGHT_FOOT = Vector3(base.x + 0.16f, base.y + 0.08f, base.z - 1.02f);

		PedData.bones.LEFT_THIGH = Vector3(base.x - 0.14f, base.y, base.z - 0.50f);
		PedData.bones.LEFT_CALF = Vector3(base.x - 0.16f, base.y, base.z - 0.78f);
		PedData.bones.LEFT_FOOT = Vector3(base.x - 0.16f, base.y + 0.08f, base.z - 1.02f);
	}

	void collectBoneData(CObject* ped, DataPed& PedData) {
		if (!IsValidPtr(ped) || !IsValidPtr(local.player)) return;
		PedData.bones.HEAD = GetBonePosition(ped, HEAD);
		PedData.bones.NECK = GetBonePosition(ped, NECK);
		PedData.bones.PELVIS = GetBonePosition(ped, PELVIS);
		PedData.bones.SPINE0 = GetBonePosition(ped, SPINE0);
		PedData.bones.SPINE1 = GetBonePosition(ped, SPINE1);
		PedData.bones.SPINE2 = GetBonePosition(ped, SPINE2);
		PedData.bones.SPINE3 = GetBonePosition(ped, SPINE3);
		PedData.bones.SPINE_ROOT = GetBonePosition(ped, SPINE_ROOT);

		PedData.bones.RIGHT_CLAVICLE = GetBonePosition(ped, RIGHT_CLAVICLE);
		PedData.bones.LEFT_CLAVICLE = GetBonePosition(ped, LEFT_CLAVICLE);

		PedData.bones.RIGHT_UPPER_ARM = GetBonePosition(ped, RIGHT_UPPER_ARM);
		PedData.bones.RIGHT_FOREARM = GetBonePosition(ped, RIGHT_FOREARM);
		PedData.bones.RIGHT_HAND = GetBonePosition(ped, RIGHT_HAND);

		PedData.bones.LEFT_UPPER_ARM = GetBonePosition(ped, LEFT_UPPER_ARM);
		PedData.bones.LEFT_FOREARM = GetBonePosition(ped, LEFT_FOREARM);
		PedData.bones.LEFT_HAND = GetBonePosition(ped, LEFT_HAND);

		PedData.bones.RIGHT_THIGH = GetBonePosition(ped, RIGHT_THIGH);
		PedData.bones.RIGHT_CALF = GetBonePosition(ped, RIGHT_CALF);
		PedData.bones.RIGHT_FOOT = GetBonePosition(ped, RIGHT_FOOT);

		PedData.bones.LEFT_THIGH = GetBonePosition(ped, LEFT_THIGH);
		PedData.bones.LEFT_CALF = GetBonePosition(ped, LEFT_CALF);
		PedData.bones.LEFT_FOOT = GetBonePosition(ped, LEFT_FOOT);

		// hands if required and distance is bigger than 30 попугаев
		float dist = get_distance(local.player->fPosition, ped->fPosition);
		if (g_config->esp.players.skel_hands && (dist < 30.f)) {
			PedData.bones.LEFT_FINGER0 = GetBonePosition(ped, 0x67F2);
			PedData.bones.LEFT_FINGER1 = GetBonePosition(ped, 0x0FF9);
			PedData.bones.LEFT_FINGER2 = GetBonePosition(ped, 0x0FFA);
			PedData.bones.LEFT_FINGER10 = GetBonePosition(ped, 0x67F3);
			PedData.bones.LEFT_FINGER11 = GetBonePosition(ped, 0x1049);
			PedData.bones.LEFT_FINGER12 = GetBonePosition(ped, 0x104A);
			PedData.bones.LEFT_FINGER20 = GetBonePosition(ped, 0x67F4);
			PedData.bones.LEFT_FINGER21 = GetBonePosition(ped, 0x1059);
			PedData.bones.LEFT_FINGER22 = GetBonePosition(ped, 0x105A);
			PedData.bones.LEFT_FINGER30 = GetBonePosition(ped, 0x67F5);
			PedData.bones.LEFT_FINGER31 = GetBonePosition(ped, 0x1029);
			PedData.bones.LEFT_FINGER32 = GetBonePosition(ped, 0x102A);
			PedData.bones.LEFT_FINGER40 = GetBonePosition(ped, 0x67F6);
			PedData.bones.LEFT_FINGER41 = GetBonePosition(ped, 0x1039);
			PedData.bones.LEFT_FINGER42 = GetBonePosition(ped, 0x103A);

			PedData.bones.RIGHT_FINGER0 = GetBonePosition(ped, 0xE5F2);
			PedData.bones.RIGHT_FINGER1 = GetBonePosition(ped, 0xFA10);
			PedData.bones.RIGHT_FINGER2 = GetBonePosition(ped, 0xFA11);
			PedData.bones.RIGHT_FINGER10 = GetBonePosition(ped, 0xE5F3);
			PedData.bones.RIGHT_FINGER11 = GetBonePosition(ped, 0xFA60);
			PedData.bones.RIGHT_FINGER12 = GetBonePosition(ped, 0xFA61);
			PedData.bones.RIGHT_FINGER20 = GetBonePosition(ped, 0xE5F4);
			PedData.bones.RIGHT_FINGER21 = GetBonePosition(ped, 0xFA70);
			PedData.bones.RIGHT_FINGER22 = GetBonePosition(ped, 0xFA71);
			PedData.bones.RIGHT_FINGER30 = GetBonePosition(ped, 0xE5F5);
			PedData.bones.RIGHT_FINGER31 = GetBonePosition(ped, 0xFA40);
			PedData.bones.RIGHT_FINGER32 = GetBonePosition(ped, 0xFA41);
			PedData.bones.RIGHT_FINGER40 = GetBonePosition(ped, 0xE5F6);
			PedData.bones.RIGHT_FINGER41 = GetBonePosition(ped, 0xFA50);
			PedData.bones.RIGHT_FINGER42 = GetBonePosition(ped, 0xFA51);
		}

	}

	void update_peds() {
		if (!IsValidPtr(Game.ReplayInterface) || !IsValidPtr(Game.ReplayInterface->ped_interface) || !IsValidPtr(local.player)) {
			return;
			//Some ptrs are invalid. bailing out... prob the game was not fully loaded
		}

		vector<pair<CObject*, DataPed>> new_list;
		new_list.reserve(Game.ReplayInterface->ped_interface->max_peds); // preallocate space (more efficient ram usage for Intel Xeon users (ddr3-1333mt 1488cl) )
		//smh not working...

		for (int i = 0; i < Game.ReplayInterface->ped_interface->max_peds; i++) {
			CObject* ped = reinterpret_cast<CObject*>(Game.ReplayInterface->ped_interface->get_ped(i));
			if (!IsValidPtr(ped)) continue;

			if (ped->HP <= 0 || (local.player == ped && !g_config->esp.local.skeleton)) {
				continue;
				//health and local check
			}

			DataPed PedData;
			auto ped_handle = pointer_to_handle_safe((uintptr_t)ped);

			// Admin check
			if (g_config->esp.admins.enabled && safe_admin_state(ped, ped_handle)) {
				PedData.isAdmin = true;
			}

			// Weapon check
			if (g_config->esp.players.eqp_weapon) {
				DWORD weapon_hash = 0;
				if (safe_weapon_hash(ped, weapon_hash)) {
					PedData.weapon = weapon_hash;
					// TODO: u should pass a hash into the peddata cuz it may be reused in a form of hash, not a string. Conversion to string must be performed in esp.
				}
			}

			PedData.index = i;
			PedData.visible = false;
			if (g_config->esp.players.skeleton || is_any_aimbot_enabled) {
				PedData.visible = is_ped_visible_safe(ped);
			}

			if (is_any_aimbot_enabled || g_config->esp.players.enabled || g_config->esp.admins.enabled ||
				g_config->esp.players.skeleton || g_config->esp.local.skeleton || g_config->esp.admins.skeleton) {
				// if esp is enabled
				ImVec2 screen;
				if (WorldToScreen2(ped->fPosition, &screen) || WorldToScreen(ped->fPosition, &screen)) {
					// Collect bone data
					collectBoneData(ped, PedData);
				}
			}

			new_list.push_back(pair<CObject*, DataPed>(ped, PedData));
		}

		ped_list.swap(new_list);
	}

	
	void tick() {
		if(IsValidPtr(local.player)) {
			tickbase::new_tick();
			//GAME TICK!!

			if (g_config->esp.players.skeleton || is_any_aimbot_enabled) {
				//update_visiblity();
				// бабуля отсоси
			}

			if (g_config->esp.vehicles.enabled) {
				__try {
					update_vehicles();
				}
				__except (EXCEPTION_EXECUTE_HANDLER) {
					vehicle_list.clear();
				}
			}

			hacks::ticklooped();

			hacks::singleuse::update();

			/*
			:skull: :skull: :skull: :skull:
			:skull: :skull: :skull: :skull:
			if((GetAsyncKeyState(g_config->keybinds.aimbot.silent_aim_key) & 0x8000) && g_config->aimbot.silentaim.enabled) {
				aimbot::do_silent_aim();
			}
			if ((GetAsyncKeyState(g_config->keybinds.aimbot.damager_key) & 0x8000) && g_config->aimbot.damager.enabled) {
				aimbot::do_damager();
			}*/
			
			dbg::since_last_tick.updateArg(tickbase::get_since_last_tick());
			dbg::current_tick.updateArg(tickbase::ticks.size());
			return;
		}
	}
	

	void beginframe_tick() {
		
	}

	bool fov_was_setted = false;
	void render() {
		if(!Game.running) return;
		if (cfg_manager::is_io_active()) return;
		const bool clean_recording = g_config->misc.clean_recording_mode && !g_config->controls.menu_state;

		if(Game.World != nullptr) {
			local.player = Game.World->getLocalPlayer();

			if(local.player) {
				if (is_any_esp_enabled || is_any_aimbot_enabled) {
					__try {
						update_peds();
					}
					__except (EXCEPTION_EXECUTE_HANDLER) {
						ped_list.clear();
					}
				}

				if (g_config->esp.objects.enabled) {
					__try {
						update_objects();
					}
					__except (EXCEPTION_EXECUTE_HANDLER) {
						object_list.clear();
					}
				}

				if (!clean_recording) {
					__try { esp::render(); }
					__except (EXCEPTION_EXECUTE_HANDLER) {}
				}

				__try { aimbot::tickall(); }
				__except (EXCEPTION_EXECUTE_HANDLER) {}

				__try {
					const auto local_handle = pointer_to_handle_safe((uintptr_t)local.player);
					const auto vehicle_handle = IsValidPtr(local.player->vehicle()) ? pointer_to_handle_safe((uintptr_t)local.player->vehicle()) : 0;
					Game.is_lp_in_vehicle = safe_is_ped_in_vehicle(local_handle, vehicle_handle);
				}
				__except (EXCEPTION_EXECUTE_HANDLER) {}
				// idk tf is this but works ok, dont killmeplssss
			}

			if (!clean_recording && g_config->world.clickwarp.enabled) {
				functions::clickwarp::clickwarp();
			}

			if (g_config->world.visuals.cam_fov && !fov_was_setted) {
				game::setFOV(g_config->world.visuals.cam_fov_v);
				fov_was_setted = true;
			}
			if (!g_config->world.visuals.cam_fov && fov_was_setted) {
				game::setFOV(50);
				fov_was_setted = false;
			}

#ifdef _DEBUG
			if (!clean_recording && g_config->misc.debug.dbg_info_render) dbg::render_all();
#endif
		}
	}
}
