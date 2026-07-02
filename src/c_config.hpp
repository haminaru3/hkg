#pragma once

//#define NO_CONFIGS

#define CONFIG_DIR_PATH "C:\\Cortex"
#define CONFIGS_PATH "C:\\Cortex\\"

#include <iostream>
#include <Windows.h>
#include <Windows.h>
#include <iostream>
#include "../includes/base64.hpp"
#include <fstream>
#include <filesystem>
#include <map>
#include <algorithm>
#include <cctype>
#include <atomic>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include <fixed_containers/fixed_map.hpp>
#ifndef NO_CONFIGS
#include "glaze/glaze.hpp"
#endif

struct s_esptext_render_params {
	int font_id;
	float text_scale;
	int condition;
	bool outline;

	bool adaptive_scale_state;
	float adaptive_scale;
	float adsc_strength;
};

struct c_variables
{
private:
	enum gmode_types { autofill, constant };
	enum geometric_pos { left, right, top, bottom };

	enum geometric_shapes { dot, square, cross }; // for radar marks

public:
	struct aimbot_struct {
		struct vector_struct {
			bool enabled = false;
			int hotkey = 0;

			int field_of_view = 10;
			int target_bones = 2;
			int smoothness = 1;

			struct vector_visual_struct {
				bool visualize_fov = false;
				int fov_type = 0;
				std::array<float, 4> outlinefov = { 1.f,1.f,1.f,1.f };
				std::array<float, 4> filledfov = { 1.f,1.f,1.f,0.1f };
			} visual;

			struct vector_filters_struct {
				bool visibility_check = false;
				bool target_npcs = false;
				bool target_adms = false;

			} filters;
		} vector;

		struct silent_struct {
			bool enabled = false;
			int hotkey = 0;

			int field_of_view = 20;
			int target_bones = 2;
			int hitchance = 100;

			bool magicbullet = false;

			struct silent_visual_struct {
				bool visualize_fov = false;
				int fov_type = 0;
				std::array<float, 4> outlinefov = { 1.f,1.f,1.f,1.f };
				std::array<float, 4> filledfov = { 1.f,1.f,1.f,0.1f };
			} visual;

			struct silent_filters_struct {
				bool visibility_check = false;
				bool target_npcs = false;
				bool target_adms = false;

			} filters;
		} silent;

		struct damager_struct {

		} damager; // todo

		struct trigger_struct {
			bool enabled = true;
		} trigger; // todo
	} aimbot;

	struct esp_struct {
		struct players_struct {
			bool enabled = false;
			float max_dist = 500.f;

			bool skeleton = false;
			std::array<float, 4> skeleton_visible_col = { 1.f,1.f,1.f,1.f };
			std::array<float, 4> skeleton_invisible_col = { 1.f,1.f,1.f,1.f };
			float skeleton_thickness = 1.f;
			bool skel_hands = false;
			//bool skel_feets = false;

			bool rect_box = false;
			std::array<float, 4> rect_box_col = { 1.f,1.f,1.f,1.f };
			float rect_box_thickness = 1.f;
			float width_scale_3d = 1.f;

			bool corners_box = false;
			std::array<float, 4> corners_box_col = { 1.f,1.f,1.f,1.f };
			float corners_size = 3.f;
			float corners_thickness = 1.f;

			bool fill_box = false;
			int fill_box_col_type = 0;
			std::array<float, 4> fill_box_solid_col = { 1.f,1.f,1.f,40/255.f };
			struct fillbox_grd_col_struct {
				std::array<float, 4> col_upr = { 1.f,1.f,1.f,10/255.f };
				std::array<float, 4> col_bot = { 1.f,1.f,1.f,40/255.f };
			} fillbox_box_grd;

			bool lethal_fill = false;
			std::array<float, 4> lethal_fill_col = { 1.f,0.f,0.f,40/255.f };
			int lethal_health = 20;

			bool snaplines = false;
			std::array<float, 4> snaplines_col = { 1.f,1.f,1.f,1.f };
			float snaplines_thickness = 1.f;
			//bool snaplines_dots = false;
			//std::array<float, 2> snpl_dots_rad = { 3.f,3.f };


			struct players_bars_struct {
				bool health_bar = false;
				bool armor_bar = false;
				geometric_pos bars_position{ bottom };

				int bars_color_type = 0; // 0 - solid // 1 - gradient //
				struct players_bars_solid_struct {

					std::array<float, 4> health_bar_col = { 0.f,1.f,0.f,1.f };

					std::array<float, 4> armor_bar_col = { 0.f,0.f,1.f,1.f };
				} solid_color;
				struct players_bars_grd_struct {
					std::array<float, 4> health_bar_col_perv = { 1.f,1.f,1.f,1.f };
					std::array<float, 4> health_bar_col_sec = { 1.f,1.f,1.f,1.f };
					std::array<float, 4> armor_bar_col_perv = { 1.f,1.f,1.f,1.f };
					std::array<float, 4> armor_bar_col_sec = { 1.f,1.f,1.f,1.f };
				} grd_color;

				std::array<float, 4> bars_bg_color = { 0.f, 0.f, 0.f, 70 / 255.f };
			} bars;

			bool eqp_weapon = false;
			std::array<float, 4> eqp_weapon_col = { 1.f,1.f,1.f,1.f };
			bool heavy_wep = false;
			std::array<float, 4> heavy_wep_col = { 1.f,0.f,0.f,1.f };

			bool distance = false;
			int distance_style = 0; // 0 - 10.0 m // 1 - [10 M] //
			std::array<float, 4> distance_col = { 1.f,1.f,1.f,1.f };

			bool health_text = false;
			std::array<float, 4> health_text_col = { 1.f,1.f,1.f,1.f };
			int health_text_style = 0;

			s_esptext_render_params text_preset{ 1, 1, 1, 1, 0, 0, 50 };

		} players;

		struct local_struct {
			bool skeleton = false;

			std::array<float, 4> skeleton_col = { 1.f,1.f,1.f,1.f };
			float skeleton_thickness = 1.f;
			//bool skel_hands = false;
			//bool skel_feets = false;

			//bool lethal_skel = false;

			//std::array<float, 4> lehtal_skel_col = { 1.f,1.f,1.f,1.f };
			//int lethal_health = 30;
		} local;

		struct admins_struct {
			bool enabled = false;
			int state_key = 0;

			bool skeleton = false;

			std::array<float, 4> skeleton_col = { 1.f,0.f,0.f,1.f };

			bool rect_box = false;

			std::array<float, 4> rect_box_col = { 1.f,0.f,0.f,1.f };
			int rect_box_type = 1;

			bool corners_box = false;

			std::array<float, 4> corners_box_col = { 1.f,0.f,0.f,1.f };

			bool fill_box = false;

			std::array<float, 4> fill_box_col = { 1.f,0.f,0.f,1.f };

			bool snaplines = false;

			std::array<float, 4> snaplines_col = { 1.f,0.f,0.f,1.f };

			struct admins_widget_struct {
				bool enabled = false;
				bool preview = false;
				std::array<float, 4> icon_color = { 1.f,0.f,0.f,1.f };
				bool animation = true;
			} widget;

			bool text_flag = false;
		} admins;

		struct objects_struct {
			bool enabled = false;
			float max_dist = 500.f;

			bool names = false;
			std::array<float, 4> names_col = { 1.f,1.f,1.f,1.f };

			bool distance = false;
			std::array<float, 4> distance_col = { 1.f,1.f,1.f,1.f };

			bool snaplines = false;
			std::array<float, 4> snaplines_col = { 1.f,1.f,1.f,1.f };

			bool render_all_stream = false;

