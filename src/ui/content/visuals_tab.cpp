#include "c_ui_tabs.hpp"
#include <fixed_containers/fixed_map.hpp>

struct s_visuals_sub {
    enum e_sub {
        PLAYERS,
        ADMINS,
        OBJECTS,
        VEHICLES,
        RADAR
    };

    e_sub current{ PLAYERS };

    std::vector<std::tuple<const char*, e_sub>> sub_arr = {
        {m_crypt("Players"), PLAYERS},
        {m_crypt("Admins"), ADMINS},
        {m_crypt("Objects"), OBJECTS},
        {m_crypt("Vehicles"), VEHICLES},
        {m_crypt("Radar"), RADAR}
    };
} visual_sub;

void c_ui_tabs::visuals_tab() {
    static DWORD selected_obj = -1;

    ui_widgets.misc.subtab_frame();
    ImGui::BeginGroup(); {
        for (const auto& [name, _id] : visual_sub.sub_arr) {
            if (ui_widgets.tab.sub(name, visual_sub.current == _id)) { visual_sub.current = _id; }
        }
    } ImGui::EndGroup();

    switch (visual_sub.current)
    {
        case visual_sub.e_sub::PLAYERS:
        //g_ui_misc.hint(m_crypt("Hint: Players ESP"), hints::players_esp_tab, hints::pos);

        _column_left("cplayers_1"); {
            ui_widgets.child.begin(m_crypt("General"), ImVec2(0, 230)); {
                ui_widgets.checkbox(m_crypt("Player ESP"), &g_config->esp.players.enabled);
                if (ui_widgets.popup.begin_settings(m_crypt("Players ESP Settings"), 220)) {
                    ui_widgets.slider_float(m_crypt("Max Distance"), &g_config->esp.players.max_dist, 1, 500, "%.0f m");
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox("Skeleton", &g_config->esp.players.skeleton);
                if (ui_widgets.popup.begin_settings(m_crypt("Skeleton Settings"), 200)) {
                    //ui_widgets.checkbox("Draw Hands", &g_config->esp.players.skeleton);
                    //ui_widgets.checkbox("Draw Feets", &g_config->esp.players.skeleton);
                    //ui_widgets.checkbox("Include Hands", &g_config->esp.players.skel_hands);
                    ui_widgets.color.edit_4(m_crypt("Visible Color"), g_config->esp.players.skeleton_visible_col, cp_flags_w_label);
                    ui_widgets.color.edit_4(m_crypt("Invisible Color"), g_config->esp.players.skeleton_invisible_col, cp_flags_w_label);
                    ui_widgets.slider_float(m_crypt("Thickness"), &g_config->esp.players.skeleton_thickness, 1.0f, 2.5f, "%.1fpx");
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Rect Box"), &g_config->esp.players.rect_box);
                if (ui_widgets.popup.begin_settings(m_crypt("Rect Box Settings"), 200)) {
                    ui_widgets.color.edit_4(m_crypt("Box Color"), g_config->esp.players.rect_box_col, cp_flags_w_label);
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Corners Box"), &g_config->esp.players.corners_box);
                if (ui_widgets.popup.begin_settings(m_crypt("Corners Box Settings"), 200)) {
                    ui_widgets.color.edit_4(m_crypt("Corners Color"), g_config->esp.players.corners_box_col, cp_flags_w_label);
                    ui_widgets.slider_float(m_crypt("Corners Size"), &g_config->esp.players.corners_size, 3.0f, 12.f);
                    //ui_widgets.slider_float(m_crypt("Thickness"), &g_config->esp.players.corners_thickness, 1.0f, 4.f, "%.1fpx");
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Fill Box"), &g_config->esp.players.fill_box);
                if (ui_widgets.popup.begin_settings(m_crypt("Fill Box Settings"), 220)) {

                    const char* ñolor_type[2] = { "Solid", "Gradient" };
                    ui_widgets.combo("Color Type", (int*)&g_config->esp.players.fill_box_col_type, ñolor_type, IM_ARRAYSIZE(ñolor_type));
                    if (g_config->esp.players.fill_box_col_type == 0 /* solid */) {
                        ui_widgets.color.edit_4(m_crypt("Fill Color"), g_config->esp.players.fill_box_solid_col, cp_flags_w_label);
                        ui_widgets.checkbox(m_crypt("On Lethal"), &g_config->esp.players.lethal_fill);
                        if (g_config->esp.players.lethal_fill) {
                            ui_widgets.color.edit_4(m_crypt("Lethal Color"), g_config->esp.players.lethal_fill_col, cp_flags_w_label);
                            ui_widgets.slider_int(m_crypt("Lethal Health"), &g_config->esp.players.lethal_health, 1.0f, 100.f, "%d%HP");
                        }
                    }
                    else if (g_config->esp.players.fill_box_col_type == 1 /* grd */) {
                        ui_widgets.color.edit_4(m_crypt("Gradient Start"), g_config->esp.players.fillbox_box_grd.col_upr, cp_flags_w_label);
                        ui_widgets.color.edit_4(m_crypt("Gradient End"), g_config->esp.players.fillbox_box_grd.col_bot, cp_flags_w_label);
                    }
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Snaplines"), &g_config->esp.players.snaplines);
                if (ui_widgets.popup.begin_settings(m_crypt("Snaplines Settings"), 220)) {
                    ui_widgets.color.edit_4(m_crypt("Line Color"), g_config->esp.players.snaplines_col, cp_flags_w_label);
                    ui_widgets.slider_float(m_crypt("Thickness"), &g_config->esp.players.snaplines_thickness, 1.0f, 2.5f, "%.1fpx");
                    //ui_widgets.checkbox(m_crypt("Dots"), &g_config->esp.players.snaplines_dots);
                    //if (g_config->esp.players.snaplines_dots) {
                    //    ui_widgets.slider_float(m_crypt("Start Dot Radius"), &g_config->esp.players.snpl_dots_rad[0], 1.0f, 10.f, "%.1fpx");
                    //    ui_widgets.slider_float(m_crypt("End Dot Radius"), &g_config->esp.players.snpl_dots_rad[1], 1.0f, 10.f, "%.1fpx");
                    //}
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Health Bar"), &g_config->esp.players.bars.health_bar);
                if (ui_widgets.popup.begin_settings(m_crypt("HP Bar Settings"), 210)) {
                    const char* bars_col_type[2] = { "Solid", "Gradient" };
                    ui_widgets.combo(m_crypt("Color Type"), &g_config->esp.players.bars.bars_color_type, bars_col_type, IM_ARRAYSIZE(bars_col_type));

                    const char* geometric_pos_list[4] = { "Top", "Bottom", "Left", "Right" };
                    ui_widgets.combo(m_crypt("Position"), (int*)&g_config->esp.players.bars.bars_position, geometric_pos_list, IM_ARRAYSIZE(geometric_pos_list));

                    switch (g_config->esp.players.bars.bars_color_type)
                    {
                    case 0: // solid
                        ui_widgets.color.edit_4(m_crypt("Health Color"), g_config->esp.players.bars.solid_color.health_bar_col, cp_flags_w_label);

                        ui_widgets.checkbox(m_crypt("Armor Bar"), &g_config->esp.players.bars.armor_bar);
                        if (g_config->esp.players.bars.armor_bar)
                            ui_widgets.color.edit_4(m_crypt("Armor Color"), g_config->esp.players.bars.solid_color.armor_bar_col, cp_flags_w_label);

                        break;
                    case 1: // grd
                        ui_widgets.color.edit_4(m_crypt("Health Primary"), g_config->esp.players.bars.grd_color.health_bar_col_perv, cp_flags_w_label);
                        ui_widgets.color.edit_4(m_crypt("Health Secondary"), g_config->esp.players.bars.grd_color.health_bar_col_sec, cp_flags_w_label);

                        ui_widgets.checkbox(m_crypt("Armor Bar"), &g_config->esp.players.bars.armor_bar);

                        if (g_config->esp.players.bars.armor_bar) {
                            ui_widgets.color.edit_4(m_crypt("Armor Primary"), g_config->esp.players.bars.grd_color.armor_bar_col_perv, cp_flags_w_label);
                            ui_widgets.color.edit_4(m_crypt("Armor Secondary"), g_config->esp.players.bars.grd_color.armor_bar_col_sec, cp_flags_w_label);
                        }

                        break;
                    }
                    ui_widgets.popup.end_settings();
                }

            } ui_widgets.child.end();
        }; _end_column;

        _column_right("cplayers_2") {
            ui_widgets.child.begin(m_crypt("Additional"), ImVec2(0, 120)); {
                ui_widgets.checkbox(m_crypt("Current Weapon"), &g_config->esp.players.eqp_weapon);
                if (ui_widgets.popup.begin_settings(m_crypt("Player Weapon Text"), 220)) {
                    ui_widgets.color.edit_4(m_crypt("Default Weapon"), g_config->esp.players.eqp_weapon_col, cp_flags_w_label);
                    ui_widgets.checkbox_cpicker(m_crypt("Heavy Weapon"), &g_config->esp.players.heavy_wep, "hvywep", g_config->esp.players.heavy_wep_col);
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Distance"), &g_config->esp.players.distance);
                if (ui_widgets.popup.begin_settings(m_crypt("Player Distance Text"), 220)) {
                    const char* stylez[2] = { "10.0 m", "[10 M]" };
                    ui_widgets.combo(m_crypt("Style"), (int*)&g_config->esp.players.distance_style, stylez, IM_ARRAYSIZE(stylez));
                    ui_widgets.color.edit_4(m_crypt("Color"), g_config->esp.players.distance_col, cp_flags_w_label);
                    ui_widgets.popup.end_settings();
                }
                //ui_widgets.checkbox(m_crypt("Health Text"), &g_config->esp.players.health_text);
                //if (ui_widgets.popup.begin_settings(m_crypt("Player Health Text"), 200)) {
                //    const char* text_position[2] = { "Flag", "On Bar" };
                //    ui_widgets.combo(m_crypt("Position"), (int*)&g_config->esp.players.health_text_style, text_position, IM_ARRAYSIZE(text_position));
                //    ui_widgets.color.edit_4(m_crypt("Color"), g_config->esp.players.health_text_col, cp_flags_w_label);
                //    ui_widgets.popup.end_settings();
                //}

                if (ui_widgets.popup.begin_settings(m_crypt("Text Manager"), 240, true)) {
                    ui_widgets.checkbox(m_crypt("Text Outline"), &g_config->esp.players.text_preset.outline);

                    const char* flags_pos[3] = { "Right", "Bottom", "Left" };
                    ui_widgets.combo(m_crypt("Text Position"), &g_config->esp.players.text_preset.condition, flags_pos, IM_ARRAYSIZE(flags_pos));

                    const char* esp_font_l[4] = { "Vag Rounded", "Comfortaa", "Verdana", "Pixel 7" };
                    ui_widgets.combo(m_crypt("Font"), (int*)&g_config->esp.players.text_preset.font_id, esp_font_l, IM_ARRAYSIZE(esp_font_l));
                    ui_widgets.slider_float(m_crypt("Text Scale"), &g_config->esp.players.text_preset.text_scale, 0.75f, 1.5f, "%.2fx");

                    ui_widgets.checkbox(m_crypt("Adaptive Scaling"), &g_config->esp.players.text_preset.adaptive_scale_state);
                    //if (g_config->esp.players.text_preset.adaptive_scale_state) {
                    //    ui_widgets.slider_float(m_crypt("Strength"), &g_config->esp.players.text_preset.adsc_strength, 0.f, 150.f, "%.0f%");
                    //}

                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();
        } _end_column;

        break; // players sub end

    case visual_sub.e_sub::ADMINS:
        //g_ui_misc.hint(m_crypt("Hint: Admins ESP"), hints::admins_esp_tab, hints::pos);

        _column_left("cadmins_1"); {
            ui_widgets.child.begin(m_crypt("General"), ImVec2(0, 150)); {
                ui_widgets.checkbox_bind(m_crypt("Admins ESP"), &g_config->esp.admins.enabled, &g_config->esp.admins.state_key);
                ui_widgets.checkbox_cpicker(m_crypt("Skeleton"), &g_config->esp.admins.skeleton, "a-selfskel", g_config->esp.admins.skeleton_col);
                ui_widgets.checkbox(m_crypt("Rect Box"), &g_config->esp.admins.rect_box);
                if (ui_widgets.popup.begin_settings(m_crypt("Admin Rect Box Settings"), 200)) {
                    const char* w2s_type[2] = { "2D", "3D" };
                    ui_widgets.combo(m_crypt("Box Type"), (int*)&g_config->esp.admins.rect_box_type, w2s_type, IM_ARRAYSIZE(w2s_type));
                    ui_widgets.color.edit_4(m_crypt("Box Color"), g_config->esp.admins.rect_box_col, cp_flags_w_label);

                    ui_widgets.popup.end_settings();
                }
                //ui_widgets.checkbox_cpicker("Corners Box", &g_config->esp.admins.corners_box, "a-crbox", g_config->esp.admins.corners_box_col);
                //ui_widgets.checkbox_cpicker(m_crypt("Fill Box"), &g_config->esp.admins.fill_box, "a-fillbox", g_config->esp.admins.fill_box_col);
                ui_widgets.checkbox_cpicker(m_crypt("Snaplines"), &g_config->esp.admins.snaplines, "a-snpln", g_config->esp.admins.snaplines_col);

            } ui_widgets.child.end();
        }; _end_column;

        _column_right("cadmins_2"); {
            ui_widgets.child.begin(m_crypt("Additional"), ImVec2(0, 65)); {
                ui_widgets.checkbox(m_crypt("Warning Widget"), &g_config->esp.admins.widget.enabled);
                if (ui_widgets.popup.begin_settings(m_crypt("Warning Widget Settings"), 200)) {
                    ui_widgets.checkbox(m_crypt("Show Preview"), &g_config->esp.admins.widget.preview);
                    ui_widgets.color.edit_4(m_crypt("Icon Color"), g_config->esp.admins.widget.icon_color, cp_flags_w_label);
                    //ui_widgets.checkbox("Appear Animation", &g_config->esp.admins.widget.animation);
                    ui_widgets.popup.end_settings();
                }
                //ui_widgets.checkbox_cpicker("Text Flag", &g_config->esp.admins.text_flag, "a-txt-flag", g_config->esp.admins.skeleton_col);
            } ui_widgets.child.end();
        }; _end_column;

        break; // admins sub end

    case visual_sub.e_sub::OBJECTS:
        //g_ui_misc.hint(m_crypt("Hint: Object ESP"), hints::objects_esp_tab, hints::pos);

        _column_left("cobj_1"); {
            ui_widgets.child.begin(m_crypt("General"), { 0, 150 }); {
                ui_widgets.checkbox(m_crypt("Objects ESP"), &g_config->esp.objects.enabled);
                if (ui_widgets.popup.begin_settings(m_crypt("Object ESP Settings"), 220)) {
                    ui_widgets.checkbox(m_crypt("Render All Stream"), &g_config->esp.objects.render_all_stream);
                    ui_widgets.slider_float(m_crypt("Max Distance"), &g_config->esp.objects.max_dist, 0.0f, 500.f, "%.0f m");
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox_cpicker(m_crypt("Name"), &g_config->esp.objects.names, "objname", g_config->esp.objects.names_col);
                ui_widgets.checkbox_cpicker(m_crypt("Distance"), &g_config->esp.objects.distance, "obj-dist", g_config->esp.objects.distance_col);
                ui_widgets.checkbox_cpicker(m_crypt("Snaplines"), &g_config->esp.objects.snaplines, "obj-snap", g_config->esp.objects.snaplines_col);
            } ui_widgets.child.end();

            ui_widgets.child.begin(m_crypt("Manager"), { 0, 150 }); {
                ui_widgets.input_text("Item Name", cheat_base->ui_bufs.obj_name, sizeof(cheat_base->ui_bufs.obj_name));
                ui_widgets.input_text("Object Hash", cheat_base->ui_bufs.obj_hash, sizeof(cheat_base->ui_bufs.obj_hash));

                fixed_containers::FixedMap<DWORD, std::string, 100> obj_list = g_config->esp.objects.tech.objects_list;
                fixed_containers::FixedMap<DWORD, std::string, 100>::iterator it;

                if (ui_widgets.button("Add", ImVec2(110, 28))) {
                    if (std::strlen(cheat_base->ui_bufs.obj_name) > 2 && std::strlen(cheat_base->ui_bufs.obj_hash) > 2) {
                        DWORD toAddInt1;
                        std::string toAddNameString1(cheat_base->ui_bufs.obj_name);
                        sscanf_s(cheat_base->ui_bufs.obj_hash, "%x", &toAddInt1);
                        it = obj_list.find(toAddInt1);
                        if (it == obj_list.end()) {

                            obj_list.insert(make_pair(toAddInt1, toAddNameString1));
                            g_config->esp.objects.tech.objects_list = obj_list;
                        }
                    }
                }

                ImGui::SameLine();
                if (ui_widgets.button("Delete", ImVec2(110, 28))) {
                    if (selected_obj != -1) {
                        it = obj_list.find(selected_obj);
                        if (it != obj_list.end()) {
                            obj_list.erase(it);
                            g_config->esp.objects.tech.objects_list = obj_list;
                            selected_obj = -1;
                        }
                    }
                }
            } ui_widgets.child.end();

        }; _end_column;

        _column_right("cobj_2") {
            ui_widgets.child.begin(m_crypt("List"), { 0, 236 }); {
                fixed_containers::FixedMap<DWORD, std::string, 100> obj_list = g_config->esp.objects.tech.objects_list;
                fixed_containers::FixedMap<DWORD, std::string, 100>::iterator it;

                ImGui::SetCursorPos({ 16, 16 });
                ImGui::BeginChild("##objlist", { 226, 218 - 30 }, false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse); {
                    for (it = obj_list.begin(); it != obj_list.end(); it++) {
                        DWORD dHash = it->first;
                        std::string dName = it->second;

                        char tHash[32];
                        sprintf(tHash, "0x%X\n", dHash);
                        std::string disp_name(tHash);

                        bool sosal = selected_obj == dHash;
                        if (ui_widgets.item_selectable(dName.c_str(), disp_name.c_str(), &sosal)) {
                            selected_obj = dHash;
                        }

                        //if (ui_widgets.selectable(dName.c_str(), selected_obj == dHash)) {
                        //    selected_obj = dHash;
                        //}
                        //if (ui_widgets.selectable_flist(dName.c_str(), std::format("0x{}{}", std::hex, dHash).c_str(), selected_obj == dHash)) {
                        //    selected_obj = dHash;
                        //}
                    }
                } ImGui::EndChild();

                //ui_widgets.checkbox_cpicker("Filtered Name", &boolean_1, "fltr-veh-name", color_1);
            } ui_widgets.child.end();


            ui_widgets.child.begin(m_crypt("Additional"), { 0, 62 }); {
                if (ui_widgets.popup.begin_settings(m_crypt("Text Manager"), 240, true)) {
                    ui_widgets.checkbox(m_crypt("Text Outline"), &g_config->esp.objects.text_preset.outline);

                    const char* flags_pos[3] = { "Right", "Centered", "Left" };
                    ui_widgets.combo(m_crypt("Text Condition"), &g_config->esp.objects.text_preset.condition, flags_pos, IM_ARRAYSIZE(flags_pos));

                    const char* esp_font_l[4] = { "Vag Rounded", "Comfortaa", "Verdana", "Pixel 7" };
                    ui_widgets.combo(m_crypt("Font"), (int*)&g_config->esp.objects.text_preset.font_id, esp_font_l, IM_ARRAYSIZE(esp_font_l));
                    ui_widgets.slider_float(m_crypt("Text Scale"), &g_config->esp.objects.text_preset.text_scale, 0.75f, 1.5f, "%.2fx");

                    ui_widgets.checkbox(m_crypt("Adaptive Scaling"), &g_config->esp.objects.text_preset.adaptive_scale_state);
                    //if (g_config->esp.objects.text_preset.adaptive_scale_state) {
                    //    ui_widgets.slider_float(m_crypt("Strength"), &g_config->esp.objects.text_preset.adsc_strength, 0.f, 150.f, "%.0f%");
                    //}

                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();
        }; _end_column;

        break; // object sub end

    case visual_sub.e_sub::VEHICLES:
        //g_ui_misc.hint(m_crypt("Hint: Vehicle ESP"), hints::veh_esp_tab, hints::pos);

        _column_left("cveh_1"); {
            ui_widgets.child.begin(m_crypt("General"), { 0, 200 }); {
                ui_widgets.checkbox(m_crypt("Vehicle ESP"), &g_config->esp.vehicles.enabled);
                if (ui_widgets.popup.begin_settings(m_crypt("Vehicle ESP Settings"), 220)) {
                    ui_widgets.checkbox(m_crypt("Ignore Local"), &g_config->esp.vehicles.ignore_local);
                    ui_widgets.slider_float(m_crypt("Max Distance"), &g_config->esp.vehicles.max_dist, 1, 500, "%.0f m");
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox_cpicker(m_crypt("3D Box"), &g_config->esp.vehicles.box, "box-col-veh", g_config->esp.vehicles.box_col);
                ui_widgets.checkbox(m_crypt("Name"), &g_config->esp.vehicles.names);
                if (ui_widgets.popup.begin_settings(m_crypt("Vehicle Name Settings"), 220)) {
                    ui_widgets.color.edit_4(m_crypt("Text Color"), g_config->esp.vehicles.names_col, cp_flags_w_label);
                    ui_widgets.checkbox(m_crypt("Add Manufacture"), &g_config->esp.vehicles.manufacture);
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox_cpicker(m_crypt("Distance"), &g_config->esp.vehicles.distance, "vehdist", g_config->esp.vehicles.distance_col);
                ui_widgets.checkbox_cpicker(m_crypt("Snaplines"), &g_config->esp.vehicles.snaplines, "v-snap", g_config->esp.vehicles.snaplines_col);
                if (ui_widgets.popup.begin_settings(m_crypt("State Indicators"), 220, true)) {
                    ui_widgets.checkbox(m_crypt("Lock State"), &g_config->esp.vehicles.lock_state);
                    if (g_config->esp.vehicles.lock_state) {
                        ui_widgets.color.edit_4(m_crypt("Unlocked Color"), g_config->esp.vehicles.unlocked_col, cp_flags_w_label);
                        ui_widgets.color.edit_4(m_crypt("Locked Color"), g_config->esp.vehicles.locked_col, cp_flags_w_label);
                    }
                    ui_widgets.checkbox(m_crypt("Engine State"), &g_config->esp.vehicles.engine_state);
                    if (g_config->esp.vehicles.engine_state) {
                        ui_widgets.color.edit_4(m_crypt("Started Color"), g_config->esp.vehicles.engine_on_col, cp_flags_w_label);
                        ui_widgets.color.edit_4(m_crypt("Stalled Color"), g_config->esp.vehicles.engine_off_col, cp_flags_w_label);
                    }
                    //ui_widgets.checkbox_cpicker(m_crypt("Number Plate"), &g_config->esp.vehicles.number_plate, "vehplate", g_config->esp.vehicles.number_plate_col);
                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();
        }; _end_column;

        _column_right("cveh_2"); {
            ui_widgets.child.begin(m_crypt("Additional"), { 0, 62 }); {
                if (ui_widgets.popup.begin_settings(m_crypt("Text Manager"), 240, true)) {
                    ui_widgets.checkbox(m_crypt("Text Outline"), &g_config->esp.vehicles.text_preset.outline);

                    // cuz i tired of le ind fix, so much shit code in it. soooooo....
                    //const char* flags_pos[3] = { "Right", "Centered", "Left" };
                    //ui_widgets.combo(m_crypt("Text Condition"), &g_config->esp.vehicles.text_preset.condition, flags_pos, IM_ARRAYSIZE(flags_pos));

                    const char* esp_font_l[4] = { "Vag Rounded", "Comfortaa", "Verdana", "Pixel 7" };
                    ui_widgets.combo(m_crypt("Font"), (int*)&g_config->esp.vehicles.text_preset.font_id, esp_font_l, IM_ARRAYSIZE(esp_font_l));
                    ui_widgets.slider_float(m_crypt("Text Scale"), &g_config->esp.vehicles.text_preset.text_scale, 0.75f, 1.5f, "%.2fx");

                    ui_widgets.checkbox(m_crypt("Adaptive Scaling"), &g_config->esp.vehicles.text_preset.adaptive_scale_state);
                    //if (g_config->esp.vehicles.text_preset.adaptive_scale_state) {
                    //    ui_widgets.slider_float(m_crypt("Strength"), &g_config->esp.vehicles.text_preset.adsc_strength, 0.f, 150.f, "%.0f%");
                    //}

                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();
        }; _end_column;

        break; // vehicles sub end

    case visual_sub.e_sub::RADAR:
        //g_ui_misc.hint(m_crypt("Hint: Radar"), hints::radar_tab, hints::pos);
        const char* rad_col_type[2] = { "Accent", "Custom" };

        _column_left("cradar_1"); {
            ui_widgets.child.begin(m_crypt("General"), { 0, 65 }); {
                ui_widgets.checkbox(m_crypt("Radar Widget"), &g_config->esp.radar.enabled);
            } ui_widgets.child.end();

            ui_widgets.child.begin(m_crypt("Window Settings"), { 0, 114 }); {
                ui_widgets.slider_float(m_crypt("Map Zoom"), &g_config->esp.radar.zoom, 0.5f, 3.f, "%.1fx");
                ui_widgets.slider_float(m_crypt("Rounding"), &g_config->esp.radar.rounding, 0, 12, "%.0fpx");
                ui_widgets.slider_float(m_crypt("Opacity"), &g_config->esp.radar.opacity, 0, 100, 
                    (g_config->esp.radar.opacity == 100) ? "Fill" : "%.0f%%");
            } ui_widgets.child.end();
        }; _end_column;

        _column_right("cradar_2"); {
            ui_widgets.child.begin("Fliters", ImVec2(0, 90)); { // 145 - 2 checks
                const char* rad_mark_types[2] = { "Dot", "Square" };
                ui_widgets.checkbox(m_crypt("Players"), &g_config->esp.radar.filters.players);
                if (ui_widgets.popup.begin_settings(m_crypt("Players On Radar"), 220)) {
                    ui_widgets.combo(m_crypt("Mark"), (int*)&g_config->esp.radar.marks.players, rad_mark_types, IM_ARRAYSIZE(rad_mark_types));
                    ui_widgets.color.edit_4(m_crypt("Mark Color"), g_config->esp.radar.filters.players_col, cp_flags_w_label);
                    ui_widgets.slider_float(m_crypt("Mark Scale"), &g_config->esp.radar.marks.players_scale, 50.f, 200.f, "%.1f%%");
                    ui_widgets.popup.end_settings();
                }
                ui_widgets.checkbox(m_crypt("Admins"), &g_config->esp.radar.filters.admins);
                if (ui_widgets.popup.begin_settings(m_crypt("Admins On Radar"), 220)) {
                    ui_widgets.combo(m_crypt("Mark"), (int*)&g_config->esp.radar.marks.admins, rad_mark_types, IM_ARRAYSIZE(rad_mark_types));
                    ui_widgets.color.edit_4(m_crypt("Mark Color"), g_config->esp.radar.filters.admins_col, cp_flags_w_label);
                    ui_widgets.slider_float(m_crypt("Mark Scale"), &g_config->esp.radar.marks.admins_scale, 50.f, 200.f, "%.1f%%");
                    ui_widgets.popup.end_settings();
                }
            } ui_widgets.child.end();
        }; _end_column;

        break;
    }
}