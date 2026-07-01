#include "c_ui_misc.h"
#include "../c_config.hpp"
#include "c_ui.h"

#include "c_fonts.h"

#include "../protection/prot_defs.h"
#include "../hash_str.hpp"

//void c_ui_misc::hint(const char* title, const char* desc, ImVec2 cur_pos) {
//    ImGuiWindow* window = ImGui::GetCurrentWindow();
//    const ImGuiID id = window->GetID(desc);
//    static std::map <ImGuiID, hint_popup_element> anim;
//    auto it_anim = anim.find(id);
//    if (it_anim == anim.end()) {
//        anim.insert({ id, { 0.0f } });
//        it_anim = anim.find(id);
//    }
//
//    ImGui::SetCursorPos(cur_pos);
//    ImVec2 pos = window->DC.CursorPos;
//
//    ImGui::PushFont(l_hint);
//    ImGui::TextColored(ImColor(1.f, 1.f, 1.f, it_anim->second.hint_symbol_opacity), "A");
//    ImGui::PopFont();
//
//    bool hovered, held;
//    const ImRect rect(pos, ImVec2(pos.x + 16, pos.y + 16));
//    bool pressed = ImGui::ButtonBehavior(rect, id, &hovered, &held, NULL);
//
//    it_anim->second.hint_symbol_opacity = ImLerp(it_anim->second.hint_symbol_opacity, (hovered ? 0.8f : 0.4f), 0.07f * (1.0f - ImGui::GetIO().DeltaTime));
//    it_anim->second.tooltip_opacity = ImLerp(it_anim->second.tooltip_opacity, (hovered ? 1.f : 0.0f), 0.3f * (1.0f - ImGui::GetIO().DeltaTime));
//
//    if (it_anim->second.tooltip_opacity > 0.01f) {
//        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
//        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, it_anim->second.tooltip_opacity);
//        ImGui::BeginTooltip();
//        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
//        ImGui::TextColored(ImColor(1.f, 1.f, 1.f, 0.8f), title);
//        ImGui::TextColored(ImColor(1.f, 1.f, 1.f, 0.4f), desc);
//        ImGui::PopTextWrapPos();
//        ImGui::EndTooltip();
//        ImGui::PopStyleVar(2);
//    }
//}

float get_fps() {
    static int frameCount = 0;
    static float timeElapsed = 0.0f;
    static float fps = 0.0f;

    frameCount++;
    timeElapsed += ImGui::GetIO().DeltaTime;

    if (timeElapsed >= 1.0f) {
        fps = (float)frameCount / timeElapsed;
        frameCount = 0;
        timeElapsed = 0.0f;
    }

    return fps / 2;
}

