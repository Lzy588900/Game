#include "do_cheats.hpp"
#include <process.h>

int __stdcall DllMain(void* _DllHandle, unsigned long _Reason,void* _Reserved )
{
	if (_Reason == DLL_PROCESS_ATTACH)_beginthreadex(nullptr, 0,initalize_d3d9, nullptr, 0, nullptr);//进程附加
	if (_Reason == DLL_PROCESS_DETACH)un_load();//进程退出
	return 1;
}