			s_esptext_render_params text_preset{ 1, 1, 1, 1, 0, 0, 60 };

			struct objects_tech_struct {
				fixed_containers::FixedMap<DWORD, std::string, 100> objects_list;
			} tech;

		} objects;

		struct vehicles_struct {
			bool enabled = false;
			float max_dist = 500.f;

			bool box = false;

			std::array<float, 4> box_col = { 1.f,1.f,1.f,1.f };

			bool names = false;

			std::array<float, 4> names_col = { 1.f,1.f,1.f,1.f };
			bool manufacture = false;

			bool distance = false;

			std::array<float, 4> distance_col = { 1.f,1.f,1.f,1.f };

			bool number_plate = false;

			std::array<float, 4> number_plate_col = { 1.f,1.f,1.f,1.f };

			bool lock_state = false;
			std::array<float, 4> unlocked_col = { 0.19f,1.f,0.19f,1.f }; // #33ff33
			std::array<float, 4> locked_col = { 1.f,0.19f,0.19f,1.f }; // #ff3333

			bool engine_state = false;
			std::array<float, 4> engine_on_col = { 0.19f,1.f,0.19f,1.f }; // #33ff33
			std::array<float, 4> engine_off_col = { 1.f,0.19f,0.19f,1.f }; // #ff3333

			bool snaplines = false;

			std::array<float, 4> snaplines_col = { 1.f,1.f,1.f,1.f };

			bool ignore_local = false;

			s_esptext_render_params text_preset{ 1, 1, 1, 1, 0, 0, 40 };

			struct vehicles_tech_struct {
				fixed_containers::FixedMap<DWORD, std::string, 100> vehicles_list;
			} tech;

		} vehicles;

		struct radar_struct {
			bool enabled = false;

			std::array<float, 4> rad_accent = { 1.f,1.f,1.f,1.f };
			int colors_type = 0;

			float zoom = 1.2f;
			float rounding = 6.0f;

			float opacity = 50.f;

			struct radar_custom_struct {

				std::array<float, 4> rad_bg = { 1.f,1.f,1.f,1.f };

				std::array<float, 4> rad_layout_lines = { 1.f,1.f,1.f,1.f };

				std::array<float, 4> rad_outline = { 1.f,1.f,1.f,1.f };
			} custom;

			struct radar_filters_struct {
				bool players = false;

				std::array<float, 4> players_col = { 1.f,1.f,1.f,1.f };
				bool admins = false;

				std::array<float, 4> admins_col = { 1.f,1.f,1.f,1.f };

				bool objects = false;

				std::array<float, 4> objects_col = { 1.f,1.f,1.f,1.f };

				bool vehicles = false;

				std::array<float, 4> vehicles_col = { 1.f,1.f,1.f,1.f };

			} filters;

			struct radar_text_struct {
				bool objects = false;
				bool vehicles = false;
			} text;

			struct radar_marks_struct {
				geometric_shapes players{ dot };
				float players_scale = 70.f;
				geometric_shapes admins{ dot };
				float admins_scale = 70.f;
				geometric_shapes objects{ dot };
				geometric_shapes vehicles{ dot };
			} marks;
		} radar;
	} esp;

	struct player_struct {
		struct health_struct {
			bool godmode = false;
			int godmode_key = 0;
			gmode_types godmode_type{ autofill };

			struct health_autofill_struct {
				int min_value = 100;
			} autofill_settings;

			struct health_constant_struct {
				int value = 100;
			} constant_settings;

			bool apply_to_armor = false;
			bool disable_on_admin = false;

			struct health_healing_struct {
				bool enabled = false;

				int heal_type = 0; // idw create another enum so
				// 0 - heal at "one heal shot" // 1 - set health at "health to set"

				int heal_value = 100; // to separate functions
				int set_value = 100; // idk why, but let it be

				bool heal_key_state = false;
				int heal_key = 0;
			} healing;

			struct health_hotkeys_struct {
				bool full_hp = false;
				int full_hp_key = 0;

				bool full_arm = false;
				int full_arm_key = 0;

				bool suicide = false;
				int suicide_key = 0;

				bool clr_visible_dmg = false;
				int cvd_key = 0;
			} btn_hotkeys;
		} health;

		struct movement_struct {
			struct movement_player_mgmt_struct {
				bool disable_ragdoll = false;

				bool disable_collision = false;
				struct player_col_struct {
					bool players = false;
					bool vehicles = false;
					bool objects = false;
				} col_filters;

				bool inf_stamina = false;
				bool fallbreak = false;
				float fb_treshold = 0.f;

				bool sh_enabled = false;
				bool sh_run = false;
				bool sh_swim = false;

				bool skip_ladder = false;
				bool ladder_to_moon = false;

				bool clr_ped_tasks_on_key = false;
				int clr_ped_tasks_key = 0;
			} player_mgmt;

			struct movement_teleport_struct {
				bool teleport_on_key = false;
				int teleport_key = 0;
			} teleport;

			struct movement_noclip_struct {
				bool enabled = false;
				int keybind = 0;

				bool allow_veh = false;

				float speed = 1.f;
				bool boost = false;
				int boost_key = 0;

				bool invert_yaw = false;
				bool enable_lean = false;

				bool catch_vehicles = false;

				bool go_stupid = false;
				int stupid_speed = 0;
				bool t_pose = false;

				bool warp_on_ground = false;
			} noclip;

			struct movement_freecam_struct {
				bool enabled = false;
				bool main_state = false;
				int keybind = 0;

				float speed = 1.f;

				bool boost = false;
				bool boost_state = false;
				int boost_key = 0;

				bool teleport_on_key = false;
				int teleport_key = 0;
			} freecam;
		} movement;

		struct player_hooks_s {
			bool get_ground_z_for_3d_coord = false;
			bool freeze_entity_position = false;
			bool task_go_straight_to_coord = false;
			bool task_stand_still = false;
		} native_hooks;

		struct player_slapper_s {
			bool enabled = false;
			float fov = 50;


		} slapper;

	} player;

	struct weapons_struct {
		int manager_type = 0; // 0 - modify // 1 - disable

		bool recoil = false;
		bool spread = false;
		bool reload = false;
		bool weapon_range = false;

		struct weapons_modify_struct {
			int recoil_percent = 100;
			int spread_percent = 100;
			int reload_speed_percent = 100;
			int weapon_range_meters = 1000;
		} modify_values;

		struct weapons_misc_struct {
			bool hitsound = false;
			int hitsound_number = 0; // щяс бы от скита сюда хитсаунд въебашить..........

			bool bullet_tracers = false;

			std::array<float, 4> tracer_color = { 1.f,1.f,1.f,1.f };
		} misc;
	} weapons;

	struct vehicles_struct {
		/* todo: fixxall and write vars to that shit */
		bool disable_collision = false;
		struct veh_collision_struct {
			bool players = false;
			bool vehicles = false;
		} col_filters;

		struct boost_struct {
			bool enabled = false;
			int key = 0;
			float accel_value = 10.f;
			// additional
			bool smooth_increase = false;
			bool pin_to_ground = false;
		} boost;

		bool instant_180 = false;
		int instant_180_key = 0;

		bool flip_on_key = false;
		int flip_key = 0;

		struct autopilot_struct {
			bool enabled = false;
			int drive_mode = 0;
			int fast_stop_key = 0;

		} autopilot;

		struct veh_health_struct {
			bool gmode = false;
			bool no_deform = false;
			bool bulletproof_tyres = false;

			bool seatbelt = false;
			bool always_clean = false;
		} health;

		struct veh_seat_warp_struct {
			bool enabled = false;
			bool into = false;
			bool exit = false;
		} seatwarp;
		
