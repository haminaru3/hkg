#pragma once
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "c_fonts.h"
#include <map>

class c_ui_misc {
public: // wigets
	void watermark_legacy();
	void watermark_modern();
	void adm_widget();
	void fov_circles();
};