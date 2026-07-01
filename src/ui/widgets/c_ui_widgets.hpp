#pragma once
#include <fixed_containers/fixed_unordered_map.hpp>
#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include "../includes/hash_str.hpp"
#include "../include/c_cursor_wrapper.h"


struct element_state_t {
	bool hovered, pressed, held;
};

class c_ui_widgets {
public:
	bool checkbox(const char* label, bool* v);
	bool keybind(const char* label, int* key, bool show_label = false);

	bool begin_combo(const char* label, const char* preview_value, ImGuiComboFlags flags);
	bool combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items);
	bool combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);

	bool slider_scalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags);

	bool slider_int(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
	bool slider_float(const char* label, float* v, float v_min, float v_max, const char* format = "%.1f", ImGuiSliderFlags flags = 0);

	static bool button(const char* label, const ImVec2& size_arg);
	void popup_key(const char* popul_label, const char* checkbox_label, bool* value_checkbox, int* value_keybind);

	bool selectable(const char* label, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
	bool selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));

	bool item_selectable(const char* label, const char* hash, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
	bool item_selectable(const char* label, const char* hash, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));

	bool input_text_ex(const char* label, const char* hint, char* buf, int buf_size, const ImVec2& size_arg, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	bool input_text(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

	struct s_ui_tabs {
		bool main(const char* icon, const char* label, bool boolean);
		bool sub(const char* label, bool boolean);
	} tab;

	struct s_ui_child {
		bool begin_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, ImGuiWindowFlags flags);
		bool begin(const char* str_id, const ImVec2& size = ImVec2(0, 0), ImGuiWindowFlags flags = 0);
		bool begin(ImGuiID id, const ImVec2& size = ImVec2(0, 0), ImGuiWindowFlags flags = 0);
		void end();
	} child;

	struct s_ui_popups {
		static bool begin_settings(const char* popup_label, float x_size, bool label = false, ImVec2 custom_size_arg = { 24.f, 16.f });
		static void end_settings();
	} popup;

	struct s_ui_cpickers {
		bool edit_4(const char* label, float col[4], ImGuiColorEditFlags flags);
		bool edit_4(const char* label, std::array<float, 4>& col, ImGuiColorEditFlags flags);
	} color;

	struct s_ui_misc {
		void window_decorations();
		void subtab_frame();
		void cfg_ex_render(std::string label, std::string f_upd_date);
		void cfg_element(std::string filename, std::string edit_date, int cfg_id);
		void in_dev_frame();
	} misc;
private:
	struct clickable_element {
		ImGuiID id;
		bool hovered, pressed, held;
		clickable_element(const ImGuiID _id)
			: id(_id), hovered(false), pressed(false), held(false) {}
	};



public:
	void checkbox_bind(const char* label_checkbox, bool* value_checkbox, int* value_keybind) {
		const char* bindtext = "_bind";
		char buffer[64];
		strncpy(buffer, label_checkbox, sizeof(buffer));
		strncat(buffer, bindtext, sizeof(buffer));

		checkbox(label_checkbox, value_checkbox);
		ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(38, 26));
		keybind(buffer, value_keybind, false);
		ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, 2));
	}
	void checkbox_cpicker(const char* label_checkbox, bool* value_checkbox, const char* label_picker, float col[4]) {

		checkbox(label_checkbox, value_checkbox);
		ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(33, 24));
		this->color.edit_4(label_picker, col, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoLabel);
		ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, 2));
	}
	void checkbox_cpicker(const char* label_checkbox, bool* value_checkbox, const char* label_picker, std::array<float, 4>& colEx) {
		checkbox(label_checkbox, value_checkbox);
		ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(33, 24));
		this->color.edit_4(label_picker, colEx.data(), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoLabel);
		ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, 2));
	}
};

