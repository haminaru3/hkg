#include "c_ui.h"

#include "c_fonts.h"
#include "widgets/c_ui_widgets.hpp"
#include "content/c_ui_tabs.hpp"
#include "c_ui_misc.h"
#include "interface_variant.hpp"

#include "../protection/prot_defs.h"
#include "../includes/hash_str.hpp"
#include "../menu_background.hpp"

#include <chrono>

#define _rndm_mainwnd_id "##mainwnd"

static c_ui_widgets ui_w; static c_ui_tabs ui_tabs; static c_ui_misc ui_misc;

static const char* menu_title_text() {
    if (g_config && !g_config->misc.menu_title.empty()) {
        return g_config->misc.menu_title.c_str();
    }

    return "Cortex";
}

void type_text_anim(const char* full_text, const char* static_text, ImVec2 text_pos, float duration, float delay, bool& animation_finished)
{
//#ifndef _DEBUG
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    static float start_time = ImGui::GetTime(); // Remember the start time of the animation
    static bool removing = false; // State of the animation (typing or removing)
    static bool delay_started = false; // State to check if the delay after completion has started
    static bool reverse_delay_started = false; // State to check if the delay before reverse animation has started
    static float delay_start_time = 0.0f; // Time when delay after completion started
    static float reverse_delay_start_time = 0.0f; // Time when delay before reverse animation started
    float current_time = ImGui::GetTime();
    float elapsed_time = current_time - start_time;
    int text_length = strlen(full_text);

    // Calculate the number of characters to display
    int chars_to_show = static_cast<int>(ImClamp((elapsed_time / duration) * text_length, 0.0f, static_cast<float>(text_length)));

    if (removing) {
        chars_to_show = text_length - chars_to_show; // Reverse the character count for removal
    }

    // Create a string that contains only a part of the text
    char display_text[256];
    strncpy(display_text, full_text, chars_to_show);
    display_text[chars_to_show] = '\0'; // Add null terminator

    // Calculate alpha for smooth appearance and disappearance of text
    float alpha = ImClamp(elapsed_time / duration, 0.0f, 1.0f);
    if (removing) {
        alpha = 1.0f - alpha; // Reverse alpha for removal
    }
    ImVec4 text_color = ImVec4(200 / 255.f, 200 / 255.f, 200 / 255.f, alpha);

    // Draw the main text with appearance animation
    ImGui::GetForegroundDrawList()->AddText(text_pos + ImVec2(8, 0), ImGui::GetColorU32(text_color), display_text);

    auto static_size = ImGui::CalcTextSize(static_text);

    window->DrawList->PushClipRect(text_pos - ImVec2(static_size.x + 8, 0), text_pos + ImVec2(-8, static_size.y));
    ImVec2 static_text_pos = ImVec2(text_pos.x - (static_size.x * alpha) - 8, text_pos.y);
    window->DrawList->AddText(static_text_pos, ImColor(255, 255, 255, static_cast<int>(alpha * 255)), static_text);
    window->DrawList->PopClipRect();

    window->DrawList->AddLine(ImVec2(text_pos.x, text_pos.y - 3), ImVec2(text_pos.x, text_pos.y + 16), ImColor(255, 255, 255, static_cast<int>(alpha * 255)));

    // Check if the typing animation is complete
    if (!removing && elapsed_time > duration)
    {
        if (!reverse_delay_started)
        {
            // Start delay before reverse animation
            reverse_delay_start_time = ImGui::GetTime();
            reverse_delay_started = true;
        }
        else if (current_time - reverse_delay_start_time >= 1.0f) // 1 second delay
        {
            // Start fade-out animation after the delay
            removing = true;
            start_time = ImGui::GetTime(); // Reset start time for fade-out
            elapsed_time = 0.0f; // Reset elapsed time for removal
        }
    }

    // Check if the fade-out animation is complete
    if (removing && elapsed_time > duration)
    {
        // Start delay after fade-out animation
        if (!delay_started)
        {
            delay_start_time = ImGui::GetTime();
            delay_started = true;
        }

        // Check if the delay period has passed
        if (current_time - delay_start_time >= delay)
        {
            // Set animation finished flag
            animation_finished = true;
            g_config->controls.menu_state = true;
        }
    }
//#else
    //animation_finished = true;
//#endif
}

