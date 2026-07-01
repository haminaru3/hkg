#include "c_ui_tabs.hpp"

std::string format_cfg_time(std::time_t time) {
    std::tm* tm = std::localtime(&time);
    std::ostringstream oss;

    oss << std::put_time(tm, "%I:%M %p | %B %d, %Y");

    return oss.str();
}

std::time_t to_time_t(std::filesystem::file_time_type ftime) {
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sctp);
}

static std::string trim_config_name(const char* value) {
    std::string result = value ? value : "";
    const auto first = result.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};

    const auto last = result.find_last_not_of(" \t\r\n");
    result = result.substr(first, last - first + 1);
    if (result.size() > 41) result.resize(41);
    return result;
}

void c_ui_tabs::configs_tab() {
    static char cfg_name_buf[42] = "";

    cfg_manager::ensure_config_dir();

	ui_widgets.misc.subtab_frame();

    ImGui::BeginGroup(); { 
        ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 2.5f);
        ImGui::BeginChild(str_hash_id("cfg-frame-inputbox"), {230, 30}); {
            ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 2.5f);

            ui_widgets.input_text("Config Name...", cfg_name_buf, sizeof(cfg_name_buf));

        } ImGui::EndChild(); ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 22);

        if (ui_widgets.button("Create New", { 85, 30 })) {
            const std::string config_name = trim_config_name(cfg_name_buf);
            if (!config_name.empty()) {
                if (!cfg_manager::save_config(config_name)) {
                    MessageBoxA(0, "failed to create config.", "=(", MB_OK | MB_ICONERROR);
                }
                else {
                    std::snprintf(cfg_name_buf, sizeof(cfg_name_buf), "%s", config_name.c_str());
                }
            }
        }
        ImGui::SameLine();
        if (ui_widgets.button("Open Configs Folder", { 160, 30 })) {
            ShellExecuteA(NULL, "open", CONFIG_DIR_PATH, NULL, NULL, SW_SHOWDEFAULT);
        }
    } ImGui::EndGroup();

    ImGui::SetCursorPos({ CORTEX_CONTENT_X, CORTEX_CONTENT_Y });
    ImGui::BeginChild(str_hash_id("cfg-block-content"), { CORTEX_SUBTAB_W,CORTEX_COL_H }, false); {
        struct cfg_list_entry {
            std::string filename;
            std::string edit_time;
        };

        std::vector<cfg_list_entry> cfg_entries;
        std::error_code ec;
        if (std::filesystem::exists(CONFIGS_PATH, ec) && !ec) {
            for (const auto& entry : std::filesystem::directory_iterator(CONFIGS_PATH, ec)) {
                if (ec) break;

                const auto status = entry.status(ec);
                if (ec || !std::filesystem::is_regular_file(status)) {
                    ec.clear();
                    continue;
                }

                const std::string filename = entry.path().filename().string();
                if (!cfg_manager::is_config_filename(filename)) continue;

                const auto ftime = std::filesystem::last_write_time(entry, ec);
                if (ec) {
                    ec.clear();
                    continue;
                }

                cfg_entries.push_back({ filename, format_cfg_time(to_time_t(ftime)) });
            }
        }

        int cfg_id = 0;
        for (const auto& cfg : cfg_entries) {
            ui_widgets.misc.cfg_element(cfg.filename, cfg.edit_time, ++cfg_id);
        }
    } ImGui::EndChild();
}
