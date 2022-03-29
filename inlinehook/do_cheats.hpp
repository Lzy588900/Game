#pragma once
#include "help_func.hpp"
#include "inline_hook.hpp"
#include <Windows.h>
#include <iostream>
#include <d3d9.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <cstdio>
#pragma  comment(lib,"d3d9.lib")
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include "ImWin.h"
#include "imgui\imgui_internal.h"


IDirect3D9* g_direct3d9 = nullptr;
D3DPRESENT_PARAMETERS  g_present;
IDirect3DDevice9* g_directdevice9 = nullptr;

inline_hook* g_Reset_hook = nullptr;
inline_hook* g_EndScene_hook = nullptr;
inline_hook* g_DrawIndexedPrimitive_hook = nullptr;

LPCSTR WindowClass = "Valve001";//Direct3DWindowClass Valve001

HWND hWnd = NULL;   //窗口句柄
HANDLE hProcess = NULL;//进程句柄
DWORD processID = NULL;
HANDLE hMoudle[4] = {};//存储需要用到的模块地址 0 server 1 engine 2client
RECT winRect = {};

WNDPROC g_original_proc = nullptr;
namespace matrix {

	DWORD GetProcessID(const char* processName) {
		/*
		* 创建进程快照
		*dwFlags：指定了获取系统进程快照的类型；
		*th32ProcessID：指向要获取进程快照的ID，获取系统内所有进程快照时是0；
		*/
		HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcess == INVALID_HANDLE_VALUE) {
			return  FALSE;
		};
		PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
		//获取第一次进程快照信息
		BOOL  bRet = Process32First(hProcess, &pe);
		while (bRet)
		{
			if (!_tcscmp(pe.szExeFile, processName)) {
				return pe.th32ProcessID;
			}
			bRet = Process32Next(hProcess, &pe);
		}
		return FALSE;

	}

	HMODULE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName) {	// 根据 PID 、模块名（需要写后缀，如：".dll"），获取模块入口地址。
	MODULEENTRY32 moduleEntry;
	HANDLE handle = NULL;
	handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid); //  获取进程快照中包含在th32ProcessID中指定的进程的所有的模块。
	if (!handle) {
		CloseHandle(handle);
		return NULL;
	}
	ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(handle, &moduleEntry)) {
		CloseHandle(handle);
		return NULL;
	}
	do {
		if (_tcscmp(moduleEntry.szModule, moduleName) == 0) { return moduleEntry.hModule; }
	} while (Module32Next(handle, &moduleEntry));
	CloseHandle(handle);
	return 0;
}
	void  GetRoleInfo() {
			int sum;//玩家数量
			int serverOffsetAddres;//偏移地址
			int clientOffsetAddres;//服务端偏移
			int clientOffsetAddres2;
			int camp;//玩家阵营
			int count=0;//统计敌人数量
			float distance= 999999.0f;

			//获取当前房间人物数量
			ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[0] + 0x9E01AC), &sum, 4, NULL);// 0 server 1 engine 2client
			if (sum==0){return;}
			ImGui::Text(u8"房间人数%d", sum);
			/*读取本人阵营以及世界坐标*/
			ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[0] + 0xA81884), &serverOffsetAddres, 4, NULL);//本人地址
			printf("本人地址%d", serverOffsetAddres);
			//ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x314), &my.camp, 4, NULL);
			//ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x1dc), &my.word, 16, NULL);
			///*读取本人矩阵*/
			//ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[2] + 0x4D94C04), &my.matrix, 64, NULL);
			//
			///*读取敌人信息 并转换为屏幕坐标*/
			//for (int i = 1; i < sum; i++) {
			//	ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[0] + (0xA81884 + 0x18 * i)), &serverOffsetAddres, 4, NULL);
			//	ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x314), &camp, 4, NULL);
			//	if (camp != my.camp)
			//	{
			//		ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x1dc), &enemyArray[count].word, 12, NULL);
			//		ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x230), &enemyArray[count].hp, 4, NULL);
			//		ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[2] + (0x4DA31EC + 0x10 * count)), &clientOffsetAddres, 4, NULL);
			//		ReadProcessMemory(hProcess, (LPCVOID)(clientOffsetAddres + 0x26A8), &clientOffsetAddres2, 4, NULL);
			//		ReadProcessMemory(hProcess, (LPCVOID)(clientOffsetAddres2+0x18C), &enemyArray[count].headBoneWord[0], 4, NULL);
			//		ReadProcessMemory(hProcess, (LPCVOID)(clientOffsetAddres2+0x19C), &enemyArray[count].headBoneWord[1], 4, NULL);
			//		ReadProcessMemory(hProcess, (LPCVOID)(clientOffsetAddres2+0x1AC), &enemyArray[count].headBoneWord[2], 4, NULL);
			//		WordToScreen(enemyArray[count].word, enemyArray[count].screen);
			//		WordToScreen(enemyArray[count].headBoneWord, enemyArray[count].headBoneScreen);
			//		count++;
			//	}
			//}
		
			///*cout << count << endl;*/
			//if (GetForegroundWindow() == hWnd ) {
			//	//方框范围
			//	HDC hDC = GetDC(hOverlayWnd);
			//	HPEN hPen = CreatePen(PS_SOLID, 2, 0x0000FF);
			//	
			//		
			//	HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
			//	SelectObject(hDC, hPen);
			//	SelectObject(hDC, hBrush);
			//	/*用背景画刷覆盖*/
			//	FillRect(hDC, &overlayWinRect, hBrush);
			//	hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			//	SelectObject(hDC, hBrush);
			//	for (int i = 0; i < count; i++) {
			//		/*if (enemyArray[i].hp <= 0) {
			//			continue;
			//		}*/
			//		/*cout << i << endl;*/
			//		GetDistance(enemyArray[i].word,my.word,enemyArray[i].screen, &distance);
			//		float rectW = 20000 / distance;
			//		float rectH = 41000 / distance;
		
			//		float rectX = (enemyArray[i].screen[0] - (rectW / 2));
			//		float rectY = (enemyArray[i].screen[1] - rectH);
			//		Rectangle(hDC, (int)rectX, (int)rectY, rectX + rectW, rectY + rectH);
			//		
			//		
			//		/*绘制血条*/
			//		MoveToEx(hDC, (int)rectX - 8, (int)rectY, NULL);//绘制底色
			//		LineTo(hDC, (int)rectX - 8, rectY + rectH);
			//		if (enemyArray[i].hp == 0) {
			//			continue;
			//		}
			//		hPen = CreatePen(PS_SOLID, 2, 0x008000);
			//		SelectObject(hDC, hPen);
			//		MoveToEx(hDC, (int)enemyArray[i].headBoneScreen[0], (int)enemyArray[i].headBoneScreen[1], NULL);
			//		LineTo(hDC, (int)enemyArray[i].headBoneScreen[0], (int)enemyArray[i].headBoneScreen[1]+10 );//绘制底色
			//		/*Rectangle(hDC, (int)enemyArray[i].headBoneScreen[0], (int)enemyArray[i].headBoneScreen[1], 10, 10);*/
			//		MoveToEx(hDC, (int)rectX - 8, rectY + rectH,NULL);
			//		LineTo(hDC, (int)rectX - 8, rectY + rectH * ( 100-enemyArray[i].hp)/100);//绘制底色
			//		cout << enemyArray[i].hp  << endl;
			//		hPen = CreatePen(PS_SOLID, 2, 0x0000FF);
			//		SelectObject(hDC, hPen);
			//		
			//		
			//	}
			//	DeleteObject(hBrush);
			//	DeleteObject(hPen);
			//	ReleaseDC(hOverlayWnd, hDC);
			//}
			//else {
			//	HDC hDC = GetDC(hOverlayWnd);
			//	HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
			//	SelectObject(hDC, hBrush);
			//	/*用背景画刷覆盖*/
			//	FillRect(hDC, &overlayWinRect, hBrush);
			//}
			//
		}
	BOOL InitData() {
		processID = GetProcessID("csgo.exe");
		printf("游戏进程ID:%d\n", processID);
		if (processID) {
			hWnd = FindWindow(NULL, "Counter-Strike: Global Offensive");
			//printf("窗口句柄:%d\n", (int)hWnd);
			GetWindowRect(hWnd, &winRect);
		//	printf("窗口分辨率:%d,%d\n", winRect.right - winRect.left, winRect.bottom - winRect.top);
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
			hMoudle[0] = GetProcessModuleHandle(processID, "server.dll");

			//printf("server(10进制):%d-(16进制)%x\n", (int)hMoudle[0], (int)hMoudle[0]);
			hMoudle[1] = GetProcessModuleHandle(processID, "engine.dll");
		//	printf("engine(10进制):%d-(16进制)%x\n", (int)hMoudle[1], (int)hMoudle[1]);
			hMoudle[2] = GetProcessModuleHandle(processID, "client.dll");
			//printf("client(10进制):%d-(16进制)%x\n",(int)hMoudle[2], (int)hMoudle[2]);
			return true;
		}
		else {

			return FALSE;
		}
		return FALSE;
	}
}

