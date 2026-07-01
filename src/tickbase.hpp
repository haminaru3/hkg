#pragma once
#include "imports.h"

namespace tickbase {
	struct tickchrono {
		uint64_t tick;
		ULONGLONG globalms;
	};

	std::vector<tickchrono> ticks;

	void new_tick() {
		if (ticks.size() == 0) {
			ticks.push_back({ 1, GetTickCount64()});
		}
		else {
			ticks.push_back({ ticks.back().tick + 1, GetTickCount64()});
		}
		
	}

	int get_since_last_tick() {
		if (ticks.size() < 2) {
			return 0;
		}
		return GetTickCount64() - ticks[ticks.size() - 2].globalms;
	}
}