void c_ui_misc::watermark_legacy() {
    if (!g_config->misc.watermark) return;
    const char* title_text = !g_config->misc.menu_title.empty() ? g_config->misc.menu_title.c_str() : "Cortex";
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
    ImGui::SetNextWindowPos({ 24.f, ImGui::GetIO().DisplaySize.y * 0.4f }, ImGuiCond_Once);

    ImGui::Begin("##watermark", nullptr, ImGuiWindowFlags_NoDecoration); {
        auto draw = ImGui::GetWindowDrawList();
        auto pos = ImGui::GetWindowPos();
        auto size = ImGui::GetWindowSize();

        float min_col = 90 / 255.f;

        float wmaccent[3] = { g_config->misc.wm_accent[0] < min_col ? min_col : g_config->misc.wm_accent[0],
                              g_config->misc.wm_accent[1] < min_col ? min_col : g_config->misc.wm_accent[1],
                              g_config->misc.wm_accent[2] < min_col ? min_col : g_config->misc.wm_accent[2], };

        char user_fps[6]; sprintf(user_fps, "%.0f", get_fps());

        auto chname_size = _calc_text_size(ui_fonts[e_fonts::BOLD], 14, title_text);
        auto username_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, "alpha");
        auto fps_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, user_fps);
        auto text_center_y = (size.y / 2 - (chname_size.y / 2));

        auto wm_size_x = chname_size.x + username_size.x + 64 + fps_size.x;

        ImGui::SetWindowSize({ wm_size_x,30 });

        // logo on background
        draw->AddText(ui_fonts[e_fonts::ICONS48], 40.0f, pos - ImVec2(-6, 4), ImColor(1.f * wmaccent[0], 1.f * wmaccent[1], 1.f * wmaccent[2], 45 / 255.f), "A");

        // cheat name
        draw->AddText(ui_fonts[e_fonts::BOLD], 14.0f, pos + ImVec2(8, text_center_y), ImColor(1.f * wmaccent[0], 1.f * wmaccent[1], 1.f * wmaccent[2], 1.f), title_text);

        // line one
        draw->AddLine(ImVec2(pos.x + chname_size.x + 12, pos.y + 8),
            ImVec2(pos.x + chname_size.x + 12, pos.y + 26), ImColor(255, 255, 255, 30));

        // username
        draw->AddText(ui_fonts[e_fonts::MEDIUM], 14.0f, pos + ImVec2(chname_size.x + 20, text_center_y), ImColor(255, 255, 255, 170), "alpha");

        // line two
        draw->AddLine(ImVec2(pos.x + chname_size.x + username_size.x + 26, pos.y + 8),
            ImVec2(pos.x + chname_size.x + username_size.x + 26, pos.y + 26), ImColor(255, 255, 255, 30));

        draw->AddText(ui_fonts[e_fonts::MEDIUM], 14.0f, pos + ImVec2(chname_size.x + username_size.x + 32, text_center_y), ImColor(255, 255, 255, 200), user_fps);
        draw->AddText(ui_fonts[e_fonts::MEDIUM], 14.0f, pos + ImVec2(chname_size.x + username_size.x + 36 + fps_size.x, text_center_y), ImColor(255, 255, 255, 70), "fps");

        // frame outline
        draw->AddRect({ pos.x + 2, pos.y + 2 }, pos + ImVec2(size.x - 2, size.y - 2), ImColor(0, 0, 0, 255), 6);
        draw->AddRect({ pos.x + 1, pos.y + 1 }, pos + ImVec2(size.x - 1, size.y - 1), ImColor(19, 19, 19, 255), 6);
        draw->AddRect(pos, pos + size, ImColor(19, 19, 19, 255), 6);
        draw->AddRect({ pos.x + 1, pos.y + 1 }, pos + ImVec2(size.x - 1, size.y - 1), ImColor(255, 255, 255, 30), 6);

    } ImGui::End();
    ImGui::PopStyleVar(2);
}

const char** get_time_for_wm() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    static char timeStr[20];
    static char dayStr[10];

    std::ostringstream timeOss;
    timeOss << (now->tm_hour % 12 == 0 ? 12 : now->tm_hour % 12) << ":"
        << std::setfill('0') << std::setw(2) << now->tm_min << " "
        << (now->tm_hour < 12 ? "AM " : "PM ");

    std::strncpy(timeStr, timeOss.str().c_str(), sizeof(timeStr));
    timeStr[sizeof(timeStr) - 1] = '\0';

    const char* daysOfWeek[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

    std::strncpy(dayStr, daysOfWeek[now->tm_wday], sizeof(dayStr));
    dayStr[sizeof(dayStr) - 1] = '\0';

    static const char* result[2];
    result[0] = timeStr;
    result[1] = dayStr;

    return result;
}

