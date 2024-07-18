/***************************************
 *          AntiRickrollPopup          *
 *         RICKROLL自动拦截器          *
 *         Author: @Wormwaker          *
 *        StartDate: 2024/4/1          *
 ***************************************/
#define UNICODE
#define _UNICODE
//#define ONLY_DEFAULT_BROWSER		
// 如果将上面这个宏启用，进程拦截只检查默认浏览器
// 后来测试了一下，由于默认浏览器的获取可能不准确，还是所有进程进行检查比较好
#include <Windows.h>
//#include <winternl.h>
#include <tlhelp32.h>
#include <io.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

size_t proc_block_cnt {0U}, wnd_block_cnt {0U};

const std::wstring rickroll_list[]
{	// 可以自行添加
	L"www.bilibili.com/video/BV1GJ411x7h7",
	L"www.bilibili.com/video/BV1uT4y1P7CX",
	L"www.bilibili.com/video/BV1hG41147Wx",
	L"www.bilibili.com/video/BV1Pg411r7V5",
};
void SetColor(UINT uFore, UINT uBack) 
{
	
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, uFore + uBack * 0x10);
}

#ifdef ONLY_DEFAULT_BROWSER
std::wstring GetDefaultBrowserPath()
{
	std::wstring defaultBrowserPath;
	HKEY hKey;
	LSTATUS status;
	
	// Check HKEY_LOCAL_MACHINE\SOFTWARE\Classes\http\shell\open\command
	status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\http\\shell\\open\\command", 0, KEY_READ, &hKey);
	if (status == ERROR_SUCCESS)
	{
		wchar_t browserPath[MAX_PATH];
		DWORD dataSize = sizeof(browserPath);
		status = RegQueryValueEx(hKey, nullptr, nullptr, nullptr, reinterpret_cast<LPBYTE>(browserPath), &dataSize);
		RegCloseKey(hKey);
		if (status == ERROR_SUCCESS)
		{
			defaultBrowserPath = std::wstring(browserPath);
			
			// Extracting the executable path
			size_t pos = defaultBrowserPath.find_first_of(L"\"");
			if (pos != std::wstring::npos)
			{
				defaultBrowserPath = defaultBrowserPath.substr(1, pos - 1);
			}
			
			return defaultBrowserPath;
		}
	}
	
	// Check HKEY_CLASSES_ROOT\http\shell\open\command
	status = RegOpenKeyEx(HKEY_CLASSES_ROOT, L"http\\shell\\open\\command", 0, KEY_READ, &hKey);
	if (status == ERROR_SUCCESS)
	{
		wchar_t browserPath[MAX_PATH];
		DWORD dataSize = sizeof(browserPath);
		status = RegQueryValueEx(hKey, nullptr, nullptr, nullptr, reinterpret_cast<LPBYTE>(browserPath), &dataSize);
		RegCloseKey(hKey);
		if (status == ERROR_SUCCESS)
		{
			defaultBrowserPath = std::wstring(browserPath);
			
			// Extracting the executable path
			size_t pos = defaultBrowserPath.find_first_of(L"\"");
			if (pos != std::wstring::npos)
			{
				defaultBrowserPath = defaultBrowserPath.substr(1, pos - 1);
			}
			
			return defaultBrowserPath;
		}
	}
	
	return L"";
}
#endif

inline bool sequ(const std::wstring& s1, const std::wstring& s2)
{
	return wcsicmp(s1.c_str(), s2.c_str()) == 0;
}

inline bool ExistFile(const std::wstring& strFile) {
	//文件或文件夹都可以
	return !_waccess(strFile.c_str(),S_OK);//S_OK表示只检查是否存在
}

bool IsFile(const std::wstring& path)
{
	DWORD attributes = GetFileAttributes(path.c_str());
	if (attributes != INVALID_FILE_ATTRIBUTES)
	{
		return !(attributes & FILE_ATTRIBUTE_DIRECTORY);
	}
	return false; //失败
}

