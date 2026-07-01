#pragma once
#include "../imports.h"

#include "functions/player/health.hpp"
#include "functions/player/misc.hpp"
#include "functions/player/movement.hpp"
#include "functions/player/physics.hpp"

#include "functions/vehicle/health.hpp"
#include "functions/vehicle/misc.hpp"
#include "functions/vehicle/movement.hpp"
#include "functions/vehicle/physics.hpp"

#include "functions/clickwarp.hpp"
#include "functions/noclip.hpp"
#include "functions/freecam.hpp"
#include "functions/weapons.hpp"

#include "visuals/world.hpp"

namespace hacks {
	void disable_noclip_after_fault() {
		__try {
			functions::noclip_disable();
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			cheat_base->funcs.noclip.state = false;
			cheat_base->funcs.noclip.boost = false;
		}

		g_config->player.movement.noclip.enabled = false;
		cheat_base->funcs.noclip.state = false;
		cheat_base->funcs.noclip.boost = false;
		add_log("noclip fault: disabled to keep client alive");
	}

	void destroy_freecam_after_fault() {
		__try {
			functions::freecam::destroy();
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
		}

		g_config->player.movement.freecam.enabled = false;
		cheat_base->funcs.freecam.state = false;
		cheat_base->funcs.freecam.teleport = false;
		add_log("freecam fault: disabled to keep client alive");
	}

	void noclip_tick_safe() {
		__try {
			if (g_config->player.movement.noclip.enabled) {
				if (cheat_base->funcs.noclip.state) functions::noclip();
				functions::noclip_switch();
			}
			else if (cheat_base->funcs.noclip.state) {
				functions::noclip_disable();
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			disable_noclip_after_fault();
		}
	}

	void freecam_tick_safe() {
		__try {
			if (g_config->player.movement.freecam.enabled) {
				if (cheat_base->funcs.freecam.state) functions::freecam::tick();
				functions::freecam::_switch();
			}
			else if (cheat_base->funcs.freecam.state) {
				cheat_base->funcs.freecam.state = false;
				functions::freecam::destroy();
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			destroy_freecam_after_fault();
		}
	}

	void player_tick() {

		{ // health.hpp
			functions::player::godmode();
		}

		{ // misc.hpp
			if (g_config->player.movement.player_mgmt.fallbreak && !Game.is_lp_in_vehicle) functions::player::fallbreak();
			if (g_config->player.movement.player_mgmt.skip_ladder && !Game.is_lp_in_vehicle) functions::player::skip_ladder();
		}

		{ // movement.hpp
			if (!Game.is_lp_in_vehicle) functions::player::inf_stamina();
			functions::player::sprintboost();
		}

		{ // physics.hpp
			functions::player::no_ragdoll();
			functions::player::no_collision();
		}
	}

	void vehicle_tick() {

		{ // health.hpp
			if (g_config->vehicles.health.gmode && Game.is_lp_in_vehicle) functions::vehicle::godmode();
			functions::vehicle::seatbelt();
			if (Game.is_lp_in_vehicle) functions::vehicle::bulletproof_tyres();
		}

		{ // misc.hpp
			if (g_config->vehicles.health.always_clean && Game.is_lp_in_vehicle) functions::vehicle::always_clean();
			if (g_config->vehicles.seatwarp.enabled && !Game.is_lp_in_vehicle) functions::vehicle::seat_warp();
			functions::vehicle::car_hijacking();
		}

		{ // movement.hpp
			if (g_config->vehicles.boost.enabled && Game.is_lp_in_vehicle) functions::vehicle::accel_boost();
			if (g_config->vehicles.instant_180 && Game.is_lp_in_vehicle) functions::vehicle::instant180();
		}

		{ // physics.hpp
			if ((g_config->vehicles.disable_collision || cheat_base->funcs.single.vehicle.coll_removed) && Game.is_lp_in_vehicle) functions::vehicle::no_collision();
		}

	}

	void misc_tick() {
		noclip_tick_safe();
		freecam_tick_safe();


		if (g_config->world.clickwarp.enabled) {
			if (GetAsyncKeyState(g_config->world.clickwarp.key) & 0x8000) {
				functions::clickwarp::get_coords_for_mark();
			}
		}

		functions::world::timechanger();
		functions::world::weatherchanger();
	}

	void ticklooped() {
		hacks::player_tick();
		hacks::vehicle_tick();
		hacks::misc_tick();
		functions::weapons::tick();
	}

	void tick() {
		if (cfg_manager::is_io_active()) return;
		if (IsValidPtr(local.player)) {
			//keybinds();
			hotkeys::keybind_runner();
		}
	}
	DWORD WINAPI init(LPVOID) {
		cout << "hacks init" << endl;
		add_log("hacks thread started");
		while (Game.running) {
			tick();
			std::this_thread::sleep_for(10ms);
		}
		cout << "exit hacks" << endl;
		return 0;
	}

}
