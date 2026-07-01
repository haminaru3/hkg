#include "c_cursor_wrapper.h"

void c_cursor_wrapper::change_cursor(e_cursors type, bool value) {
	if (value) {
		ImGui::SetMouseCursor(type);
	}
}