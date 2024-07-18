/***************************************
 *           Keystrokes.cpp            *
 *            游戏按键显示             *
 *         Author: Wormwaker           *
 *       StartDate: 2024/1/14          *
 ***************************************/
#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdarg>
using namespace std;
#define KEY_DOWN(vk) (GetAsyncKeyState(vk)&0x8000?1:0)

int scr_w = 0, scr_h = 0, taskbar_h = 0;
HDC hdcOrigin = NULL, hdcBuffer = NULL;
HWND hwnd = NULL;
HWND hwnd_console = NULL;

HINSTANCE _hInstance = NULL;
HINSTANCE _hPrevInstance = NULL;
LPSTR _lpCmdLine = NULL;
int _nShowCmd = SW_SHOWNORMAL;

#define CJZAPI __stdcall
template <typename _T>
string CJZAPI str(const _T& value)
{
	stringstream ss;
	ss << value;
	string res;
	ss >> res;
	return res;
}
string CJZAPI sprintf2(const char* szFormat, ...)
{
	va_list _list;
	va_start(_list, szFormat);
	char szBuffer[1024] = "\0";
	_vsnprintf(szBuffer, 1024, szFormat, _list);
	va_end(_list);
	return string{szBuffer};
}
bool CJZAPI ExistProcess(DWORD dwPid)	//判断是否存在指定进程
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);	
	if (INVALID_HANDLE_VALUE == hSnapshot) 	
	{		
		return false;	
	}	
	PROCESSENTRY32 pe = { sizeof(pe) };	
	BOOL fOk;	
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) 	
	{		
		if (pe.th32ProcessID == dwPid) 		
		{			
			CloseHandle(hSnapshot);			
			return true;		
		}	
	}	
	return false;
}
bool CJZAPI ExistProcess(LPCSTR lpName)	//判断是否存在指定进程
{	//******警告！区分大小写！！！！******// 
	//*****警告！必须加扩展名！！！！*****// 
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);	
	if (INVALID_HANDLE_VALUE == hSnapshot) 	
	{		
		return false;	
	}	
	PROCESSENTRY32 pe = { sizeof(pe) };	
	BOOL fOk;	
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) 	
	{		
		if (! stricmp(pe.szExeFile, lpName)) 		
		{			
			CloseHandle(hSnapshot);			
			return true;		
		}	
	}	
	return false;
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
inline HWND GetTaskbarWindow(void)
{
	return WindowFromPoint(POINT{ GetScreenWidth() / 2,GetScreenHeight() - 2 });
}
inline HFONT CJZAPI CreateFont(int height, int width, LPCSTR lpFamilyName)
{
	return CreateFont(height,width,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,lpFamilyName);
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
COLORREF RainbowColor() {
	// 假设时间按秒计算，这里使用系统时间或其他适当的时间源
	double timeInSeconds = GetTickCount() / 1000.0;
	
	// 色相按时间变化
	double hue = fmod(timeInSeconds * 30.0, 360.0);  // 假设每秒变化30度
	
	// 饱和度和亮度保持不变
	double saturation = 1.0;
	double value = 1.0;
	
	// 将HSV颜色转换为RGB并返回
	return HSVtoRGB(hue, saturation, value);
}

inline void CJZAPI SetTextColor(COLORREF color)
{
	SetTextColor(hdcBuffer, color);
}
inline void CJZAPI TextOut(int x, int y, LPCSTR lpText, HDC hdc = hdcBuffer)
{
	TextOut(hdc, x, y, lpText, strlen(lpText));
}
inline void CJZAPI TextOutShadow(int x, int y, LPCSTR lpText, HDC hdc = hdcBuffer)
{
	COLORREF oclr = GetTextColor(hdc);
	SetTextColor(RGB(0,0,0));
	TextOut(x+2,y+2,lpText,hdc);
	SetTextColor(oclr);
	TextOut(x,y,lpText,hdc);
}
void ClearDevice(HDC _hdc = hdcBuffer, HWND _hwnd = hwnd)
{
	// 清屏：使用透明色填充整个客户区域
	RECT rcClient;
	GetClientRect(_hwnd, &rcClient);
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(_hdc, &rcClient, hBrush);
	DeleteObject(hBrush);
}
int left_cps = 0;
int right_cps = 0;
#define CPS_UPDATE_CD 1000
void UpdateCPS(void)
{
	static DWORD startTime = GetTickCount();
	static int leftClicks = 0, rightClicks = 0;
	static bool leftButtonDown = false, rightButtonDown = false;
	
	bool leftButtonPressed = KEY_DOWN(VK_LBUTTON);
	if (leftButtonPressed && !leftButtonDown) 
		leftClicks++;
	leftButtonDown = leftButtonPressed;
	
	bool rightButtonPressed = KEY_DOWN(VK_RBUTTON);
	if (rightButtonPressed && !rightButtonDown)
		rightClicks++;
	rightButtonDown = rightButtonPressed;
	
	DWORD currentTime = GetTickCount();
	DWORD elapsedTime = currentTime - startTime;
	
	if (elapsedTime >= CPS_UPDATE_CD) 
	{
		left_cps = leftClicks * (1000.0 / CPS_UPDATE_CD);
		right_cps = rightClicks* (1000.0 / CPS_UPDATE_CD);
		
		startTime = currentTime;
		leftClicks = 0;
		rightClicks = 0;
	}
}
inline const int& GetCPS(bool left0_right1)
{
	return left0_right1 ? right_cps : left_cps;
}
void DrawKeyBox(BYTE key, const char* name, int fs, int left, int top, int right, int bottom)
{
	static const char* font = "Minecraft AE Pixel";
	
	COLORREF color;
	if(KEY_DOWN(key))
		color = RGB(200, 200, 200) | (200 << 24);
	else
		color = RGB(1, 1, 1) | (80 << 24);
	HBRUSH hBrush = CreateSolidBrush(color);
	auto prevObj = SelectObject(hdcBuffer, hBrush);
	Rectangle(hdcBuffer, left + (KEY_DOWN(key)?2:0), top + (KEY_DOWN(key)?2:0), right + (KEY_DOWN(key)?2:0), bottom + (KEY_DOWN(key)?2:0));
	SelectObject(hdcBuffer, prevObj);
	DeleteObject(hBrush);
	
	HFONT hFont = CreateFont(fs, 0, font);
	prevObj = SelectObject(hdcBuffer, hFont);
	SetTextColor(RainbowColor());
	SetBkMode(hdcBuffer, TRANSPARENT);
	string text{name};
	int x = left + (KEY_DOWN(key)?2:0) + (right - left) / 2.0f - text.length() * fs / 4.0f;
	int y = top + (KEY_DOWN(key)?2:0) + (bottom - top) / 2.0f - fs / 2.0f;
	TextOutShadow(x, y, text.c_str());
	DeleteObject(hFont);
	SelectObject(hdcBuffer, prevObj);
}
void DrawMouseKeys(int left, int top)
{
	static const int box_w = 85;
	static const int box_h = 45;
	static const int box_gap = 15;
	static const int fs = 15;
	static const int small_fs = 10;
	static const char* font = "Minecraft AE Pixel";
	
	COLORREF color;
	if(KEY_DOWN(VK_LBUTTON))
		color = RGB(200, 200, 200) | (200 << 24);
	else
		color = RGB(1, 1, 1) | (80 << 24);
	HBRUSH hBrush = CreateSolidBrush(color);
	auto prevObj = SelectObject(hdcBuffer, hBrush);
	Rectangle(hdcBuffer, left + (KEY_DOWN(VK_LBUTTON)?2:0), top+ (KEY_DOWN(VK_LBUTTON)?2:0), left + (KEY_DOWN(VK_LBUTTON)?2:0) + box_w, top + box_h+ (KEY_DOWN(VK_LBUTTON)?2:0));
	SelectObject(hdcBuffer, prevObj);
	DeleteObject(hBrush);
	
	if(KEY_DOWN(VK_RBUTTON))
		color = RGB(200, 200, 200) | (200 << 24);
	else
		color = RGB(1, 1, 1) | (80 << 24);
	hBrush = CreateSolidBrush(color);
	prevObj = SelectObject(hdcBuffer, hBrush);
	Rectangle(hdcBuffer, left + (KEY_DOWN(VK_RBUTTON)?2:0) + box_w + box_gap, top + (KEY_DOWN(VK_RBUTTON)?2:0), left + (KEY_DOWN(VK_RBUTTON)?2:0) + box_w*2 + box_gap, top + box_h+ (KEY_DOWN(VK_RBUTTON)?2:0));
	SelectObject(hdcBuffer, prevObj);
	DeleteObject(hBrush);
	
	HFONT hFont = CreateFont(small_fs, 0, font);
	prevObj = SelectObject(hdcBuffer, hFont);
	SetTextColor(RainbowColor());
	SetBkMode(hdcBuffer, TRANSPARENT);
	
	string text = sprintf2("%d CPS", GetCPS(0));
	int x = left + (KEY_DOWN(VK_LBUTTON)?2:0) + box_w / 2.0f - text.length() * small_fs / 4.0f - 3;
	int y = top + (KEY_DOWN(VK_LBUTTON)?2:0) + box_h * 0.75f - small_fs / 2.0f;
	TextOutShadow(x, y, text.c_str());
	
	text = sprintf2("%d CPS", GetCPS(1));
	x = box_gap + (KEY_DOWN(VK_RBUTTON)?2:0) + box_w + left + box_w / 2.0f - text.length() * small_fs / 4.0f - 3;
	y = top + (KEY_DOWN(VK_RBUTTON)?2:0) + box_h * 0.75f - small_fs / 2.0f;
	TextOutShadow(x, y, text.c_str());
	
	DeleteObject(hFont);
	SelectObject(hdcBuffer, prevObj);
	
	hFont = CreateFont(fs, 0, font);
	prevObj = SelectObject(hdcBuffer, hFont);
	SetTextColor(RainbowColor());
	SetBkMode(hdcBuffer, TRANSPARENT);
	
	text = "LMB";
	x = left + (KEY_DOWN(VK_LBUTTON)?2:0) + box_w / 2.0f - text.length() * fs / 4.0f - 4;
	y = top + (KEY_DOWN(VK_LBUTTON)?2:0) + box_h * 0.22f;
	TextOutShadow(x, y, text.c_str());
	
	text = "RMB";
	x = box_gap + (KEY_DOWN(VK_RBUTTON)?2:0) + box_w + left + box_w / 2.0f - text.length() * fs / 4.0f - 4;
	y = top + (KEY_DOWN(VK_RBUTTON)?2:0) + box_h * 0.22f;
	TextOutShadow(x, y, text.c_str());
	
	DeleteObject(hFont);
	SelectObject(hdcBuffer, prevObj);
}
void DrawKeyMouseInfo()
{
	static const int _left = 15;
	static const int _top = 40;
	static const int _pen_size = 2;
	
	static const int box_w = 54;
	static const int box_h = 54;
	static const int box_gap = 10;
	static const int fs = 24;
	
	COLORREF color = RainbowColor();
	HPEN hPen;
	hPen = CreatePen(PS_SOLID, _pen_size, color);
	SelectObject(hdcBuffer, hPen);
	
	DrawKeyBox('A', "A", fs, _left, _top + box_h + box_gap, _left + box_w, _top + box_h + box_gap + box_h);
	DrawKeyBox('W', "W", fs, _left + box_w + box_gap, _top, _left + box_w*2 + box_gap, _top + box_h);
	DrawKeyBox('S', "S", fs, _left + box_w + box_gap, _top + box_h + box_gap, _left + box_w*2 + box_gap, _top + box_h + box_gap + box_h);
	DrawKeyBox('D', "D", fs, _left + box_w * 2 + box_gap * 2, _top + box_h + box_gap, _left + box_w*3 + box_gap * 2, _top + box_h + box_gap + box_h);
	
	static const int space_h = 35;
	DrawKeyBox(' ', "--", fs, _left, _top + box_h*2 + box_gap*2, _left + box_w * 3 + box_gap * 2, _top + box_h*2+box_gap*2+space_h);
	DrawKeyBox(VK_SHIFT, "Shift", fs * 0.75, _left, _top + box_h*2 + box_gap*3 + space_h, _left + box_w * 3 + box_gap * 2, _top + box_h*2+box_gap*3+space_h*2);
	
	DrawMouseKeys(_left, _top + box_h*3 + box_gap*3 + space_h*2);
	
	DeleteObject(hPen);
}

void DrawUI()
{
	DrawKeyMouseInfo();
}

#define TIMER_PAINT_CD 10L
#define TIMER_UPDATE_CD 5L
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
VOID CALLBACK TimerProc_Update(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  UINT_PTR idEvent,
	_In_  DWORD dwTime
	)
{
	UpdateCPS();
}
PAINTSTRUCT ps;
void BeginDraw()
{
	hdcOrigin = BeginPaint(hwnd, &ps);
	SetBkMode(hdcBuffer, TRANSPARENT);
}
void EndDraw()
{
	EndPaint(hwnd, &ps);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) 
{
	static HBITMAP hBitmap = NULL;
	
	switch(Message) 
{
	case WM_CREATE:{
		hdcBuffer = CreateCompatibleDC(NULL);
		SetTimer(hwnd, 0, TIMER_PAINT_CD, TimerProc_Paint);
		SetTimer(hwnd, 2, TIMER_UPDATE_CD, TimerProc_Update);
		break;
	}
	case WM_DESTROY: {
		if (hdcBuffer)
			DeleteDC(hdcBuffer), hdcBuffer = nullptr;
		if (hBitmap)
			DeleteObject(hBitmap), hBitmap = nullptr;
		KillTimer(hwnd, 0);
		KillTimer(hwnd, 2);
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
bool TerminalCheck(DWORD dwPid, HWND _hwnd)
{	//检查是否为win11新终端
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);	
	if (INVALID_HANDLE_VALUE == hSnapshot) 	
	{		
		return false;	
	}	
	PROCESSENTRY32 pe = { sizeof(pe) };	
	BOOL fOk;	
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) 	
	{		
		if (! stricmp(pe.szExeFile, "WindowsTerminal.exe")
			&& pe.th32ProcessID == dwPid) 		
		{			
			CloseHandle(hSnapshot);			
{
	char title[MAX_PATH];
	GetWindowText(_hwnd, title, MAX_PATH);
	if(strcmp(title, _pgmptr) && strcmp(title, "Keystrokes"))
		return false;
	return true;
}		
		}	
	}	
	return false;
}
BOOL CALLBACK EnumWindowsProc(HWND _hwnd, LPARAM lParam)
{
	DWORD pid;
	GetWindowThreadProcessId(_hwnd, &pid);
	if(TerminalCheck(pid, _hwnd))
	{
		hwnd_console = _hwnd;
		return FALSE;
	}
	return TRUE;
}
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
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
	wc.lpszClassName = "KeyStrokesWindowClass";
	
	if(!RegisterClass(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST, 
		"KeyStrokesWindowClass", "KeyStrokes by Wormwaker", WS_POPUP,
		0, /* x */
		0, /* y */
		scr_w, /* width */
		scr_h, /* height */
		NULL,
		//		NULL,
		NULL,hInstance,NULL);
	
	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	SetConsoleTitle("Keystrokes");
	if(ExistProcess("WindowsTerminal.exe"))
	{	//win11电脑且使用新版终端
		EnumWindows(EnumWindowsProc, 0);
	}else{	//旧版控制台主机
		hwnd_console = GetConsoleWindow();
	}
#ifndef SHOWCONSOLE
	if(hwnd_console != INVALID_HANDLE_VALUE && hwnd_console != nullptr)
		ShowWindow(hwnd_console, SW_HIDE);
#endif
	// 设置窗口透明度
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	ShowWindow(hwnd, SW_SHOWMAXIMIZED);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, scr_w,scr_h, SWP_NOSIZE);
	MSG msg;
	
	while(GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
		ShowWindowAsync(hwnd, SW_SHOWMAXIMIZED); //保持显示并最大化
	}
	
	return 0;
}

