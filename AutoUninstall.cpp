#define DEVCPP
#define DELETE_NO_DELAY
//#define NO_DEBUGLOG

//Linker Options 额外的链接参数
//-lgdi32 -lpsapi
#include <Windows.h>
#include <winternl.h>
#include <psapi.h>
#include <thread>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <io.h>
#include <tlhelp32.h>
#include <sys/stat.h>
#include <cmath>
using namespace std;
#define CJZAPI __stdcall
#define KEY_DOWN(sth) (GetAsyncKeyState(sth) & 0x8000 ? 1 : 0)
#define fequ(f1, f2) (abs((f1)-(f2)) < 0.001)

#define ADDRESS intptr_t

#define TEXT_LEFT 80
#define TEXT_TOP 100
#define TEXT_FS 24
#define TEXT_V_ADD 27
#define TEXT_FONTNAME "Minecraft AE Pixel"

#define MSG_SHOWN_TIME 4000.0		//ms
#define MSG_FS 22
#define MSG_V_ADD 26
#define MSG_FONTNAME "Minecraft AE Pixel"

void AddMessage(const string& s);
void ErrMessage(const string& s);
#define LOG_NAME "Uninstall.LOG"

#ifndef NO_DEBUGLOG
void DebugLog(const string& s)
{
	FILE*fp = fopen(LOG_NAME, "a+");
	fprintf(fp, (s+'\n').c_str());
	fclose(fp);
}
#else
#define DebugLog(...)
#endif

HINSTANCE _hInstance{nullptr};
HINSTANCE _hPrevInstance{nullptr};
LPSTR _lpCmdLine{nullptr};
int _nShowCmd;

bool admin{false};
int scr_w;
int scr_h;
int taskbar_h;

HWND hwnd{nullptr};
HWND hwnd_target{nullptr};

template <typename T>
string CJZAPI str(const T& value)
{
	stringstream ss;
	ss << value;
	string s;
	ss >> s;
	return s;
}
inline bool CJZAPI strequ(char *str, const char *obj)
{	//比较是否一样 
	return (strcmp((const char *)str, obj) == 0 ? true : false);
}
inline bool CJZAPI sequ(const string& s1, const string& s2)
{
	return _stricmp(s1.c_str(), s2.c_str()) == 0;
}
string CJZAPI strtail(const string& s, int cnt = 1) {
	//012 len=3
	//abc   s.substr(2,1)
	if (cnt > s.size())
		return s;
	return s.substr(s.size() - cnt, cnt);
}
string CJZAPI strhead(const string& s, int cnt = 1) {
	if (cnt > s.size())
		return s;
	return s.substr(0, cnt);
}
string CJZAPI strxtail(const string& s, int cnt = 1) {
	if (cnt > s.size())
		return "";
	return s.substr(0, s.size() - cnt);
}
string CJZAPI strxhead(const string& s, int cnt = 1) {
	if (cnt > s.size())
		return "";
	return s.substr(cnt, s.size() - cnt);
}
string CJZAPI strip(const string& s)
{
	string res = s;
	while(!res.empty() && (res.at(0) == '\r' || res.at(0) == '\n' || res.at(0) == '\0'))
		res = res.substr(1, res.size() - 1);
	while(!res.empty() && (res.at(res.size()-1) == '\r' || res.at(res.size()-1) == '\n' || res.at(0) == '\0'))
		res = res.substr(0, res.size() - 1);
	return res;
}

string ConvertErrorCodeToString(DWORD ErrorCode)  
{  
	HLOCAL LocalAddress {nullptr};  
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,  
			    	NULL, ErrorCode, 0, (PTSTR)&LocalAddress, 0, NULL);  
	return strxtail(LPSTR(LocalAddress), 2);  	//去除换行符
}  
BOOL CJZAPI IsFile(const string& lpPath)
{	//是否是文件 
	int res;
#ifndef DEVCPP 
	struct _stat buf;
	res = _stat(lpPath.c_str(), &buf);
#else
	struct _stat64i32 buf;
	res = _stat64i32(lpPath.c_str(), &buf);
#endif
	return (buf.st_mode & _S_IFREG);
}
BOOL CJZAPI IsDir(const string& lpPath)
{	//是否是文件夹
	int res;
#ifndef DEVCPP 
	struct _stat buf;
	res = _stat(lpPath.c_str(), &buf);
#else
	struct _stat64i32 buf;
	res = _stat64i32(lpPath.c_str(), &buf);
#endif
	return (buf.st_mode & _S_IFDIR);
}
inline BOOL CJZAPI ExistFile(const string& strFile) {
	//文件或文件夹都可以
	return !_access(strFile.c_str(), S_OK);//S_OK表示只检查是否存在
}
string CJZAPI GetFileDirectory(string path)
{	//返回的最后会有反斜线
	if (IsDir(path))
	{
		if (strtail(path) != "\\")
			path += "\\";
		return path;
	}
	if (strtail(path) == "\\")
		path = strxtail(path);
	string ret = "";
	bool suc = false;
	for (int i = path.size() - 1; i >= 0; i--)
	{
		if (path.at(i) == '\\')
		{
			ret = path.substr(0, i + 1);
			suc = true;
			break;
		}
	}
	if (!suc)
		SetLastError(3L);	//还是要return的
	if (strtail(ret) != "\\")
		ret += "\\";
	return ret;
}
vector<string> CJZAPI GetDirFiles(const string& dir, const string& filter = "*.*")
{
	if (dir.empty() || (filter != "" && !ExistFile(dir)))
	{
		return vector<string>();
	}
	_finddata_t fileDir;
	ADDRESS lfDir = 0;
	vector<string> files{};
	string dirp = dir + filter;	//它是查找式的
	
	if ((lfDir = _findfirst(dirp.c_str(), &fileDir)) == -1)
	{
		return vector<string>();
	}
	else {
		do {	//遍历目录
			if (!strequ(fileDir.name, ".") && !strequ(fileDir.name, ".."))	//排除这两个狗东西
				files.push_back(string(fileDir.name));
		} while (_findnext(lfDir, &fileDir) == 0);
	}
	_findclose(lfDir);
	return files;
}
void GetDirFilesR_Proc(vector<string>* result, const string& odir /*backslashed*/, const string& childDir, const string& filter)
{
	vector<string> matchedFiles = GetDirFiles(odir + childDir, filter);
	for (auto& f : matchedFiles)
		if (IsFile(odir + childDir + f))
		{
			result->push_back(childDir + f);
		}
	matchedFiles.clear();
	vector<string> all = GetDirFiles(odir + childDir, "*");
	for (auto& ele : all)
		if (IsDir(odir + childDir + ele))
		{
			GetDirFilesR_Proc(result, odir, childDir + ele + "\\", filter);
		}
	
}
vector<string> CJZAPI GetDirFilesR(const string& dir /*backslashed*/, const string& filter = "*.*")
{
	vector<string> result;
	GetDirFilesR_Proc(&result, dir, "", filter);
	return result;
}

