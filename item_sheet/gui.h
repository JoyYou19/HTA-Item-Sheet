#pragma once

#include <d3d9.h>

namespace gui
{
	//Constants that represent the window size
	constexpr int WIDTH = 1600;
	constexpr int HEIGHT = 900;

	inline bool exit = true;

	//winapi window vars
	inline HWND window = nullptr;
	inline WNDCLASSEXA windowClass = { };

	//points for window movement
	inline POINTS position = { };

	//directx state vars
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters = { };

	//Handle window creation and destruction
	void CreateHWindow(
		const char* windowName,
		const char* className) noexcept;
	void DestroyHWindow() noexcept;

	//handle device creation and destruction
	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	//handle ImGui creation and destruction
	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;

	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;

}