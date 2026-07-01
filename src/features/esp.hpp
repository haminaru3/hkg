#pragma once
#include "../imports.h"
#include "visuals/players.hpp"
#include "visuals/vehicles.hpp"
#include "visuals/objects.hpp"
#include "visuals/radar.hpp"

namespace esp {
	void render() {
		__try {
			const bool render_players =
				g_config->esp.players.enabled ||
				g_config->esp.players.skeleton ||
				g_config->esp.local.skeleton ||
				g_config->esp.admins.enabled ||
				g_config->esp.admins.skeleton;

			if (render_players) players_frame();
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
		}

		__try {
			if (g_config->esp.vehicles.enabled) vehicles_frame();
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
		}

		__try {
			if (g_config->esp.objects.enabled) objects_frame();
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
		}

		__try {
			if (g_config->esp.radar.enabled) radar_wnd();
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
		}
	}
}