inline double CJZAPI EaseOutCubic(double _x)
{
	return 1 - pow(1 - _x, 3);
}
inline double CJZAPI EaseInExpo(double _x)
{
	return fequ(_x, 0.0) ? 0 : pow(2, 10 * _x - 10);
}
template <typename _T>
inline _T CJZAPI ClampA(_T& val, _T min=0, _T max=2147483647) {	//限定范围
	if(val < min) val = min;
	else if(val > max) val = max;
	return val;
}
template <typename _T>
inline _T CJZAPI Clamp(_T val, _T min=0, _T max=2147483647) {	//限定范围
	if(val < min) val = min;
	else if(val > max) val = max;
	return val;
}
template <typename _T>
inline double CJZAPI Lerp(_T startValue, _T endValue, double ratio)
{
	return startValue + (endValue - startValue) * ratio;
}
template <typename _T>
inline double CJZAPI LerpClamp(_T startValue, _T endValue, double ratio)
{
	return Clamp<double>(Lerp(startValue,endValue,ratio), min(startValue,endValue), max(endValue,startValue));
}
inline HFONT CJZAPI CreateFont(int height, int width, LPCSTR lpFamilyName)
{
	return CreateFont(height,width,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,lpFamilyName);
}
inline int GetScreenHeight(void) //获取屏幕高度
{
	return GetSystemMetrics(SM_CYSCREEN);
}
inline int GetScreenWidth(void) //获取屏幕宽度
{
	return GetSystemMetrics(SM_CXSCREEN);
}
RECT CJZAPI GetSystemWorkAreaRect(void) //获取工作区矩形 
{
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0);    // 获得工作区大小
	return rt;
}
LONG CJZAPI GetTaskbarHeight(void) 		//获取任务栏高度 
{	
	INT cyScreen = GetScreenHeight();
	RECT rt = GetSystemWorkAreaRect();
	return (cyScreen - (rt.bottom - rt.top));
}
BOOL CJZAPI IsRunAsAdmin(HANDLE hProcess=GetCurrentProcess()) 
{	//是否有管理员权限 
	BOOL bElevated = FALSE;  	
	HANDLE hToken = NULL;   	// Get current process token	
	if ( !OpenProcessToken(hProcess, TOKEN_QUERY, &hToken ) )		
		return FALSE; 	
	TOKEN_ELEVATION tokenEle;	
	DWORD dwRetLen = 0;   	// Retrieve token elevation information	
	if ( GetTokenInformation( hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen ) ) 	
	{  		
		if ( dwRetLen == sizeof(tokenEle) ) 		
		{			
			bElevated = tokenEle.TokenIsElevated;  		
		}	
	}   	
	CloseHandle( hToken );  	
	return bElevated;  
} 
VOID CJZAPI AdminRun(LPCSTR csExe, LPCSTR csParam=NULL, DWORD nShow=SW_SHOW)  
{ 	//以管理员身份运行
	SHELLEXECUTEINFO ShExecInfo; 
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);  
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS ;  
	ShExecInfo.hwnd = NULL;  
	ShExecInfo.lpVerb = "runas";  
	ShExecInfo.lpFile = csExe/*"cmd"*/; 
	ShExecInfo.lpParameters = csParam;   
	ShExecInfo.lpDirectory = NULL;  
	ShExecInfo.nShow = nShow;  
	ShExecInfo.hInstApp = NULL;   
	BOOL ret = ShellExecuteEx(&ShExecInfo);  
	/*WaitForSingleObject(ShExecInfo.hProcess, INFINITE);  
	  GetExitCodeProcess(ShExecInfo.hProcess, &dwCode);  */
	CloseHandle(ShExecInfo.hProcess);
	return;
} //from https://blog.csdn.net/mpp_king/article/details/80194563
HANDLE CJZAPI GetProcessHandle(DWORD pid)	//通过进程ID获取进程句柄
{
	return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
}HANDLE CJZAPI GetProcessHandle(LPCSTR lpName)	//通过进程名获取进程句柄
{	//******警告！区分大小写！！！！******// 
	//*****警告！必须加扩展名！！！！*****// 
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
	{
		//cout << "\n进程：" << pe.szExeFile;
		if (!_stricmp(pe.szExeFile, lpName))
		{
			CloseHandle(hSnapshot);
			return GetProcessHandle(pe.th32ProcessID);
		}
	}
	CloseHandle(hSnapshot);
	return NULL;
}
//from  https://blog.csdn.net/fzuim/article/details/60954959
string GetProcessName(DWORD pid)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return string("");
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
	{
		if (pe.th32ProcessID == pid)
		{
			CloseHandle(hSnapshot);
			//			  cout<<"text="<<pe.szExeFile;	
			//					pname=pe.szExeFile;	
			//			  return (LPSTR)pe.szExeFile;	
			return (string)pe.szExeFile;
		}
	}
	CloseHandle(hSnapshot);
	return string("");
}
bool CJZAPI HaveProcess(DWORD pid)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return false;
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
		if (pe.th32ProcessID == pid)
		{
			CloseHandle(hSnapshot);
			return true;
		}
	CloseHandle(hSnapshot);
	return false;
}
BOOL CJZAPI HaveProcess(LPCSTR lpName)
{	//******警告！区分大小写！！！！******// 
	//*****警告！必须加扩展名！！！！*****// 
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
		if (!_stricmp(pe.szExeFile, lpName))	//相同名称 忽略大小写
		{
			CloseHandle(hSnapshot);
			return true;
		}
	CloseHandle(hSnapshot);
	return false;
}
bool FreezeProcess(HANDLE hProc)	//冻结进程
{
	int pid = GetProcessId(hProc);
	THREADENTRY32 th32;
	th32.dwSize = sizeof(th32);
	
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		ErrMessage("CreateToolhelp32Snapshot 调用失败");
		return false;
	}
	
	bool b;
	b = Thread32First(hThreadSnap, &th32);
	while (b)
	{
		if (th32.th32OwnerProcessID == pid)
		{
			HANDLE oth = OpenThread(THREAD_ALL_ACCESS, FALSE, th32.th32ThreadID);
			if (!(SuspendThread(oth)))
			{
				//				SetColor(11, 0);
				//				printf("\n已冻结线程ID: %d", th32.th32ThreadID);
				//				printf("\n已冻结进程ID: %d", th32.th32OwnerProcessID);
			}
			CloseHandle(oth);
			break;
		}
		Thread32Next(hThreadSnap, &th32);
	}
	CloseHandle(hThreadSnap);
	return true;
}
bool UnfreezeProcess(HANDLE hProc)	//解冻进程
{
	int pid = GetProcessId(hProc);
	THREADENTRY32 th32;
	th32.dwSize = sizeof(th32);
	
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		ErrMessage("CreateToolhelp32Snapshot 调用失败");
		return false;
	}
	
	bool b;
	b = Thread32First(hThreadSnap, &th32);
	while (b)
	{
		if (th32.th32OwnerProcessID == pid)
		{
			HANDLE oth = OpenThread(THREAD_ALL_ACCESS, FALSE, th32.th32ThreadID);
			if (!(ResumeThread(oth)))
			{
				//				printf("\n已解冻线程ID: %d", th32.th32ThreadID);
				//				printf("\n已解冻进程ID: %d", th32.th32OwnerProcessID);
			}
			CloseHandle(oth);
			break;
		}
		Thread32Next(hThreadSnap, &th32);
	}
	CloseHandle(hThreadSnap);
	return true;
}

