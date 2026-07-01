#pragma once

#include "../imports.h"

namespace hacks {
	namespace singleuse {
		///  <summary>
		///  Sets the health of the player.
		///  </summary>
		///  <param name="health">The amount of health you want</param>
		void sethealth(float health) {
			local.player->HP = health + 100.f;
		}


		///  <summary>
		///  Adds health to the player's HP.
		///  </summary>
		///  <param name="health">The amount of health to</param>
		void addhealth(float health) {
			local.player->HP = local.player->HP + health;
		}

		///  <summary>
		///  Sets the armor level.
		///  </summary>
		///  <param name="armor">The amount of armor</param>
		void setarmor(float armor) {
			local.player->SetArmor(armor);
		}


		///  <summary>
		///  Adds armor to player's armor.
		///  </summary>
		///  <param name="armor">Armor to add to</param>
		void addarmor(float armor) {
			local.player->SetArmor(local.player->GetArmor() + armor);
		}

		// ADD A CHECK FOR ANTI-ANTI-CHEAT (HOOKING GET GROUND Z)
		void tp_to_wp() {
			Vector3 pos;
			tick::thread_invoker::queue([] {
				native::type::any entity;
				bool is_in_vehicle = native::ped::is_ped_in_any_vehicle(native::player::player_ped_id(), false);
				if (is_in_vehicle) {
					entity = native::ped::get_vehicle_ped_is_in(native::player::get_player_ped(-1), false);
				}
				else {
					entity = native::player::get_player_ped(-1);
				}

				native::type::any blip = native::ui::get_first_blip_info_id(8);
				if (native::ui::does_blip_exist(blip)) {
					auto tpos = native::ui::get_blip_coords(blip);
					float z = 0.0f;
					native::entity::set_entity_coords_no_offset(entity, tpos.x, tpos.y, tpos.z, true, true, true);

					if (native::gameplay::get_ground_z_for_3d_coord(tpos.x, tpos.y, tpos.z + 1000, &z, false)) {
						native::entity::set_entity_coords_no_offset(entity, tpos.x, tpos.y, z, true, true, true);
					}
				}
				});
		}
		

		///  <summary>
		///  Instakill player
		///  </summary>
		void suicide() {
			sethealth(-100); //idk
		}


		///  <summary>
		///  Clear PED visible damage
		///  </summary>
		void clear_visible_damage() {
			native::ped::reset_ped_visible_damage(native::player::get_player_ped(-1));
		}


		///  <summary>
		///  self explanatory
		///  </summary>
		void clear_ped_tasks() {
			tick::thread_invoker::queue([] { native::brain::clear_ped_tasks_immediately(pointer_to_handle((uintptr_t)local.player)); });		}


		///  <summary>
		///  Flip player vehicle
		///  </summary>
		void flip_vehicle() {
			bool is_in_vehicle = native::ped::is_ped_in_any_vehicle(native::player::player_ped_id(), false);
			if (is_in_vehicle) {
				auto entity = native::ped::get_vehicle_ped_is_in(native::player::get_player_ped(-1), false);
				float heading = native::cam::get_gameplay_cam_relative_heading() + native::entity::get_entity_heading(entity);

				native::entity::set_entity_rotation(entity, 0, 0, heading, 0, true);
			}
		}

		void start_autopilot() {
			if (Game.is_lp_in_vehicle) {
				native::type::any blip = native::ui::get_first_blip_info_id(8);
				if (native::ui::does_blip_exist(blip)) {
					auto vehicle = native::ped::get_vehicle_ped_is_in(native::player::get_player_ped(-1), false);
					PVector3 blip_coords = native::ui::get_blip_coords(blip);
					//native::brain::task_vehicle_drive_to_coord(
					//	native::player::player_ped_id(),				// ped
					//	vehicle,										// veh
					//	blip_coords.x, blip_coords.y, blip_coords.z,	// end coords
					//	60.0f,											// speed
					//	1,
					//	native::entity::get_entity_model(vehicle),
					//	786603,											// drive style flag
					//	5.0f,											// min dist until end coords
					//	1.0f
					//);
				}
				else
					g_singleuse->autopilot_inuse = false;
			}
			else
				g_singleuse->autopilot_inuse = false;
		}

		void stop_autopilot() {
			//do shit
			if (Game.is_lp_in_vehicle && g_singleuse->autopilot_inuse) {
				auto vehicle = native::ped::get_vehicle_ped_is_in(native::player::get_player_ped(-1), false);
				native::brain::task_vehicle_temp_action(native::player::player_ped_id(), vehicle, 1, 1000);
			}
		}


		///  <summary>
		///  Fix vehicle including:
		///  - engine health
		///  - body health
		///  - petrol tank health
		///  - deformations
		///  - full fix (idk)
		///  </summary>

		void fix_vehicle() {
			int player_ped = native::player::player_ped_id();
			int vehicle = native::ped::get_vehicle_ped_is_in(player_ped, false);
			if (native::entity::does_entity_exist(vehicle))
			{
				native::vehicle::set_vehicle_engine_health(vehicle, 1000.0f);
				native::vehicle::set_vehicle_body_health(vehicle, 1000.0f);
				native::vehicle::set_vehicle_deformation_fixed(vehicle);
				native::vehicle::set_vehicle_fixed(vehicle);
				native::vehicle::set_vehicle_petrol_tank_health(vehicle, 1000.0f);
			}
		}


