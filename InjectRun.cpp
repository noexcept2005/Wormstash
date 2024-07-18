//by Wormwaker
//请以32位编译该程序
//-m32
#include <Windows.h>
#include <iostream>
#include <string>
#include <conio.h>
using namespace std;

typedef struct _PACKAGE_RUN {	//运行包裹
	DWORD dwLoadLibraryA;
	DWORD dwGetProcAddress;
	CHAR Kernel32Dll[24];
	CHAR WinExec[16];
	CHAR Command[2048];		//命令不能超过这个长度
}PACKAGE_RUN,*PPACKAGE_RUN;
//相关类型定义
typedef FARPROC (__stdcall* T_GetProcAddress)(HMODULE, LPCTSTR);
typedef HMODULE(__stdcall* T_LoadLibraryA) (LPCTSTR);
typedef UINT(__stdcall* T_WinExec)(LPCTSTR,UINT);

DWORD WINAPI RemoteThreadProc_Run(LPVOID lpParam)
{			//远程线程
	//取件
	PPACKAGE_RUN pack = (PPACKAGE_RUN)lpParam;
	//声明
	T_LoadLibraryA __LoadLibraryA;
	T_GetProcAddress __GetProcAddress;
	T_WinExec _WinExec;
	//使用门牌号获取函数地址
	__LoadLibraryA = (T_LoadLibraryA)pack->dwLoadLibraryA;
	__GetProcAddress = (T_GetProcAddress)pack->dwGetProcAddress;
	//动态加载Kernel32.dll
	HMODULE hMod = __LoadLibraryA(pack->Kernel32Dll);
	//盗窃对话框函数
	_WinExec = (T_WinExec)__GetProcAddress(hMod, pack->WinExec);
	if (!_WinExec)
	{	//fail
		return -1;
	}
	_WinExec(pack->Command, SW_SHOWNORMAL);
	return 0;
}

bool InjectRun(HANDLE hProc, const string& cmd)		//注射式运???
{				//目标进程句柄
	if (!hProc)
	{
		cout << "错误：句柄无效"  << endl;
		return false;
	}
	PACKAGE_RUN pack{ 0 };
	pack.dwLoadLibraryA = (DWORD)GetProcAddress(GetModuleHandle("KERNEL32.dll"), "LoadLibraryA");
	pack.dwGetProcAddress = (DWORD)GetProcAddress(GetModuleHandle("KERNEL32.dll"), "GetProcAddress");
	lstrcpy(pack.Kernel32Dll,"KERNEL32.dll");
	lstrcpy(pack.WinExec, "WinExec");
	lstrcpy(pack.Command, cmd.c_str());
	if (!pack.dwLoadLibraryA || !pack.dwGetProcAddress)
	{
		cout << "错误：无法获取LoadLibraryA或GetProcAddress的地址" << endl;
		return false;
	}
	LPVOID lpData = VirtualAllocEx(hProc, NULL, sizeof pack, MEM_COMMIT/* | MEM_RELEASE*/, PAGE_READWRITE);
	if (!lpData)
	{
		cout << "错误：无法远程开辟内存1" << endl;
		return false;
	}
	DWORD useless = 0;
	BOOL ret = WriteProcessMemory(hProc, lpData, &pack, sizeof pack, &useless);
	if (!ret)
	{
		cout << "错误：无法修改进程内存1" << endl;
		return false;
	}
	DWORD dwFunSize = 0x2000;	//够大
	LPVOID lpCode = VirtualAllocEx(hProc, NULL, dwFunSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!lpCode)
	{
		cout << "错误：无法远程开辟内存2" << endl;
		return false;
	}
	ret = WriteProcessMemory(hProc, lpCode, (LPVOID)&RemoteThreadProc_Run, dwFunSize, &useless);
	if (!ret)
	{
		cout << "错误：无法修改进程内存2" << endl;
		return false;
	}
	HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)lpCode, lpData, 0, NULL);
	if (!hThread)
	{
		cout << "错误：无法创建远程线程" << endl;
		return false;
	}	//completed
	CloseHandle(hThread);
	CloseHandle(hProc);
	return true; 
}

int main()
{
	SetConsoleTitle("注入式运行- by Wormwaker");
	HANDLE hProc = nullptr;
	cout << "输入进程ID：" << endl;
	DWORD dwPid = 0;
	cin >> dwPid;
	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if(!hProc || INVALID_HANDLE_VALUE == hProc)
	{
		cout << "错误：无法获取进程句柄。可以考虑用管理员权限打开本程序重试。"<< endl;
		return 0;
	}
	cout << "输入要让它运行的命令：" << endl;
	char cmd[1024] = "\0";
	cin.ignore();
	cin.getline(cmd, 1024, '\n');
	cout << "你输入的命令为：" << cmd << endl;
	cout << "按任意键开始注入" << endl;
	_getch();
	bool res = InjectRun(hProc, cmd);
	if(!res)
	{
		cout << "结论：操作失败，错误码="<< GetLastError() << endl;
	}else{
		cout << "操作成功完成。"<< endl;
	}
	return 0;
}
