#include "Matrix.h"
#include <iostream>
#include <sstream>
using namespace std;
/**/


MSG msg = {};
WNDCLASSEX wc = {};

/*
* 蒙版窗口数据
*/
HWND hOverlayWnd;
RECT overlayWinRect;

struct Enemy {
	int hp;
	float word[3];//世界坐标
	float screen[2];//屏幕坐标
	float headBoneWord[3];//头部骨骼世界坐标
	float headBoneScreen[3];//头部骨骼世界坐标
}enemyArray[5];
struct My {
	int hp;
	int camp;
	float word[3];//世界坐标
	float screen[2];//屏幕坐标
	float matrix[4][4];//本人矩阵
}my;
/*根据进程名称获取进程ID*/

//string DecIntToHexStr(long long num)
//{
//	string str;
//	long long Temp = num / 16;
//	int left = num % 16;
//	if (Temp > 0)
//		str += DecIntToHexStr(Temp);
//	if (left < 10)
//		str += (left + '0');
//	else
//		str += ('A' + left - 10);
//	return str;
//}
//
//string DecStrToHexStr(string str)
//{
//	long long Dec = 0;
//	for (int i = 0; i < str.size(); ++i)
//		Dec = Dec * 10 + str[i] - '0';
//	return DecIntToHexStr(Dec);
//}
//DWORD GetProcessID(const char* processName) {
//	/*
//	* 创建进程快照
//	*dwFlags：指定了获取系统进程快照的类型；
//	*th32ProcessID：指向要获取进程快照的ID，获取系统内所有进程快照时是0；
//	*/
//	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (hProcess == INVALID_HANDLE_VALUE) {
//		return  FALSE;
//	};
//	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
//	//获取第一次进程快照信息
//	BOOL  bRet = Process32First(hProcess, &pe);
//	while (bRet)
//	{
//		if (!_tcscmp(pe.szExeFile, processName)) {
//			return pe.th32ProcessID;
//		}
//		bRet = Process32Next(hProcess, &pe);
//	}
//	return FALSE;
//
//}
/*根据进程ID和进程名称获取模块句柄*/
//HMODULE GetProcessModuleHandle(DWORD pid, CONST TCHAR* moduleName) {	// 根据 PID 、模块名（需要写后缀，如：".dll"），获取模块入口地址。
//	MODULEENTRY32 moduleEntry;
//	HANDLE handle = NULL;
//	handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid); //  获取进程快照中包含在th32ProcessID中指定的进程的所有的模块。
//	if (!handle) {
//		CloseHandle(handle);
//		return NULL;
//	}
//	ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
//	moduleEntry.dwSize = sizeof(MODULEENTRY32);
//	if (!Module32First(handle, &moduleEntry)) {
//		CloseHandle(handle);
//		return NULL;
//	}
//	do {
//		if (_tcscmp(moduleEntry.szModule, moduleName) == 0) { return moduleEntry.hModule; }
//	} while (Module32Next(handle, &moduleEntry));
//	CloseHandle(handle);
//	return 0;
//}
/*将世界坐标转化为屏幕坐标*/
//BOOL WordToScreen(float from[3], float to[2])
//{
//	
//	if (!GetWindowRect(hWnd, &winRect)) {
//		return FALSE;
//	}
//	/*cout << winRect.left << winRect.top << winRect.bottom << winRect.right << endl;*/
//	winRect.top += 25;
//	//计算与玩家的相机比较的角度。
//	float w = my.matrix[3][0] * from[0] + my.matrix[3][1] * from[1] + my.matrix[3][2] * from[2] + my.matrix[3][3]; //Calculate the angle in compareson to the player's camera.
//	if (w > 0.001) //如果对象在视图中.
//	{
//		float fl1DBw = 1 / w;
//		to[0] = ((winRect.right - winRect.left) / 2) + (0.5f * ((my.matrix[0][0] * from[0] + my.matrix[0][1] * from[1] + my.matrix[0][2] * from[2] + my.matrix[0][3]) * fl1DBw) * (winRect.right - winRect.left) + 0.5f);
//		to[1] = ((winRect.bottom - winRect.top) / 2) - (0.5f * ((my.matrix[1][0] * from[0] + my.matrix[1][1] * from[1] + my.matrix[1][2] * from[2] + my.matrix[1][3]) * fl1DBw) * (winRect.bottom - winRect.top) + 0.5f);
//		return true;
//	}
//	return false;
//}
/*计算距离*/
//void GetDistance(float enemyWorld[3],float myWorld[3], float enemyScreen[2],float* distance) {
//	//判断是否在屏幕内 
//	if ((enemyScreen[0] > 0 && enemyScreen[0] < winRect.right - winRect.left) &&
//		(enemyScreen[1] > 0 && enemyScreen[1] < winRect.bottom - winRect.top))
//	{
//		/**distance = fabs(enemyX - ((winRect.right - winRect.left) / 2)) +
//			fabs(enemyY - ((winRect.bottom - winRect.top) / 2));*/
//		/*求三维坐标之间的距离*/
//		*distance=sqrt((enemyWorld[0] - myWorld[0]) * (enemyWorld[0] - myWorld[0]) + (enemyWorld[1] - myWorld[1]) * (enemyWorld[1] - myWorld[1]) + (enemyWorld[2] - myWorld[2]) * (enemyWorld[2] - myWorld[2]));
//	}
//	else
//	{
//		*distance = 999999.0f;
//	}
//
//}
/*获取角色数据*/
//void CALLBACK GetRoleInfo(HWND   hwnd, UINT   uMsg, UINT   idEvent, DWORD   dwTime) {
//	int sum;//玩家数量
//	int serverOffsetAddres;//偏移地址
//	int clientOffsetAddres;//服务端偏移
//	int clientOffsetAddres2;
//	int camp;//玩家阵营
//	int count=0;//统计敌人数量
//	float distance= 999999.0f;
//	
//	
//	//获取当前房间人物数量
//	ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[1] + 0x7A6D78), &sum, 4, NULL);
//	if (sum==0){return;}
//	/*读取本人阵营以及世界坐标*/
//	ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[0] + 0xA81884), &serverOffsetAddres, 4, NULL);
//	ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x314), &my.camp, 4, NULL);
//	ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x1dc), &my.word, 16, NULL);
//	/*读取本人矩阵*/
//	ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[2] + 0x4D94C04), &my.matrix, 64, NULL);
//	
//	/*读取敌人信息 并转换为屏幕坐标*/
//	for (int i = 1; i < sum; i++) {
//		ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[0] + (0xA81884 + 0x18 * i)), &serverOffsetAddres, 4, NULL);
//		ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x314), &camp, 4, NULL);
//		if (camp != my.camp)
//		{
//			ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x1dc), &enemyArray[count].word, 12, NULL);
//			ReadProcessMemory(hProcess, (LPCVOID)(serverOffsetAddres + 0x230), &enemyArray[count].hp, 4, NULL);
//			ReadProcessMemory(hProcess, (LPCVOID)((int)hMoudle[2] + (0x4DA31EC + 0x10 * count)), &clientOffsetAddres, 4, NULL);
//			ReadProcessMemory(hProcess, (LPCVOID)(clientOffsetAddres + 0x26A8), &clientOffsetAddres2, 4, NULL);
//			ReadProcessMemory(hProcess, (LPCVOID)(clientOffsetAddres2+0x18C), &enemyArray[count].headBoneWord[0], 4, NULL);
//			ReadProcessMemory(hProcess, (LPCVOID)(clientOffsetAddres2+0x19C), &enemyArray[count].headBoneWord[1], 4, NULL);
//			ReadProcessMemory(hProcess, (LPCVOID)(clientOffsetAddres2+0x1AC), &enemyArray[count].headBoneWord[2], 4, NULL);
//			WordToScreen(enemyArray[count].word, enemyArray[count].screen);
//			WordToScreen(enemyArray[count].headBoneWord, enemyArray[count].headBoneScreen);
//			count++;
//		}
//	}
//
//	/*cout << count << endl;*/
//	if (GetForegroundWindow() == hWnd ) {
//		//方框范围
//		HDC hDC = GetDC(hOverlayWnd);
//		HPEN hPen = CreatePen(PS_SOLID, 2, 0x0000FF);
//		
//			
//		HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
//		SelectObject(hDC, hPen);
//		SelectObject(hDC, hBrush);
//		/*用背景画刷覆盖*/
//		FillRect(hDC, &overlayWinRect, hBrush);
//		hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
//		SelectObject(hDC, hBrush);
//		for (int i = 0; i < count; i++) {
//			/*if (enemyArray[i].hp <= 0) {
//				continue;
//			}*/
//			/*cout << i << endl;*/
//			GetDistance(enemyArray[i].word,my.word,enemyArray[i].screen, &distance);
//			float rectW = 20000 / distance;
//			float rectH = 41000 / distance;
//
//			float rectX = (enemyArray[i].screen[0] - (rectW / 2));
//			float rectY = (enemyArray[i].screen[1] - rectH);
//			Rectangle(hDC, (int)rectX, (int)rectY, rectX + rectW, rectY + rectH);
//			
//			
//			/*绘制血条*/
//			MoveToEx(hDC, (int)rectX - 8, (int)rectY, NULL);//绘制底色
//			LineTo(hDC, (int)rectX - 8, rectY + rectH);
//			if (enemyArray[i].hp == 0) {
//				continue;
//			}
//			hPen = CreatePen(PS_SOLID, 2, 0x008000);
//			SelectObject(hDC, hPen);
//			MoveToEx(hDC, (int)enemyArray[i].headBoneScreen[0], (int)enemyArray[i].headBoneScreen[1], NULL);
//			LineTo(hDC, (int)enemyArray[i].headBoneScreen[0], (int)enemyArray[i].headBoneScreen[1]+10 );//绘制底色
//			/*Rectangle(hDC, (int)enemyArray[i].headBoneScreen[0], (int)enemyArray[i].headBoneScreen[1], 10, 10);*/
//			MoveToEx(hDC, (int)rectX - 8, rectY + rectH,NULL);
//			LineTo(hDC, (int)rectX - 8, rectY + rectH * ( 100-enemyArray[i].hp)/100);//绘制底色
//			cout << enemyArray[i].hp  << endl;
//			hPen = CreatePen(PS_SOLID, 2, 0x0000FF);
//			SelectObject(hDC, hPen);
//			
//			
//		}
//		DeleteObject(hBrush);
//		DeleteObject(hPen);
//		ReleaseDC(hOverlayWnd, hDC);
//	}
//	else {
//		HDC hDC = GetDC(hOverlayWnd);
//		HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
//		SelectObject(hDC, hBrush);
//		/*用背景画刷覆盖*/
//		FillRect(hDC, &overlayWinRect, hBrush);
//	}
//	
//}
/*初始化各游戏数据*/
//BOOL InitData(HANDLE &hProcess, HANDLE *hMoudle, HWND &hWnd) {
//	DWORD processID = GetProcessID("csgo.exe");
//	printf("游戏进程ID:%d\n", processID);
//	if (processID) {
//		hWnd = FindWindow(NULL, "Counter-Strike: Global Offensive");
//		printf("窗口句柄:%d\n", hWnd);
//		GetWindowRect(hWnd, &winRect);
//		printf("窗口分辨率:%d,%d\n", winRect.right-winRect.left, winRect.bottom - winRect.top);
//		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
//		hMoudle[0] = GetProcessModuleHandle(processID, "server.dll");
//		
//		printf("server(10进制):%d-(16进制)%x\n", hMoudle[0],(int)hMoudle[0]);
//		hMoudle[1] = GetProcessModuleHandle(processID, "engine.dll");
//		printf("engine(10进制):%d-(16进制)%x\n", hMoudle[1],(int)hMoudle[1]);
//		hMoudle[2] = GetProcessModuleHandle(processID, "client.dll");
//		printf("client(10进制):%d-(16进制)%x\n", hMoudle[2],(int)hMoudle[2]);
//		return true;
//	}
//	else {
//
//		return FALSE;
//	}
//	return FALSE;
//}
/*消息回调处理函数*/
//LRESULT OverWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
//	HDC HDCD;
//	PAINTSTRUCT ps;
//	switch (msg)
//	{
//	case WM_DESTROY:
//		PostQuitMessage(0);
//	case WM_CLOSE:
//		DestroyWindow(hWnd);
//		break;
//	case WM_KEYDOWN:
//	
//	default:
//		return DefWindowProc(hWnd, msg, wParam, lParam);
//	}
//}
///*创建蒙版窗口*/
//void CreateOverlayWindow() {
//	//HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetRoleInfo, NULL, NULL, NULL);
//	GetWindowRect(hWnd, &winRect);
//	winRect.top += 25;
//	
//	ZeroMemory(&wc, sizeof(wc));
//
//	/*创建窗口类*/
//	wc.cbSize = sizeof(wc);
//	wc.style = CS_HREDRAW | CS_VREDRAW; //水平和垂直重回
//	wc.lpfnWndProc = (WNDPROC)OverWndMsgProc;
//	wc.hInstance = GetModuleHandle(NULL); //不依赖任何项
//	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
//	wc.lpszClassName = _T("OverlayWindow");
//
//	/*注册窗口类*/
//	RegisterClassEx(&wc); 
//	hOverlayWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, _T("over"),
//		WS_POPUP, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, NULL, NULL, wc.hInstance, NULL);
//
//	if (hOverlayWnd == NULL) {
//		return;
//	}
//	SetLayeredWindowAttributes(hOverlayWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
//	ShowWindow(hOverlayWnd, SW_SHOW);
//	UpdateWindow(hOverlayWnd);
//
//	ZeroMemory(&msg, sizeof(msg));
//	while (msg.message != WM_QUIT)
//	{
//		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//		//如果窗口不在前端则隐藏
//		if (GetForegroundWindow() == hWnd) {
//			SetWindowPos(hOverlayWnd, HWND_TOPMOST, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top
//				, SWP_SHOWWINDOW);
//		}
//		else {
//			SetWindowPos(hOverlayWnd, HWND_TOPMOST, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top
//				, SWP_SHOWWINDOW);
//		}
//		//窗口跟随
//		MoveWindow(hOverlayWnd, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, TRUE);
//		overlayWinRect = { 0,0, winRect.right - winRect.left, winRect.bottom - winRect.top };
//		//判读窗口是否隐藏
//		//if (GetForegroundWindow() == hWnd)
//		//{
//		//	HDC hDC = GetDC(hOverlayWnd); //此处为蒙版窗口上下文
//		//	HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
//		//	SelectObject(hDC, hBrush);
//		//	FillRect(hDC, &overlayWinRect, hBrush);
//		//	DeleteObject(hBrush);
//		//	ReleaseDC(hOverlayWnd, hDC);
//		//}
//	}
//
//}
//