bool IsProcessNew(HANDLE hProcess, const std::chrono::system_clock::time_point& currentTime, const std::chrono::seconds& threshold)
{	//貌似没用
	FILETIME creationTime, exitTime, kernelTime, userTime;
	if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime))
	{
		ULARGE_INTEGER creationTimeUI;
		creationTimeUI.LowPart = creationTime.dwLowDateTime;
		creationTimeUI.HighPart = creationTime.dwHighDateTime;
		
		auto processCreationTime = static_cast<std::chrono::system_clock::time_point>(std::chrono::nanoseconds(creationTimeUI.QuadPart * 100));
		
		return currentTime - processCreationTime <= threshold;
	}
	return false;
}

//typedef NTSTATUS (NTAPI *_NtQueryInformationProcess)(
//	HANDLE ProcessHandle,
//	DWORD ProcessInformationClass,
//	PVOID ProcessInformation,
//	DWORD ProcessInformationLength,
//	PDWORD ReturnLength
//	);
//std::wstring GetProcessCommandLine(HANDLE hProcess)
//{
//	UNICODE_STRING commandLine;
//	std::wstring commandLineContents {L""};
//	_NtQueryInformationProcess NtQuery = (_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
//	if (NtQuery)
//	{
//		PROCESS_BASIC_INFORMATION pbi;
//		NTSTATUS isok = NtQuery(hProcess, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);        
//		if (NT_SUCCESS(isok))
//		{
//			PEB peb;
//			RTL_USER_PROCESS_PARAMETERS upps;
//			PVOID rtlUserProcParamsAddress;
//			if (ReadProcessMemory(hProcess, &(((_PEB*) pbi.PebBaseAddress)->ProcessParameters), &rtlUserProcParamsAddress, sizeof(PVOID), NULL))
//			{
//				if (ReadProcessMemory(hProcess,
//					&(((_RTL_USER_PROCESS_PARAMETERS*) rtlUserProcParamsAddress)->CommandLine),
//					&commandLine, sizeof(commandLine), NULL))
//				{
////					commandLineContents = (TCHAR *)malloc(commandLine.Length + sizeof(TCHAR));
////					memset(commandLineContents, 0, commandLine.Length + sizeof(TCHAR));
////					ReadProcessMemory(hProcess, commandLine.Buffer,
////						commandLineContents, commandLine.Length, NULL);
//					
//					WCHAR* buffer = new WCHAR[commandLine.Length / sizeof(WCHAR) + 1];
//					if (ReadProcessMemory(hProcess, commandLine.Buffer, buffer, commandLine.Length, NULL))
//					{
//						buffer[commandLine.Length / sizeof(WCHAR)] = L'\0';
//						commandLineContents = buffer;
//					}
//					delete[] buffer;
//				}
//			}
//		}
//	}
//	
//	return commandLineContents;
//}// original code from https://blog.csdn.net/u013676868/article/details/103275407