void c_userinterface::init() {
    _ui_fonts::init();
}

void c_userinterface::bg_color() {
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(windowSize);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.f });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, 0.f });
    ImGui::Begin("##menu-bg", NULL, ImGuiWindowFlags_NoDecoration | ImGuiButtonFlags_NoNavFocus | ImGuiWindowFlags_NoMouseInputs);

    ImVec2 pos = ImGui::GetWindowPos();
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImGuiStyle* s = &ImGui::GetStyle();
    if (g_config->misc.background.bg_fill) draw->AddRectFilled({ 0,0 }, windowSize, ImColor(0.f, 0.f, 0.f, (g_config->misc.background.bg_alpha / 100) * s->Alpha)); //s->Alpha

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    if (!start_animation) {
        ImGui::PushFont(ui_fonts[e_fonts::DECIMA]);
        std::string init_title = std::string("> ") + menu_title_text();
        type_text_anim(m_crypt("successfully initialized."), init_title.c_str(), ImVec2(windowSize.x / 2, windowSize.y / 2), 1.f, 1.f, start_animation);
        ImGui::PopFont();
    }

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(3);

    ImGui::End();
}

void c_userinterface::wigets_pool() {
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1);
    ((g_config->misc.wm_style == 1) ? ui_misc.watermark_legacy() : ui_misc.watermark_modern());

    //if (g_config->esp.radar.enabled && start_animation) ui_misc.radar();

    if (g_config->esp.admins.widget.enabled) ui_misc.adm_widget();

    //g_notify.draw();

    ImGui::PopStyleVar();
}

void c_userinterface::render_ui() {
    if (g_config->misc.clean_recording_mode && !g_config->controls.menu_state) return;

    begin_frame();
    if (menu_alpha > 0.f && start_animation) {
        ui_tabs.render_main();
    }
    wigets_pool();
    end_frame();
}

float animation_progress2 = 0.f, second_alpha = 0.f;

void c_userinterface::begin_frame() {
    bg_color();

    ui_misc.fov_circles();

    animation_progress = ImClamp(g_config->controls.menu_state ? 
        animation_progress + (ImGui::GetIO().DeltaTime * 4.0f) : animation_progress - (ImGui::GetIO().DeltaTime * 4.0f), 0.0f, 1.0f);

    menu_alpha = ImLerp(0.0f, 1.0f, animation_progress);

    animation_progress2 = ImClamp(g_config->controls.menu_state ?
        animation_progress2 + (ImGui::GetIO().DeltaTime * 3.0f) : animation_progress2 - (ImGui::GetIO().DeltaTime * 6.0f), 0.0f, 1.0f);

    second_alpha = ImLerp(0.0f, 1.0f, animation_progress2);

    if (menu_alpha > 0.f && start_animation) {
        ImGuiStyle* s = &ImGui::GetStyle();
        ImVec2 menu_size = ImVec2(CORTEX_MENU_W, CORTEX_MENU_H);

        ImGui::SetNextWindowSize(menu_size);
        ImGui::SetNextWindowPos((ImGui::GetIO().DisplaySize * 0.5f) - (menu_size * 0.5f), ImGuiCond_Once);

        s->Alpha = menu_alpha;
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1 * s->Alpha); // end_frame -- pop
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { menu_size.x * (1.f - s->Alpha), menu_size.y * (1.f - s->Alpha) }); // end_frame -- pop
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, second_alpha)); // end_frame -- pop

        ImGui::Begin(_rndm_mainwnd_id, nullptr, ImGuiWindowFlags_NoDecoration);

        auto draw = ImGui::GetWindowDrawList();
        auto pos = ImGui::GetWindowPos();
        auto size = ImGui::GetWindowSize();

        ImColor menu_bg = ImColor(16 / 255.f, 16 / 255.f, 16 / 255.f, s->Alpha);