typedef LONG (__stdcall *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);
DWORD GetParentProcessId(DWORD dwProcessId)
{
	LONG						status;
	DWORD						dwParentPID = (DWORD)-1;
	HANDLE						hProcess;
	PROCESS_BASIC_INFORMATION	pbi;
	
	PROCNTQSIP NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(  
		GetModuleHandle("ntdll"), "NtQueryInformationProcess"); 
	if(NULL == NtQueryInformationProcess)
	{
		return (DWORD)-1;
	}
	// Get process handle
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE, dwProcessId);
	if (!hProcess)
	{
		return (DWORD)-1;
	}
	// Retrieve information
	status = NtQueryInformationProcess( hProcess,
		ProcessBasicInformation,
		(PVOID)&pbi,
		sizeof(PROCESS_BASIC_INFORMATION),
		NULL
		);
	// Copy parent Id on success
	if  (!status)
	{
		dwParentPID = pbi.InheritedFromUniqueProcessId;
	}
	CloseHandle (hProcess);
	return dwParentPID;
}	//from https://www.cnblogs.com/jkcx/p/7457339.html

HMODULE hKernel = nullptr;
typedef WINBOOL (WINAPI *T_EnumProcessModules) (HANDLE, HMODULE*, DWORD, LPDWORD);
typedef WINBOOL (WINAPI *T_GetModuleFileNameExA) (HANDLE, HMODULE, LPSTR, DWORD);
string CJZAPI GetProcessPath(HANDLE hProc)	//获取进程EXE绝对路径
{
	//注意：动态调用！
	hKernel = LoadLibrary("KERNEL32.dll");	//核心32 DLL模块句柄
	if(!hKernel || hKernel == INVALID_HANDLE_VALUE)
	{
		ErrMessage("无法获取Kernel32.dll的模块句柄");
		return string("");
	}
	T_EnumProcessModules EPM = nullptr;		//函数变量 
	T_GetModuleFileNameExA GMFNE = nullptr;	//函数变量 
	EPM = (T_EnumProcessModules)GetProcAddress(hKernel, "K32EnumProcessModules");
	GMFNE = (T_GetModuleFileNameExA)GetProcAddress(hKernel, "K32GetModuleFileNameExA");
	if (!EPM || !GMFNE)
	{	//没找到 
		FreeLibrary(hKernel);
		ErrMessage("加载Kernel32.dll时出错: 找不到指定的函数: -> K32EnumProcessModules() -> K32GetModuleFileNameExA()");
		return string("");
	}
	
	//关键操作 
	HMODULE hMod;
	DWORD need;
	CHAR thePath[MAX_PATH]{ 0 };
	EPM(hProc, &hMod, sizeof(hMod), &need);
	GMFNE(hProc, hMod, thePath, sizeof(thePath));
	FreeLibrary(hKernel);
	return string(thePath);
}
string GetProcessPath2(DWORD pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess != NULL)
	{
		char buffer[MAX_PATH];
		DWORD dwSize = GetModuleFileNameExA(hProcess, NULL, buffer, MAX_PATH);
		CloseHandle(hProcess);
		if (dwSize != 0)
		{
			return string(buffer);
		}
	}else{
		ErrMessage("无法获取进程句柄");
	}
	return "";
}
//-----------------------------------------------------------------------------------------------------------   
// 函数: UninjectDll   
// 功能: 从目标进程中卸载一个指定 Dll 模块文件.   
// 参数: [in] const TCHAR* ptszDllFile - Dll 文件名及路径   
//       [in] HANDLE hProc - 目标进程句柄
// 返回: bool - 卸载成功返回 true, 卸载失败则返回 false.   
// 说明: 采用远程线程注入技术实现   
//-----------------------------------------------------------------------------------------------------------   
bool CJZAPI UninjectDll(const TCHAR* ptszDllFile, HANDLE hProc, bool mute = false)
{
	// 参数无效   
	if (NULL == ptszDllFile || 0 == lstrlen(ptszDllFile))
		return false;
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	HANDLE hProcess = hProc;
	HANDLE hThread = NULL;
	DWORD pid;
	pid = GetProcessId(hProc);
	// 获取模块快照   
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (INVALID_HANDLE_VALUE == hModuleSnap)
	{
		if (!mute)
			ErrMessage("获取模块快照时遭到失败");
		return false;
	}
	MODULEENTRY32 me32;
	memset(&me32, 0, sizeof(MODULEENTRY32));
	me32.dwSize = sizeof(MODULEENTRY32);
	// 开始遍历   
	if (FALSE == Module32First(hModuleSnap, &me32))
	{
		if (!mute)
			ErrMessage("没有模块或是模块遍历遭到错误");
		CloseHandle(hModuleSnap);
		return false;
	}
	// 遍历查找指定模块   
	bool isFound = false;
	do
	{
		isFound = (0 == _stricmp(me32.szModule, ptszDllFile) || 0 == _stricmp(me32.szExePath, ptszDllFile));
		if (isFound) // 找到指定模块   
		{
			break;
		}
	} while (TRUE == Module32Next(hModuleSnap, &me32));
	CloseHandle(hModuleSnap);
	if (false == isFound)
	{
		if (!mute)
			ErrMessage("模块未找到");
		return false;
	}
	// 从 Kernel32.dll 中获取 FreeLibrary 函数地址   
	LPTHREAD_START_ROUTINE lpThreadFun = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("Kernel32"), "FreeLibrary");
	if (NULL == lpThreadFun)
	{
		if (!mute)
			ErrMessage("无法从 Kernel32.dll 取函数 FreeLibrary");
		CloseHandle(hProcess);
		return false;
	}
	// 创建远程线程调用 FreeLibrary  
	hThread = CreateRemoteThread(hProcess, NULL, 0, lpThreadFun, me32.modBaseAddr /* 模块地址 */, 0, NULL);
	if (NULL == hThread)
	{
		if (!mute)
			ErrMessage("远程线程句柄为空");
		CloseHandle(hProcess);
		return false;
	}
	// 等待远程线程结束   
	WaitForSingleObject(hThread, INFINITE);
	// 清理   
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return true;
}
void UninjectDllFromAllProcesses(const char* dllpath)
{
	if(!dllpath || !ExistFile(dllpath))
		return;
	
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		ErrMessage("无法创建进程快照");
		return;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
	{
		HANDLE hProc = GetProcessHandle(pe.th32ProcessID);
		if (hProc && hProc != INVALID_HANDLE_VALUE)
		{
			if (UninjectDll(dllpath, hProc, true))
				AddMessage("成功从 " + str(pe.szExeFile) + "中卸载 " + dllpath);
		}	
	}
	CloseHandle(hSnapshot);
}

