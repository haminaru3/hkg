#pragma once

#include <filesystem>
#include <fstream>
#include <string>

#include <D3DX11tex.h>

#include "../includes/renderer/rend_features.h"
#include "assets/default_menu_background_jpg.hpp"

extern imgui_render renderer;

inline ID3D11ShaderResourceView* g_menu_background_texture = nullptr;
inline std::string g_menu_background_loaded_request;

inline std::string default_menu_background_path() {
	return std::string(CONFIG_DIR_PATH) + "\\menu_background.jpg";
}

inline void ensure_default_menu_background_file() {
	try {
		std::filesystem::create_directories(CONFIG_DIR_PATH);

		const auto path = default_menu_background_path();
		if (std::filesystem::exists(path) && std::filesystem::file_size(path) > 0) {
			return;
		}

		std::ofstream file(path, std::ios::binary | std::ios::trunc);
		if (!file.is_open()) return;

		file.write(reinterpret_cast<const char*>(k_default_menu_background_jpg), k_default_menu_background_jpg_size);
	}
	catch (...) {
	}
}

inline bool create_menu_background_from_file(ID3D11Device* device, const std::string& path, ID3D11ShaderResourceView** out_texture) {
	if (!device || path.empty() || !out_texture) return false;

	try {
		if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
			return false;
		}

		const std::wstring wide_path = std::filesystem::path(path).wstring();
		return SUCCEEDED(D3DX11CreateShaderResourceViewFromFileW(device, wide_path.c_str(), nullptr, nullptr, out_texture, nullptr)) && *out_texture;
	}
	catch (...) {
		return false;
	}
}

inline bool create_menu_background_from_memory(ID3D11Device* device, ID3D11ShaderResourceView** out_texture) {
	if (!device || !out_texture) return false;

	return SUCCEEDED(D3DX11CreateShaderResourceViewFromMemory(
		device,
		k_default_menu_background_jpg,
		k_default_menu_background_jpg_size,
		nullptr,
		nullptr,
		out_texture,
		nullptr)) && *out_texture;
}

inline bool refresh_menu_background_texture(bool force = false) {
	if (!renderer.pDevice) return false;

	ensure_default_menu_background_file();

	std::string request = default_menu_background_path();
	if (g_config && !g_config->misc.menu_background_path.empty()) {
		request = g_config->misc.menu_background_path;
	}

	if (!force && g_menu_background_texture && request == g_menu_background_loaded_request) {
		return true;
	}

	ID3D11ShaderResourceView* next_texture = nullptr;
	if (!create_menu_background_from_file(renderer.pDevice, request, &next_texture)) {
		const auto fallback_path = default_menu_background_path();
		if (!create_menu_background_from_file(renderer.pDevice, fallback_path, &next_texture)) {
			create_menu_background_from_memory(renderer.pDevice, &next_texture);
		}
	}

	if (!next_texture) {
		return false;
	}

	if (g_menu_background_texture) {
		g_menu_background_texture->Release();
	}

	g_menu_background_texture = next_texture;
	g_menu_background_loaded_request = request;
	return true;
}