		struct veh_explts_s {
			bool return_stealing = false;
		} exploits;

		struct veh_hooks_s {
			bool get_is_vehicle_engine_running = false;
			bool set_vehicle_engine_on = false; 
			bool set_vehicle_undriveable = false;
			bool get_entity_speed = false;
			bool get_entity_velocity = false;
		} native_hooks;

	} vehicles;

	struct world_struct {
		struct world_visuals_struct {
			bool timechanger = false;
			int nformated_time = 1200;
			int time_hour = 12;
			int time_minute = 00;

			bool weatherchanger = false;
			int weather_type = 0;

			bool cam_fov = false;
			float cam_fov_v = 50.f;

		} visuals;

		struct w_clickwarp_struct {
			bool enabled = false; int key = 0, cancel_key = 0;
			int marker_type = 1;

			bool draw_distance = false;
			
			std::array<float, 4> marker_col = { 1.f,1.f,1.f,1.f };
			std::array<float, 4> distance_col = { 1.f,1.f,1.f,1.f };
		} clickwarp;
	} world;

	struct controls_struct {
		bool menu_state = true;
		bool menu_custom_key = false;
		int menu_key = VK_INSERT;

		bool unload_on_key = false;
		int unload_key = 0;
	} controls;

	struct background_struct {
		int menu_style = 0; // 0 - classic // 1 - blue orange // 2 - red // 3 - green // 4 - hokage
		std::string menu_title = "Cortex";
		bool menu_background_photo = true;
		std::string menu_background_path = "";
		bool clean_recording_mode = false;
		bool watermark = false;
		int wm_style = 0; // 0 - modern // 1 - legacy //

		std::array<float, 4> wm_accent = { 1.f,1.f,1.f,1.f }; // legacy

		bool wm_bg = true; // modern

		struct background_bg_struct {
			bool enabled = true;
			bool bg_fill = true;
			float bg_alpha = 25.f;
		} background;

		struct experimental_struct {
			bool visible_check = false;
		} experimental;

#ifdef _DEBUG
		struct debug_struct {
			bool dbg_info_render = false;
			bool native_logger = false;

			bool nlog_console = false;
			bool nlog_file = false;
		} debug;
#endif
	} misc;
};

inline std::unique_ptr<c_variables> g_config;

#ifndef NO_CONFIGS
namespace cfg_manager {
	inline std::atomic_bool config_io_active{ false };

	inline bool is_io_active() {
		return config_io_active.load(std::memory_order_acquire);
	}

	struct scoped_config_io {
		bool locked = false;

		scoped_config_io() {
			bool expected = false;
			locked = config_io_active.compare_exchange_strong(expected, true, std::memory_order_acq_rel);
			if (locked) {
				std::this_thread::sleep_for(std::chrono::milliseconds(25));
			}
		}

		~scoped_config_io() {
			if (locked) {
				config_io_active.store(false, std::memory_order_release);
			}
		}

		explicit operator bool() const {
			return locked;
		}
	};

	inline bool ensure_config_dir() {
		try {
			if (!std::filesystem::exists(CONFIG_DIR_PATH)) {
				std::filesystem::create_directories(CONFIG_DIR_PATH);
			}

			return std::filesystem::is_directory(CONFIG_DIR_PATH);
		}
		catch (...) {
			return false;
		}
	}

	inline std::string encode_config_filename(const std::string& config_name) {
		if (config_name.empty()) return {};

		std::string filename = base64::to_base64(config_name);
		std::replace(filename.begin(), filename.end(), '+', '-');
		std::replace(filename.begin(), filename.end(), '/', '_');
		return filename;
	}

	inline bool decode_config_filename(const std::string& filename, std::string& config_name) {
		config_name.clear();
		if (filename.empty() || filename.size() > 128) return false;
		if (filename.find_first_of("\\/.") != std::string::npos) return false;

		for (unsigned char ch : filename) {
			const bool valid = std::isalnum(ch) || ch == '-' || ch == '_' || ch == '+' || ch == '=';
			if (!valid) return false;
		}

		try {
			std::string base64_name = filename;
			std::replace(base64_name.begin(), base64_name.end(), '-', '+');
			std::replace(base64_name.begin(), base64_name.end(), '_', '/');
			config_name = base64::from_base64(base64_name);
		}
		catch (...) {
			config_name.clear();
			return false;
		}

		if (config_name.empty() || config_name.size() > 41) return false;
		for (unsigned char ch : config_name) {
			if (std::iscntrl(ch)) return false;
		}

		return true;
	}

	inline bool is_config_filename(const std::string& filename) {
		std::string decoded;
		return decode_config_filename(filename, decoded);
	}

	inline std::string json_escape(const std::string& value) {
		std::string escaped;
		escaped.reserve(value.size() + 8);

		for (const char ch : value) {
			switch (ch) {
			case '\\': escaped += "\\\\"; break;
			case '"': escaped += "\\\""; break;
			case '\b': escaped += "\\b"; break;
			case '\f': escaped += "\\f"; break;
			case '\n': escaped += "\\n"; break;
			case '\r': escaped += "\\r"; break;
			case '\t': escaped += "\\t"; break;
			default:
				if (static_cast<unsigned char>(ch) < 0x20) {
					escaped += ' ';
				}
				else {
					escaped += ch;
				}
				break;
			}
		}

		return escaped;
	}

	inline int clamp_copy_int(int value, int min_value, int max_value, int fallback) {
		return (value < min_value || value > max_value) ? fallback : value;
	}

	inline float clamp_copy_float(float value, float min_value, float max_value, float fallback) {
		return (value < min_value || value > max_value) ? fallback : value;
	}