#define WVC_AMP 12
#define WVC_OMEGA 13.0
#define WVC_PHASE0 0
clock_t lastWvBeg=0;
inline COLORREF WaveColor(COLORREF originClr, float phi=0.0f) {	//originClr将成为最大值
	//闪烁的颜色 赋予游戏文字灵性
	short val = WVC_AMP * sin(WVC_OMEGA*((clock()-lastWvBeg)/1000.0)+WVC_PHASE0+phi) - WVC_AMP*2;
	short r=GetRValue(originClr)+val,g=GetGValue(originClr)+val,b=GetBValue(originClr)+val;
	ClampA<short>(r,2,255);
	ClampA<short>(g,2,255);
	ClampA<short>(b,2,255);
	return RGB(r,g,b);
}
// 辅助函数：RGB到HSV颜色转换
void RGBtoHSV(COLORREF rgb, double& h, double& s, double& v) {
	double r = GetRValue(rgb) / 255.0;
	double g = GetGValue(rgb) / 255.0;
	double b = GetBValue(rgb) / 255.0;
	
	double minVal = std::min(std::min(r, g), b);
	double maxVal = std::max(std::max(r, g), b);
	double delta = maxVal - minVal;
	
	// 计算色相
	if (delta > 0) {
		if (maxVal == r)
			h = 60.0 * fmod((g - b) / delta, 6.0);
		else if (maxVal == g)
			h = 60.0 * ((b - r) / delta + 2.0);
		else if (maxVal == b)
			h = 60.0 * ((r - g) / delta + 4.0);
	} else {
		h = 0.0;
	}
	
	// 计算饱和度和亮度
	s = (maxVal > 0) ? (delta / maxVal) : 0.0;
	v = maxVal;
}

// 辅助函数：HSV到RGB颜色转换
COLORREF HSVtoRGB(double h, double s, double v) {
	int hi = static_cast<int>(floor(h / 60.0)) % 6;
	double f = h / 60.0 - floor(h / 60.0);
	double p = v * (1.0 - s);
	double q = v * (1.0 - f * s);
	double t = v * (1.0 - (1.0 - f) * s);
	
	switch (hi) {
		case 0: return RGB(static_cast<int>(v * 255), static_cast<int>(t * 255), static_cast<int>(p * 255));
		case 1: return RGB(static_cast<int>(q * 255), static_cast<int>(v * 255), static_cast<int>(p * 255));
		case 2: return RGB(static_cast<int>(p * 255), static_cast<int>(v * 255), static_cast<int>(t * 255));
		case 3: return RGB(static_cast<int>(p * 255), static_cast<int>(q * 255), static_cast<int>(v * 255));
		case 4: return RGB(static_cast<int>(t * 255), static_cast<int>(p * 255), static_cast<int>(v * 255));
		case 5: return RGB(static_cast<int>(v * 255), static_cast<int>(p * 255), static_cast<int>(q * 255));
		default: return RGB(0, 0, 0);  // Shouldn't reach here
	}
}

// 主函数：返回随时间变化的彩虹色
COLORREF RainbowColor(float phi = 0.0f) {
					//phi: 0.0~1.0
	// 假设时间按秒计算，这里使用系统时间或其他适当的时间源
	double timeInSeconds = (GetTickCount()) / 1000.0 + 12.0 * phi;
	
	// 色相按时间变化
	double hue = fmod(timeInSeconds * 30.0, 360.0);  // 假设每秒变化30度
	
	// 饱和度和亮度保持不变
	double saturation = 1.0;
	double value = 1.0;
	
	// 将HSV颜色转换为RGB并返回
	return HSVtoRGB(hue, saturation, value);
}
COLORREF RainbowColorQuick(float phi = 0.0f) {
	// 假设时间按秒计算，这里使用系统时间或其他适当的时间源
	double timeInSeconds = GetTickCount() / 1000.0 + 6.0 * phi;
	
	// 色相按时间变化
	double hue = fmod(timeInSeconds * 60.0, 360.0);  // 假设每秒变化30度
	
	// 饱和度和亮度保持不变
	double saturation = 1.0;
	double value = 1.0;
	
	// 将HSV颜色转换为RGB并返回
	return HSVtoRGB(hue, saturation, value);
}
COLORREF CJZAPI StepColor(COLORREF startColor, COLORREF endColor, double rate) 
{
	if(fequ(rate,0.0))	return startColor;
	if(fequ(rate,1.0)) return endColor;
	//颜色的渐变
	int r = (GetRValue(endColor) - GetRValue(startColor));
	int g = (GetGValue(endColor) - GetGValue(startColor));
	int b = (GetBValue(endColor) - GetBValue(startColor));
	
	int nSteps = max(abs(r), max(abs(g), abs(b)));
	if (nSteps < 1) nSteps = 1;
	
	// Calculate the step size for each color
	float rStep = r / (float)nSteps;
	float gStep = g / (float)nSteps;
	float bStep = b / (float)nSteps;
	
	// Reset the colors to the starting position
	float fr = GetRValue(startColor);
	float fg = GetGValue(startColor);
	float fb = GetBValue(startColor);
	
	COLORREF color;
	for (int i = 0; i < int(nSteps * rate); i++) {
		fr += rStep;
		fg += gStep;
		fb += bStep;
		color = RGB((int)(fr + 0.5), (int)(fg + 0.5), (int)(fb + 0.5));
		//color 即为重建颜色
	}
	return color;
}//from https://bbs.csdn.net/topics/240006897 , owner: zgl7903
inline COLORREF CJZAPI InvertedColor(COLORREF color)
{
	return RGB(GetBValue(color), GetGValue(color), GetRValue(color));
}
//////////////////////////////////////////////////////////////////////////////

