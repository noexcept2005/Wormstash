#define USINGABB
#include <stdcjz.h>
//「开始」菜单
//DV2ControlHost
bool _s;
DWORD Tip(LPVOID sth)
{
	_s = true;
	int ch = MessageBox(GetForegroundWindow(),"这台电脑不得关机。\n是否开启睡眠？","警告",MB_ICONEXCLAMATION|MB_YESNO);
					if(ch == IDYES)
					{
						Execute("D:\\BlackScreen\\BlackScreen.exe");
						Sleep(300);
					}
	_s=false;
	return 0;
}
int main()
{
	POINT pt;
	HWND hwnd;
	HideConsole();
	while(1)
	{
		Sleep(8);
		/*waitfor(KEY_DOWN(VK_LWIN) || KEY_DOWN(VK_RWIN))
		;*/
//		waitfor(HaveWindow("DV2ControlHost","「开始」菜单"))
//		;
		while(1)
		{
			hwnd = FindWindow("DV2ControlHost","「开始」菜单");
			if(hwnd && IsWindowVisible(hwnd))
			    break;
			Sleep(10);
		}
		while(1)
		{
			pt = GetCursorPos();
			if(pt.x >= 260 && pt.x <= 365 &&pt.y >= 992 && pt.y <= 1025)
			{
				SetCursorPos(0,GetScreenHeight());
				KeyPR(VK_LWIN);
//				if(GetHour() >= 21)
				{
					if(!_s)
					   CreateThread(Tip);
					Sleep(200);
				}
				Sleep(30);
			}
			Sleep(10);
			if(KEY_DOWN(VK_LWIN) || KEY_DOWN(VK_RWIN) || !IsWindowVisible(hwnd))
			    break;
		}
	}
}