	inline std::string build_safe_config_json(const std::string& config_name) {
		auto color4 = [](const std::array<float, 4>& c) {
			return nlohmann::json::array({ c[0], c[1], c[2], c[3] });
		};

		nlohmann::json root;
		root["_name"] = config_name;
		root["aimbot"] = {
			{ "vector", { { "enabled", g_config->aimbot.vector.enabled }, { "target_bones", 2 } } },
			{ "silent", { { "enabled", g_config->aimbot.silent.enabled }, { "target_bones", 2 } } },
			{ "trigger", { { "enabled", g_config->aimbot.trigger.enabled } } }
		};
		root["player"]["movement"]["noclip"]["enabled"] = g_config->player.movement.noclip.enabled;
		root["player"]["movement"]["freecam"]["enabled"] = g_config->player.movement.freecam.enabled;
		root["player"]["movement"]["freecam"]["main_state"] = g_config->player.movement.freecam.main_state;

		if (!g_config) return root.dump();

		root["misc"] = {
			{ "menu_style", clamp_copy_int(g_config->misc.menu_style, 0, 4, 0) },
			{ "menu_title", (g_config->misc.menu_title.empty() || g_config->misc.menu_title.size() > 48) ? std::string("Cortex") : g_config->misc.menu_title },
			{ "menu_background_photo", g_config->misc.menu_background_photo },
			{ "menu_background_path", (g_config->misc.menu_background_path.size() <= MAX_PATH) ? g_config->misc.menu_background_path : std::string() },
			{ "clean_recording_mode", g_config->misc.clean_recording_mode },
			{ "watermark", g_config->misc.watermark },
			{ "wm_style", clamp_copy_int(g_config->misc.wm_style, 0, 1, 0) },
			{ "background", {
				{ "enabled", g_config->misc.background.enabled },
				{ "bg_fill", g_config->misc.background.bg_fill },
				{ "bg_alpha", clamp_copy_float(g_config->misc.background.bg_alpha, 0.f, 100.f, 25.f) }
			} }
		};
		root["controls"] = {
			{ "menu_state", g_config->controls.menu_state },
			{ "menu_custom_key", g_config->controls.menu_custom_key },
			{ "menu_key", clamp_copy_int(g_config->controls.menu_key, 1, 255, VK_INSERT) },
			{ "unload_on_key", false },
			{ "unload_key", 0 }
		};

		root["esp"]["players"] = {
			{ "enabled", g_config->esp.players.enabled },
			{ "max_dist", g_config->esp.players.max_dist },
			{ "skeleton", g_config->esp.players.skeleton },
			{ "skeleton_visible_col", color4(g_config->esp.players.skeleton_visible_col) },
			{ "skeleton_invisible_col", color4(g_config->esp.players.skeleton_invisible_col) },
			{ "skeleton_thickness", g_config->esp.players.skeleton_thickness },
			{ "skel_hands", g_config->esp.players.skel_hands },
			{ "rect_box", g_config->esp.players.rect_box },
			{ "rect_box_col", color4(g_config->esp.players.rect_box_col) },
			{ "rect_box_thickness", g_config->esp.players.rect_box_thickness },
			{ "width_scale_3d", g_config->esp.players.width_scale_3d },
			{ "corners_box", g_config->esp.players.corners_box },
			{ "corners_box_col", color4(g_config->esp.players.corners_box_col) },
			{ "corners_size", g_config->esp.players.corners_size },
			{ "corners_thickness", g_config->esp.players.corners_thickness },
			{ "fill_box", g_config->esp.players.fill_box },
			{ "fill_box_col_type", g_config->esp.players.fill_box_col_type },
			{ "fill_box_solid_col", color4(g_config->esp.players.fill_box_solid_col) },
			{ "fillbox_box_grd", {
				{ "col_upr", color4(g_config->esp.players.fillbox_box_grd.col_upr) },
				{ "col_bot", color4(g_config->esp.players.fillbox_box_grd.col_bot) }
			} },
			{ "lethal_fill", g_config->esp.players.lethal_fill },
			{ "lethal_fill_col", color4(g_config->esp.players.lethal_fill_col) },
			{ "lethal_health", g_config->esp.players.lethal_health },
			{ "snaplines", g_config->esp.players.snaplines },
			{ "snaplines_col", color4(g_config->esp.players.snaplines_col) },
			{ "snaplines_thickness", g_config->esp.players.snaplines_thickness },
			{ "bars", {
				{ "health_bar", g_config->esp.players.bars.health_bar },
				{ "armor_bar", g_config->esp.players.bars.armor_bar },
				{ "bars_position", static_cast<int>(g_config->esp.players.bars.bars_position) },
				{ "bars_color_type", g_config->esp.players.bars.bars_color_type },
				{ "solid_color", {
					{ "health_bar_col", color4(g_config->esp.players.bars.solid_color.health_bar_col) },
					{ "armor_bar_col", color4(g_config->esp.players.bars.solid_color.armor_bar_col) }
				} },
				{ "grd_color", {
					{ "health_bar_col_perv", color4(g_config->esp.players.bars.grd_color.health_bar_col_perv) },
					{ "health_bar_col_sec", color4(g_config->esp.players.bars.grd_color.health_bar_col_sec) },
					{ "armor_bar_col_perv", color4(g_config->esp.players.bars.grd_color.armor_bar_col_perv) },
					{ "armor_bar_col_sec", color4(g_config->esp.players.bars.grd_color.armor_bar_col_sec) }
				} },
				{ "bars_bg_color", color4(g_config->esp.players.bars.bars_bg_color) }
			} },
			{ "eqp_weapon", g_config->esp.players.eqp_weapon },
			{ "eqp_weapon_col", color4(g_config->esp.players.eqp_weapon_col) },
			{ "heavy_wep", g_config->esp.players.heavy_wep },
			{ "heavy_wep_col", color4(g_config->esp.players.heavy_wep_col) },
			{ "distance", g_config->esp.players.distance },
			{ "distance_style", g_config->esp.players.distance_style },
			{ "distance_col", color4(g_config->esp.players.distance_col) },
			{ "health_text", g_config->esp.players.health_text },
			{ "health_text_col", color4(g_config->esp.players.health_text_col) },
			{ "health_text_style", g_config->esp.players.health_text_style },
			{ "text_preset", {
				{ "font_id", g_config->esp.players.text_preset.font_id },
				{ "text_scale", g_config->esp.players.text_preset.text_scale },
				{ "condition", g_config->esp.players.text_preset.condition },
				{ "outline", g_config->esp.players.text_preset.outline },
				{ "adaptive_scale_state", g_config->esp.players.text_preset.adaptive_scale_state },
				{ "adsc_strength", g_config->esp.players.text_preset.adsc_strength }
			} }
		};
		root["esp"]["admins"] = {
			{ "enabled", g_config->esp.admins.enabled },
			{ "skeleton", g_config->esp.admins.skeleton },
			{ "skeleton_col", color4(g_config->esp.admins.skeleton_col) },
			{ "rect_box", g_config->esp.admins.rect_box },
			{ "rect_box_col", color4(g_config->esp.admins.rect_box_col) },
			{ "snaplines", g_config->esp.admins.snaplines },
			{ "snaplines_col", color4(g_config->esp.admins.snaplines_col) },
			{ "widget_enabled", g_config->esp.admins.widget.enabled }
		};
		root["esp"]["objects"] = {
			{ "enabled", g_config->esp.objects.enabled },
			{ "max_dist", g_config->esp.objects.max_dist },
			{ "names", g_config->esp.objects.names },
			{ "names_col", color4(g_config->esp.objects.names_col) },
			{ "distance", g_config->esp.objects.distance },
			{ "distance_col", color4(g_config->esp.objects.distance_col) },
			{ "snaplines", g_config->esp.objects.snaplines },
			{ "snaplines_col", color4(g_config->esp.objects.snaplines_col) }
		};
		root["esp"]["vehicles"] = {
			{ "enabled", g_config->esp.vehicles.enabled },
			{ "max_dist", g_config->esp.vehicles.max_dist },
			{ "ignore_local", g_config->esp.vehicles.ignore_local },
			{ "box", g_config->esp.vehicles.box },
			{ "box_col", color4(g_config->esp.vehicles.box_col) },
			{ "names", g_config->esp.vehicles.names },
			{ "names_col", color4(g_config->esp.vehicles.names_col) },
			{ "manufacture", g_config->esp.vehicles.manufacture },
			{ "distance", g_config->esp.vehicles.distance },
			{ "distance_col", color4(g_config->esp.vehicles.distance_col) },
			{ "number_plate", g_config->esp.vehicles.number_plate },
			{ "number_plate_col", color4(g_config->esp.vehicles.number_plate_col) },
			{ "lock_state", g_config->esp.vehicles.lock_state },
			{ "unlocked_col", color4(g_config->esp.vehicles.unlocked_col) },
			{ "locked_col", color4(g_config->esp.vehicles.locked_col) },
			{ "engine_state", g_config->esp.vehicles.engine_state },
			{ "engine_on_col", color4(g_config->esp.vehicles.engine_on_col) },
			{ "engine_off_col", color4(g_config->esp.vehicles.engine_off_col) },
			{ "snaplines", g_config->esp.vehicles.snaplines },
			{ "snaplines_col", color4(g_config->esp.vehicles.snaplines_col) }
		};
		root["esp"]["radar"] = {
			{ "enabled", g_config->esp.radar.enabled },
			{ "zoom", g_config->esp.radar.zoom },
			{ "rounding", g_config->esp.radar.rounding },
			{ "opacity", g_config->esp.radar.opacity },
			{ "filters", {
				{ "players", g_config->esp.radar.filters.players },
				{ "players_col", color4(g_config->esp.radar.filters.players_col) },
				{ "admins", g_config->esp.radar.filters.admins },
				{ "admins_col", color4(g_config->esp.radar.filters.admins_col) }
			} }
		};
		return root.dump();
	}