HDC hdcOrigin{nullptr};
HDC hdcBuffer{nullptr};
PAINTSTRUCT ps;
HFONT hFontText{nullptr};
HFONT hFontMsg{nullptr};

void ClearDevice(HDC _hdc = hdcBuffer, HWND _hwnd = hwnd)
{
	// 清屏：使用透明色填充整个客户区域
	RECT rcClient;
	GetClientRect(_hwnd, &rcClient);
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(_hdc, &rcClient, hBrush);
	DeleteObject(hBrush);
}
void BeginDraw()
{
	hdcOrigin = BeginPaint(hwnd, &ps);
	hFontText = CreateFontA(TEXT_FS, 0, TEXT_FONTNAME);
	hFontMsg = CreateFontA(MSG_FS, 0, MSG_FONTNAME);
	SelectObject(hdcBuffer, hFontText);
	SetBkMode(hdcBuffer, TRANSPARENT);
}
void EndDraw()
{
	DeleteObject(hFontText);
	DeleteObject(hFontMsg);
	EndPaint(hwnd, &ps);
}

vector<pair<string, clock_t>> s_msgs{};
#define MSG_MAX_CNT ((scr_h - taskbar_h) / MSG_V_ADD)

void AddMessage(const string& s)
{
	DebugLog(s);
	
	s_msgs.push_back(make_pair(s, clock()));
	if(s_msgs.size() > MSG_MAX_CNT)
		s_msgs.erase(s_msgs.begin());
}
void ErrMessage(const string& s)
{
	AddMessage("错误：" + s + "  ( " + ConvertErrorCodeToString(GetLastError()) + " )");
}

const string s_cmdguides[] {
	"[Alt+1] Uninstall 卸载",
	"[Alt+2] Kill and Visit 诛并访其家",
	"[Alt+3] Delete EXE 删除程序",
	"[Alt+4] Force Exterminate 强制灭门",
	"[Alt+5] Freeze 冻结",
	"[Alt+6] Soul Detach 灵魂脱壳",
	"[Alt+7] Visit 登门拜访",
};

void DrawUI()
{
	DWORD pid{0L};
	DWORD ppid{0L};
	
	GetWindowThreadProcessId(hwnd_target, &pid);
	ppid = GetParentProcessId(pid);
	
	string procName {GetProcessName(pid)};
	string pprocName {GetProcessName(ppid)};
	
	SelectObject(hdcBuffer, hFontText);
	SetBkMode(hdcBuffer, admin?OPAQUE:TRANSPARENT);
	
	vector<string> strs
	{
		"Target Application 目标应用:",
		procName + "  ( " + str(pid) + " )  ",
		"",
	};
	
	for(const auto& S : s_cmdguides)
		strs.emplace_back(S);
	strs.emplace_back("");
	strs.emplace_back("Parent:  " + pprocName + "  ( " + str(ppid) + " )  ");
	if (!admin)
		strs.emplace_back("[Alt+A] Admin 提权");
	
	int i{0};
	for(const auto& s : strs)
	{
		if(s.empty())
		{
			++i;
			continue;
		}
		if (admin && i != strs.size() - 1)
		{
			SetTextColor(hdcBuffer, RainbowColor(i * 0.1f));
			SetBkColor(hdcBuffer, StepColor(InvertedColor(RainbowColor(i * 0.1f)), RGB(1,1,1), 0.8));
		}else{
			SetTextColor(hdcBuffer, StepColor(RainbowColor(i * 0.1f), RGB(255, 200, 200), 0.8));
		}
		TextOutA(hdcBuffer, TEXT_LEFT, TEXT_TOP + i * TEXT_V_ADD, s.c_str(),s.length());
		++i;
	}
	
	if(!s_msgs.empty())
	{
		SetBkColor(hdcBuffer, RGB(1, 1, 1));
		SelectObject(hdcBuffer, hFontMsg);
		int j{0};
		for(auto const& _msg : s_msgs)
		{
			if(clock() - _msg.second >= MSG_SHOWN_TIME)
			{
				++j;
				continue;
			}
			
			SetTextColor(hdcBuffer, StepColor(RGB(0, 255, 0), RGB(255, 0, 0), (clock() - _msg.second) / MSG_SHOWN_TIME));
			RECT rt { .left = 0,
					  .top = scr_h - taskbar_h - MSG_V_ADD * s_msgs.size() + MSG_V_ADD * j,  
				      .right = scr_w,  
				      .bottom = scr_h - taskbar_h + MSG_FS - MSG_V_ADD * s_msgs.size() + MSG_V_ADD * j,};
			DrawTextA(hdcBuffer, _msg.first.c_str(), _msg.first.length(), &rt, DT_CENTER);
			++j;
		}
	}
//	else{
//		const string _msg{"-/-"};
//		SetTextColor(hdcBuffer, RGB(255, 0, 0));
//		RECT rt { .left = 0,.top = scr_h - taskbar_h - 100,  .right = scr_w,  .bottom = scr_h - taskbar_h - 60,};
//		DrawTextA(hdcBuffer, _msg.c_str(), _msg.length(), &rt, DT_CENTER);
//	}
}

