#pragma once
#include "../../../imports.h"

namespace vehicle {
	namespace hooks {
		namespace original {
			uintptr_t get_is_vehicle_engine_running, set_vehicle_engine_on, set_vehicle_undriveable, get_entity_speed, get_entity_velocity;
		}

		namespace detours {
			void get_is_vehicle_engine_running(native::nsdk::NativeContext& context) {
				// static --> bool <-- get_is_vehicle_engine_running(type::vehicle vehicle) // 0xae31e7df9b5b132e
				if (g_config->vehicles.native_hooks.get_is_vehicle_engine_running) {
					if (Game.is_lp_in_vehicle) {
						auto entity = context.GetArgument<int32_t>(0);
						auto lp_veh = pointer_to_handle((uintptr_t)local.player->vehicle());
						if (entity == lp_veh) {
							context.SetResult<bool>(0, false);
						}
					}
				}
				reinterpret_cast<decltype(&get_is_vehicle_engine_running)>(original::get_is_vehicle_engine_running)(context);
			}

			void set_vehicle_engine_on(native::nsdk::NativeContext& context) {
				// set_vehicle_engine_on(type::vehicle vehicle, bool --> value <--, bool instantly, bool noautoturnon)
				if (g_config->vehicles.native_hooks.set_vehicle_engine_on) {
					if (Game.is_lp_in_vehicle) {
						auto entity = context.GetArgument<int32_t>(0);
						auto lp_veh = pointer_to_handle((uintptr_t)local.player->vehicle());
						if (entity == lp_veh) {
							context.SetArgument<bool>(1, true);
						}
					}
				}
				reinterpret_cast<decltype(&set_vehicle_engine_on)>(original::set_vehicle_engine_on)(context);
			}

			void set_vehicle_undriveable(native::nsdk::NativeContext& context) {
				if (g_config->vehicles.native_hooks.set_vehicle_undriveable) {
					if (Game.is_lp_in_vehicle) {
						auto entity = context.GetArgument<int32_t>(0);
						auto lp_veh = pointer_to_handle((uintptr_t)local.player->vehicle());
						if (entity == lp_veh) {
							context.SetArgument<bool>(1, false);
						}
					}
				}
				reinterpret_cast<decltype(&set_vehicle_undriveable)>(original::set_vehicle_undriveable)(context);
			}

			void get_entity_speed(native::nsdk::NativeContext& context) { 
				// static --> float <-- get_entity_speed(type::entity entity) // 0xd5037ba82e12416f
				if (g_config->vehicles.native_hooks.get_entity_speed) {
					if (Game.is_lp_in_vehicle) {
						auto entity = context.GetArgument<int32_t>(0);
						auto lp_veh = pointer_to_handle((uintptr_t)local.player->vehicle());
						if (entity == lp_veh) {
							context.SetResult<float>(0, 0.f); // formula = this * 3.6f
						}
					}
				}
				reinterpret_cast<decltype(&get_entity_speed)>(original::get_entity_speed)(context);
			}

			void get_entity_velocity(native::nsdk::NativeContext& context) {
				// static --> PVector3 <-- get_entity_velocity(type::entity entity) // 0x4805d2b1d8cf94a9
				if (g_config->vehicles.native_hooks.get_entity_velocity) {
					if (Game.is_lp_in_vehicle) {
						auto entity = context.GetArgument<int32_t>(0);
						auto lp_veh = pointer_to_handle((uintptr_t)local.player->vehicle());
						if (entity == lp_veh || entity == pointer_to_handle((uintptr_t)local.player)) {
							context.SetResult<PVector3>(0, PVector3(0,0,0)); // or PVector3(1,1,1)
						}
					}
				}
				reinterpret_cast<decltype(&get_entity_velocity)>(original::get_entity_velocity)(context);
			}
		}
	}
}