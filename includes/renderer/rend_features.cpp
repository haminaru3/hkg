#pragma once
#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include <algorithm>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include "rend_features.h"

#include "../../src/ui/c_ui.h"
#include "../../src/ui/c_fonts.h"
#include "../../src/ui/include/esp_fonts_data.h"

#define M_PI 3.14159265358979323846
#define SAFE_RELEASE(pObject) { if(pObject) { (pObject)->Release(); (pObject) = NULL; } }

bool imgui_render::ready() {
	return pDevice && pContext;
}
     
void imgui_render::SetResourceLoad(vResourceLoadCall funct2) {
	
	loadCall = funct2;
}

void imgui_render::Initialize(HWND targetWindow, IDXGISwapChain* pSwapchain) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	pSwapChain = pSwapchain;

	io.IniFilename = NULL;
	ImGui::StyleColorsDark();

	if (SUCCEEDED(pSwapchain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice))) {
		pSwapchain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
		pDevice->GetImmediateContext(&pContext);
	}

	loadCall(pDevice);

	ImGui_ImplWin32_Init(targetWindow);
	ImGui_ImplDX11_Init(pDevice, pContext);

	g_ui.init();

	{
		ImFontConfig font_config;
		font_config.PixelSnapH = false;
		font_config.OversampleH = 5;
		font_config.OversampleV = 5;
		font_config.RasterizerMultiply = 1.2f;

		static const ImWchar ranges[] = { 0x0020, 0x00FF, 0x0400, 0x052F, 0x2DE0, 0x2DFF, 0xA640, 0xA69F, 0xE000, 0xE226, 0, };
		font_config.GlyphRanges = ranges;

		struct FontData {
			void* raw_data;
			float size;
			e_esp_fonts font_id;
		};

		FontData fonts[] = {
			{ vag_rounded_raw, 13.0f, e_esp_fonts::VAG_R },
			{ comfortaa_med_raw, 13.0f, e_esp_fonts::COMFORTAA },
			{ verdana_reg_raw, 13.0f, e_esp_fonts::VERDANA },
			{ smallestpixel7_raw, 13.0f, e_esp_fonts::SMALLEST_PIXEL_7 },
		};

		for (const auto& font : fonts) {
			esp_fonts[font.font_id] = io.Fonts->AddFontFromMemoryTTF(font.raw_data, sizeof(font.raw_data), font.size, &font_config, ranges);
		}
	}

	return;
}
void imgui_render::release() {
	pContext->OMSetRenderTargets(0, 0, 0);
	RenderTargetView->Release();


	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	

	pDevice = nullptr;
	pContext = nullptr;
	RenderTargetView = nullptr;
	pSwapChain = nullptr;


	finishedInit = false;

}
void imgui_render::reset(UINT Width, UINT Height) {
	ID3D11Texture2D* pBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(void**)&pBuffer);
	pDevice->CreateRenderTargetView(pBuffer, NULL,
		&RenderTargetView);
	// Perform error handling here!
	pBuffer->Release();

	pContext->OMSetRenderTargets(1, &RenderTargetView, NULL);

	// Set up the viewport.
	D3D11_VIEWPORT vp;
	vp.Width = Width;
	vp.Height = Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1, &vp);

}

void imgui_render::BeginScene() {
	ImGuiIO& io = ImGui::GetIO();
	if (RenderTargetView == nullptr) {
		ID3D11Texture2D* backbuffer = NULL;
		//hres = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);

		hres = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);//pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));

		if (FAILED(hres)) {
			pContext->OMGetRenderTargets(1, &RenderTargetView, NULL);
			return;
		}


		hres = pSwapChain->GetDevice(IID_PPV_ARGS(&pDevice));

		if (FAILED(hres)) {
			pContext->OMGetRenderTargets(1, &RenderTargetView, NULL);
			
			return;
		}


		hres = pDevice->CreateRenderTargetView(backbuffer, NULL, &RenderTargetView);
		backbuffer->Release();
		if (FAILED(hres)) {
			return;
		}
	} 
	pContext->OMSetRenderTargets(1, &RenderTargetView, NULL);
	//else {
	//	pContext->OMSetRenderTargets(1, &RenderTargetView, NULL);
	//}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
	ImGui::Begin("##Backbuffer", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration);

	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);
}

