/************************************
 *             反监视程序           *
 *  可以免疫大多数截图，监视或录屏  *
 *            by Wormwaker			*
 ************************************/
#include <iostream>
#include <windows.h>

inline int GetScreenHeight(void) //获取屏幕高度 
{
	return GetSystemMetrics(SM_CYSCREEN);
}
inline int GetScreenWidth(void) //获取屏幕宽度 
{
	return GetSystemMetrics(SM_CXSCREEN);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		// 创建窗口的设备上下文
		HDC hdc = GetDC(hWnd);
		
		// 创建透明位图
		static int width = GetScreenWidth();
		static int height = GetScreenHeight();
		HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
		
		// 创建与位图相关联的设备上下文
		HDC hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBitmap);
		
		// 绘制透明背景
		BLENDFUNCTION blend = { 0 };
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.SourceConstantAlpha = 1;
									//127 + 110 * sin((clock() - start) / 10.0); // 透明度（0-255，255为完全不透明）
		
		RECT rcClient;
		GetClientRect(hWnd, &rcClient);
		FillRect(hdcMem, &rcClient, (HBRUSH)(COLOR_WINDOW + 1));
		
		// 使用UpdateLayeredWindow函数设置窗口为透明窗口
		SIZE sizeWnd = { width, height };
		POINT ptSrc = { 0, 0 };
		UpdateLayeredWindow(hWnd, hdc, NULL, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
		std::cout<<"\n窗口已经启动！";
		
		// 释放资源
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
		ReleaseDC(hWnd, hdc);
	}
		break;
	case WM_DESTROY:
		std::cout<<"\n窗口关闭！";
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
inline HWND GetTaskbarWindow(void)
{
	return WindowFromPoint(POINT{GetScreenWidth()/2,GetScreenHeight()-2});
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	std::cout<<"\n反监视保护正在开启！";
	const char* CLASS_NAME = "AntiMonitorWindowClass";
	ShowWindow(GetConsoleWindow(), SW_SHOWMINIMIZED);
	//SetParent(GetConsoleWindow(), GetTaskbarWindow());
	
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);
	
	HWND hWnd = CreateWindowEx(WS_EX_LAYERED, CLASS_NAME, "反监视保护 by Wormwaker", WS_OVERLAPPEDWINDOW,
		0,0,GetScreenWidth(),GetScreenHeight(), GetTaskbarWindow(), NULL, hInstance, NULL);
	
	ShowWindow(hWnd, nCmdShow);
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, GetScreenWidth(), GetScreenHeight(), SWP_NOSIZE);
	SetWindowDisplayAffinity(hWnd, 1);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		ShowWindowAsync(hWnd, SW_SHOWMAXIMIZED);
	}
	
	return 0;
}


