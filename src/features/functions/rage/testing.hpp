#ifdef _DEBUG
#include "../../../imports.h"
#include "../../threads.hpp"
// это худшее что я писал за всю свою жизнь

// you can use sleeps in these threaded functions. Threads are automatically joined after finishing.

namespace rage {
	namespace thread_state {
		struct thread_st {
			std::thread* thread = nullptr;
			std::atomic<bool> is_finished{ false };
			std::atomic<bool> is_running{ false };
		};
		thread_st explode_veh_st;
		thread_st kill_everyone_st;
		thread_st heavens_gate_st;
		thread_st carspam_st;
		thread_st worldtradecenter_st;
	}

	namespace threaded {
		void explode_veh() {
			Vector3 initial_pos = local.player->ObjectNavigation->Position;
			for (auto& this_vehicle : game::vehicle_list) {
				logs::add("test2");
				CVehicle* veh = this_vehicle.first;
				DataVehicle& data = this_vehicle.second;
				if (!veh->isVehicleDestroyed()) { // avoid using already blown up car
					auto veh_handle = pointer_to_handle((uintptr_t)veh);
					std::cout << "vehicle " << veh << std::endl;
					if (native::vehicle::is_vehicle_seat_free(veh_handle, -1)) {
						auto local_player = pointer_to_handle((uintptr_t)local.player);
						logs::add("found car doing shit");
						native::ped::set_ped_into_vehicle(local_player, veh_handle, -1);
						g_config->player.health.godmode = true;
						std::this_thread::sleep_for(96ms); // better use increments of 16 to avoid running natives out of tick
						native::vehicle::set_vehicle_petrol_tank_health(veh_handle, -999.0f);
						std::this_thread::sleep_for(128ms);
						g_config->player.health.godmode = false;
					}
				}

			}

			local.player->ObjectNavigation->setPosition(initial_pos);

			thread_state::explode_veh_st.is_finished = true;
			return;
		}

		void kill_everyone() {
			// not implemented

			/* Logic of the function:
			*  Iterate thru player pool and kill them via damager (do this after finishing killaura/damager)
			*/
			thread_state::kill_everyone_st.is_finished = true;
		}

		void heavens_gate() {
			// not implemented

			/* Logic of the function:
			*  Iterate thru vehicle pool, if player can take the driver seat - he takes it and teleports the car above skybox, where it doesn't fall down.
			*  Try freezing the vehicle to prevent it from falling even more.
			*/
			thread_state::heavens_gate_st.is_finished = true;
			return;
		}

		void carspam() {
			// not implemented

			/* Logic of the function:
			*  Iterate thru vehicle pool, if player can take the driver seat - he takes it and teleports the car to his initial position (carspam.js script frfrfr)
			*/
			thread_state::carspam_st.is_finished = true;
			return;
		}

		void worldtradecenter() {
			//not implemented

			/* Logic of the function:
			*  Teleport to Fort Zankudo. Iterate thru veh pool, take the first veh with type "plane". Take control and teleport to EMS. Aim it straight down where taxeriki epta.
			*  Shoot the car via setting it's speed to 540km/h and clear ped tasks. Repeat until no planes are available or the time limit exceeds 120s. (prolly lower)
			*/
			thread_state::worldtradecenter_st.is_finished = true;
			return;
		}
	}

	void start_explode_veh() {
		if (!thread_state::explode_veh_st.is_running.exchange(true)) {
			thread_state::explode_veh_st.is_finished = false;

			thread_state::explode_veh_st.thread = new std::thread(threaded::explode_veh);
		}
	}

	void start_kill_everyone() {
		if (!thread_state::kill_everyone_st.is_running.exchange(true)) {
			thread_state::kill_everyone_st.is_finished = false;

			thread_state::kill_everyone_st.thread = new std::thread(threaded::kill_everyone);
		}
	}

	void start_heavens_gate() {
		if (!thread_state::heavens_gate_st.is_running.exchange(true)) {
			thread_state::heavens_gate_st.is_finished = false;

			thread_state::heavens_gate_st.thread = new std::thread(threaded::heavens_gate);
		}
	}

	void start_carspam() {
		if (!thread_state::carspam_st.is_running.exchange(true)) {
			thread_state::carspam_st.is_finished = false;

			thread_state::carspam_st.thread = new std::thread(threaded::carspam);
		}
	}

	void start_worldtradecenter() {
		if (!thread_state::worldtradecenter_st.is_running.exchange(true)) {
			thread_state::worldtradecenter_st.is_finished = false;

			thread_state::worldtradecenter_st.thread = new std::thread(threaded::worldtradecenter);
		}
	}