void imgui_render::Render() {
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
void imgui_render::EndScene() {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DrawList->PushClipRectFullScreen();

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(3);

	g_ui.render_ui();

	ImGui::EndFrame();
}

bool inScreen(ImVec2 pos) {
	return true;
}

float imgui_render::RenderText(const std::string& text, const ImVec2& position, RGBA color, s_esptext_render_params preset) {
	if (inScreen(position)) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		float base_font_size = 13.f, text_size;
		preset.font_id = std::clamp(preset.font_id, 0, 3);
		preset.condition = std::clamp(preset.condition, 0, 2);
		if (preset.text_scale < 0.5f || preset.text_scale > 2.0f) preset.text_scale = 1.0f;
		if (!esp_fonts[preset.font_id]) preset.font_id = 0;

		if (preset.adaptive_scale_state) 
			text_size = preset.adaptive_scale * preset.text_scale;
		else
			text_size = base_font_size * preset.text_scale;


		std::stringstream stream(text);
		std::string line;

		float y = 0.0f;
		int i = 0;
		while (std::getline(stream, line)) {
			ImVec2 textSize = esp_fonts[preset.font_id]->CalcTextSizeA(text_size, FLT_MAX, 0.0f, text.c_str());

			switch (preset.condition)
			{
			case left:
				if (preset.outline) {
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x) + 1.0f, (position.y + textSize.y * i) + 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x) - 1.0f, (position.y + textSize.y * i) - 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x) + 1.0f, (position.y + textSize.y * i) - 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x) - 1.0f, (position.y + textSize.y * i) + 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
				}
				window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { position.x, position.y + textSize.y * i }, IM_COL32(color.r, color.g, color.b, color.a), text.c_str());
				break;
			case centered:
				if (preset.outline) {
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x - textSize.x / 2.0f) + 1.0f, (position.y + textSize.y * i) + 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x - textSize.x / 2.0f) - 1.0f, (position.y + textSize.y * i) - 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x - textSize.x / 2.0f) + 1.0f, (position.y + textSize.y * i) - 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x - textSize.x / 2.0f) - 1.0f, (position.y + textSize.y * i) + 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
				}
				window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { position.x - textSize.x / 2.0f, position.y + textSize.y * i }, IM_COL32(color.r, color.g, color.b, color.a), text.c_str());
				break;
			case right:
				if (preset.outline) {
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x - textSize.x) + 1.0f, (position.y + textSize.y * i) + 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x - textSize.x) - 1.0f, (position.y + textSize.y * i) - 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x - textSize.x) + 1.0f, (position.y + textSize.y * i) - 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
					window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { (position.x - textSize.x) - 1.0f, (position.y + textSize.y * i) + 1.0f }, IM_COL32(0, 0, 0, color.a), text.c_str());
				}
				window->DrawList->AddText(esp_fonts[preset.font_id], text_size, { position.x - textSize.x, position.y + textSize.y * i }, IM_COL32(color.r, color.g, color.b, color.a), text.c_str());
				break;
			}

			y = position.y + textSize.y * (i + 1);
			i++;
		}

		return y;
	}
	return 0.f;
}


void imgui_render::RenderDot(const ImVec2& from, const ImVec2& to, RGBA color, float thickness) {
	if (inScreen(from) && inScreen(to))
		imgui_render::RenderRect(from, to, color, 5.0f, 0, thickness);
}

void imgui_render::RenderLine(const ImVec2& from, const ImVec2& to, RGBA color, float thickness) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (inScreen(from) && inScreen(to))
		window->DrawList->AddLine(from, to, IM_COL32(color.r, color.g, color.b, color.a), thickness);
}