void c_ui_misc::watermark_modern() {
    const char* title_text = !g_config->misc.menu_title.empty() ? g_config->misc.menu_title.c_str() : "Cortex";
    static float watermark_anim;
    watermark_anim = ImLerp(watermark_anim, (g_config->misc.watermark && g_config->misc.wm_style == 0)
        ? 1.0f : 0.0f, (g_config->misc.watermark && g_config->misc.wm_style == 0) ? ImGui::GetIO().DeltaTime * 12.f : ImGui::GetIO().DeltaTime * 15.f);

    if (watermark_anim > 0.01f) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
        ImGui::SetNextWindowPos({ 24.f, ImGui::GetIO().DisplaySize.y * 0.4f }, ImGuiCond_Once);

        ImGui::Begin("##watermark2", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground); {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            ImDrawList* draw = window->DrawList;
            ImVec2 pos = window->Pos;
            ImVec2 size = window->Size;

            const char* username = "alpha";
            char user_fps[8]; sprintf(user_fps, "%.0f fps", get_fps());
            const char** cur_timedate = get_time_for_wm();

            float min_col = 90 / 255.f;

            float wmaccent[3] = { g_config->misc.wm_accent[0] < min_col ? min_col : g_config->misc.wm_accent[0],
                                  g_config->misc.wm_accent[1] < min_col ? min_col : g_config->misc.wm_accent[1],
                                  g_config->misc.wm_accent[2] < min_col ? min_col : g_config->misc.wm_accent[2], };

            ImVec2 username_size = _calc_text_size(ui_fonts[e_fonts::BOLD], 14, username);
            ImVec2 fps_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, user_fps);
            ImVec2 time_size = _calc_text_size(ui_fonts[e_fonts::BOLD], 14, cur_timedate[0]);
            ImVec2 day_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 14, cur_timedate[1]);

            auto wm_size_x = 142 + username_size.x + 40 + fps_size.x + 16 + (time_size.x + day_size.x) + 16;

            ImGui::SetWindowSize({ wm_size_x * watermark_anim, 37 });

            ImColor bg_col = ImColor(1.f, 1.f, 1.f, 40 / 255.f * watermark_anim);
            ImColor text_col = ImColor(1.f, 1.f, 1.f, 1.f * watermark_anim);
            ImColor half_text_col = ImColor(1.f, 1.f, 1.f, 0.7f * watermark_anim);
            ImColor block_col = ImColor(19 / 255.f, 19 / 255.f, 19 / 255.f, 1.f * watermark_anim);
            ImColor half_block_col = ImColor(0.f, 0.f, 0.f, 120 / 255.f * watermark_anim);

            if (g_config->misc.wm_bg) draw->AddRectFilled(pos, pos + ImVec2(wm_size_x, 37), bg_col, 12.f); // bg

            draw->AddRectFilled(pos + ImVec2(8, 6), pos + ImVec2(8 + 42, 6 + 25), half_block_col, 6.f, ImDrawFlags_RoundCornersLeft); // logo bg
            draw->AddRectFilled(pos + ImVec2(39, 6), pos + ImVec2(39 + 99, 6 + 25), block_col, 6.f); // bg Cortex

            draw->AddRectFilled(pos + ImVec2(142 + username_size.x + 16, 6),
                pos + ImVec2(142 + username_size.x + 40 + fps_size.x, 6 + 25), half_block_col, 6.f); // bg fps

            draw->AddRectFilled(pos + ImVec2(142, 6), pos + ImVec2(142 + username_size.x + 24, 6 + 25), block_col, 6.f); // bg uname

            draw->AddRectFilled(pos + ImVec2(142 + username_size.x + 40 + fps_size.x + 4, 6),
                pos + ImVec2(142 + username_size.x + 40 + fps_size.x + 16 + time_size.x + day_size.x + 8, 6 + 25), block_col, 6.f); // bg date


            draw->AddText(ui_fonts[e_fonts::ICONS18], 18.f, pos + ImVec2(15, 9), text_col, "A");
            draw->AddText(ui_fonts[e_fonts::BOLD], 14.f, pos + ImVec2(50, 11), text_col, title_text);

            draw->AddText(ui_fonts[e_fonts::BOLD], 14.f, pos + ImVec2(154, 11), text_col, username);
            draw->AddText(ui_fonts[e_fonts::MEDIUM], 14.f, pos + ImVec2(142 + username_size.x + 32, 11), half_text_col, user_fps);

            draw->AddText(ui_fonts[e_fonts::BOLD], 14.f, pos + ImVec2(142 + username_size.x + 40 + fps_size.x + 16, 11), text_col, cur_timedate[0]);
            draw->AddText(ui_fonts[e_fonts::MEDIUM], 14.f, pos + ImVec2(142 + username_size.x + 40 + fps_size.x + 16 + time_size.x, 11), half_text_col, cur_timedate[1]);
        } ImGui::End();
        ImGui::PopStyleVar(1);
    }
}