// 以下获取进程命令行参数代码来自于：https://www.cnblogs.com/2018shawn/p/15557662.html
// NtQueryInformationProcess for pure 32 and 64-bit processes
typedef NTSTATUS (NTAPI *_NtQueryInformationProcess)(
	IN HANDLE ProcessHandle,
	ULONG ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef NTSTATUS (NTAPI *_NtReadVirtualMemory)(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress,
	OUT PVOID Buffer,
	IN SIZE_T Size,
	OUT PSIZE_T NumberOfBytesRead);

// NtQueryInformationProcess for 32-bit process on WOW64
typedef NTSTATUS (NTAPI *_NtWow64ReadVirtualMemory64)(
	IN HANDLE ProcessHandle,
	IN PVOID64 BaseAddress,
	OUT PVOID Buffer,
	IN ULONG64 Size,
	OUT PULONG64 NumberOfBytesRead);

// PROCESS_BASIC_INFORMATION for pure 32 and 64-bit processes
typedef struct _PROCESS_BASIC_INFORMATION {
	PVOID Reserved1;
	PVOID PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;

// PROCESS_BASIC_INFORMATION for 32-bit process on WOW64
// The definition is quite funky, as we just lazily doubled sizes to match offsets...
typedef struct _PROCESS_BASIC_INFORMATION_WOW64 {
	PVOID Reserved1[2];
	PVOID64 PebBaseAddress;
	PVOID Reserved2[4];
	ULONG_PTR UniqueProcessId[2];
	PVOID Reserved3[2];
} PROCESS_BASIC_INFORMATION_WOW64;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;

typedef struct _UNICODE_STRING_WOW64 {
	USHORT Length;
	USHORT MaximumLength;
	PVOID64 Buffer;
} UNICODE_STRING_WOW64;

std::wstring GetProcessCommandLine(HANDLE hProcess)
{
	if(!hProcess || hProcess == INVALID_HANDLE_VALUE)
	{
		SetColor(12, 0);
		printf("Invalid Process Handle!\n");
		return L"";
	}
	DWORD err {0L};
	// determine if 64 or 32-bit processor
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);
	
	// determine if this process is running on WOW64
	BOOL wow;
	IsWow64Process(GetCurrentProcess(), &wow);
	
	// use WinDbg "dt ntdll!_PEB" command and search for ProcessParameters offset to find the truth out
	DWORD ProcessParametersOffset = si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? 0x20 : 0x10;
	DWORD CommandLineOffset = si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? 0x70 : 0x40;
	
	// read basic info to get ProcessParameters address, we only need the beginning of PEB
	DWORD pebSize = ProcessParametersOffset + 8;
	PBYTE peb = (PBYTE)malloc(pebSize);
	ZeroMemory(peb, pebSize);
	
	// read basic info to get CommandLine address, we only need the beginning of ProcessParameters
	DWORD ppSize = CommandLineOffset + 16;
	PBYTE pp = (PBYTE)malloc(ppSize);
	ZeroMemory(pp, ppSize);
	
	PWSTR cmdLine;
	
	if (wow)
	{
		// we're running as a 32-bit process in a 64-bit OS
		PROCESS_BASIC_INFORMATION_WOW64 pbi;
		ZeroMemory(&pbi, sizeof(pbi));
		
		// get process information from 64-bit world
		_NtQueryInformationProcess query = (_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWow64QueryInformationProcess64");
		err = query(hProcess, 0, &pbi, sizeof(pbi), NULL);
		if (err != 0)
		{
			SetColor(12, 0);
			printf("NtWow64QueryInformationProcess64 failed\n");
			CloseHandle(hProcess);
			return L"";
		}
		
		// read PEB from 64-bit address space
		_NtWow64ReadVirtualMemory64 read = (_NtWow64ReadVirtualMemory64)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWow64ReadVirtualMemory64");
		err = read(hProcess, pbi.PebBaseAddress, peb, pebSize, NULL);
		if (err != 0)
		{
			SetColor(12, 0);
			printf("NtWow64ReadVirtualMemory64 PEB failed\n");
			CloseHandle(hProcess);
			return L"";
		}
		
		// read ProcessParameters from 64-bit address space
		// PBYTE* parameters = (PBYTE*)*(LPVOID*)(peb + ProcessParametersOffset); // address in remote process address space
		PVOID64 parameters = (PVOID64) * ((PVOID64*)(peb + ProcessParametersOffset)); // corrected 64-bit address, see comments
		err = read(hProcess, parameters, pp, ppSize, NULL);
		if (err != 0)
		{
			SetColor(12, 0);
			printf("NtWow64ReadVirtualMemory64 Parameters failed\n");
			CloseHandle(hProcess);
			return L"";
		}
		
		// read CommandLine
		UNICODE_STRING_WOW64* pCommandLine = (UNICODE_STRING_WOW64*)(pp + CommandLineOffset);
		cmdLine = (PWSTR)malloc(pCommandLine->MaximumLength);
		err = read(hProcess, pCommandLine->Buffer, cmdLine, pCommandLine->MaximumLength, NULL);
		if (err != 0)
		{
			SetColor(12, 0);
			printf("NtWow64ReadVirtualMemory64 Parameters failed\n");
			CloseHandle(hProcess);
			return L"";
		}
	}
	else
	{
		// we're running as a 32-bit process in a 32-bit OS, or as a 64-bit process in a 64-bit OS
		PROCESS_BASIC_INFORMATION pbi;
		ZeroMemory(&pbi, sizeof(pbi));
		
		// get process information
		_NtQueryInformationProcess query = (_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
		err = query(hProcess, 0, &pbi, sizeof(pbi), NULL);
		if (err != 0)
		{
			SetColor(12, 0);
			printf("NtQueryInformationProcess failed %ld\n", GetLastError());
			CloseHandle(hProcess);
			return L"";
		}
		
		// read PEB
		if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, peb, pebSize, NULL))
		{
			SetColor(12, 0);
			printf("ReadProcessMemory PEB failed\n");
			CloseHandle(hProcess);
			return L"";
		}
		
		// read ProcessParameters
		PBYTE* parameters = (PBYTE*)*(LPVOID*)(peb + ProcessParametersOffset); // address in remote process adress space
		if (!ReadProcessMemory(hProcess, parameters, pp, ppSize, NULL))
		{
			SetColor(12, 0);
			printf("ReadProcessMemory Parameters failed\n");
			CloseHandle(hProcess);
			return L"";
		}
		
		// read CommandLine
		UNICODE_STRING* pCommandLine = (UNICODE_STRING*)(pp + CommandLineOffset);
		cmdLine = (PWSTR)malloc(pCommandLine->MaximumLength);
		if (!ReadProcessMemory(hProcess, pCommandLine->Buffer, cmdLine, pCommandLine->MaximumLength, NULL))
		{
			SetColor(12, 0);
			printf("ReadProcessMemory Parameters failed\n");
			CloseHandle(hProcess);
			return L"";
		}
	}
	return std::wstring{cmdLine};
}

void TerminateProcessWhoWannaRickroll(const std::wstring& exeName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		SetColor(12, 0);
		std::cerr << "Failed to create process snapshot." << std::endl;
		return;
	}
	
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	
//	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
	std::chrono::seconds threshold(5);
	
	if (Process32First(hSnapshot, &pe32))
	{
		do
		{
			if (exeName.empty() || wcsicmp(pe32.szExeFile, exeName.c_str()) == 0)	//名称匹配
			{
				//std::wcout << pe32.szExeFile << '\n';
				//打开进程句柄
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
				if (hProcess != NULL)
				{
					//是否为新进程
					//if (IsProcessNew(hProcess, currentTime, threshold))
					{
//						//获取命令行参数
//						wchar_t commandLine[MAX_PATH];
//						DWORD dwSize = sizeof(commandLine);
						std::wstring commandLine {GetProcessCommandLine(hProcess)};
//						if (QueryFullProcessImageName(hProcess, 0, commandLine, &dwSize))
						if (!commandLine.empty())
						{
							//std::wcout << L"CommandLine:" << commandLine << L"\n\n";
							//你被骗了
							for(const auto& url : rickroll_list)
								if (wcsstr(commandLine.c_str(), url.c_str()) != nullptr)
								{
									proc_block_cnt++;
									//终结其生命
									SetColor(14, 0);
									std::wcout << L"\r***************************                               \n";
									SetColor(10, 0);
									std::wcout << L"  SOMEONE WANNA RICKROLL! \n";
									SetColor(13, 0);
									std::wcout << L"Rickroll URL: ";
									SetColor(2, 0);
									std::wcout << url << L'\n';
									SetColor(13, 0);
									std::wcout << L"Process Name: ";
									SetColor(14, 0);
									std::wcout << pe32.szExeFile << L'\n';
									SetColor(13, 0);
									std::wcout << L"Process ID: ";
									SetColor(15, 0);
									std::wcout << pe32.th32ProcessID << L'\n';
									SetColor(11, 0);
									std::wcout << L"Terminating process..." << std::endl;
									TerminateProcess(hProcess, 0);
									SetColor(14, 0);
									std::wcout << L"***************************\n\n";
								}
						}else{
							std::wcout << L"QueryFullProcessImageName Failed! - " << GetLastError() << L'\n';
						}
					}
					CloseHandle(hProcess);
				}
			}
		} while (Process32NextW(hSnapshot, &pe32));
	}
	
	CloseHandle(hSnapshot);
}

