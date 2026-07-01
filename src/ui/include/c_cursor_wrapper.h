#pragma once
#include "imgui.h"

enum e_cursors {
	default_c = ImGuiMouseCursor_Arrow,
	hand = ImGuiMouseCursor_Hand,
	input = ImGuiMouseCursor_TextInput,
	horizontal_resize = ImGuiMouseCursor_ResizeEW,
	not_allowed = ImGuiMouseCursor_NotAllowed
};

class c_cursor_wrapper {
public:
	void change_cursor(e_cursors type, bool value);
};

static c_cursor_wrapper ui_cursor;