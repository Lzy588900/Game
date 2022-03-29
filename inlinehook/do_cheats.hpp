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

HWND hWnd = NULL;   //���ھ��
HANDLE hProcess = NULL;//���̾��
DWORD processID = NULL;
HANDLE hMoudle[4] = {};//�洢��Ҫ�õ���ģ���ַ 0 server 1 engine 2client
RECT winRect = {};

WNDPROC g_original_proc = nullptr;
namespace matrix {

	DWORD GetProcessID(const char* processName) {
		/*
		* �������̿���
		*dwFlags��ָ���˻�ȡϵͳ���̿��յ����ͣ�
		*th32ProcessID��ָ��Ҫ��ȡ���̿��յ�ID����ȡϵͳ�����н��̿���ʱ��0��
		*/
		HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcess == INVALID_HANDLE_VALUE) {
			return  FALSE;
		};
		PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
		//��ȡ��һ�ν��̿�����Ϣ
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

	HMODULE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName) {	// ���� PID ��ģ��������Ҫд��׺���磺".dll"������ȡģ����ڵ�ַ��
	MODULEENTRY32 moduleEntry;
	HANDLE handle = NULL;
	handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid); //  ��ȡ���̿����а�����th32ProcessID��ָ���Ľ��̵����е�ģ�顣
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
			int sum;//�������
			int serverOffsetAddres;//ƫ�Ƶ�ַ
			int clientOffsetAddres;//�����ƫ��
			int clientOffsetAddres2;
			int camp;//�����Ӫ
			int count=0;//ͳ�Ƶ�������
			float distance= 999999.0f;

			//��ȡ��ǰ������������
			ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[0] + 0x9E01AC), &sum, 4, NULL);// 0 server 1 engine 2client
			if (sum==0){return;}
			ImGui::Text(u8"��������%d", sum);
			/*��ȡ������Ӫ�Լ���������*/
			ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[0] + 0xA81884), &serverOffsetAddres, 4, NULL);//���˵�ַ
			printf("���˵�ַ%d", serverOffsetAddres);
			//ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x314), &my.camp, 4, NULL);
			//ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x1dc), &my.word, 16, NULL);
			///*��ȡ���˾���*/
			//ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[2] + 0x4D94C04), &my.matrix, 64, NULL);
			//
			///*��ȡ������Ϣ ��ת��Ϊ��Ļ����*/
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
			//	//����Χ
			//	HDC hDC = GetDC(hOverlayWnd);
			//	HPEN hPen = CreatePen(PS_SOLID, 2, 0x0000FF);
			//	
			//		
			//	HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
			//	SelectObject(hDC, hPen);
			//	SelectObject(hDC, hBrush);
			//	/*�ñ�����ˢ����*/
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
			//		/*����Ѫ��*/
			//		MoveToEx(hDC, (int)rectX - 8, (int)rectY, NULL);//���Ƶ�ɫ
			//		LineTo(hDC, (int)rectX - 8, rectY + rectH);
			//		if (enemyArray[i].hp == 0) {
			//			continue;
			//		}
			//		hPen = CreatePen(PS_SOLID, 2, 0x008000);
			//		SelectObject(hDC, hPen);
			//		MoveToEx(hDC, (int)enemyArray[i].headBoneScreen[0], (int)enemyArray[i].headBoneScreen[1], NULL);
			//		LineTo(hDC, (int)enemyArray[i].headBoneScreen[0], (int)enemyArray[i].headBoneScreen[1]+10 );//���Ƶ�ɫ
			//		/*Rectangle(hDC, (int)enemyArray[i].headBoneScreen[0], (int)enemyArray[i].headBoneScreen[1], 10, 10);*/
			//		MoveToEx(hDC, (int)rectX - 8, rectY + rectH,NULL);
			//		LineTo(hDC, (int)rectX - 8, rectY + rectH * ( 100-enemyArray[i].hp)/100);//���Ƶ�ɫ
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
			//	/*�ñ�����ˢ����*/
			//	FillRect(hDC, &overlayWinRect, hBrush);
			//}
			//
		}
	BOOL InitData() {
		processID = GetProcessID("csgo.exe");
		printf("��Ϸ����ID:%d\n", processID);
		if (processID) {
			hWnd = FindWindow(NULL, "Counter-Strike: Global Offensive");
			//printf("���ھ��:%d\n", (int)hWnd);
			GetWindowRect(hWnd, &winRect);
		//	printf("���ڷֱ���:%d,%d\n", winRect.right - winRect.left, winRect.bottom - winRect.top);
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
			hMoudle[0] = GetProcessModuleHandle(processID, "server.dll");

			//printf("server(10����):%d-(16����)%x\n", (int)hMoudle[0], (int)hMoudle[0]);
			hMoudle[1] = GetProcessModuleHandle(processID, "engine.dll");
		//	printf("engine(10����):%d-(16����)%x\n", (int)hMoudle[1], (int)hMoudle[1]);
			hMoudle[2] = GetProcessModuleHandle(processID, "client.dll");
			//printf("client(10����):%d-(16����)%x\n",(int)hMoudle[2], (int)hMoudle[2]);
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

		ImGui::Begin(u8"��Ϸ����");
		ImGui::SetWindowSize(ImVec2(200, 300), 0);
		ImGui::Text(u8"���ھ��%d", hWnd);
		ImGui::Text(u8"����ID%d", processID);
		ImGui::Text(u8"���ڷֱ���:%d,%d\n", winRect.right - winRect.left, winRect.bottom - winRect.top);
		ImGui::Text(u8"server%d", hMoudle[0]);
		ImGui::Text(u8"engine%d", hMoudle[1]);
		ImGui::Text(u8"client%d", hMoudle[2]);
		ImGui::GetID("��Ϸ����");
		ImGui::OpenPopup("��Ϸ����");
		HWND iwn = GetTopWindow(hWnd);
		ImGui::Text(u8"��Ϸ����-%d", iwn);
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

	//printf("����end����\n");

	
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
	check_error(g_direct3d9, "Direct3DCreate9ʧ��");
	memset(&g_present, 0, sizeof(g_present));
	g_present.Windowed = TRUE;
	g_present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_present.BackBufferFormat = D3DFMT_UNKNOWN;
	g_present.AutoDepthStencilFormat = D3DFMT_D16;
	g_present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	HRESULT result= g_direct3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, FindWindow(WindowClass,NULL),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&g_present, &g_directdevice9);
	check_error(result == 0, "createdeviceʧ��");

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