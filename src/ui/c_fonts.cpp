#include "c_fonts.h"
#include "include/fonts_data.h"

ImFont* ui_fonts[8]{};

namespace _ui_fonts {
    void init() {
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImFontConfig font_config;
        font_config.PixelSnapH = false;
        font_config.OversampleH = 5;
        font_config.OversampleV = 5;
        font_config.RasterizerMultiply = 1.2f;

        static const ImWchar ranges[] = { 0x0020, 0x00FF, 0x0400, 0x052F, 0x2DE0, 0x2DFF, 0xA640, 0xA69F, 0xE000, 0xE226, 0, };
        font_config.GlyphRanges = ranges;

        struct FontData {
            void* raw_data;
            size_t data_size;
            float size;
            e_fonts font_id;
        };

        FontData fonts[] = {
            { SFUITextMedium, sizeof(SFUITextMedium), 14.0f, e_fonts::MEDIUM },
            { SFUITextBold, sizeof(SFUITextBold), 18.0f, e_fonts::BOLD },
            { icons_raw, sizeof(icons_raw), 14.0f, e_fonts::ICONS14 },
            { icons_raw, sizeof(icons_raw), 18.0f, e_fonts::ICONS18 },
            { icons_raw, sizeof(icons_raw), 48.0f, e_fonts::ICONS48 },
            { combo_arrow_raw, sizeof(combo_arrow_raw), 8.0f, e_fonts::ARROW },
            { hint_symbol_raw, sizeof(hint_symbol_raw), 16.0f, e_fonts::HINT },
            { decima_mono_x_raw, sizeof(decima_mono_x_raw), 16.0f, e_fonts::DECIMA }
        };

        for (const auto& font : fonts) {
            ui_fonts[font.font_id] = io.Fonts->AddFontFromMemoryTTF(font.raw_data, font.data_size, font.size, &font_config, ranges);
        }
    }
}