#if CORTEX_UI_VARIANT == 1
        menu_bg = ImColor(7 / 255.f, 18 / 255.f, 22 / 255.f, 0.94f * s->Alpha);
#elif CORTEX_UI_VARIANT == 2
        menu_bg = ImColor(10 / 255.f, 11 / 255.f, 12 / 255.f, 0.98f * s->Alpha);
#elif CORTEX_UI_VARIANT == 3
        menu_bg = ImColor(14 / 255.f, 10 / 255.f, 13 / 255.f, 0.92f * s->Alpha);
#endif
        switch (g_config->misc.menu_style) {
        case 1: menu_bg = ImColor(10 / 255.f, 31 / 255.f, 55 / 255.f, s->Alpha); break;
        case 2: menu_bg = ImColor(38 / 255.f, 13 / 255.f, 18 / 255.f, s->Alpha); break;
        case 3: menu_bg = ImColor(14 / 255.f, 34 / 255.f, 27 / 255.f, s->Alpha); break;
        case 4: menu_bg = ImColor(18 / 255.f, 17 / 255.f, 16 / 255.f, s->Alpha); break;
        }

#if CORTEX_UI_VARIANT == 1
        draw->AddRectFilledMultiColor(pos, ImVec2(pos.x + size.x, pos.y + size.y),
            ImColor(8 / 255.f, 23 / 255.f, 27 / 255.f, s->Alpha),
            ImColor(23 / 255.f, 25 / 255.f, 33 / 255.f, s->Alpha),
            ImColor(11 / 255.f, 12 / 255.f, 16 / 255.f, s->Alpha),
            ImColor(11 / 255.f, 34 / 255.f, 35 / 255.f, s->Alpha));
        draw->AddRectFilled(ImVec2(pos.x + 22, pos.y + 82), ImVec2(pos.x + size.x - 22, pos.y + size.y - 24),
            ImColor(1.f, 1.f, 1.f, 0.025f * s->Alpha), 10.f);
#elif CORTEX_UI_VARIANT == 2
        draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), menu_bg, 6.f);
        draw->AddRectFilled(ImVec2(pos.x + 82, pos.y + 16), ImVec2(pos.x + size.x - 18, pos.y + size.y - 16),
            ImColor(25 / 255.f, 27 / 255.f, 29 / 255.f, 0.36f * s->Alpha), 6.f);
        for (int i = 0; i < 7; ++i) {
            const float y = pos.y + 84.f + (i * 62.f);
            draw->AddLine(ImVec2(pos.x + 98.f, y), ImVec2(pos.x + size.x - 36.f, y),
                ImColor(1.f, 1.f, 1.f, 0.025f * s->Alpha));
        }
#elif CORTEX_UI_VARIANT == 3
        if (g_config->misc.menu_background_photo && refresh_menu_background_texture(false) && g_menu_background_texture) {
            draw->AddImageRounded((ImTextureID)g_menu_background_texture, pos, ImVec2(pos.x + size.x, pos.y + size.y),
                ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 210), 9);
            draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                ImColor(6 / 255.f, 5 / 255.f, 7 / 255.f, 0.58f * s->Alpha), 9);
        }
        else {
            draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), menu_bg, 9.f);
        }
#else
        if (g_config->misc.menu_style == 4 && g_config->misc.menu_background_photo && refresh_menu_background_texture(false) && g_menu_background_texture) {
            draw->AddImageRounded((ImTextureID)g_menu_background_texture, pos, ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 170), 7);
            draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImColor(8 / 255.f, 8 / 255.f, 8 / 255.f, 0.62f * s->Alpha), 7);
        }
        else {
            draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), menu_bg, 7);
        }
#endif

        ui_w.misc.window_decorations();
    }
}

void c_userinterface::end_frame() {
    if (menu_alpha > 0.f && start_animation) {
        ImGui::End();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(1);
    }
}
