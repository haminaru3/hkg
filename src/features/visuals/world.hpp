#pragma once
#include "../functions/func_utils.hpp"

namespace functions {
	namespace world {
		void timechanger() {
			static int orig_h, orig_m;
			static bool time_changed = false;
			if (g_config->world.visuals.timechanger) {
				if (!time_changed) {
					orig_h = native::time::get_clock_hours();
					orig_m = native::time::get_clock_minutes();
				}
				native::network::network_override_clock_time(g_config->world.visuals.time_hour, g_config->world.visuals.time_minute, 0);
				native::time::set_clock_time(g_config->world.visuals.time_hour, g_config->world.visuals.time_minute, 0);
				native::time::pause_clock(true);
				time_changed = true;
			} else if (time_changed) {
				native::network::network_clear_clock_time_override();
				native::time::set_clock_time(orig_h, orig_m, 0);
				native::time::pause_clock(false);
				time_changed = false;
			}
		}
		void weatherchanger() {
			static bool weather_changed = false; static int w_changed_index = 0;
			if (w_changed_index != g_config->world.visuals.weather_type && g_config->world.visuals.weatherchanger) {
				//if (cur_weather_hash() != weather_types_hashes[g_config->world.visuals.weather_type]) {
				utils::set_weather_by_index(g_config->world.visuals.weather_type);
				weather_changed = true; w_changed_index = g_config->world.visuals.weather_type;
				//}
			} if (weather_changed && !g_config->world.visuals.weatherchanger) { //-V646
				utils::set_weather_by_index(0);
				weather_changed = false;
			}
		}
	}
}