void CloseWindowWhoWannaRickroll()
{
	HWND hwnd = GetForegroundWindow();
	WCHAR title[1024]{0};
	GetWindowTextW(hwnd, title, 1024);
	std::wstring no_space_lower_title{wcslwr(title)};
	no_space_lower_title.erase(
		std::remove_if(
			no_space_lower_title.begin(), no_space_lower_title.end(), 
				[](wchar_t ch) { return std::iswspace(ch); }), 		//删除空格
		no_space_lower_title.end());
	
	if(wcsstr(no_space_lower_title.c_str(), L"nevergonnagiveyouup")
	|| wcsstr(no_space_lower_title.c_str(), L"rickroll")
	|| wcsstr(no_space_lower_title.c_str(), L"你被骗了"))
	{
		DWORD pid{0L};
		GetWindowThreadProcessId(hwnd, &pid);
		wnd_block_cnt++;
		SetColor(14, 0);
		std::wcout << L"\r**************************                                   \n";
		SetColor(10, 0);
		std::wcout << L" SOMEONE WANNA RICKROLL!\n";
		SetColor(13, 0);
		std::wcout << L"\nWindow Caption: ";
//		std::wcout << (wcslen(title) == 0 ? (L"(null)") : std::wstring(title).c_str());
		SetColor(7, 0);
		printf("%S", title);
		std::wcout << L'\n';
		SetColor(13, 0);
		std::wcout << L"Process ID:" << pid << L'\n';
		SetColor(1, 0);
		std::wcout << L"Closing it (Ctrl+W)...\n";
		keybd_event(VK_LCONTROL, 0, 0, 0);
		keybd_event('W', 0, 0, 0);
		Sleep(10);
		keybd_event('W', 0, 2, 0);
		keybd_event(VK_LCONTROL, 0, 2, 0);
		Sleep(500);
		if(nullptr != FindWindowW(nullptr, title))
		{
			SetColor(12, 0);
			std::wcout << L"Failed, send close message to it...\n";
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			Sleep(500);
			if(nullptr != FindWindowW(nullptr, title))
			{
				SetColor(12, 0);
				std::wcout << L"Failed, Try to destroy it...\n";
				SendMessage(hwnd, WM_DESTROY, 0, 0);
			}
		}
		SetColor(14, 0);
		std::wcout << L"**************************\n\n";	
	}
}

