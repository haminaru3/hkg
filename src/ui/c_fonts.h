#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include <string>

enum e_fonts {
	MEDIUM, BOLD,
	ICONS14, ICONS18, ICONS48,
	ARROW, HINT,
	DECIMA
};

extern ImFont* ui_fonts[8];

namespace _ui_fonts {
	extern void init();
}

#define _calc_text_size(font, size, text) font->CalcTextSizeA(static_cast<float>(size), FLT_MAX, 0.0f, text);