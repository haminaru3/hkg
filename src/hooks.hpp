#pragma once
#include "imports.h"
#include <dwmapi.h>

#include "features/functions/vehicle/hooks.hpp"
#include "features/functions/player/hooks.hpp"
#include "pointers.hpp"

using namespace std;
IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef HRESULT(__stdcall* D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(__stdcall* D3D11ResizeBuffersHook) (IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

D3D11PresentHook phookD3D11Present = NULL;
D3D11ResizeBuffersHook phookD3D11ResizeBuffers = NULL;

struct s_nativehook_data {
	uint64_t native_hash;
	void* detour;
	uintptr_t* original;
	const char* log_name;
};

void enable_native_hooks(const std::vector<s_nativehook_data>& hooks) {
	for (const auto& hook : hooks) {
		auto nativeHandler = native::invoker::find_native_handler(hook.native_hash);
		if (nativeHandler) {
			MH_CreateHook(nativeHandler, hook.detour, reinterpret_cast<void**>(hook.original));
			if (MH_EnableHook(nativeHandler) == MH_OK) {
				logs::add(std::format("[+] {} {} | hash: {}", hook.log_name, "hook", hook.native_hash));
			}
		}
	}
}

using ReturnAddrCheckFn = bool(__fastcall*)(__int64, unsigned __int64);
ReturnAddrCheckFn OriginalReturnAddrCheck = nullptr;

bool __fastcall hooked_returnaddr_check(__int64 a1, unsigned __int64 returnaddr) {

	return true;
}

using namespace std::chrono_literals;
namespace hook {

	string renderMethod = "2";
	//==========================================================================================================================

	std::chrono::time_point<std::chrono::high_resolution_clock> resize_counter = std::chrono::high_resolution_clock::now();
	bool present_setup = false;


	//=========================================================================================================================//
	typedef LRESULT(__stdcall* WINPROC) (_In_ HWND hWnd,
		_In_ UINT Msg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam);

	WINPROC OriginalDefWindowProc = nullptr;
	WINPROC OriginalWindowProc = nullptr;


	LRESULT CALLBACK hook_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		static bool callOnce = false;
		if (!callOnce) {
			add_log("hook WndProc called");
			callOnce = true;
		}


		if (uMsg == WM_KEYUP) {
			int menu_key = g_config->controls.menu_custom_key ? g_config->controls.menu_key : VK_INSERT;

			if (wParam == menu_key) {
				g_config->controls.menu_state = !g_config->controls.menu_state;
			}
			if ((wParam == g_config->controls.unload_key) && g_config->controls.unload_on_key) {
				Game.running = false;
			}
		}

		if (present_setup && Game.window) {
			ImGuiIO& io = ImGui::GetIO();
			POINT mPos;
			GetCursorPos(&mPos);
			ScreenToClient(Game.window, &mPos);
			ImGui::GetIO().MousePos.x = mPos.x;
			ImGui::GetIO().MousePos.y = mPos.y;

			if (uMsg == WM_KEYUP) {
				Game.keyStates[(int)wParam] = false;
			}
			else if (uMsg == WM_KEYDOWN) {
				Game.keyStates[(int)wParam] = true;
			}


			if (uMsg == WM_MOUSEMOVE) {
				//logs::add("WM_MOUSEMOVE");
				
			}

			if (g_config->controls.menu_state) {
				if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
					return true;
				}
				return 1l;
			}
		}

		return CallWindowProc(Game.originalWndProc, hWnd, uMsg, wParam, lParam);
	}

	HRESULT __stdcall hookedD3D11ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {

		if (present_setup && Game.window) {
			Game.renderReady = false;
			renderer.release();
			add_log("reloading d3d11 device");
			std::this_thread::sleep_for(100ms);


			if (Game.originalWndProc) {
				SetWindowLongPtr(Game.window, GWLP_WNDPROC, (LONG_PTR)Game.originalWndProc);
			}

			//TODO crash when resizing + unloading
			return phookD3D11ResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
		}

		return phookD3D11ResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
	}
	int64_t reload_time = 2000;
	HRESULT __stdcall hookedD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
		if (!present_setup) {

			auto now = std::chrono::high_resolution_clock::now();


			auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(now - resize_counter).count();
			if (dur > reload_time) {
				present_setup = true;
				cout << "present" << endl;
				add_log("initiated d3d11");
				//get device


				DXGI_SWAP_CHAIN_DESC desc;

				if (SUCCEEDED(pSwapChain->GetDesc(&desc))) {
					Game.window = desc.OutputWindow;
					add_log("get output window");


					renderer.SetResourceLoad(load_image_assets);
					renderer.Initialize(Game.window, pSwapChain);
					add_log("renderer hopefully didnt crash");
					//ui::pre_init();
					//ui::create_styles();

					Game.renderReady = true;
				}

				resize_counter = std::chrono::high_resolution_clock::now();
			}
		}
		if (!Game.renderReady && present_setup) {
			resize_counter = std::chrono::high_resolution_clock::now();
			reload_time = 5000;
			add_log("waiting for init");
			present_setup = false;
		}
		if (!Game.renderReady) return phookD3D11Present(pSwapChain, SyncInterval, Flags);


		//if ((WNDPROC)GetWindowLongPtr(Game.window, GWLP_WNDPROC) != Game.originalWndProc) {
		//	Game.originalWndProc = (WNDPROC)SetWindowLongPtr(Game.window, GWLP_WNDPROC, (LONG_PTR)hook_WndProc);
		//}


		WINDOWINFO info;
		GetWindowInfo(Game.window, &info);
		
		Game.screen.x = ((info.rcClient.right) - (info.rcClient.left));
		Game.screen.y = ((info.rcClient.bottom) - (info.rcClient.top));


		if (Game.menuOpen) {
			//WantCaptureMouse 

			ImGui::GetIO().WantCaptureMouse = true;
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else {
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}

		ImGui::GetIO().MouseDrawCursor = g_config->controls.menu_state;

		game::beginframe_tick();
		

		renderer.BeginScene();
		
		game::render();
		
		renderer.EndScene();
		
		renderer.Render();
		
		return phookD3D11Present(pSwapChain, SyncInterval, Flags);
	}

	//==========================================================================================================================

	LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	uintptr_t discordoverlay_presentd3d11 = 0;
	uintptr_t discordoverlay_resized3d11 = 0;

	ID3D11Device* pDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;
	IDXGISwapChain* pSwapChain;
	HWND pWindow;
	WNDCLASSEXA wc;

	DWORD_PTR* pSwapChainVtable = NULL;
	DWORD_PTR* pContextVTable = NULL;
	DWORD_PTR* pDeviceVTable = NULL;

	bool cleanupOverlay = false;

	bool hook_d3d11(HWND hWnd, bool destroyAfter) {

		D3D_FEATURE_LEVEL requestedLevels[] = { D3D_FEATURE_LEVEL_11_0 };
		D3D_FEATURE_LEVEL obtainedLevel;

		DXGI_SWAP_CHAIN_DESC scd;
		ZeroMemory(&scd, sizeof(scd));
		scd.BufferCount = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		scd.OutputWindow = pWindow;
		scd.SampleDesc.Count = 1;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Windowed = ((GetWindowLongPtr(pWindow, GWL_STYLE) & WS_POPUP) != 0) ? false : true;
		//scd.Windowed = true;

		scd.BufferDesc.Width = 1;
		scd.BufferDesc.Height = 1;
		scd.BufferDesc.RefreshRate.Numerator = 0;
		scd.BufferDesc.RefreshRate.Denominator = 1;

		UINT createFlags = 0;
		IDXGISwapChain* d3dSwapChain = 0;

		if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags, requestedLevels, sizeof(requestedLevels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &scd, &pSwapChain, &pDevice, &obtainedLevel, &pContext))) {
			//MessageBox(Game.window, "Failed to create directX device and swapchain!", "Error", MB_ICONERROR);
			add_log("Failed to create directX device and swapchain!");
			return false;
		}

		pSwapChainVtable = (DWORD_PTR*)pSwapChain;
		pSwapChainVtable = (DWORD_PTR*)pSwapChainVtable[0];

		pContextVTable = (DWORD_PTR*)pContext;
		pContextVTable = (DWORD_PTR*)pContextVTable[0];

		pDeviceVTable = (DWORD_PTR*)pDevice;
		pDeviceVTable = (DWORD_PTR*)pDeviceVTable[0];

		if (MH_CreateHook((DWORD_PTR*)pSwapChainVtable[8], hookedD3D11Present, reinterpret_cast<void**>(&phookD3D11Present)) != MH_OK) {
			add_log("failed creating present hook");
			return false;
		}
		if (MH_EnableHook((DWORD_PTR*)pSwapChainVtable[8]) != MH_OK) {
			add_log("failed enabling present hook");
			return false;
		}

		if (MH_CreateHook((DWORD_PTR*)pSwapChainVtable[13], hookedD3D11ResizeBuffers, reinterpret_cast<void**>(&phookD3D11ResizeBuffers)) != MH_OK) {
			add_log("failed creating ResizeBuffers hook");
			return false;
		}
		if (MH_EnableHook((DWORD_PTR*)pSwapChainVtable[13]) != MH_OK) {
			add_log("failed enabling ResizeBuffers hook");
			return false;
		}


		DWORD dwOld;
		if (!VirtualProtect(phookD3D11Present, 2, PAGE_EXECUTE_READWRITE, &dwOld)) {
			add_log("failed vp");
			return false;
		}
		if (!destroyAfter) {
			add_log("overlay renderer created");
			return true;
		}
		// remove stuff

		pSwapChain->Release();
		pSwapChain = NULL;

		pDevice->Release();
		pDevice = NULL;

		pContext->Release();
		pContext = NULL;

		::DestroyWindow(hWnd);
		::UnregisterClass(wc.lpszClassName, wc.hInstance);

		add_log("hwnd renderer created");
		return true;
	}

	bool hook_d3d11() {
		wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXGIMsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "Valve001", NULL };
		RegisterClassExA(&wc);
		pWindow = CreateWindowA("Valve001", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

		return hook_d3d11(pWindow, true);
	}

	void shutdown_d3d11() {

		pSwapChain->Release();
		pSwapChain = NULL;

		pDevice->Release();
		pDevice = NULL;

		pContext->Release();
		pContext = NULL;
		if (pWindow) {

			::DestroyWindow(pWindow);

			::UnregisterClass(wc.lpszClassName, wc.hInstance);

		}
	}

	//game thread hook
	//https://github.com/citizenfx/fivem/blob/cbe56f78f86bebb68d7960a38c3cdc31c7d76790/code/components/rage-scripting-five/src/scrThread.cpp#L58
	void call_thread(game_thread* this_ptr, int ops_to_execute) {
		if ((
			/*Ragemp*/(this_ptr->m_context.m_script_hash == 0x26FB4AB9 || this_ptr->m_context.m_script_hash == 1104607124 || this_ptr->m_context.m_script_hash == 3381724246) ||
			/*AltV*/(this_ptr->m_context.m_script_hash == 2003913879 || this_ptr->m_context.m_script_hash == 3522812357/* || this_ptr->m_context.m_script_hash == 20 || this_ptr->m_context.m_script_hash == 21*/)
			)) {
			auto& thread = game_thread::get();
			auto orig_thread = thread;
			thread = this_ptr;
			game_fiber->on_tick();
			thread = orig_thread;
		}
	}

	void on_native_thread() {

		static uint64_t    last = 0;
		uint64_t        cur = *(uint64_t*)(FrameCount);
		if (last != cur) {
			last = cur;

			Game.tick_count++; // counter

			game::tick();

			tick::pnative.on_tick();
		}

		game_fiber->wait(0);
	}

	void call_fiber() {
		//Log::Warning(_xor_("[+] Fiber >> Initializing fiber..."));
		add_log("init fiber");
		game_fiber = std::make_unique<fiber::fiber_task>(nullptr, on_native_thread);
	}

	uintptr_t hooked_native_thread(game_thread* this_ptr, int ops_to_execute) {
		call_thread(this_ptr, ops_to_execute);
		return original_native_thread(this_ptr, ops_to_execute);
	}

	//uintptr_t original_get_is_task_active = 0;
	//void on_get_is_task_active(native::nsdk::NativeContext& context) {
	//	int32_t task = context.GetArgument<int32_t>(1);
	//	reinterpret_cast<decltype(&on_get_is_task_active)>(original_get_is_task_active)(context);
	//}

	bool hook_game() {
		//ReturnAddrCheckFn ReturnAddrOriginalPtr = *(ReturnAddrCheckFn*)pointers::fix_mov(pointers::findPattern("altv-client.dll", "48 39 91 ?? ?? ?? ?? 41"));
		//MH_CreateHook((void*)ReturnAddrOriginalPtr, (void*)&hooked_returnaddr_check, (void**)&OriginalReturnAddrCheck);
		//if (ReturnAddrOriginalPtr) {
		//	if (MH_EnableHook(ReturnAddrOriginalPtr) == MH_OK) {
		//		logs::add("ZAEBIS");
		//	}
		//}

		call_fiber(); //set game_fiber
		MH_CreateHook(gta_script_thread_tick, hooked_native_thread, reinterpret_cast<void**>(&original_native_thread));
		if (gta_script_thread_tick) {
			if (MH_EnableHook(gta_script_thread_tick) == MH_OK) {
				logs::add("hijack thread success");
			}
		}

		std::vector<s_nativehook_data> vehicle_hooks = {
			{ 0xd5037ba82e12416f, vehicle::hooks::detours::get_entity_speed, &vehicle::hooks::original::get_entity_speed, "get_entity_speed" },
			{ 0x4805d2b1d8cf94a9, vehicle::hooks::detours::get_entity_velocity, &vehicle::hooks::original::get_entity_velocity, "get_entity_velocity" },
			{ 0xae31e7df9b5b132e, vehicle::hooks::detours::get_is_vehicle_engine_running, &vehicle::hooks::original::get_is_vehicle_engine_running, "get_is_vehicle_engine_running" },
			{ 0x2497c4717c8b881e, vehicle::hooks::detours::set_vehicle_engine_on, &vehicle::hooks::original::set_vehicle_engine_on, "set_vehicle_engine_on" },
			{ 0x8aba6af54b942b95, vehicle::hooks::detours::set_vehicle_undriveable, &vehicle::hooks::original::set_vehicle_undriveable, "set_vehicle_undriveable" },
		};

		enable_native_hooks(vehicle_hooks);

		std::vector<s_nativehook_data> player_hooks = {
			{ 0xC906A7DAB05C8D2B, player::hooks::detours::get_ground_z_for_3d_coord, &player::hooks::original::get_ground_z_for_3d_coord, "get_ground_z_for_3d_coord" },
			{ 0x428ca6dbd1094446, player::hooks::detours::freeze_entity_position, &player::hooks::original::freeze_entity_position, "freeze_entity_position" },
			{ 0xd76b57b44f1e6f8b, player::hooks::detours::task_go_straight_to_coord, &player::hooks::original::task_go_straight_to_coord, "task_go_straight_to_coord" },
			{ 0x919be13eed931959, player::hooks::detours::task_stand_still, &player::hooks::original::task_stand_still, "task_stand_still" },
		};

		enable_native_hooks(player_hooks);

		add_log("game hooks done");
		return true;

	}
	void patch_wndproc() {
		if (!present_setup) return;
		if ((GetWindowLongPtr(Game.window, GWLP_WNDPROC) != (LONG_PTR)hook_WndProc)) {
			add_log("wndproc needs rehook");
			Game.originalWndProc = (WNDPROC)SetWindowLongPtr(Game.window, GWLP_WNDPROC, (LONG_PTR)hook_WndProc);
		}

		return;
	}



	bool hook_renderer() {
		//cout << "[!] render -> " << (check_for_gameoverlay64() ? (check_for_discordoverlay() ? "discord" : "steam") : check_for_nvidiaoverlay() ? "nvidia" : "own") << endl;
		add_log("using own renderer");
		Game.renderMethod = 2;
		return hook_d3d11();
	}

	bool disable() {
		if (Game.originalWndProc) {
			SetWindowLongPtr(Game.window, GWLP_WNDPROC, (LONG_PTR)Game.originalWndProc);
		}
		add_log("unload");

		MH_DisableHook(MH_ALL_HOOKS);

		if (cleanupOverlay) {
			shutdown_d3d11();
		}

		//CRASH!!!

		//FreeConsole();

		return true;
	}
	bool Ready() {
		if ((GetModuleHandleA("d3d11.dll") != NULL) || (GetModuleHandleA("dxgi.dll") != NULL)) return true;
		return false;
	}

	bool enable() {
			if (Ready()) {
				if (MH_Initialize() != MH_OK) {
					add_log("init failed");
					return false;
				}

				bool renderer = hook_renderer();

				bool game = hook_game();

				Game.tick = game::tick;

//#ifndef DEBUG
//				g_config->controls.menu_state = true;
//#endif // DEBUG


				return renderer && game;
			}
			std::this_thread::sleep_for(1000ms);
		return false;
	}
}