namespace MyImgui {

	void LoadMyWin()
	{
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin(u8"游戏辅助");
		ImGui::SetWindowSize(ImVec2(200, 300), 0);
		ImGui::Text(u8"窗口句柄%d", hWnd);
		ImGui::Text(u8"进程ID%d", processID);
		ImGui::Text(u8"窗口分辨率:%d,%d\n", winRect.right - winRect.left, winRect.bottom - winRect.top);
		ImGui::Text(u8"server%d", hMoudle[0]);
		ImGui::Text(u8"engine%d", hMoudle[1]);
		ImGui::Text(u8"client%d", hMoudle[2]);
		ImGui::GetID("游戏辅助");
		ImGui::OpenPopup("游戏辅助");
		HWND iwn = GetTopWindow(hWnd);
		ImGui::Text(u8"游戏辅助-%d", iwn);
		ShowWindow(iwn, 0);
		matrix::GetRoleInfo();
		ImGui::End();
		ImGui::EndFrame();
		ImGui::Render();

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	}
}
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
		matrix::InitData();
	}

	//printf("进入end函数\n");

	
	g_EndScene_hook->restore_address();

	MyImgui::LoadMyWin();

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
//#ifdef _DEBUG
//	AllocConsole();
//	SetConsoleTitleA("test");
//	freopen("CON", "w", stdout);
//#endif
//
	
	g_direct3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	check_error(g_direct3d9, "Direct3DCreate9失败");
	memset(&g_present, 0, sizeof(g_present));
	g_present.Windowed = TRUE;
	g_present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_present.BackBufferFormat = D3DFMT_UNKNOWN;
	g_present.AutoDepthStencilFormat = D3DFMT_D16;
	g_present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	HRESULT result= g_direct3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, FindWindow(WindowClass,NULL),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&g_present, &g_directdevice9);
	check_error(result == 0, "createdevice失败");

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