void imgui_render::RenderCircleRainbow(const ImVec2& position, float radius, float thickness, uint32_t segments) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();


	if (inScreen(position)) {
		//window->DrawList->AddCircle(position, radius, segments, thickness);

		for (int i = 0; i < segments; ++i) {
			auto pos = position;
			float angle = (i / static_cast<float>(segments)) * 2 * M_PI;
			auto lastPos = ImVec2(pos.x + cos(angle) * radius, pos.y + sin(angle) * radius);
			auto nextPos = ImVec2(pos.x + cos(angle + 2 * M_PI / segments) * radius, pos.y + sin(angle + 2 * M_PI / segments) * radius);

			ImU32 currentColor = true ? ImGui::ColorConvertFloat4ToU32(ImColor::HSV((fmod(ImGui::GetTime(), 5.0f) / 5.0f - i / static_cast<float>(segments)) + 1.0f, 0.5f, 1.0f)) : IM_COL32(255, 255, 255, 255);

			ImU32 fillCol = false ? ImGui::ColorConvertFloat4ToU32({ ImGui::ColorConvertU32ToFloat4(currentColor).x, ImGui::ColorConvertU32ToFloat4(currentColor).y, ImGui::ColorConvertU32ToFloat4(currentColor).z, 0.2f }) : 0; // 0.2f = fill opacity


			//window->DrawList->AddLine(lastPos, nextPos, IM_COL32(0, 0, 0, 255), 4.f);
			window->DrawList->AddLine(lastPos, nextPos, currentColor, thickness);
		}
	}
}

void imgui_render::RenderCircle(const ImVec2& position, float radius, RGBA color, float thickness, uint32_t segments) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();


	if (inScreen(position))
		window->DrawList->AddCircle(position, radius, IM_COL32(color.r, color.g, color.b, color.a), segments, thickness);
}

void imgui_render::RenderCircleFilled(const ImVec2& position, float radius, RGBA color, uint32_t segments) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (inScreen(position))
		window->DrawList->AddCircleFilled(position, radius, IM_COL32(color.r, color.g, color.b, color.a), segments);
}

void imgui_render::RenderRect(const ImVec2& from, const ImVec2& to, RGBA color, float rounding, uint32_t roundingCornersFlags, float thickness) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (inScreen(from) && inScreen(to))
		window->DrawList->AddRect(from, to, IM_COL32(color.r, color.g, color.b, color.a), rounding, roundingCornersFlags, thickness);
}

void imgui_render::RenderRectFilled(const ImVec2& from, const ImVec2& to, RGBA color, float rounding, uint32_t roundingCornersFlags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (inScreen(from) && inScreen(to))
		window->DrawList->AddRectFilled(from, to, IM_COL32(color.r, color.g, color.b, color.a), rounding, roundingCornersFlags);
}

void imgui_render::RenderRectFilledMulti(const ImVec2& from, const ImVec2& to, RGBA left_color, RGBA right_color, bool horizontal) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (inScreen(from) && inScreen(to)) {
		if (horizontal) {
			window->DrawList->AddRectFilledMultiColor(from, to, 
				IM_COL32(left_color.r, left_color.g, left_color.b, left_color.a),
				IM_COL32(left_color.r, left_color.g, left_color.b, left_color.a),
				IM_COL32(right_color.r, right_color.g, right_color.b, right_color.a),
				IM_COL32(right_color.r, right_color.g, right_color.b, right_color.a));
		}
		else if (!horizontal) {
			window->DrawList->AddRectFilledMultiColor(from, to,
				IM_COL32(left_color.r, left_color.g, left_color.b, left_color.a),
				IM_COL32(right_color.r, right_color.g, right_color.b, right_color.a),
				IM_COL32(right_color.r, right_color.g, right_color.b, right_color.a),
				IM_COL32(left_color.r, left_color.g, left_color.b, left_color.a));
		}

	}

}