void c_ui_misc::adm_widget() {
    if (cheat_base->admin_counter > 0 || g_config->esp.admins.widget.preview) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
        ImGui::Begin("##adm_warning", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground); {
            auto pos = ImGui::GetWindowPos();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            auto text = std::format("Admins are watching you: {}", cheat_base->admin_counter);
            auto text_size = _calc_text_size(ui_fonts[e_fonts::MEDIUM], 13, text.c_str());

            ImGui::SetWindowSize({ text_size.x + 8, text_size.y + 6 });

            draw_list->AddText(ui_fonts[e_fonts::MEDIUM], 13.0f, pos + ImVec2(3, 4), ImColor(0, 0, 0, 180), text.c_str());
            draw_list->AddText(ui_fonts[e_fonts::MEDIUM], 13.0f, pos + ImVec2(2, 3), ImColor(255, 255, 255, 230), text.c_str());

        } ImGui::End();
        ImGui::PopStyleVar(1);
    }
}

void c_ui_misc::fov_circles() {
    static float fov_rad_anim_s = 0.f, fov_rad_anim_v = 0.f;
    ImVec2* wnd_size = &ImGui::GetIO().DisplaySize;
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    if (g_config->aimbot.vector.visual.visualize_fov && g_config->aimbot.vector.enabled) {
        ImColor col_out(g_config->aimbot.vector.visual.outlinefov[0], g_config->aimbot.vector.visual.outlinefov[1], g_config->aimbot.vector.visual.outlinefov[2], g_config->aimbot.vector.visual.outlinefov[3]);
        ImColor col_fill(g_config->aimbot.vector.visual.filledfov[0], g_config->aimbot.vector.visual.filledfov[1], g_config->aimbot.vector.visual.filledfov[2], g_config->aimbot.vector.visual.filledfov[3]);

        fov_rad_anim_v = ImLerp(fov_rad_anim_v, static_cast<float>(g_config->aimbot.vector.field_of_view), ImGui::GetIO().DeltaTime * 14.f);

        switch (g_config->aimbot.vector.visual.fov_type)
        {
        case 0:
            dl->AddCircle(ImVec2(wnd_size->x / 2, wnd_size->y / 2), fov_rad_anim_v, col_out, 512, 1);
            break;
        case 1:
            dl->AddCircleFilled(ImVec2(wnd_size->x / 2, wnd_size->y / 2), fov_rad_anim_v, col_fill, 512);
            break;
        case 2:
            dl->AddCircleFilled(ImVec2(wnd_size->x / 2, wnd_size->y / 2), fov_rad_anim_v, col_fill, 512);
            dl->AddCircle(ImVec2(wnd_size->x / 2, wnd_size->y / 2), fov_rad_anim_v, col_out, 512, 1);
            break;
        }
    }

    if (g_config->aimbot.silent.visual.visualize_fov && g_config->aimbot.silent.enabled) {
        ImColor col_out(g_config->aimbot.silent.visual.outlinefov[0], g_config->aimbot.silent.visual.outlinefov[1], g_config->aimbot.silent.visual.outlinefov[2], g_config->aimbot.silent.visual.outlinefov[3]);
        ImColor col_fill(g_config->aimbot.silent.visual.filledfov[0], g_config->aimbot.silent.visual.filledfov[1], g_config->aimbot.silent.visual.filledfov[2], g_config->aimbot.silent.visual.filledfov[3]);

        fov_rad_anim_s = ImLerp(fov_rad_anim_s, static_cast<float>(g_config->aimbot.silent.field_of_view), ImGui::GetIO().DeltaTime * 14.f);

        switch (g_config->aimbot.silent.visual.fov_type)
        {
        case 0:
            dl->AddCircle(ImVec2(wnd_size->x / 2, wnd_size->y / 2), fov_rad_anim_s, col_out, 512, 1);
            break;
        case 1:
            dl->AddCircleFilled(ImVec2(wnd_size->x / 2, wnd_size->y / 2), fov_rad_anim_s, col_fill, 512);
            break;
        case 2:
            dl->AddCircleFilled(ImVec2(wnd_size->x / 2, wnd_size->y / 2), fov_rad_anim_s, col_fill, 512);
            dl->AddCircle(ImVec2(wnd_size->x / 2, wnd_size->y / 2), fov_rad_anim_s, col_out, 512, 1);
            break;
        }
    }
}
