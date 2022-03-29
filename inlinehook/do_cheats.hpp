#pragma once
#include "help_func.hpp"
#include "inline_hook.hpp"
#include <d3d9.h>
#include <stdio.h>
#pragma  comment(lib,"d3d9.lib")
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include "ImWin.h"
#include "Matrix/Matrix.h"

IDirect3D9* g_direct3d9 = nullptr;
D3DPRESENT_PARAMETERS  g_present;
IDirect3DDevice9* g_directdevice9 = nullptr;

inline_hook* g_Reset_hook = nullptr;
inline_hook* g_EndScene_hook = nullptr;
inline_hook* g_DrawIndexedPrimitive_hook = nullptr;

LPCSTR WindowClass = "Valve001";//Direct3DWindowClass Valve001

WNDPROC g_original_proc = nullptr;
HRESULT __stdcall self_Reset(IDirect3DDevice9* directdevice9, D3DPRESENT_PARAMETERS* pPresentationParameters)
{

	g_Reset_hook->restore_address();

	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT result = directdevice9->Reset(pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();


	g_Reset_hook->motify_address();
	return result;
}
void initialize_imgui(IDirect3DDevice9* directdevice9)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simhei.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	ImGui_ImplWin32_Init(FindWindow(WindowClass, nullptr));
	ImGui_ImplDX9_Init(directdevice9);

}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK self_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(g_original_proc, hWnd, uMsg, wParam, lParam);
}
HRESULT __stdcall self_EndScene(IDirect3DDevice9* directdevice9)
{
	static bool first_call = true;
	if (first_call)
	{

		first_call = false;
		initialize_imgui(directdevice9);
		g_original_proc = (WNDPROC)SetWindowLongA(FindWindow(WindowClass, nullptr), GWL_WNDPROC, (LONG)self_proc);
		InitData();
	}

	//printf("½øÈëendº¯Êý\n");

	
	g_EndScene_hook->restore_address();

	LoadMyWin();

	HRESULT result = directdevice9->EndScene();
	g_EndScene_hook->motify_address();
	return result;
}
//HRESULT __stdcall self_DrawIndexedPrimitive(IDirect3DDevice9* g_directdevice9, D3DPRIMITIVETYPE type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
//{
//	return 1;
//}
unsigned __stdcall initalize_d3d9(void* data)
{
#ifdef _DEBUG
	AllocConsole();
	SetConsoleTitleA("test");
	freopen("CON", "w", stdout);
#endif

	
	g_direct3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	check_error(g_direct3d9, "Direct3DCreate9Ê§°Ü");
	memset(&g_present, 0, sizeof(g_present));
	g_present.Windowed = TRUE;
	g_present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_present.BackBufferFormat = D3DFMT_UNKNOWN;
	g_present.AutoDepthStencilFormat = D3DFMT_D16;
	g_present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	HRESULT result= g_direct3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, FindWindow(WindowClass,NULL),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&g_present, &g_directdevice9);
	check_error(result == 0, "createdeviceÊ§°Ü");

	int* direct3d9_table = (int*)*(int*)g_direct3d9;
	int* direct3DDevice9_table = (int*)*(int*)g_directdevice9;
	g_Reset_hook = new inline_hook(direct3DDevice9_table[16],(int)self_Reset);
	g_EndScene_hook = new inline_hook(direct3DDevice9_table[42], (int)self_EndScene);
//	g_DrawIndexedPrimitive_hook = new inline_hook(direct3DDevice9_table[16], (int)self_DrawIndexedPrimitive);
	g_EndScene_hook->motify_address();
	g_Reset_hook->motify_address();
	return 0;

}
void un_load()
{
	g_Reset_hook->restore_address();
	g_EndScene_hook->restore_address();
	
}