#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>

int notify_select = 0;
ImVec2 notification_position(15, 15); // Переменная для хранения положения уведомлений
void draw_progress_circle(const char* name, float progress, float max, float radius, const ImVec2& size, float alpha)
{
    const float thickness = 3.f;
    float position = (progress / max) * 6.28f;
    ImVec2 center = ImGui::GetCursorScreenPos() + size;

    auto draw_list = ImGui::GetForegroundDrawList();
    draw_list->PathClear();
    draw_list->PathArcTo(center, radius, 0.f, 2.f * IM_PI, 120);
    draw_list->PathStroke(ImColor(23, 23, 23, static_cast<int>(alpha * 255)), false, thickness);

    draw_list->AddCircle(center, radius + 3, ImColor(255, 255, 255, static_cast<int>(alpha * 22)), 120);
    draw_list->AddCircle(center, radius - 3, ImColor(255, 255, 255, static_cast<int>(alpha * 22)), 120);

    draw_list->PathClear();
    draw_list->PathArcTo(center, radius, IM_PI * 1.5f, IM_PI * 1.5f + position, 120);
    draw_list->PathStroke(ImColor(255, 255, 255, static_cast<int>(alpha * 255)), false, thickness);

    int percent = static_cast<int>((progress / max) * 100);
    std::string percent_str = std::to_string(percent) + "%";
}
struct s_notify_base {
    int id;
    std::string message;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
};
class c_notifymanager {
public:
    c_notifymanager() : notificationIdCounter(0) {}

    void spawn(const std::string& message, int durationMs) {
        auto now = std::chrono::steady_clock::now();
        notifications.push_back({ notificationIdCounter++, message, now, now + std::chrono::milliseconds(durationMs) });
    }

    void draw() {
        auto now = std::chrono::steady_clock::now();
        notifications.erase(std::remove_if(notifications.begin(), notifications.end(),
            [now](const s_notify_base& notification) { return now >= notification.endTime; }), notifications.end());

        // Сортировка по времени создания
        std::sort(notifications.begin(), notifications.end(), [](const s_notify_base& a, const s_notify_base& b) {
            return a.startTime < b.startTime;
            });

        int currentNotificationPosition = 0;
        for (auto& notification : notifications) {
            if (now < notification.endTime) {
                float duration = std::chrono::duration_cast<std::chrono::milliseconds>(notification.endTime - notification.startTime).count();
                float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - notification.startTime).count();
                float percentage = (duration - elapsed) / duration * 100.0f;
                draw_notification(currentNotificationPosition++, notification.message, percentage, elapsed, duration);
            }
        }
    }

private:
    std::vector<s_notify_base> notifications;
    int notificationIdCounter;

    void draw_notification(int position, const std::string& message, float percentage, float elapsed, float duration) {
        auto draw_list = ImGui::GetForegroundDrawList();
        float alpha = percentage > 10.0f ? 1.0f : percentage / 10.0f;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 15,10 });

        float slide_in_progress = (std::min)(elapsed / 200.0f, 1.0f);
        float slide_out_progress = (std::max)(percentage - 10.0f, 0.0f) / 10.0f;

        float offset = (1.0f - slide_in_progress) * 42;
        ImVec2 window_pos = notification_position + ImVec2(0, position * (42 + 8) + offset * -1);

        ImGui::SetNextWindowPos(window_pos);

        ImGui::Begin(("notify" + std::to_string(position)).c_str(), nullptr, 
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | 
            ImGuiWindowFlags_NoBackground);

        ImVec2 pos = ImGui::GetWindowPos(), region = ImGui::GetContentRegionMax();
        draw_list->AddRectFilled(pos, pos + region, ImColor(19, 19, 19, static_cast<int>(alpha * 255)), 8);
        draw_list->AddRect(pos, pos + region, ImColor(255, 255, 255, static_cast<int>(alpha * 30)), 8);

        float duePercentage = 100.0f - percentage;
        if (notify_select == 0)
            draw_progress_circle("circle", percentage, 100, 7.f, ImVec2(6, 11), alpha);
        else if (notify_select == 1)
            draw_list->AddRectFilled(pos + ImVec2(5.f, region.y - 3),
                pos + ImVec2((region.x - 5.f) * (duePercentage / 100.0f), region.y - 3),
                ImColor(1.f, 1.f, 1.f, alpha), 8, ImDrawFlags_RoundCornersBottom);

        if (notify_select == 0) ImGui::SetCursorPosX(38);
        auto text_size = ImGui::CalcTextSize(message.c_str());
        //ImGui::TextColored(ImColor(1.f, 1.f, 1.f, alpha), "%s", message.c_str());
        draw_list->AddText(window_pos + ImVec2(((notify_select == 0) ? 36 : 24), 12), ImColor(1.f, 1.f, 1.f, alpha), message.c_str());
        ImGui::Dummy(ImVec2(text_size.x + ((notify_select == 0) ? 36 : 15), text_size.y + 15));

        ImGui::End();
        ImGui::PopStyleVar(1);
    }
};
c_notifymanager g_notify;