	inline bool find_json_value_start(const std::string& source, const std::string& key, size_t& value_pos) {
		const std::string needle = "\"" + key + "\"";
		size_t key_pos = source.find(needle);
		if (key_pos == std::string::npos) return false;

		size_t colon_pos = source.find(':', key_pos + needle.size());
		if (colon_pos == std::string::npos) return false;

		value_pos = colon_pos + 1;
		while (value_pos < source.size() && std::isspace(static_cast<unsigned char>(source[value_pos]))) {
			++value_pos;
		}

		return value_pos < source.size();
	}

	inline bool read_json_string_field(const std::string& source, const std::string& key, std::string& value) {
		size_t pos = 0;
		if (!find_json_value_start(source, key, pos) || source[pos] != '"') return false;
		++pos;

		std::string parsed;
		while (pos < source.size()) {
			const char ch = source[pos++];
			if (ch == '"') {
				value = parsed;
				return true;
			}

			if (ch == '\\' && pos < source.size()) {
				const char escaped = source[pos++];
				switch (escaped) {
				case '"': parsed += '"'; break;
				case '\\': parsed += '\\'; break;
				case '/': parsed += '/'; break;
				case 'b': parsed += '\b'; break;
				case 'f': parsed += '\f'; break;
				case 'n': parsed += '\n'; break;
				case 'r': parsed += '\r'; break;
				case 't': parsed += '\t'; break;
				default: parsed += escaped; break;
				}
			}
			else {
				parsed += ch;
			}
		}

		return false;
	}

	inline bool read_json_bool_field(const std::string& source, const std::string& key, bool& value) {
		size_t pos = 0;
		if (!find_json_value_start(source, key, pos)) return false;

		if (source.compare(pos, 4, "true") == 0) {
			value = true;
			return true;
		}

		if (source.compare(pos, 5, "false") == 0) {
			value = false;
			return true;
		}

		return false;
	}

	inline bool read_json_int_field(const std::string& source, const std::string& key, int& value) {
		size_t pos = 0;
		if (!find_json_value_start(source, key, pos)) return false;

		const char* start = source.c_str() + pos;
		char* end = nullptr;
		const long parsed = std::strtol(start, &end, 10);
		if (end == start) return false;

		value = static_cast<int>(parsed);
		return true;
	}

	inline bool read_json_float_field(const std::string& source, const std::string& key, float& value) {
		size_t pos = 0;
		if (!find_json_value_start(source, key, pos)) return false;

		const char* start = source.c_str() + pos;
		char* end = nullptr;
		const float parsed = std::strtof(start, &end);
		if (end == start) return false;

		value = parsed;
		return true;
	}

	inline void sanitize_config_values(c_variables& cfg);
	inline void disable_runtime_toggles(c_variables& cfg);

