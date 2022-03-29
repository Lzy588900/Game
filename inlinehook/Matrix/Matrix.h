#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <cstdio>
#include <gdiplus.h>




	
	BOOL InitData(HANDLE *hProcess, HANDLE *hMoudle, HWND *hWnd);
	DWORD GetProcessID(const char* processName);
	HMODULE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName);
	void CreateOverlayWindow();
	void CALLBACK GetRoleInfo(HWND   hwnd, UINT   uMsg, UINT   idEvent, DWORD   dwTime);
	void GetDistance(float from[3], float to);
	void AutoAim();//×ÔÃé



