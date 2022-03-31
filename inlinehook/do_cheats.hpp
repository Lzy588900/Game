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

//HWND hWnd = NULL;   //���ھ��
HANDLE hProcess = NULL;//���̾��
DWORD processID = NULL;
HANDLE hMoudle[4] = {};//�洢��Ҫ�õ���ģ���ַ 0 server 1 engine 2client
RECT winRect = {};
ImDrawList* draw_list;
WNDPROC g_original_proc = nullptr;
static HWND window = NULL;
bool first_call = true;
bool show = true;
bool Cursor = false;
struct Vctors2
{
	float x, y;
	int TeamFlag, Healthy;
	int BoxWidth, BoxHeight;
	float Distance;
};
struct Enemy {
	int hp;
	float pos[3];//��������
	Vctors2 screen;//��Ļ����
	float headBoneWord[3];//ͷ��������������
	float headBoneScreen[3];//ͷ��������������
}personArray[10];

struct My {
	int hp;
	int camp;
	float word[3];//��������
	float screen[2];//��Ļ����
	float matrix[4][4];//���˾���
}my;


namespace matrix {
	void GetDistance(float enemyWorld[3], float myWorld[3], float enemyScreen[2], float* distance) {
		//�ж��Ƿ�����Ļ�� 
		if ((enemyScreen[0] > 0 && enemyScreen[0] < winRect.right - winRect.left) &&
			(enemyScreen[1] > 0 && enemyScreen[1] < winRect.bottom - winRect.top))
		{
			/**distance = fabs(enemyX - ((winRect.right - winRect.left) / 2)) +
				fabs(enemyY - ((winRect.bottom - winRect.top) / 2));*/
				/*����ά����֮��ľ���*/
			*distance = sqrt((enemyWorld[0] - myWorld[0]) * (enemyWorld[0] - myWorld[0]) + (enemyWorld[1] - myWorld[1]) * (enemyWorld[1] - myWorld[1]) + (enemyWorld[2] - myWorld[2]) * (enemyWorld[2] - myWorld[2]));
		}
		else
		{
			*distance = 999999.0f;
		}

	}
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
	BOOL WordToScreen(float pos[3], Vctors2& Screen)
	{
		GetWindowRect(window, &winRect);
	/*cout << winRect.left << winRect.top << winRect.bottom << winRect.right << endl;*/
		winRect.top += 25;
		//��������ҵ�����ȽϵĽǶȡ�
		float w = my.matrix[3][0] * pos[0] + my.matrix[3][1] * pos[1] + my.matrix[3][2] * pos[2] + my.matrix[3][3]; //Calculate the angle in compareson to the player's camera.
		if (w > 0.001) //�����������ͼ��.
		{
			
			Screen.Distance = w / 100;
			Screen.x = ((winRect.right - winRect.left) / 2) + (0.5f * ((my.matrix[0][0] * pos[0] + my.matrix[0][1] * pos[1] + my.matrix[0][2] * pos[2] + my.matrix[0][3]) * Screen.Distance) * (winRect.right - winRect.left) + 0.5f);
			Screen.y = ((winRect.bottom - winRect.top) / 2) - (0.5f * ((my.matrix[1][0] * pos[0] + my.matrix[1][1] * pos[1] + my.matrix[1][2] * pos[2] + my.matrix[1][3]) * Screen.Distance) * (winRect.bottom - winRect.top) + 0.5f);
			Screen.BoxHeight = w - Screen.y;
			Screen.BoxWidth = Screen.BoxHeight / 2;
			return true;
		}
		return false;
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
			int PersonAddres;//ƫ�Ƶ�ַ
			int clientOffsetAddres;//�����ƫ��
			int clientOffsetAddres2;
			int camp;//�����Ӫ
			int count=0;//ͳ�Ƶ�������
			float distance= 999999.0f;
			ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
			const ImU32 col = ImColor(colf);
			//��ȡ��ǰ������������
			ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[0] + 0x9E01AC), &sum, 4, NULL);// 0 server 1 engine 2client
			if (sum==0){return;}
			ImGui::Text(u8"��������%d", sum);
			/*��ȡ������Ӫ�Լ���������*/
			ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[0] + 0x00939D34), &PersonAddres, 4, NULL);//�Լ���ַ
			ImGui::Text(u8"�Լ���ַ%x", PersonAddres);
			//ReadProcessMemory(hProcess, (LPCVOID)(PersonAddres + 0x314), &my.camp, 4, NULL);//������Ӫ
		//	ImGui::Text(u8"������Ӫ%d", my.camp);
			//ReadProcessMemory(hProcess, (LPCVOID)(PersonAddres + 0x1d8), &my.word, 12, NULL);//����x
			//ImGui::Text(u8"����x%f", my.word[0]);
		
			ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[1] + 0x75038C), &my.matrix, 64, NULL);	//*��ȡ���˾���*/
			//
			///*��ȡ������Ϣ ��ת��Ϊ��Ļ����*/
			for (int i = 1; i < sum; i++) {
				ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[0] + (0x00939D34 + 0x18 * i)), &PersonAddres, 4, NULL);
				
				ReadProcessMemory(hProcess, (LPCVOID)(PersonAddres + 0x314), &camp, 4, NULL);//��Ӫ
				ReadProcessMemory(hProcess, (LPCVOID)(PersonAddres + 0x1d8), &personArray[i].pos, 12, NULL);//����
				ReadProcessMemory(hProcess, (LPCVOID)(PersonAddres + 0x21c), &personArray[i].hp, 4, NULL);//Ѫ��
				ImGui::Text(u8"PersonAddres=%x,��Ӫ=%d,����x=%f,����y=%f,����z=%f", PersonAddres, camp,personArray[i].pos[0], personArray[i].pos[1], personArray[i].pos[2]);
				ImGui::Text(u8"Ѫ��%d", personArray[i].hp);
				WordToScreen(personArray[i].pos, personArray[i].screen);
				ImGui::Text(u8"X=%f,X=%f", personArray[i].screen.x, personArray[i].screen.y);
				//WordToScreen(enemyArray[count].headBoneWord, enemyArray[count].headBoneScreen);
				
			}
		
			for (int i = 0; i < sum; i++) {

				personArray[i].screen;
				draw_list->AddRect(ImVec2(personArray[i].screen.x, personArray[i].screen.x+ personArray[i].screen.BoxWidth), ImVec2(personArray[i].screen.y, personArray[i].screen.y + personArray[i].screen.BoxHeight), col, 0.0f, 0, 3.0f);

			}
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
	BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
	{
		DWORD wndProcId;
		GetWindowThreadProcessId(handle, &wndProcId);

		if (GetCurrentProcessId() != wndProcId)
			return TRUE; // skip to next window

		window = handle;
		return FALSE; // window found abort search
	}

	HWND GetProcessWindow()
	{
		window = NULL;
		EnumWindows(EnumWindowsCallback, NULL);
		return window;
	}
	BOOL InitData() {
		processID = GetProcessID("csgo.exe");
		printf("��Ϸ����ID:%d\n", processID);
		if (processID) {
			GetProcessWindow();
		//	hWnd = FindWindow(NULL, "Counter-Strike: Global Offensive");
			//printf("���ھ��:%d\n", (int)hWnd);
			
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


		draw_list = ImGui::GetForegroundDrawList();
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin(u8"��Ϸ����");
		//ImGui::SetWindowSize(ImVec2(200, 300), 0);
		//ImGui::Text(u8"���ھ��%d", window);
		//ImGui::Text(u8"����ID%d", processID);
		//ImGui::Text(u8"���ڷֱ���:%d,%d\n", winRect.right - winRect.left, winRect.bottom - winRect.top);
		//ImGui::Text(u8"server%d", hMoudle[0]);
		//ImGui::Text(u8"engine%d", hMoudle[1]);
		//ImGui::Text(u8"client%d", hMoudle[2]);
		//ImGui::GetID("��Ϸ����");
		//ImGui::OpenPopup("��Ϸ����");
	//	HWND iwn = GetTopWindow(window);
	//	ImGui::Text(u8"��Ϸ����-%d", iwn);
		//ShowWindow(iwn, 0);
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
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
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
	
	if (first_call)
	{
		first_call = false;
		initialize_imgui(directdevice9);
		g_original_proc = (WNDPROC)SetWindowLongA(FindWindow(WindowClass, nullptr), GWL_WNDPROC, (LONG)self_proc);
		matrix::InitData();
	}
	if (GetAsyncKeyState(VK_HOME) & 1)
	{
		show = !show;
	}


	g_EndScene_hook->restore_address();
	if (show){MyImgui::LoadMyWin();}

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