int main()
{
	_wsystem(L"title Anti-Rick-Roll-Popup v1.0 - @Wormwaker");
#ifdef ONLY_DEFAULT_BROWSER
	std::wstring defaultBrowser = GetDefaultBrowserPath();
	std::wstring realPath {L""};
	
	std::wcout.imbue(std::locale(""));  //设置语言环境
	SetColor(8, 0);
	std::wcout << L"Reg value = ";
	std::wcout << defaultBrowser << std::endl;
	
	for(int i{0}; i < defaultBrowser.size() - 3; ++i)
	{
		if(sequ(defaultBrowser.substr(i, 4), L".exe"))
		{
			realPath = defaultBrowser.substr(0, i+4);
			if(ExistFile(realPath) && IsFile(realPath))
			{
				break;
			}
		}
	}
	SetColor(14, 0);
	std::wcout << L"Real path = ";
	SetColor(6, 0);
	std::wcout << realPath << std::endl;
	
	std::wstring exename{realPath};
	for(int i {realPath.size()-1}; i >= 0; --i)
	{
		if(realPath.at(i) == '\\')
		{
			exename = realPath.substr(i + 1, realPath.size() - i - 1);
			break;
		}
	}
	SetColor(14, 0);
	std::wcout << L"EXE Name = " << exename << std::endl;
#else
	std::wstring exename{L""};
#endif
	
	SetColor(15, 0);
	std::wcout << L"Anti-Rickroll-Popup Started!  \n\n\n\n";
	while(1)
	{
		TerminateProcessWhoWannaRickroll(exename);
		CloseWindowWhoWannaRickroll();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		SetColor(15, 0);
		std::wcout << "\r[BLOCKED RICKROLL TOTAL: " << (proc_block_cnt+wnd_block_cnt) << " (Process: " << proc_block_cnt << " , Window: " << wnd_block_cnt << " ) ]     ";
	}
	return 0;
}