void DoUninstall()
{
	DWORD pid{0L};
	BOOL bRet = GetWindowThreadProcessId(hwnd_target, &pid);
	if(!bRet)
	{
		ErrMessage("无法获取 PID");
		return;
	}
//	HANDLE hProc = GetProcessHandle(pid);
//	if(!hProc || hProc == INVALID_HANDLE_VALUE)
//	{
//		ErrMessage("无法获取进程句柄");
//		return;
//	}
	string path = GetProcessPath2(pid);
	if(path.empty())
	{
		ErrMessage("无法获取进程家的路径");
	}
	string dirpath {GetFileDirectory(path)};
	AddMessage("目录路径：" + dirpath);
	vector<string> uninsts = GetDirFilesR(dirpath, "uninst*.exe");
	if(uninsts.empty())
	{
		AddMessage("错误：没有发现任何卸载程序！可使用灭门应对之");
		return;
	}
	if(uninsts.size() > 1)
		AddMessage("有多项程序符合条件，将选择第一个" + uninsts.at(0));
	else
		AddMessage("唯一符合程序：" + uninsts.at(0));
	
	AdminRun((dirpath + uninsts.at(0)).c_str());
	Sleep(500);
	if(HaveProcess(uninsts.at(0).c_str()))
		AddMessage("卸载程序已经开始运行");
	else
		ErrMessage("卸载程序运行失败");
}
void DoKillAndVisit()
{
	DWORD pid{0L};
	BOOL bRet = GetWindowThreadProcessId(hwnd_target, &pid);
	if(!bRet)
	{
		ErrMessage("无法获取 PID");
		return;
	}
	HANDLE hProc = GetProcessHandle(pid);
	if(!hProc || hProc == INVALID_HANDLE_VALUE)
	{
		ErrMessage("无法获取进程句柄");
		return;
	}
	string path = GetProcessPath2(pid);
	if(path.empty())
	{
		ErrMessage("无法获取进程家的路径");
	}
	bRet = TerminateProcess(hProc, 0);
	if(!bRet)
	{
		ErrMessage("终止进程失败");
		return;
	}
	AddMessage("终止进程 Pid=" + str(pid) + " hProc=" + str(reinterpret_cast<ADDRESS>(hProc)));
	
	if(path.empty())
		return;
	if(!ExistFile(path))
	{
		ErrMessage("路径无效：" + path);
		return;
	}
//	AddMessage(path);
	string dirpath {strxtail(GetFileDirectory(path))};
	WinExec(("explorer.exe \"" + dirpath + "\"").c_str(), SW_SHOWMAXIMIZED);
	AddMessage("拜访：" + dirpath);
}
void DoDeleteExe()
{
	DWORD pid{0L};
	BOOL bRet = GetWindowThreadProcessId(hwnd_target, &pid);
	if(!bRet)
	{
		ErrMessage("无法获取 PID");
		return;
	}
	HANDLE hProc = GetProcessHandle(pid);
	if(!hProc || hProc == INVALID_HANDLE_VALUE)
	{
		ErrMessage("无法获取进程句柄");
		return;
	}
	string path = GetProcessPath2(pid);
	if(path.empty())
	{
		ErrMessage("无法获取进程家的路径");
	}
	bRet = TerminateProcess(hProc, 0);
	if(!bRet)
	{
		ErrMessage("终止进程失败");
		return;
	}
	AddMessage("终止进程 Pid=" + str(pid) + " hProc=" + str(reinterpret_cast<ADDRESS>(hProc)));
	
	if(path.empty())
		return;
	if(!ExistFile(path))
	{
		ErrMessage("路径无效：" + path);
		return;
	}
	bRet = DeleteFileA(path.c_str());
	if(!bRet)
	{
		ErrMessage("EXE 删除失败");
		return;
	}
	AddMessage("删除成功：" + path);
}

int _tot_cnt = 0;
int _suc_cnt = 0;
vector<string> _remain_files;	//残党
vector<string> _remain_folders;

VOID CJZAPI _rmfolder(const string& dir)	//dir必须\结尾
{
	vector<string> files = GetDirFiles(dir, "*");
	_tot_cnt += files.size();			//including folders
	
	for (int i = 0; i < files.size(); i++)
	{
		if (IsFile(dir + files.at(i)))
		{
#ifndef DELETE_NO_DELAY
			Sleep(1);
#endif
			BOOL ret = DeleteFileA((dir + files.at(i)).c_str());
			if (!ret)
			{
				ErrMessage("[×] 未能成功杀死 " + files.at(i));
				_remain_files.emplace_back(dir + files.at(i));
			}
			else {
				AddMessage("[√] 成功杀死 " + files.at(i));
				_suc_cnt++;
			}
		}
		else {
			AddMessage("----- 进入目录  " + files.at(i) +"  -----");
			string dir_new = dir;
			dir_new += files.at(i) + "\\";
#ifndef DELETE_NO_DELAY
			Sleep(2);
#endif
			_rmfolder(dir_new);	//递归灭代
			BOOL ret = RemoveDirectoryA((dir + files.at(i)).c_str());	//rmdir
			if (!ret)
			{
				ErrMessage("[！]抄家失败");
				_remain_folders.emplace_back((dir + files.at(i)));
			}
			else {
				AddMessage("[√] 成功被抄家");
				_suc_cnt++;
			}
		}
	}
}
VOID CJZAPI _rmfolder_mute(const string& dir)	//dir必须\结尾
{
	vector<string> files = GetDirFiles(dir, "*");
	
	for (int i = 0; i < files.size(); i++)
	{
		if (IsFile(dir + files.at(i)))
		{
#ifndef DELETE_NO_DELAY
			Sleep(1);
#endif
			DeleteFileA((dir + files.at(i)).c_str());
		}
		else {
			string dir_new = dir;
			dir_new += files.at(i) + "\\";
#ifndef DELETE_NO_DELAY
			Sleep(2);
#endif
			_rmfolder_mute(dir_new);	//递归灭代
			RemoveDirectoryA((dir + files.at(i)).c_str());	//rmdir
		}
	}
}

