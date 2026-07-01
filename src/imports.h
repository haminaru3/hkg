#pragma once
#define _CRT_SECURE_NO_WARNINGS

// ----- DEFINE FOR LIVE DEBUG BUILD
#define LIVE_DEBUG_BUILD

#include <Windows.h>
#include <iostream>

#include <dwmapi.h> 
#include <TlHelp32.h>
#include <stdio.h>

#include <ctime>
#include <chrono>
#include <thread>
#include <iostream>
#include <cstdint>
#include <string>
#include <Windows.h>
#include <vector>
#include <format>

#include <sstream>
#include <string_view>
#include <algorithm>
#include <xmmintrin.h>
#include <map>
#include <directxmath.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <fixed_containers/fixed_unordered_map.hpp>
#include <fixed_containers/fixed_map.hpp>

#define PI 3.14159265

#define IM_MAX(_A,_B)       (((_A) >= (_B)) ? (_A) : (_B))

#ifndef IsValidPtr
#define IsValidPtr(x) (x && !IsBadReadPtr(&x, sizeof(void*) && x != nullptr && x > nullptr))
#endif

#ifndef is_any_aimbot_enabled
#define is_any_aimbot_enabled (g_config->aimbot.vector.enabled || g_config->aimbot.silent.enabled)
#endif

#ifndef is_any_esp_enabled
#define is_any_esp_enabled (g_config->esp.players.enabled || g_config->esp.players.skeleton || g_config->esp.local.skeleton || g_config->esp.admins.enabled || g_config->esp.admins.skeleton || g_config->esp.radar.filters.players || g_config->esp.radar.filters.admins)
#endif

// for convenience
using json = nlohmann::json;
bool debug = false;

using namespace std;

#include "MinHook.h"
#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif

#include "../includes/SimpleMath.h"

using namespace DirectX::SimpleMath;


#include "../includes/renderer/rend_features.h"
#include "xor.h"
#include "logger.h"
#include <D3DX11tex.h>

imgui_render renderer;

#include "menu_background.hpp"

template <typename ...Args>
std::string arg_to_string(Args&&... args) {
	std::ostringstream oss;
	(oss << ... << std::forward<Args>(args));
	return oss.str();
}

void load_image_assets(ID3D11Device* device)
{
	refresh_menu_background_texture(true);
	return;
};


#include "structs.h"

typedef void(__fastcall* Game_tick) ();

namespace fiber
{
	using script_func_t = void(*)();

	class fiber_task
	{
	public:
		explicit fiber_task(HMODULE hmod, script_func_t func) :
			m_hmodule(hmod),
			m_function(func)
		{ }

		~fiber_task() noexcept
		{
			if (m_script_fiber)
			{
				DeleteFiber(m_script_fiber);
			}
		}

		HMODULE get_hmodule()
		{
			return m_hmodule;
		}

		script_func_t get_function()
		{
			return m_function;
		}

		void on_tick()
		{
			if (GetTickCount64() < m_wake_at)
				return;

			if (!m_main_fiber)
			{
				m_main_fiber = IsThreadAFiber() ? GetCurrentFiber() : ConvertThreadToFiber(nullptr);
			}

			if (m_script_fiber)
			{
				current_fiber_script = this;
				SwitchToFiber(m_script_fiber);
				current_fiber_script = nullptr;
			}
			else
			{

				m_script_fiber = CreateFiber(0, [](PVOID param)
					{
						auto this_script = static_cast<fiber_task*>(param);
						while (true)
						{
							this_script->m_function();
						}

					}, this);
			}
		}
		void wait(std::uint32_t time)
		{
			m_wake_at = GetTickCount64() + time;
			SwitchToFiber(m_main_fiber);
		}

		inline static fiber_task* get_current_script()
		{
			return current_fiber_script;
		}
	private:
		HMODULE m_hmodule{};
		script_func_t m_function{};

		std::uint32_t m_wake_at{};
		void* m_script_fiber{};
		void* m_main_fiber{};

		inline static fiber_task* current_fiber_script{};
	};
}
std::unique_ptr<fiber::fiber_task> game_fiber;


struct HackBase {
	fixed_containers::FixedMap<int, bool, 100> keyStates;
	int renderMethod = -1;
	bool renderReady = false;
	bool running = false;

	bool menuOpen = true;

	ImVec2 screen = ImVec2(0, 0);
	HWND window = 0;
	WNDPROC originalWndProc = nullptr;

	CReplayInterface* ReplayInterface = nullptr;
	CWorld* World = nullptr;

	CWeaponPatcher weaponPatcher;

	CViewPort* viewPort = nullptr;
	game_state_t* gta_game_state;

	PVector3 cw_coords;

	uintptr_t CamAddr = 0;

	Game_tick tick;
	uint64_t tick_count = 0;

	int game_mod_base = 0;

	bool is_lp_in_vehicle = false;

	CPlayerAngles* getCam() {
		if (CamAddr){
			return *(CPlayerAngles**)(CamAddr + 0x0);
		}
		return 0;
	}
	
	CGameCameraAngles* CGameCamera;
} Game;

//local player etc
struct LocalData {
	CObject* player = nullptr;
} local;


#include "tickbase.hpp"

#include "functions.h"

#include "c_config.hpp"
#include "database.hpp"

#include "tick.hpp"

#include "live_debug.hpp"
#include "keybind.hpp"
#include "features/aimbot.hpp"
#include "features/esp.hpp"
#include "features/hacks.hpp"
#include "features/functions/rage/testing.hpp"
#include "features/hacks_singleuse.hpp"



#include "game.hpp"
#include "hooks.hpp"
#include "pointers.hpp"
