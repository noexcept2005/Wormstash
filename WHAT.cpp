//#define FREEZE_PROCESS

#include <Windows.h>
#include <graphics.h>
#include <tlhelp32.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#define CJZAPI __stdcall

#define MEME_DURATION 9000

using namespace std;

int scr_w = 0, scr_h = 0;
float g_scale = 1.0f;

#define NOSIGNAL_FREQ 1000
#define K(sth) ((GetAsyncKeyState(sth) & 0x8000))
// && GetForegroundWindow() == getHWnd()
inline int GetScreenHeight(void) //获取屏幕高度
{
	return GetSystemMetrics(SM_CYSCREEN);
}
inline int GetScreenWidth(void) //获取屏幕宽度
{
	return GetSystemMetrics(SM_CXSCREEN);
}
float GetScreenScaleFactor()
{
	// 获取窗口当前显示的监视器
	// 使用桌面的句柄.
	HWND hWnd = GetForegroundWindow();
	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	
	// 获取监视器逻辑宽度与高度
	MONITORINFOEX miex;
	miex.cbSize = sizeof(miex);
	GetMonitorInfo(hMonitor, &miex);
	int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
	//int cyLogical = (miex.rcMonitor.bottom - miex.rcMonitor.top);
	
	// 获取监视器物理宽度与高度
	DEVMODE dm;
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;
	EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
	int cxPhysical = dm.dmPelsWidth;
	//int cyPhysical = dm.dmPelsHeight;
	// 缩放比例计算  实际上使用任何一个即可
	double horzScale = ((double)cxPhysical / (double)cxLogical);
	//double vertScale = ((double)cyPhysical / (double)cyLogical);
	return horzScale;
}
void FocusWindow(HWND _hwnd)
{
	DWORD dwCurrentThread = GetCurrentThreadId();
	DWORD dwFGThread      = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
	
	AttachThreadInput(dwCurrentThread, dwFGThread, TRUE);
	
	// Possible actions you may wan to bring the window into focus.
	SetForegroundWindow(_hwnd);
	SetCapture(_hwnd);
	SetFocus(_hwnd);
	SetActiveWindow(_hwnd);
	EnableWindow(_hwnd, TRUE);
	
	AttachThreadInput(dwCurrentThread, dwFGThread, FALSE);
}	//from https://www.coder.work/article/555749

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
bool FreezeProcess(HANDLE hProc)	//冻结进程
{
	int pid = GetProcessId(hProc);
	THREADENTRY32 th32;
	th32.dwSize = sizeof(th32);
	
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
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
		}
		b = Thread32Next(hThreadSnap, &th32);
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
		}
		b = Thread32Next(hThreadSnap, &th32);
	}
	CloseHandle(hThreadSnap);
	return true;
}
bool FreezeWindow(HWND hWnd)
{
	return //EnableWindow(hWnd, FALSE);// &&
	InvalidateRect(hWnd, nullptr, FALSE);
}
bool UnfreezeWindow(HWND hWnd)
{
	return //EnableWindow(hWnd, TRUE);// && 
	ValidateRect(hWnd, nullptr);
}

HWND hwnd;

