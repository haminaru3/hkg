#include "c_ui_widgets.hpp"
#include "../c_fonts.h"

const char* const key_names[] = {
 "Unknown",
 "LMB",
 "RMB",
 "Cancel",
 "MMB",
 "MButton4",
 "MButton5",
 "Unknown",
 "Back",
 "Tab",
 "Unknown",
 "Unknown",
 "Clear",
 "Return",
 "Unknown",
 "Unknown",
 "Shift",
 "Ctrl",
 "Menu",
 "Pause",
 "CapsLock",
 "VK_KANA",
 "Unknown",
 "VK_JUNJA",
 "VK_FINAL",
 "VK_KANJI",
 "Unknown",
 "Escape",
 "VK_CONVERT",
 "VK_NONCONVERT",
 "VK_ACCEPT",
 "VK_MODECHANGE",
 "Space",
 "VK_PRIOR",
 "Next",
 "End",
 "Home",
 "Left",
 "Up",
 "Right",
 "Down",
 "Select",
 "Print",
 "VK_EXECUTE",
 "VK_SNAPSHOT",
 "Insert",
 "Delete",
 "Help",
 "0",
 "1",
 "2",
 "3",
 "4",
 "5",
 "6",
 "7",
 "8",
 "9",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "A",
 "B",
 "C",
 "D",
 "E",
 "F",
 "G",
 "H",
 "I",
 "J",
 "K",
 "L",
 "M",
 "N",
 "O",
 "P",
 "Q",
 "R",
 "S",
 "T",
 "U",
 "V",
 "W",
 "X",
 "Y",
 "Z",
 "LWin",
 "RWin",
 "VK_APPS",
 "Unknown",
 "VK_SLEEP",
 "NUMPAD0",
 "NUMPAD1",
 "NUMPAD2",
 "NUMPAD3",
 "NUMPAD4",
 "NUMPAD5",
 "NUMPAD6",
 "NUMPAD7",
 "NUMPAD8",
 "NUMPAD9",
 "MULTIPLY",
 "ADD",
 "SEPARATOR",
 "SUBTRACT",
 "DECIMAL",
 "DIVIDE",
 "F1",
 "F2",
 "F3",
 "F4",
 "F5",
 "F6",
 "F7",
 "F8",
 "F9",
 "F10",
 "F11",
 "F12",
 "F13",
 "F14",
 "F15",
 "F16",
 "F17",
 "F18",
 "F19",
 "F20",
 "F21",
 "F22",
 "F23",
 "F24",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "NumLock",
 "Scroll",
 "VK_OEM_NEC_EQUAL",
 "VK_OEM_FJ_MASSHOU",
 "VK_OEM_FJ_TOUROKU",
 "VK_OEM_FJ_LOYA",
 "VK_OEM_FJ_ROYA",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "Unknown",
 "LShift",
 "RShift",
 "LCtrl",
 "RCtrl",
 "LMenu",
 "RMenu"
};

struct s_keybind_animation {
    float buf_op = 0.f, size = 0.f;
};

struct s_keybind_element {
    s_keybind_animation anim;
    element_state_t state;
};

bool c_ui_widgets::keybind(const char* label, int* key, bool show_label)
{
    static fixed_containers::FixedUnorderedMap<ImGuiID, s_keybind_element, 512> _element;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiID id = window->GetID(label);
    ImGuiStyle* s = &ImGui::GetStyle();
    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;

    const float width = (ImGui::GetContentRegionMax().x - s->WindowPadding.x);
    const ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(width, 17.f));

    ImGui::ItemSize(ImRect(rect.Min, rect.Max + ImVec2(0, 2)));
    if (!ImGui::ItemAdd(rect, id))
        return false;

    char buf_display[64] = "None";

    bool value_changed = false;
    int k = *key;

    std::string active_key = "";
    active_key += key_names[*key];

    if (*key != 0 && g.ActiveId != id) {
        strcpy_s(buf_display, active_key.c_str());
    }
    else if (g.ActiveId == id) {
        strcpy_s(buf_display, "...");
    }

    _element.try_emplace(id, s_keybind_element());
    auto this_element = _element.find(id);
    s_keybind_animation* this_anim = &this_element->second.anim;
    element_state_t* this_state = &this_element->second.state;

    const ImVec2 label_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 12, buf_display);
    this_anim->size = ImLerp(this_anim->size, label_size.x, ImGui::GetIO().DeltaTime * 12.f);

    ImRect clickable(ImVec2(rect.Max.x - 8 - this_anim->size, rect.Min.y), rect.Max);
    this_state->hovered = ImGui::ItemHoverable(clickable, id);

    bool on_edit = (g.ActiveId == id || buf_display == "...");

    this_anim->buf_op = ImLerp(this_anim->buf_op, this_state->hovered ? 1.f : on_edit ? 0.2f : 0.7f, ImGui::GetIO().DeltaTime * 15.f);

    window->DrawList->AddRectFilled(clickable.Min - ImVec2(4, 0), clickable.Max + ImVec2(4, 0), ImColor(19 / 255.f, 19 / 255.f, 19 / 255.f, 1.f * s->Alpha), 4);
    window->DrawList->AddRect(clickable.Min - ImVec2(4, 0), clickable.Max + ImVec2(4, 0), ImColor(1.f, 1.f, 1.f, 0.05f * s->Alpha), 4);

    if (show_label) window->DrawList->AddText(rect.Min + ImVec2(0, 1), ImColor(1.f, 1.f, 1.f, 0.7f * s->Alpha), label);

    ImGui::PushClipRect(clickable.Min, clickable.Max - ImVec2(4,0), true);
    window->DrawList->AddText(ui_fonts[e_fonts::MEDIUM], 12, clickable.Min + ImVec2(4, 2),
        ImColor(1.0f, 1.0f, 1.0f, this_anim->buf_op * s->Alpha), buf_display);
    ImGui::PopClipRect();

    if (this_state->hovered && io.MouseClicked[0])
    {
        if (g.ActiveId != id) {
            memset(io.MouseDown, 0, sizeof(io.MouseDown));
            memset(io.KeysDown, 0, sizeof(io.KeysDown));
            *key = 0;
        }
        ImGui::SetActiveID(id, window);
        ImGui::FocusWindow(window);
    }
    else if (io.MouseClicked[0]) {
        // release focus when user click outside box
        if (g.ActiveId == id)
            ImGui::ClearActiveID();
    }

    if (g.ActiveId == id) {
        for (auto i = 0; i < 5; i++) {
            if (io.MouseDown[i]) {
                switch (i) {
                case 0:
                    k = 0x01;
                    break;
                case 1:
                    k = 0x02;
                    break;
                case 2:
                    k = 0x04;
                    break;
                case 3:
                    k = 0x05;
                    break;
                case 4:
                    k = 0x06;
                    break;
                }
                value_changed = true;
                ImGui::ClearActiveID();
            }
        }
        if (!value_changed) {
            for (auto i = 0x08; i <= 0xA5; i++) {
                if (io.KeysDown[i]) {
                    k = i;
                    value_changed = true;
                    ImGui::ClearActiveID();
                }
            }
        }

        if (ImGui::IsKeyPressedMap(ImGuiKey_Escape)) {
            *key = 0;
            ImGui::ClearActiveID();
        }
        else {
            *key = k;
        }
    }

    ui_cursor.change_cursor((g.ActiveId == id) ? e_cursors::input : this_state->hovered ? e_cursors::hand : e_cursors::default_c, (this_state->hovered));

    return value_changed;
}