	inline void apply_safe_loaded_config(const std::string& buffer) {
		if (!g_config) return;

		const bool previous_menu_state = g_config->controls.menu_state;

		auto parsed = nlohmann::json::parse(buffer, nullptr, false);
		if (!parsed.is_discarded() && parsed.is_object()) {
			auto set_bool = [](const nlohmann::json& obj, const char* key, bool& out) {
				if (obj.contains(key) && obj[key].is_boolean()) out = obj[key].get<bool>();
			};
			auto set_int = [](const nlohmann::json& obj, const char* key, int& out) {
				if (obj.contains(key) && obj[key].is_number_integer()) out = obj[key].get<int>();
			};
			auto set_float = [](const nlohmann::json& obj, const char* key, float& out) {
				if (obj.contains(key) && obj[key].is_number()) out = obj[key].get<float>();
			};
			auto set_string = [](const nlohmann::json& obj, const char* key, std::string& out) {
				if (obj.contains(key) && obj[key].is_string()) out = obj[key].get<std::string>();
			};
			auto set_color = [](const nlohmann::json& obj, const char* key, std::array<float, 4>& out) {
				if (!obj.contains(key) || !obj[key].is_array() || obj[key].size() != 4) return;
				for (size_t i = 0; i < 4; ++i) {
					if (!obj[key][i].is_number()) return;
				}
				for (size_t i = 0; i < 4; ++i) {
					out[i] = obj[key][i].get<float>();
				}
			};

			if (parsed.contains("misc") && parsed["misc"].is_object()) {
				const auto& misc = parsed["misc"];
				set_int(misc, "menu_style", g_config->misc.menu_style);
				set_string(misc, "menu_title", g_config->misc.menu_title);
				set_bool(misc, "menu_background_photo", g_config->misc.menu_background_photo);
				set_string(misc, "menu_background_path", g_config->misc.menu_background_path);
				set_bool(misc, "clean_recording_mode", g_config->misc.clean_recording_mode);
				set_bool(misc, "watermark", g_config->misc.watermark);
				set_int(misc, "wm_style", g_config->misc.wm_style);
				if (misc.contains("background") && misc["background"].is_object()) {
					const auto& bg = misc["background"];
					set_bool(bg, "enabled", g_config->misc.background.enabled);
					set_bool(bg, "bg_fill", g_config->misc.background.bg_fill);
					set_float(bg, "bg_alpha", g_config->misc.background.bg_alpha);
				}
			}

			if (parsed.contains("controls") && parsed["controls"].is_object()) {
				const auto& controls = parsed["controls"];
				set_bool(controls, "menu_custom_key", g_config->controls.menu_custom_key);
				set_int(controls, "menu_key", g_config->controls.menu_key);
			}

			if (parsed.contains("aimbot") && parsed["aimbot"].is_object()) {
				const auto& aimbot = parsed["aimbot"];
				if (aimbot.contains("vector") && aimbot["vector"].is_object()) {
					set_bool(aimbot["vector"], "enabled", g_config->aimbot.vector.enabled);
				}
				if (aimbot.contains("silent") && aimbot["silent"].is_object()) {
					set_bool(aimbot["silent"], "enabled", g_config->aimbot.silent.enabled);
				}
				if (aimbot.contains("trigger") && aimbot["trigger"].is_object()) {
					set_bool(aimbot["trigger"], "enabled", g_config->aimbot.trigger.enabled);
				}
			}

			if (parsed.contains("esp") && parsed["esp"].is_object()) {
				const auto& esp = parsed["esp"];
				if (esp.contains("players") && esp["players"].is_object()) {
					const auto& players = esp["players"];
					set_bool(players, "enabled", g_config->esp.players.enabled);
					set_float(players, "max_dist", g_config->esp.players.max_dist);
					set_bool(players, "skeleton", g_config->esp.players.skeleton);
					set_color(players, "skeleton_visible_col", g_config->esp.players.skeleton_visible_col);
					set_color(players, "skeleton_invisible_col", g_config->esp.players.skeleton_invisible_col);
					set_float(players, "skeleton_thickness", g_config->esp.players.skeleton_thickness);
					set_bool(players, "skel_hands", g_config->esp.players.skel_hands);
					set_bool(players, "rect_box", g_config->esp.players.rect_box);
					set_color(players, "rect_box_col", g_config->esp.players.rect_box_col);
					set_float(players, "rect_box_thickness", g_config->esp.players.rect_box_thickness);
					set_float(players, "width_scale_3d", g_config->esp.players.width_scale_3d);
					set_bool(players, "corners_box", g_config->esp.players.corners_box);
					set_color(players, "corners_box_col", g_config->esp.players.corners_box_col);
					set_float(players, "corners_size", g_config->esp.players.corners_size);
					set_float(players, "corners_thickness", g_config->esp.players.corners_thickness);
					set_bool(players, "fill_box", g_config->esp.players.fill_box);
					set_int(players, "fill_box_col_type", g_config->esp.players.fill_box_col_type);
					set_color(players, "fill_box_solid_col", g_config->esp.players.fill_box_solid_col);
					if (players.contains("fillbox_box_grd") && players["fillbox_box_grd"].is_object()) {
						const auto& fill_grad = players["fillbox_box_grd"];
						set_color(fill_grad, "col_upr", g_config->esp.players.fillbox_box_grd.col_upr);
						set_color(fill_grad, "col_bot", g_config->esp.players.fillbox_box_grd.col_bot);
					}
					set_bool(players, "lethal_fill", g_config->esp.players.lethal_fill);
					set_color(players, "lethal_fill_col", g_config->esp.players.lethal_fill_col);
					set_int(players, "lethal_health", g_config->esp.players.lethal_health);
					set_bool(players, "snaplines", g_config->esp.players.snaplines);
					set_color(players, "snaplines_col", g_config->esp.players.snaplines_col);
					set_float(players, "snaplines_thickness", g_config->esp.players.snaplines_thickness);
					if (players.contains("bars") && players["bars"].is_object()) {
						const auto& bars = players["bars"];
						set_bool(bars, "health_bar", g_config->esp.players.bars.health_bar);
						set_bool(bars, "armor_bar", g_config->esp.players.bars.armor_bar);
						set_int(bars, "bars_position", reinterpret_cast<int&>(g_config->esp.players.bars.bars_position));
						set_int(bars, "bars_color_type", g_config->esp.players.bars.bars_color_type);
						if (bars.contains("solid_color") && bars["solid_color"].is_object()) {
							const auto& solid = bars["solid_color"];
							set_color(solid, "health_bar_col", g_config->esp.players.bars.solid_color.health_bar_col);
							set_color(solid, "armor_bar_col", g_config->esp.players.bars.solid_color.armor_bar_col);
						}
						if (bars.contains("grd_color") && bars["grd_color"].is_object()) {
							const auto& grad = bars["grd_color"];
							set_color(grad, "health_bar_col_perv", g_config->esp.players.bars.grd_color.health_bar_col_perv);
							set_color(grad, "health_bar_col_sec", g_config->esp.players.bars.grd_color.health_bar_col_sec);
							set_color(grad, "armor_bar_col_perv", g_config->esp.players.bars.grd_color.armor_bar_col_perv);
							set_color(grad, "armor_bar_col_sec", g_config->esp.players.bars.grd_color.armor_bar_col_sec);
						}
						set_color(bars, "bars_bg_color", g_config->esp.players.bars.bars_bg_color);
					}
					set_bool(players, "eqp_weapon", g_config->esp.players.eqp_weapon);
					set_color(players, "eqp_weapon_col", g_config->esp.players.eqp_weapon_col);
					set_bool(players, "heavy_wep", g_config->esp.players.heavy_wep);
					set_color(players, "heavy_wep_col", g_config->esp.players.heavy_wep_col);
					set_bool(players, "distance", g_config->esp.players.distance);
					set_int(players, "distance_style", g_config->esp.players.distance_style);
					set_color(players, "distance_col", g_config->esp.players.distance_col);
					set_bool(players, "health_text", g_config->esp.players.health_text);
					set_color(players, "health_text_col", g_config->esp.players.health_text_col);
					set_int(players, "health_text_style", g_config->esp.players.health_text_style);
					if (players.contains("text_preset") && players["text_preset"].is_object()) {
						const auto& text = players["text_preset"];
						set_int(text, "font_id", g_config->esp.players.text_preset.font_id);
						set_float(text, "text_scale", g_config->esp.players.text_preset.text_scale);
						set_int(text, "condition", g_config->esp.players.text_preset.condition);
						set_bool(text, "outline", g_config->esp.players.text_preset.outline);
						set_bool(text, "adaptive_scale_state", g_config->esp.players.text_preset.adaptive_scale_state);
						set_float(text, "adsc_strength", g_config->esp.players.text_preset.adsc_strength);
					}
				}

				if (esp.contains("admins") && esp["admins"].is_object()) {
					const auto& admins = esp["admins"];
					set_bool(admins, "enabled", g_config->esp.admins.enabled);
					set_bool(admins, "skeleton", g_config->esp.admins.skeleton);
					set_color(admins, "skeleton_col", g_config->esp.admins.skeleton_col);
					set_bool(admins, "rect_box", g_config->esp.admins.rect_box);
					set_color(admins, "rect_box_col", g_config->esp.admins.rect_box_col);
					set_bool(admins, "snaplines", g_config->esp.admins.snaplines);
					set_color(admins, "snaplines_col", g_config->esp.admins.snaplines_col);
					set_bool(admins, "widget_enabled", g_config->esp.admins.widget.enabled);
				}

				if (esp.contains("objects") && esp["objects"].is_object()) {
					const auto& objects = esp["objects"];
					set_bool(objects, "enabled", g_config->esp.objects.enabled);
					set_float(objects, "max_dist", g_config->esp.objects.max_dist);
					set_bool(objects, "names", g_config->esp.objects.names);
					set_color(objects, "names_col", g_config->esp.objects.names_col);
					set_bool(objects, "distance", g_config->esp.objects.distance);
					set_color(objects, "distance_col", g_config->esp.objects.distance_col);
					set_bool(objects, "snaplines", g_config->esp.objects.snaplines);
					set_color(objects, "snaplines_col", g_config->esp.objects.snaplines_col);
				}

				if (esp.contains("vehicles") && esp["vehicles"].is_object()) {
					const auto& vehicles = esp["vehicles"];
					set_bool(vehicles, "enabled", g_config->esp.vehicles.enabled);
					set_float(vehicles, "max_dist", g_config->esp.vehicles.max_dist);
					set_bool(vehicles, "ignore_local", g_config->esp.vehicles.ignore_local);
					set_bool(vehicles, "box", g_config->esp.vehicles.box);
					set_color(vehicles, "box_col", g_config->esp.vehicles.box_col);
					set_bool(vehicles, "names", g_config->esp.vehicles.names);
					set_color(vehicles, "names_col", g_config->esp.vehicles.names_col);
					set_bool(vehicles, "manufacture", g_config->esp.vehicles.manufacture);
					set_bool(vehicles, "distance", g_config->esp.vehicles.distance);
					set_color(vehicles, "distance_col", g_config->esp.vehicles.distance_col);
					set_bool(vehicles, "number_plate", g_config->esp.vehicles.number_plate);
					set_color(vehicles, "number_plate_col", g_config->esp.vehicles.number_plate_col);
					set_bool(vehicles, "lock_state", g_config->esp.vehicles.lock_state);
					set_color(vehicles, "unlocked_col", g_config->esp.vehicles.unlocked_col);
					set_color(vehicles, "locked_col", g_config->esp.vehicles.locked_col);
					set_bool(vehicles, "engine_state", g_config->esp.vehicles.engine_state);
					set_color(vehicles, "engine_on_col", g_config->esp.vehicles.engine_on_col);
					set_color(vehicles, "engine_off_col", g_config->esp.vehicles.engine_off_col);
					set_bool(vehicles, "snaplines", g_config->esp.vehicles.snaplines);
					set_color(vehicles, "snaplines_col", g_config->esp.vehicles.snaplines_col);
				}

				if (esp.contains("radar") && esp["radar"].is_object()) {
					const auto& radar = esp["radar"];
					set_bool(radar, "enabled", g_config->esp.radar.enabled);
					set_float(radar, "zoom", g_config->esp.radar.zoom);
					set_float(radar, "rounding", g_config->esp.radar.rounding);
					set_float(radar, "opacity", g_config->esp.radar.opacity);
					if (radar.contains("filters") && radar["filters"].is_object()) {
						const auto& filters = radar["filters"];
						set_bool(filters, "players", g_config->esp.radar.filters.players);
						set_color(filters, "players_col", g_config->esp.radar.filters.players_col);
						set_bool(filters, "admins", g_config->esp.radar.filters.admins);
						set_color(filters, "admins_col", g_config->esp.radar.filters.admins_col);
					}
				}
			}

			g_config->controls.menu_state = previous_menu_state;
			g_config->controls.unload_on_key = false;
			g_config->aimbot.vector.target_bones = 2;
			g_config->aimbot.silent.target_bones = 2;
			sanitize_config_values(*g_config);
			g_config->controls.menu_state = previous_menu_state;
			g_config->controls.unload_on_key = false;
			return;
		}

		read_json_int_field(buffer, "menu_style", g_config->misc.menu_style);
		read_json_string_field(buffer, "menu_title", g_config->misc.menu_title);
		read_json_bool_field(buffer, "menu_background_photo", g_config->misc.menu_background_photo);
		read_json_string_field(buffer, "menu_background_path", g_config->misc.menu_background_path);
		read_json_bool_field(buffer, "clean_recording_mode", g_config->misc.clean_recording_mode);
		read_json_bool_field(buffer, "watermark", g_config->misc.watermark);
		read_json_int_field(buffer, "wm_style", g_config->misc.wm_style);
		read_json_bool_field(buffer, "bg_fill", g_config->misc.background.bg_fill);
		read_json_float_field(buffer, "bg_alpha", g_config->misc.background.bg_alpha);
		read_json_bool_field(buffer, "menu_custom_key", g_config->controls.menu_custom_key);
		read_json_int_field(buffer, "menu_key", g_config->controls.menu_key);

		g_config->controls.menu_state = previous_menu_state;
		g_config->controls.unload_on_key = false;
		g_config->aimbot.vector.target_bones = 2;
		g_config->aimbot.silent.target_bones = 2;

		sanitize_config_values(*g_config);
		g_config->controls.menu_state = previous_menu_state;
		g_config->controls.unload_on_key = false;
	}

