// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "imports.h"
#include "peb.hpp"
//#include "protection/shadowsyscall.hpp"
#include "protection/prot_defs.h"
#include "../includes/lazy_importer.hpp"
#include "features/threads.hpp"

#include "cheat_base.h"
// TODO: Try it later
//// TLS callback declaration
//extern "C" void NTAPI _tls_callback(PVOID DllHandle, DWORD Reason, PVOID Reserved);
//
//// CRT initialization function
//extern "C" BOOL WINAPI _CRT_INIT(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

// Global object constructors
typedef void (__cdecl *_PVFV)(void);
extern _PVFV __xc_a[], __xc_z[];

DWORD __stdcall main_init(PVOID) {
	//VM_DOLPHIN_WHITE_START;

	if (!std::filesystem::exists(CONFIG_DIR_PATH)) {
		std::filesystem::create_directories(CONFIG_DIR_PATH);
	}

	logs::add(u_crypt("main thread ok"));
	if (pointers::findGameState()) {

		do {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		} while (*Game.gta_game_state != game_state_t::playing);

		add_log("playing now");
		pointers::findAllPatterns();
		if (hook::enable()) {
			Game.running = true;

			_create_thread(h2o2_base.internal.aim_thread, aimbot::init);
			_create_thread(h2o2_base.internal.hacks_thread, hacks::init);
#ifdef DEBUG
			_create_thread(h2o2_base.internal.threading_helper, rage::runner);
#endif
		}
	}

	//tools::unlink_module_peb(h2o2_base.internal.module);
	add_log("peb unlinked");
	hotkeys::init_hotkeys();
	add_log("hotkeys inited");
#ifdef _DEBUG
	g_config->controls.unload_key = VK_F5;
	g_config->controls.unload_on_key = true;
#endif
	while (Game.running) {
		hook::patch_wndproc();
		std::this_thread::sleep_for(500ms);
	}

	//tools::relink_module_peb(h2o2_base.internal.module);
	hook::disable();
	logs::add(u_crypt("peb relink + hooks release"));

	//VM_DOLPHIN_WHITE_END;

	FreeLibraryAndExitThread(h2o2_base.internal.module, 0);
	return 1;
}

void on_terminate() {
	MessageBoxA(NULL, "something went wrong....... please check console for natives Cortex", ";(", MB_ICONQUESTION);
}

BOOL APIENTRY DllMain(HMODULE h_module, DWORD  reason_for_call, LPVOID lp_reserved) {
	switch (reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		// TODO: Try it later
		//// Initialize TLS
		//_tls_callback(h_module, DLL_PROCESS_ATTACH, lp_reserved);

		//// Initialize CRT
		//if (!_CRT_INIT(h_module, DLL_PROCESS_ATTACH, lp_reserved))
		//	return FALSE;

		//// Call global object constructors
		//for (_PVFV* pctor = __xc_a; pctor < __xc_z; ++pctor) {
		//	if (*pctor != nullptr)
		//		(**pctor)();
		//}
		//AllocConsole();
		//logs::clear();
		//logs::add(u_crypt("entry"));
		//std::cout << "test1" << std::endl;
		h2o2_base.internal.module = h_module;
		g_config = std::make_unique<c_variables>();
		g_singleuse = std::make_unique<hack_queue>();
		cheat_base = std::make_unique<c_cheat_base>();
		//logs::add(u_crypt("trying to create thread"));
		_create_thread(h2o2_base.internal.main_thread, main_init);
		return TRUE;
	}
	case DLL_THREAD_ATTACH:
	//	_tls_callback(h_module, DLL_THREAD_ATTACH, lp_reserved);
	//	_CRT_INIT(h_module, DLL_THREAD_ATTACH, lp_reserved);
		break;
	case DLL_THREAD_DETACH:
	//	_tls_callback(h_module, DLL_THREAD_DETACH, lp_reserved);
	//	_CRT_INIT(h_module, DLL_THREAD_DETACH, lp_reserved);
		break;
	case DLL_PROCESS_DETACH:
	//	_tls_callback(h_module, DLL_PROCESS_DETACH, lp_reserved);
	//	_CRT_INIT(h_module, DLL_PROCESS_DETACH, lp_reserved);
		break;
	}
	return TRUE;
}