inline color_t EgeColor(COLORREF clr) {
	//RGB -> EGERGB
	return EGERGB(GetRValue(clr),GetGValue(clr),GetBValue(clr));
}
template <typename _T>
inline _T Clamp(_T val, _T min=0, _T max=2147483647) {	//限定范围
	if(val < min) val = min;
	else if(val > max) val = max;
	return val;
}
int CJZAPI RandomRange(int Min=0,int Max=RAND_MAX,bool rchMin=true,bool rchMax=true) {
	//随机数???区???
	int a;
	//a = random(Max);
	a = rand() * rand() % Max;
	
	if(rchMin && rchMax)	//[a,b]
		return (a%(Max - Min + 1)) + Min;
	else if(rchMin && !rchMax)		//[a,b)
		return (a%(Max - Min)) + Min;
	else if(!rchMin && rchMax)		//(a,b]
		return (a%(Max - Min)) + Min + 1;
	else							//(a,b)
		return (a%(Max - Min - 1)) + Min + 1;
}
template<typename _T>
inline _T CJZAPI Choice(initializer_list<_T> choices) {
	vector<_T> vec(choices);
	return vec[RandomRange(0, vec.size())];
}
template<typename _T>
inline _T CJZAPI Choice(const vector<_T>& choices_vector) {
	return choices_vector[RandomRange(0, choices_vector.size())];
}
template<typename _T>
inline _T& CJZAPI ChoiceRef(vector<_T>& choices_vector) {
	return choices_vector[RandomRange(0, choices_vector.size())];
}
void Start()
{
	cout << "Start\n";
	POINT pt;
	GetCursorPos(&pt);
#ifdef FREEZE_PROCESS
	HWND hwnd_there = WindowFromPoint(pt);
	DWORD self_pid = GetCurrentProcessId();
	DWORD pid = 0L;
	if (hwnd_there != nullptr && hwnd_there != INVALID_HANDLE_VALUE)
	{
		if (hwnd_there == hwnd)
			pid = self_pid;
		
		GetWindowThreadProcessId(hwnd_there, &pid);
		if (pid && pid != self_pid)
		{
			cout << "Process ID=" << pid << '\n';
			HANDLE hProc = GetProcessHandle(pid);
			if (!FreezeProcess(hProc))
			{
				cout << "Process Freezing Failed\n";
				CloseHandle(hProc);
				return;
			}
			cout << "Process Freezing Success\n";
			CloseHandle(hProc);
		}
		
	}
#endif
	cout << "Floating Self Window...\n";
	
	SetLayeredWindowAttributes(hwnd, RGB(4,4,4), 0, LWA_COLORKEY);
	FocusWindow(getHWnd());
	
	SetCursorPos(scr_w, scr_h*0.5);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_DRAWFRAME);
	
	static const vector<string> texts
	{
		"WHAT",
		"WHAT",
		"WHAT",
		"WHAT",
		"WHAT",
		"WHAT",
		"What",
		"WTF",
		"???",
		"HOW",
		"Bruh",
		"LOL",
		"LMAO",
	};
	
	int box_w = scr_w * 0.72;
	int box_left = scr_w / 2 - box_w / 2;
	int box_right = scr_w / 2 + box_w / 2;
	int box_top = scr_h * 0.15;
	int box_bottom = scr_h * 0.6;
	int box_h = (box_bottom - box_top);
	
	constexpr int what_fs = 120;
	static const char* what_fontname = "Tahoma";
	int text_top = scr_h * 0.67;
	
	cout << "Taking a screenshot...\n";
	// 创建内存位图
	HDC hdc = GetDC(0);
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, scr_w* g_scale, scr_h* g_scale);
	SelectObject(hMemDC, hBitmap);
	
	// 将窗口内容复制到内存位图
	BitBlt(hMemDC, 0, 0, scr_w* g_scale, scr_h* g_scale, hdc, 0, 0, SRCCOPY);
	
	// 获取像素数据
	BYTE* pixels = new BYTE[int(scr_w * scr_h * g_scale * g_scale * 4)]; // RGBA格式，每个像素占4字节
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = scr_w * g_scale;
	bmi.bmiHeader.biHeight = -scr_h * g_scale; // 负表示顶部到底部扫描
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	GetDIBits(hMemDC, hBitmap, 0, scr_h * g_scale, pixels, &bmi, DIB_RGB_COLORS);
	
	int src_x = Clamp<long>(pt.x - box_w / 2, 0, scr_w - box_w - 1);
	int src_y = Clamp<long>(pt.y - box_h / 2, 0, scr_h - box_h - 1);
//	
//	src_x *= g_scale;
//	src_y *= g_scale;
//	
	cout << "Copying Pixels...\n";
	
	PIMAGE pimg = newimage(scr_w* g_scale, scr_h* g_scale);
	color_t* pbuffer = getbuffer(pimg);
	for (int y = 0; y < scr_h* g_scale; ++y)
		for (int x = 0; x < scr_w* g_scale; ++x)
			pbuffer[int(y * scr_w* g_scale + x)] = 
			EGERGB(
				pixels[4 * int(y * scr_w* g_scale + x) + 2], 
				pixels[4 * int(y * scr_w* g_scale + x) + 1], 
				pixels[4 * int(y * scr_w* g_scale + x)]);
	