	inline void sanitize_config_values(c_variables& cfg) {
		if (cfg.misc.menu_title.empty() || cfg.misc.menu_title.size() > 48) {
			cfg.misc.menu_title = "Cortex";
		}

		if (cfg.misc.menu_background_path.size() > MAX_PATH) {
			cfg.misc.menu_background_path.clear();
		}

		if (cfg.misc.menu_style < 0 || cfg.misc.menu_style > 4) {
			cfg.misc.menu_style = 0;
		}

		if (cfg.misc.wm_style < 0 || cfg.misc.wm_style > 1) {
			cfg.misc.wm_style = 0;
		}

		auto clamp_int = [](int& value, int min_value, int max_value) {
			if (value < min_value || value > max_value) value = min_value;
			};

		auto sanitize_text = [&](s_esptext_render_params& preset) {
			clamp_int(preset.font_id, 0, 3);
			clamp_int(preset.condition, 0, 2);
			if (preset.text_scale < 0.5f || preset.text_scale > 2.0f) preset.text_scale = 1.0f;
			if (preset.adsc_strength < 1.0f || preset.adsc_strength > 250.0f) preset.adsc_strength = 50.0f;
			};

		cfg.aimbot.vector.target_bones = 2;
		clamp_int(cfg.aimbot.vector.visual.fov_type, 0, 2);
		clamp_int(cfg.aimbot.vector.field_of_view, 1, 500);
		clamp_int(cfg.aimbot.vector.smoothness, 1, 100);
		cfg.aimbot.silent.target_bones = 2;
		clamp_int(cfg.aimbot.silent.visual.fov_type, 0, 2);
		clamp_int(cfg.aimbot.silent.field_of_view, 1, 500);
		clamp_int(cfg.aimbot.silent.hitchance, 0, 100);
		clamp_int(cfg.weapons.manager_type, 0, 1);
		clamp_int(cfg.world.visuals.weather_type, 0, 13);
		if (cfg.player.movement.noclip.speed <= 0.f || cfg.player.movement.noclip.speed > 50.f) {
			cfg.player.movement.noclip.speed = 1.f;
		}
		if (cfg.player.movement.freecam.speed <= 0.f || cfg.player.movement.freecam.speed > 50.f) {
			cfg.player.movement.freecam.speed = 1.f;
		}
#ifdef _DEBUG
		cfg.misc.debug.dbg_info_render = false;
		cfg.misc.debug.native_logger = false;
		cfg.misc.debug.nlog_console = false;
		cfg.misc.debug.nlog_file = false;
#endif

		clamp_int(cfg.esp.players.fill_box_col_type, 0, 1);
		clamp_int(cfg.esp.players.bars.bars_color_type, 0, 1);
		clamp_int(reinterpret_cast<int&>(cfg.esp.players.bars.bars_position), 0, 3);
		clamp_int(cfg.esp.players.distance_style, 0, 1);
		clamp_int(cfg.esp.admins.rect_box_type, 0, 1);
		clamp_int(reinterpret_cast<int&>(cfg.esp.radar.marks.players), 0, 1);
		clamp_int(reinterpret_cast<int&>(cfg.esp.radar.marks.admins), 0, 1);

		sanitize_text(cfg.esp.players.text_preset);
		sanitize_text(cfg.esp.objects.text_preset);
		sanitize_text(cfg.esp.vehicles.text_preset);
	}

	inline void disable_runtime_toggles(c_variables& cfg) {
		cfg.aimbot.vector.enabled = false;
		cfg.aimbot.silent.enabled = false;
		cfg.aimbot.trigger.enabled = false;

		cfg.esp.players.enabled = false;
		cfg.esp.admins.enabled = false;
		cfg.esp.objects.enabled = false;
		cfg.esp.vehicles.enabled = false;
		cfg.esp.radar.enabled = false;

		cfg.player.health.godmode = false;
		cfg.player.health.healing.enabled = false;
		cfg.player.health.btn_hotkeys.full_hp = false;
		cfg.player.health.btn_hotkeys.full_arm = false;
		cfg.player.health.btn_hotkeys.suicide = false;
		cfg.player.health.btn_hotkeys.clr_visible_dmg = false;
		cfg.player.movement.player_mgmt.disable_ragdoll = false;
		cfg.player.movement.player_mgmt.disable_collision = false;
		cfg.player.movement.player_mgmt.inf_stamina = false;
		cfg.player.movement.player_mgmt.fallbreak = false;
		cfg.player.movement.player_mgmt.sh_enabled = false;
		cfg.player.movement.player_mgmt.skip_ladder = false;
		cfg.player.movement.player_mgmt.clr_ped_tasks_on_key = false;
		cfg.player.movement.teleport.teleport_on_key = false;
		cfg.player.movement.noclip.enabled = false;
		cfg.player.movement.noclip.boost = false;
		cfg.player.movement.freecam.enabled = false;
		cfg.player.movement.freecam.main_state = false;
		cfg.player.movement.freecam.boost = false;
		cfg.player.movement.freecam.boost_state = false;
		cfg.player.movement.freecam.teleport_on_key = false;
		cfg.player.native_hooks.get_ground_z_for_3d_coord = false;
		cfg.player.native_hooks.freeze_entity_position = false;
		cfg.player.native_hooks.task_go_straight_to_coord = false;
		cfg.player.native_hooks.task_stand_still = false;
		cfg.player.slapper.enabled = false;

		cfg.weapons.recoil = false;
		cfg.weapons.spread = false;
		cfg.weapons.reload = false;
		cfg.weapons.weapon_range = false;
		cfg.weapons.misc.bullet_tracers = false;

		cfg.vehicles.disable_collision = false;
		cfg.vehicles.boost.enabled = false;
		cfg.vehicles.instant_180 = false;
		cfg.vehicles.flip_on_key = false;
		cfg.vehicles.seatwarp.enabled = false;
		cfg.vehicles.health.gmode = false;
		cfg.vehicles.health.no_deform = false;
		cfg.vehicles.health.bulletproof_tyres = false;
		cfg.vehicles.health.seatbelt = false;
		cfg.vehicles.health.always_clean = false;
		cfg.vehicles.exploits.return_stealing = false;
		cfg.vehicles.native_hooks.get_is_vehicle_engine_running = false;
		cfg.vehicles.native_hooks.set_vehicle_engine_on = false;
		cfg.vehicles.native_hooks.set_vehicle_undriveable = false;
		cfg.vehicles.native_hooks.get_entity_speed = false;
		cfg.vehicles.native_hooks.get_entity_velocity = false;

		cfg.world.visuals.timechanger = false;
		cfg.world.visuals.weatherchanger = false;
		cfg.world.visuals.cam_fov = false;
		cfg.world.clickwarp.enabled = false;
	}

