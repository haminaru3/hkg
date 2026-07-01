#pragma once
#include "../../../imports.h"

namespace player {
	namespace hooks {
		namespace original {
			uintptr_t get_ground_z_for_3d_coord, freeze_entity_position, task_go_straight_to_coord, task_stand_still;
		}

		namespace detours {
            void get_ground_z_for_3d_coord(native::nsdk::NativeContext& context) {
                // static bool get_ground_z_for_3d_coord(float x, float y, float z, float* groundZ, bool unk) // 0xC906A7DAB05C8D2B
                if (g_config->player.native_hooks.get_ground_z_for_3d_coord) {
                    auto x = context.GetArgument<float>(0);
                    auto y = context.GetArgument<float>(1);
                    if (x == local.player->fPosition.x || y == local.player->fPosition.y) {
                        return;
                        //auto z = context.GetArgument<float>(2);
                        //auto groundZ = context.GetArgument<float*>(3);
                        //auto unk = context.GetArgument<bool>(4);
                        //auto result = context.GetResult<bool>();

                        //// log output
                        //std::cout << "-- ** get_ground_z_for_3d_coord call:" << std::endl;
                        //std::cout << "- x: " << x;
                        //std::cout << ", y: " << y;
                        //std::cout << ", z: " << z << std::endl;
                        //std::cout << "- groundZ: " << groundZ << std::endl;
                        //std::cout << "- unk: " << unk << std::endl;
                        //std::cout << "- result: " << result << std::endl;
                    }
                }
                reinterpret_cast<decltype(&get_ground_z_for_3d_coord)>(original::get_ground_z_for_3d_coord)(context);
            }

            void freeze_entity_position(native::nsdk::NativeContext& context) {
                // static void freeze_entity_position(type::entity entity, bool toggle) // 0x428ca6dbd1094446
                if (g_config->player.native_hooks.freeze_entity_position) {
                    auto entity = context.GetArgument<int32_t>(0);
                    auto lp = native::player::player_ped_id();
                    if (entity == lp) {
                        context.SetArgument<bool>(1, false);
                    }
                }
                reinterpret_cast<decltype(&freeze_entity_position)>(original::freeze_entity_position)(context);
            }

            void task_go_straight_to_coord(native::nsdk::NativeContext& context) {
                // static void task_go_straight_to_coord(type::ped ped, float x, float y, float z, float speed, int timeout, float targetheading, float distancetoslide)
                // 0xd76b57b44f1e6f8b
                if (g_config->player.native_hooks.task_go_straight_to_coord) {
                    auto entity = context.GetArgument<int32_t>(0);
                    auto lp = native::player::player_ped_id();
                    if (entity == lp) {
                        return;
                    }
                }
                reinterpret_cast<decltype(&task_go_straight_to_coord)>(original::task_go_straight_to_coord)(context);
            }

            void task_stand_still(native::nsdk::NativeContext& context) {
                // static void task_stand_still(type::ped ped, int time) // 0x919be13eed931959
                if (g_config->player.native_hooks.task_stand_still) {
                    auto entity = context.GetArgument<int32_t>(0);
                    auto lp = native::player::player_ped_id();
                    if (entity == lp) {
                        return;
                    }
                }
                reinterpret_cast<decltype(&task_stand_still)>(original::task_stand_still)(context);
            }
		}
	}
}