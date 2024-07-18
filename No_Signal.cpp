#include <Windows.h>
#include <graphics.h>
#include <ctime>
#include <string>

using namespace std;

int scr_w = 0, scr_h = 0;
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

PIMAGE imgNoSignal = nullptr;
HWND hwnd;

void Load()
{
	imgNoSignal = newimage();
	getimage(imgNoSignal, "NoSignal.png");
}
void Dispose()
{
	delimage(imgNoSignal);
}
inline color_t EgeColor(COLORREF clr) {
	//RGB -> EGERGB
	return EGERGB(GetRValue(clr),GetGValue(clr),GetBValue(clr));
}
DWORD ThrBeep(LPVOID none)
{
	Beep(NOSIGNAL_FREQ, 2000);
	return 1;
}
void Start()
{
	clock_t start = clock();
	SetLayeredWindowAttributes(hwnd, RGB(4,4,4), 0, LWA_COLORKEY);
	CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ThrBeep, nullptr, 0, nullptr);
	FocusWindow(getHWnd());
	POINT pt;
	GetCursorPos(&pt);
	SetCursorPos(scr_w, scr_h*0.5);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_DRAWFRAME);
	
	bool clicked{false};
	for(;clock() - start < 2000;)
	{
		if(clock() - start > 100 && !clicked)
		{
			clicked = true;
			SetCursorPos(scr_w, scr_h*0.5);
			
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			Sleep(50);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
		
		cleardevice();
		
		PIMAGE imgScr = newimage(scr_w, scr_h);
		putimage(imgScr, 0, 0, scr_w, scr_h, imgNoSignal, 0, 0, getwidth(imgNoSignal), getheight(imgNoSignal));
		putimage(nullptr, 0, 0, imgScr);
		delimage(imgScr);
		
		delay_fps(60);
		
		if(K(VK_ESCAPE))
			break;
	}
	SetCursorPos(pt.x, pt.y);
	SetLayeredWindowAttributes(hwnd, RGB(4,4,4), 0, LWA_COLORKEY | LWA_ALPHA);
	cleardevice();
	delay_fps(60);
	
}
int main()
{
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
	
	Load();
	
	while(1)
	{
		if(K(VK_RCONTROL) && K(VK_OEM_5))
		{
			Start();
			while(K(VK_RCONTROL) && K(VK_OEM_5));
		}
		
		if(K(VK_LMENU) && K(VK_RMENU) && K(VK_LCONTROL) && K(VK_RCONTROL))
			break;
	}
	Dispose();
	
	closegraph();
	return 0;
}