		///  <summary>
		///  Fix vehicle deformation
		///  </summary>
		void fix_vehicle_deformations() {
			int player_ped = native::player::player_ped_id();
			int vehicle = native::ped::get_vehicle_ped_is_in(player_ped, false);
			if (native::entity::does_entity_exist(vehicle))
			{
				native::vehicle::set_vehicle_deformation_fixed(vehicle);
			}
		}


		///  <summary>
		///  Clean all dirt from vehicle
		///  </summary>
		void clean_vehicle() {
			int player_ped = native::player::player_ped_id();
			int vehicle = native::ped::get_vehicle_ped_is_in(player_ped, false);
			if (native::entity::does_entity_exist(vehicle))
			{
				native::vehicle::set_vehicle_dirt_level(vehicle, 0.0f);
			}
		}


		///  <summary>
		///  self explanatory
		///  </summary>

		void kill_vehicle_engine() {
			int player_ped = native::player::player_ped_id();
			int vehicle = native::ped::get_vehicle_ped_is_in(player_ped, false);
			if (native::entity::does_entity_exist(vehicle))
			{
				native::vehicle::set_vehicle_engine_health(vehicle, -4000.0f);
			}
		}


		///  <summary>
		///  self explanatory
		///  </summary>
		void kill_vehicle_gastank() {
			int player_ped = native::player::player_ped_id();
			int vehicle = native::ped::get_vehicle_ped_is_in(player_ped, false);
			if (native::entity::does_entity_exist(vehicle))
			{																		// Begins leaking gas at around 650 health
				native::vehicle::set_vehicle_petrol_tank_health(vehicle, -999.0f);  // -999.90002441406 appears to be minimum health, although nothing special occurs
			}
		}

		///  <summary>
		///  self explanatory
		///  </summary>
		void pop_vehicle_tyres() {
			int player_ped = native::player::player_ped_id();
			int vehicle = native::ped::get_vehicle_ped_is_in(player_ped, false);
			if (native::entity::does_entity_exist(vehicle))
			{
				for (int i = 0; i < 8; i++) {
					native::vehicle::set_vehicle_tyre_burst(vehicle, i, true, 1000.0f);
				}
			}
		}

		///  <summary>
		///  set_vehicle_engine_on + set_vehicle_undriveable = false
		///  </summary>
		void start_veh_engine() {
			if (Game.is_lp_in_vehicle) {
				auto lp_veh = pointer_to_handle((uintptr_t)local.player->vehicle());
				native::vehicle::set_vehicle_undriveable(lp_veh, false);
				native::vehicle::set_vehicle_engine_on(lp_veh, true, true, false);
			}
		}

		void update() {
			if (g_singleuse->sethealth) {
				sethealth(100);
				g_singleuse->sethealth = false;
			}

			if (g_singleuse->setarmor) {
				setarmor(100);
				g_singleuse->setarmor = false;
			}
			
			if (g_singleuse->tptowp) {
				tp_to_wp();
				g_singleuse->tptowp = false;
			}

			if (g_singleuse->suicide) {
				suicide();
				g_singleuse->suicide = false;
			}

			if (g_singleuse->clearvisibledamage) {
				clear_visible_damage();
				g_singleuse->clearvisibledamage = false;
			}

			if (g_singleuse->clearpedtasks) {
				clear_ped_tasks();
				g_singleuse->clearpedtasks = false;
			}

			if (g_singleuse->flipvehicle) {
				flip_vehicle();
				g_singleuse->flipvehicle = false;
			}

			if (g_singleuse->startautopilot) {
				start_autopilot();
				g_singleuse->startautopilot = false;
			}

			if (g_singleuse->stopautopilot) {
				stop_autopilot();
				g_singleuse->stopautopilot = false, g_singleuse->autopilot_inuse = false;
			}

			if (g_singleuse->fixvehicle) {
				fix_vehicle();
				g_singleuse->fixvehicle = false;
			}

			if (g_singleuse->fixvehdeformations) {
				fix_vehicle_deformations();
				g_singleuse->fixvehdeformations = false;
			}

			if (g_singleuse->cleanvehicle) {
				clean_vehicle();
				g_singleuse->cleanvehicle = false;
			}

			if (g_singleuse->killvehengine) {
				kill_vehicle_engine();
				g_singleuse->killvehengine = false;
			}

			if (g_singleuse->killvehgastank) {
				kill_vehicle_gastank();
				g_singleuse->killvehgastank = false;
			}

			if (g_singleuse->popvehicletyres) {
				pop_vehicle_tyres();
				g_singleuse->popvehicletyres = false;
			}

			if (g_singleuse->startvehengine) {
				start_veh_engine();
				g_singleuse->startvehengine = false;
			}
			
			if (g_singleuse->unloadcheat) {
				Game.running = false;
				g_singleuse->unloadcheat = false;
			}

#ifdef _DEBUG
			if (g_singleuse->dumpentrypoints) {
				native::invoker::dump_natives();
				g_singleuse->dumpentrypoints = false;
			}
			
			if (g_singleuse->testthread) {
				rage::start_explode_veh();
				g_singleuse->testthread = false;
			}
#endif
		}

	}
}