	// НЕ ОТКРЫВАТЬ Я ПРЕДУПРЕДИЛ
	DWORD runner(LPVOID) {
		logs::add("threadkilling thread started (wtf)");

		while (Game.running) {

			// прошу понять и простить это пиздец бля полный

			// explode_veh_st
			{
				if (thread_state::explode_veh_st.is_finished && thread_state::explode_veh_st.is_running) {
					logs::add("explode_veh thread finished");
					if (thread_state::explode_veh_st.thread) {
						if (thread_state::explode_veh_st.thread->joinable()) {
							thread_state::explode_veh_st.thread->join();
						}
						delete thread_state::explode_veh_st.thread;
						thread_state::explode_veh_st.thread = nullptr;
					}

					thread_state::explode_veh_st.is_running = false;
					thread_state::explode_veh_st.is_finished = false;
				}
			}
			// kill_everyone_st
			{
				if (thread_state::kill_everyone_st.is_finished && thread_state::kill_everyone_st.is_running) {
					logs::add("kill_everyone thread finished");
					if (thread_state::kill_everyone_st.thread) {
						if (thread_state::kill_everyone_st.thread->joinable()) {
							thread_state::kill_everyone_st.thread->join();
						}
						delete thread_state::kill_everyone_st.thread;
						thread_state::kill_everyone_st.thread = nullptr;
					}

					thread_state::kill_everyone_st.is_running = false;
					thread_state::kill_everyone_st.is_finished = false;
				}
			}
			// heavens_gate_st
			{
				if (thread_state::heavens_gate_st.is_finished && thread_state::heavens_gate_st.is_running) {
					logs::add("heavens_gate thread finished");
					if (thread_state::heavens_gate_st.thread) {
						if (thread_state::heavens_gate_st.thread->joinable()) {
							thread_state::heavens_gate_st.thread->join();
						}
						delete thread_state::heavens_gate_st.thread;
						thread_state::heavens_gate_st.thread = nullptr;
					}

					thread_state::heavens_gate_st.is_running = false;
					thread_state::heavens_gate_st.is_finished = false;
				}
			}
			// carspam_st
			{
				if (thread_state::carspam_st.is_finished && thread_state::carspam_st.is_running) {
					logs::add("carspam thread finished");
					if (thread_state::carspam_st.thread) {
						if (thread_state::carspam_st.thread->joinable()) {
							thread_state::carspam_st.thread->join();
						}
						delete thread_state::carspam_st.thread;
						thread_state::carspam_st.thread = nullptr;
					}

					thread_state::carspam_st.is_running = false;
					thread_state::carspam_st.is_finished = false;
				}
			}
			// worldtradecenter_st
			{
				if (thread_state::worldtradecenter_st.is_finished && thread_state::worldtradecenter_st.is_running) {
					logs::add("worldtradecenter thread finished");
					if (thread_state::worldtradecenter_st.thread) {
						if (thread_state::worldtradecenter_st.thread->joinable()) {
							thread_state::worldtradecenter_st.thread->join();
						}
						delete thread_state::worldtradecenter_st.thread;
						thread_state::worldtradecenter_st.thread = nullptr;
					}

					thread_state::worldtradecenter_st.is_running = false;
					thread_state::worldtradecenter_st.is_finished = false;
				}
			}

			std::this_thread::sleep_for(100ms);
		}

		//prevent running the thread after unloading thus crashing the game

		// explode_veh_st
		{
			if (thread_state::explode_veh_st.thread) {
				if (thread_state::explode_veh_st.thread->joinable()) {
					thread_state::explode_veh_st.thread->join();
				}
				delete thread_state::explode_veh_st.thread;
				thread_state::explode_veh_st.thread = nullptr;
			}
		}
		// kill_everyone_st
		{
			if (thread_state::kill_everyone_st.thread) {
				if (thread_state::kill_everyone_st.thread->joinable()) {
					thread_state::kill_everyone_st.thread->join();
				}
				delete thread_state::kill_everyone_st.thread;
				thread_state::kill_everyone_st.thread = nullptr;
			}
		}
		// heavens_gate_st
		{
			if (thread_state::heavens_gate_st.thread) {
				if (thread_state::heavens_gate_st.thread->joinable()) {
					thread_state::heavens_gate_st.thread->join();
				}
				delete thread_state::heavens_gate_st.thread;
				thread_state::heavens_gate_st.thread = nullptr;
			}
		}
		// carspam_st
		{
			if (thread_state::carspam_st.thread) {
				if (thread_state::carspam_st.thread->joinable()) {
					thread_state::carspam_st.thread->join();
				}
				delete thread_state::carspam_st.thread;
				thread_state::carspam_st.thread = nullptr;
			}
		}
		// worldtradecenter_st
		{
			if (thread_state::worldtradecenter_st.thread) {
				if (thread_state::worldtradecenter_st.thread->joinable()) {
					thread_state::worldtradecenter_st.thread->join();
				}
				delete thread_state::worldtradecenter_st.thread;
				thread_state::worldtradecenter_st.thread = nullptr;
			}
		}

		logs::add("stopping thread helper");
		return 0;
	}
}
#endif