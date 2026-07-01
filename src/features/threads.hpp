#pragma once
#include "../imports.h"
#include <thread>
#include <functional>
#include "fixed_containers/fixed_unordered_map.hpp"

// i want to kms after this shit please never touch this shit if it works
// s/o потрошитель best codestyle ever

//namespace threading {
//	fixed_containers::FixedUnorderedMap<std::thread::id, std::thread, 8192> threads; // compiletime destroyer
//
//	//DWORD WINAPI runner(LPVOID) {
//	//	std::cout << "threading runner init" << std::endl;
//	//	while (Game.running) {
//
//	//		for (size_t i = 0; i < threads.size(); i++) {
//	//			if (threads.at(i)) {
//	//				if (threads[i].second.joinable()) {
//	//					threads[i].second.join();
//	//					std::cout << "helo i am ezoterik and i helped thread " << threads[i].second.get_id() << " commit suecide. he should be ded and no foto v trysah" << std::endl;
//	//					threads.erase(threads.begin() + i); // hope this works
//	//				}
//	//			}
//	//		}
//
//	//		std::this_thread::sleep_for(20ms);
//	//	}
//	//	
//	//	std::cout << "exit threading helper" << std::endl;
//	//	return 0;
//	//}
//
//	void kill_myself(std::thread::id thrid) { // runtime destroyer fr
//		if (threading::threads[thrid].joinable()) {
//			threading::threads[thrid].join();
//			std::cout << "helo i am ezoterik meneger with thread number " << std::this_thread::get_id() << "  and i em commiting suecide right now wish me lak" << std::endl;
//		}
//	}
//};
