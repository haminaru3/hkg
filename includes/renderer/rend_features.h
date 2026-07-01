#pragma once
#include <fstream>
#include <iostream>
#include <ctime>

#include <dwmapi.h> 
#include <TlHelp32.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_internal.h"

#include "../../src/c_config.hpp"

// Color Struct Storing rgba value
struct RGBA {
	RGBA(int r, int g, int b, int a) :
		r(r), g(g), b(b), a(a) { }

	RGBA(float col[4]) :
		r(col[0] * 255), g(col[1] * 255), b(col[2] * 255), a(col[3] * 255) { }
	RGBA(std::array<float, 4> col) :
		r(col[0] * 255), g(col[1] * 255), b(col[2] * 255), a(col[3] * 255) { }
	RGBA(std::array<float, 4> col, float alpha) :
		r(col[0] * 255), g(col[1] * 255), b(col[2] * 255), a((col[3] * 255) * alpha) { }
	int r ;
	int g ;
	int b ;
	int a ;
};

#define RGBACOL_WHITE		RGBA(255,255,255,255)
#define RGBACOL_BLACK		RGBA(0,0,0,255)
#define RGBACOL_BLACK_TRANS RGBA(0,0,0,255)

class imgui_render {
public:

	bool finishedInit = false;

	enum e_esp_fonts {
		VAG_R,
		COMFORTAA,
		VERDANA,
		SMALLEST_PIXEL_7
	};

	enum e_text_align {
		left,
		centered,
		right
	};

	ImFont* esp_fonts[4];

	HRESULT hres;
	D3D11_VIEWPORT viewport;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* RenderTargetView = nullptr;
	typedef void (*vResourceLoadCall)(ID3D11Device*);


	vResourceLoadCall loadCall = nullptr;

	void SetResourceLoad(vResourceLoadCall funct2);

	void Initialize(HWND targetWindow, IDXGISwapChain* pSwapchain);
	
	void BeginScene();
	void EndScene();

	void Render();

	bool ready();
	void release();
	void reset(UINT Width, UINT Height);

	//float RenderText(const std::string& text, const ImVec2& position, float size, RGBA color, const int font_id, int cond = left, bool outine = true);
	float RenderText(const std::string& text, const ImVec2& position, RGBA color, s_esptext_render_params preset);
	void RenderLine(const ImVec2& from, const ImVec2& to,  RGBA color, float thickness = 1.0f);
	void RenderCircle(const ImVec2& position, float radius, RGBA color, float thickness = 1.0f, uint32_t segments = 16);

	void RenderCircleRainbow(const ImVec2& position, float radius, float thickness = 2.0f, uint32_t segments = 16);
	void RenderCircleFilled(const ImVec2& position, float radius, RGBA color, uint32_t segments = 16);
	void RenderRect(const ImVec2& from, const ImVec2& to, RGBA color, float rounding = 0.0f, uint32_t roundingCornersFlags = ImDrawCornerFlags_All, float thickness = 1.0f);
	void RenderDot(const ImVec2& from, const ImVec2& to, RGBA color, float thickness = 1.0f);
	void RenderRectFilled(const ImVec2& from, const ImVec2& to,RGBA color, float rounding = 0.0f, uint32_t roundingCornersFlags = ImDrawCornerFlags_All);
	void RenderRectFilledMulti(const ImVec2& from, const ImVec2& to, RGBA left_color, RGBA right_color, bool horizontal = false);
};