bool CJZAPI TerminateFamily(DWORD pid, const string& dirpath)
{
	DebugLog("TerminateFamily:  pid="+str(pid)+" dirpath="+dirpath);
	bool parent_in_family{false};
	DWORD ppid = GetParentProcessId(pid);
	string ppath = GetFileDirectory(GetProcessPath2(ppid));
	if(!ppath.empty() && ppath.find(dirpath) != string::npos)
		parent_in_family = true;
	
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		ErrMessage("CreateToolhelp32Snapshot 调用失败");
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	int suc_cnt = 0;
	int tot_cnt = 0;
	vector<DWORD> pids {};
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
	{
		bool yes{false};
		
		if(pe.th32ProcessID == pid								//本人
		|| parent_in_family && pe.th32ParentProcessID == ppid	//同属一个父亲
		|| parent_in_family && pe.th32ProcessID == ppid			//父亲本人
		|| pe.th32ParentProcessID == pid						//子女
		  )
		{
			yes = true;
		}else{
			string path = GetProcessPath2(pe.th32ProcessID);
			if(!path.empty())
			{
				if(path.find(dirpath) != string::npos)
				{
					DebugLog(str(pe.szExeFile)+"    "+path+"   【满足条件 v】");
					yes = true;			//位置相近
				}
				else
					DebugLog(str(pe.szExeFile)+"    "+path);
			}
		}
		
		if(yes)
		{
			pids.push_back(pe.th32ProcessID);
			HANDLE hProc = GetProcessHandle(pe.th32ProcessID);
			if(hProc && hProc != INVALID_HANDLE_VALUE)
			{
				tot_cnt++;
				BOOL bRet = TerminateProcess(hProc, 0);
				if(!bRet)
					ErrMessage("终止进程 " + str(pe.szExeFile) + " 失败");
				else
				{
					suc_cnt++;
					AddMessage("成功终止进程 " + str(pe.szExeFile));
				}
				CloseHandle(hProc);
			}else{
				ErrMessage("无法获取进程 " + str(pe.szExeFile) + " 句柄");
			}
		}
	}
	CloseHandle(hSnapshot);
	Sleep(500);
	if (suc_cnt > 0)
		AddMessage("成功剿灭 " + str(suc_cnt) + " 个进程, 共有 " + str(tot_cnt) + " 个嫌疑进程");
	else
		AddMessage(str(tot_cnt) + " 个嫌疑进程一个都没剿灭qwq");
	string spids{};
	for(const auto& p : pids)
		spids += str(p) + ", ";
	AddMessage("所有嫌疑进程[" + str(pids.size()) + "]： " + strxtail(spids));
	
	return suc_cnt == tot_cnt;
}
BOOL CJZAPI ExterminateFamily(const string& dirpath)	//灭族：斩杀所有同目录下及以下的文件
{
	_remain_files.clear();
	_suc_cnt = 0;
	AddMessage("灭门：" + dirpath);
	_tot_cnt = 0;
	
	Sleep(800);
	
	_rmfolder(dirpath);
	
	Sleep(1000);
	
	if(_suc_cnt < _tot_cnt)
	{
		AddMessage("剩余 " + str(_tot_cnt - _suc_cnt) + " 个余党，开启围剿工作");
		for(auto& p : _remain_files)
		{
			if(p.length() > 4 && sequ(strtail(p, 4),".dll") )
			{	//被某进程占用，优先考虑explorer.exe
				HANDLE hProcExplorer = GetProcessHandle("explorer.exe");
				if(hProcExplorer && hProcExplorer != INVALID_HANDLE_VALUE)
				{
					if(UninjectDll((p).c_str(), hProcExplorer))
						AddMessage("[*] 成功从 explorer.exe 卸载 " + p);
					else{	//其他进程占用 暴力枚举
						UninjectDllFromAllProcesses(p.c_str());
					}
				}
				else
					ErrMessage("[！] 无法获取 explorer.exe 进程句柄");
			}
			bool res = DeleteFileA((p).c_str());
			if (res)
			{
				_suc_cnt++;
				AddMessage("余党 " + p + " 已被剿灭");
			}
			else
				ErrMessage("[!] 余党 " + p + " 仍未被剿灭");
		}
		for(auto& d : _remain_folders)
		{
			bool res = RemoveDirectoryA((d).c_str());
			if (res)
			{
				_suc_cnt++;
				AddMessage("残党 " + d + " 已被剿灭");
			}else
			{
				if(GetLastError() == 145)	//目录不是空的。
				{
					_rmfolder_mute((d+"\\").c_str());
					bool res2 = RemoveDirectoryA((d).c_str());
					if (res2)
					{
						_suc_cnt++;
						AddMessage("残党 " + d + " 已被剿灭");
					}else
						ErrMessage("[！] 残党 " + d + " 仍未被剿灭");
				}else
					ErrMessage("[！] 残党 " + d + " 仍未被剿灭");
			}
				
		}
		Sleep(1000);
	}
	
	AddMessage("成功杀死 " + str(_suc_cnt) + " 人，共有 " + str(_tot_cnt) + " 人");
	double ratio = ((double)_suc_cnt / _tot_cnt * 100.0f);
	AddMessage("灭族率  " + str(ratio) + "%");
	if (fequ(ratio, 1.0))
	{
		AddMessage("   恭喜~ 九族已诛~   ");
	}
	if (_suc_cnt == 0)
		return FALSE;
	return TRUE;
}
void DoForceExterminate()
{
	DWORD pid{0L};
	BOOL bRet = GetWindowThreadProcessId(hwnd_target, &pid);
	if(!bRet)
	{
		ErrMessage("无法获取 PID");
		return;
	}
//	HANDLE hProc = GetProcessHandle(pid);
//	if(!hProc || hProc == INVALID_HANDLE_VALUE)
//	{
//		ErrMessage("无法获取进程句柄");
//		return;
//	}
	string path = GetProcessPath2(pid);
	if(path.empty())
	{
		ErrMessage("无法获取进程家的路径");
	}
	
	if(path.empty())
		return;
	if(!ExistFile(path))
	{
		ErrMessage("路径无效：" + path);
		return;
	}
	string dirpath { GetFileDirectory(path) };
	
	bool ok = TerminateFamily(pid, dirpath);
	if(!ok)
	{
		int ch = MessageBoxA(nullptr, ("进程剿灭未完全成功，还要继续灭门吗？\n" + dirpath).c_str(), "AutoUninstall ", MB_ICONQUESTION|MB_YESNO);
		if(IDNO == ch)
		{
			AddMessage("取消灭门");
			return;
		}
	}
	ExterminateFamily(dirpath);
}
void DoFreeze()
{
	DWORD pid{0L};
	BOOL bRet = GetWindowThreadProcessId(hwnd_target, &pid);
	if(!bRet)
	{
		ErrMessage("无法获取 PID");
		return;
	}
	HANDLE hProc = GetProcessHandle(pid);
	if(!hProc || hProc == INVALID_HANDLE_VALUE)
	{
		ErrMessage("无法获取进程句柄");
		return;
	}
	bRet = FreezeProcess(hProc);
	if(!bRet)
	{
		ErrMessage("进程冻结失败");
		return;
	}
	AddMessage("冻结进程 Pid="+str(pid)+"  hProc="+str(reinterpret_cast<ADDRESS>(hProc)));
}
VOID CJZAPI DisfigureWindow(HWND hwnd)
{
	LONG avatar = GetWindowLong(hwnd,0);
	avatar = avatar & ~WS_CAPTION;		//去除标题栏
	SetWindowLong(hwnd, GWL_STYLE, avatar);
}
void DoSoulDetach()
{
	DisfigureWindow(hwnd_target);
	AddMessage("使其灵魂脱壳");
}
void DoVisit()
{
	DWORD pid{0L};
	BOOL bRet = GetWindowThreadProcessId(hwnd_target, &pid);
	if(!bRet)
	{
		ErrMessage("无法获取 PID");
		return;
	}
//	HANDLE hProc = GetProcessHandle(pid);
//	if(!hProc || hProc == INVALID_HANDLE_VALUE)
//	{
//		ErrMessage("无法获取进程句柄");
//		return;
//	}
	string path = GetProcessPath2(pid);
	if(path.empty())
	{
		ErrMessage("无法获取进程家的路径");
	}
	string dirpath {strxtail(GetFileDirectory(path))};
	WinExec(("explorer.exe \"" + dirpath + "\"").c_str(), SW_SHOWMAXIMIZED);
	AddMessage("登门拜访：" + dirpath);
}