//	if (hwnd_there && INVALID_HANDLE_VALUE != hwnd_there)
//	{
//		if (hwnd_there != hwnd)
//		{
//			cout << "Freezing Window...\n";
//			if (!FreezeWindow(hwnd_there))
//				cout << "Window Freezing Failed\n";
//			else
//				cout << "Window Freezing Success\n";
//		}
//	}
	
	PlaySound("WHAT.wav", NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
	clock_t start = clock();
	
	string text = Choice(texts);
	
	bool clicked{false};
	for(;clock() - start < MEME_DURATION;)
	{
		if(clock() - start > 100 && !clicked)
		{
			clicked = true;
			SetCursorPos(scr_w, scr_h*0.5);
			
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			Sleep(50);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
		
		//PIMAGE imgScr = newimage(scr_w, scr_h);
		PIMAGE imgScr = nullptr;
		
		cleardevice();
		
		setcolor(BLACK, imgScr);
		setfillcolor(BLACK, imgScr);
		fillrect(0, 0, box_left, scr_h, imgScr);
		fillrect(box_left, 0, box_right, box_top, imgScr);
		fillrect(box_right, 0, scr_w, scr_h, imgScr);
		fillrect(box_left, box_bottom, box_right, scr_h, imgScr);
		
//		putimage(imgScr, box_left, box_top, pimg);
		putimage(imgScr, box_left, box_top, box_w, box_h, pimg, src_x, src_y, box_w* g_scale, box_h* g_scale);
		
//		setcolor(RED, imgScr);
//		rectangle(box_left, box_top, box_right, box_bottom, imgScr);
		
		setfont(what_fs, 0, what_fontname, imgScr);
		setcolor(EGERGB(255, 255, 255), imgScr);
		int text_w = textwidth(text.c_str(), imgScr);
		
		outtextxy(scr_w / 2 - text_w / 2, text_top, text.c_str(), imgScr);
	
		//putimage(nullptr, 0, 0, imgScr);
		//delimage(imgScr);
		
		delay_fps(60);
		
		if(K(VK_ESCAPE))
			break;
	}
	delete[] pixels;
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
	delimage(pimg);
#ifdef FREEZE_PROCESS
	if (pid && pid != self_pid)
	{
		HANDLE hProc = GetProcessHandle(pid);
		if (!UnfreezeProcess(hProc))
		{
			cout << "Process Unfreezing Failed\n";
		}else
			cout << "Process Unfreezing Success\n";
		
		CloseHandle(hProc);
	}
//	if (hwnd_there)
//	{
//		cout << "Unfreezing Window...\n";
//		if (!UnfreezeWindow(hwnd_there))
//			cout << "Window Unfreezing Failed\n";
//		else
//			cout << "Window Unfreezing Success\n";
//	}
#endif
	SetCursorPos(pt.x, pt.y);
	SetLayeredWindowAttributes(hwnd, RGB(4,4,4), 0, LWA_COLORKEY | LWA_ALPHA);
	cleardevice();
	delay_fps(60);
	cout << "End\n";
}
int main()
{
	SetProcessDPIAware();
	
	g_scale = GetScreenScaleFactor();
	scr_w = GetScreenWidth();
	scr_h = GetScreenHeight();
	
	initgraph(scr_w, scr_h);
	
	setbkmode(TRANSPARENT);
	setbkcolor(EGERGB(4,4,4));
	setrendermode(RENDER_MANUAL);
	hwnd = getHWnd();
	::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & (~(WS_CAPTION | WS_SYSMENU | WS_SIZEBOX)));
	::SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & (~(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME)) | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
	SetWindowPos(hwnd, HWND_TOPMOST, 0,0, getwidth(), getheight(), SWP_SHOWWINDOW
		| SWP_FRAMECHANGED | SWP_DRAWFRAME);
	SetLayeredWindowAttributes(hwnd, RGB(4,4,4), 0, LWA_COLORKEY | LWA_ALPHA);
	FocusWindow(hwnd);
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	
	while(1)
	{
		if(K(VK_RMENU) && K(VK_OEM_5))
		{
			Start();
			while(K(VK_RMENU) && K(VK_OEM_5));
		}
		
		if(K(VK_LMENU) && K(VK_RMENU) && K(VK_LCONTROL) && K(VK_RCONTROL))
			break;
	}
	
	closegraph();
	return 0;
}