	inline void sanitize_loaded_config(c_variables& cfg, const c_variables& previous) {
		sanitize_config_values(cfg);

		cfg.controls.menu_state = previous.controls.menu_state;
		cfg.controls.unload_on_key = false;
	}

	inline void sanitize_config() {
		if (!g_config) return;
		sanitize_config_values(*g_config);
	}

	///  <summary>
	///  Save config to file.
	///  </summary>
	///  <param name="config_name">name of config file</param>
	inline bool save_config(std::string config_name) {
		try {
			if (!g_config || !ensure_config_dir()) return false;
			scoped_config_io io_guard;
			if (!io_guard) return false;

			if (config_name.empty() || config_name.size() > 41) return false;
			std::string filename = encode_config_filename(config_name);
			if (filename.empty()) return false;

			std::string buffer = build_safe_config_json(config_name);
			if (buffer.empty()) return false;

			// Move the buffer to the next three bits of the buffer.
			for (int i = 0; i < buffer.length(); i++) {
				if (i % 2 == 0) {
					buffer[i] = buffer[i] - 3;
				}
				else {
					buffer[i] = buffer[i] - 2;
				}
			}

			std::string exportstr = base64::to_base64(base64::to_base64(buffer));

			std::filesystem::path cfg_path = std::filesystem::path(CONFIGS_PATH) / filename;
			std::filesystem::path tmp_path = cfg_path;
			tmp_path += ".tmp";

			std::ofstream file(tmp_path, std::ios::out | std::ios::trunc | std::ios::binary);
			if (!file.is_open()) return false;

			file << exportstr;
			file.close();

			if (!file) return false;

			std::error_code ec;
			std::filesystem::rename(tmp_path, cfg_path, ec);
			if (ec) {
				std::filesystem::remove(cfg_path, ec);
				ec.clear();
				std::filesystem::rename(tmp_path, cfg_path, ec);
			}

			if (ec) {
				std::filesystem::remove(tmp_path, ec);
				return false;
			}

			return std::filesystem::exists(cfg_path, ec) && !ec;
		}
		catch (...) {
			return false;
		}
	}

	///  <summary>
	///  мудак
	///  </summary>
	inline bool load_config(std::string filename) {
		try {
			if (!ensure_config_dir()) return false;
			if (!is_config_filename(filename)) return false;
			scoped_config_io io_guard;
			if (!io_guard) return false;

			std::filesystem::path cfg_path = std::filesystem::path(CONFIGS_PATH) / filename;
			if (!std::filesystem::exists(cfg_path)) return false;
			if (!std::filesystem::is_regular_file(cfg_path)) return false;

			std::ifstream file(cfg_path, std::ios::in | std::ios::binary);
			if (!file.is_open()) return false;

			std::stringstream ss;
			ss << file.rdbuf();
			file.close();

			std::string encoded = ss.str();
			if (encoded.empty()) return false;

			std::string buffer = base64::from_base64(base64::from_base64(encoded));
			if (buffer.empty()) return false;

			// Add 3 bytes to the buffer.
			for (int i = 0; i < buffer.length(); i++) {
				if (i % 2 == 0) {
					buffer[i] = buffer[i] + 3;
				}
				else {
					buffer[i] = buffer[i] + 2;
				}
			}

			const auto json_start = buffer.find_first_not_of(" \t\r\n");
			if (json_start == std::string::npos || buffer[json_start] != '{') {
				return false;
			}

			if (!g_config) return false;

			apply_safe_loaded_config(buffer);

			return true;
		}
		catch (...) {
			return false;
		}
	}

	inline bool delete_config(std::string filename) {
		try {
			if (!ensure_config_dir()) return false;
			if (filename.empty()) return false;
			if (filename.find_first_of("\\/") != std::string::npos) return false;
			if (!is_config_filename(filename)) return false;

			std::filesystem::path cfg_path = std::filesystem::path(CONFIGS_PATH) / filename;
			std::error_code ec;
			if (!std::filesystem::exists(cfg_path, ec) || ec) return false;
			if (!std::filesystem::is_regular_file(cfg_path, ec) || ec) return false;

			return std::filesystem::remove(cfg_path, ec) && !ec;
		}
		catch (...) {
			return false;
		}
	}
}

#endif

struct c_cheat_base {
	int admin_counter = 0;
	int closest_dist = 0;

	struct {
		char cfg_name[42] = "";
		char obj_name[25] = "";
		char obj_hash[64] = "";
	} ui_bufs;

	struct {
		int original_hour = 0;
		int original_min = 0;
	} w_time;

	struct {
		struct {
			bool state = false;
			bool boost = false;
		} noclip;

		struct {
			bool state = false;
			bool boost = false;
			bool teleport = false;
		} freecam;

		struct {
			struct {
				bool ragdoll_removed = false;
				bool ped_coll_removed = false;

				bool sprintboost = false;
			} player;

			struct {
				bool coll_removed = false;
				bool seatbelt = false;
				bool bulletproof_tyres = false;
				bool hijack_flags = false;
			} vehicle;
		} single;

		bool clickwarp_state = false;
	} funcs;
};

inline std::unique_ptr<c_cheat_base> cheat_base;

// SINGLEUSE MGR

struct hack_queue {
	bool sethealth = false; // ✔
	bool addhealth = false; // not used
	bool setarmor = false;  // ✔
	bool addarmor = false;  // not used
	bool tptowp = false;    // ✔
	bool suicide = false;   // ✔
	bool clearvisibledamage = false; // ✔
	bool clearpedtasks = false;      // ✔
	bool flipvehicle = false;		 // ✔

	bool startautopilot = false;     // nah
	bool autopilot_inuse = false;
	bool stopautopilot = false;      // nah

	bool fixvehicle = false;         // ✔
	bool fixvehdeformations = false; // ✔
	bool cleanvehicle = false;       // ✔
	bool killvehengine = false;      // ✔
	bool killvehgastank = false;	 // ✔
	bool popvehicletyres = false;    // ✔
	bool startvehengine = false;	 // sosal
	bool unloadcheat = false;		 // ✔
	
#ifdef _DEBUG
	bool dumpentrypoints = false;
	bool testthread = false;
#endif
};

inline std::unique_ptr<hack_queue> g_singleuse;