using VOIDFUNC = void(*)(void);
const VOIDFUNC pFunctions[]
{
	DoUninstall,
	DoKillAndVisit,
	DoDeleteExe,
	DoForceExterminate,
	DoFreeze,
	DoSoulDetach,
	DoVisit,
};
void Hotkeys()
{
	if(KEY_DOWN(VK_LMENU))
	{
		for(int i{1}; i <= 7; ++i)
		{
			if(KEY_DOWN('0' + i))
			{
//					MessageBoxA(nullptr, strxhead(s_cmdguides[i-1],8).c_str(), "6", 0);
				AddMessage(strxhead(s_cmdguides[i-1],8));
				thread thr(pFunctions[i-1]);
				thr.detach();
				while(KEY_DOWN('0' + i));
				break;
			}
		}
	}
//	if(KEY_DOWN(VK_SPACE))
//	{
//		while(KEY_DOWN(VK_SPACE));
//		AddMessage(str(clock()));
//	}
	if(!admin && KEY_DOWN(VK_MENU) && KEY_DOWN('A'))
	{
		AddMessage("<!> 申请管理员身份...");
		while(KEY_DOWN(VK_MENU) && KEY_DOWN('A'));
		AdminRun(_pgmptr);
		SendMessage(hwnd, WM_DESTROY, 0, 0);
		Sleep(500);
		exit(0);
	}
}

#define TIMER_PAINT_CD 10L
VOID CALLBACK TimerProc_Paint(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  UINT_PTR idEvent,
	_In_  DWORD dwTime
	)
{
	RECT rect;
	GetClientRect(hwnd,&rect);
	InvalidateRect(hwnd, &rect, FALSE);	//会发送WM_PAINT消息
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) 
{
	static HBITMAP hBitmap = NULL;
	
	switch(Message) 
	{
		case WM_CREATE:{
			hdcBuffer = CreateCompatibleDC(NULL);
			SetTimer(hwnd, 0, TIMER_PAINT_CD, TimerProc_Paint);
			break;
		}
		case WM_DESTROY: {
			KillTimer(hwnd, 1);
			if (hdcBuffer)
				DeleteDC(hdcBuffer), hdcBuffer = nullptr;
			if (hBitmap)
				DeleteObject(hBitmap), hBitmap = nullptr;
			PostQuitMessage(0);
			break;
		}
		case WM_PAINT:{
			BeginDraw();
			
			// 获取客户区域的大小
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			int clientWidth = rcClient.right - rcClient.left;
			int clientHeight = rcClient.bottom - rcClient.top;
			
			// 创建双缓冲
			if (hBitmap)
				DeleteObject(hBitmap);
			
			hBitmap = CreateCompatibleBitmap(hdcOrigin, clientWidth, clientHeight);
			SelectObject(hdcBuffer, hBitmap);
			
			ClearDevice();
			DrawUI();
			
			// 将缓冲区的内容一次性绘制到屏幕上
			BitBlt(hdcOrigin, 0, 0, clientWidth, clientHeight, hdcBuffer, 0, 0, SRCCOPY);
			EndDraw();
			break;
		}
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

#define APPWND_CLASS_NAME "AutoUninstallClass"
#define APPWND_CAPTION_NAME "AutoUninstall"

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	
#ifndef NO_DEBUGLOG
	if(ExistFile(LOG_NAME))
		DeleteFileA(LOG_NAME);
	
	system(("whoami > \"" + string{LOG_NAME} + "\"").c_str());
	if (!ExistFile(LOG_NAME))
	{
		FILE*fp = fopen(LOG_NAME, "w");
		fclose(fp);
	}
#endif
	
	admin = IsRunAsAdmin();
	_hInstance = hInstance;
	_hPrevInstance = hPrevInstance;
	_lpCmdLine = lpCmdLine;
	_nShowCmd = nShowCmd;
	
	scr_w = GetScreenWidth();
	scr_h = GetScreenHeight();
	taskbar_h = GetTaskbarHeight();
	
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = APPWND_CLASS_NAME;
	
	if(!RegisterClass(&wc)) {
		MessageBoxA(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	// 创建透明窗口
	hwnd = CreateWindowExA(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST, 
		APPWND_CLASS_NAME,
		APPWND_CAPTION_NAME,
		WS_POPUP,
		0, 0, // 透明窗口位置
		scr_w, scr_h - taskbar_h, // 透明窗口大小
		NULL,
		NULL,
		NULL,
		NULL
		);
	ShowWindow(hwnd, SW_SHOWMAXIMIZED);
	
	// 设置透明度
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_COLORKEY | LWA_ALPHA);
	SetConsoleTitleA("AutoUninstall - by Wormwaker");
	
	MSG msg;
	while(1)
	{
		clock_t lastAim {clock()};
		while(GetMessage(&msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&msg); /* Translate key codes to chars if present */
			DispatchMessage(&msg); /* Send it to WndProc */
			
			if(clock() - lastAim >= 250)
			{
				POINT pt{0, 0};
				GetCursorPos(&pt);
				hwnd_target = WindowFromPoint(pt);
			}
			if(KEY_DOWN(VK_MENU) && KEY_DOWN(VK_OEM_3))
				goto _end;
			Hotkeys();
			
		}
	}
_end:
	SendMessage(hwnd, WM_DESTROY, 0, 0);
	Sleep(500);
	return 0;
}
