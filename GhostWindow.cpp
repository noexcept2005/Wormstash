#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <conio.h>
using namespace std;

HWND hwnd{nullptr};
int alpha{80};

map<HWND, LONG> wnd_longs;
vector<pair<UINT, string>> outputs;

void Output(UINT uForeColor, const char* s)
{
	if (outputs.size() > 8)
		outputs.erase(outputs.begin());
	outputs.emplace_back(uForeColor, string{s});
}
void SetColor(UINT uFore, UINT uBack) 
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, uFore + uBack * 0x10);
}
void SetPos(short x, short y) 
{
	COORD pos = {x, y};
	HANDLE out=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(out, pos);
}

void LimitAlpha()
{
	if (alpha < 10)
		alpha = 10;
	else if(alpha > 255)
		alpha = 255;
}
bool IsWindowHandleValid(HWND _hwnd)
{
	if (!_hwnd || _hwnd == INVALID_HANDLE_VALUE)
		return false;
	SetLastError(0L);
	GetWindowLong(_hwnd, GWL_STYLE);
	return GetLastError() == 0L;
}
void DisplayWindowInfo()
{
	if (!IsWindowHandleValid(hwnd))
	{
		return;
	}
	char title[200]{0};
	GetWindowTextA(hwnd, title, 200);
	Output(14, "**********************");
	Output(14, ("窗口句柄：" + to_string(intptr_t(hwnd))).c_str());
	Output(14, ("窗口标题："s + title).c_str());
	Output(14, "**********************");
}

void DrawUI()
{
	system("cls");
	SetPos(0, 0);
	SetColor(2, 0);
	cout << "---------------------------------\n";
	cout << "\n";
	SetColor(14, 0);
	cout << "请按下：\n\n";
	SetColor(15, 0);
	cout << "  [Space] 捕获前端窗口\n\n";
	cout << "  [Enter] 捕获鼠标位置窗口(仅未应用窗口)\n\n";
	SetColor(IsWindowHandleValid(hwnd)?7:12, 0);
	cout << "  [1] 应用状态\n\n";
	cout << "  [2] 恢复原状态\n\n";
	SetColor(wnd_longs.empty()?12:7, 0);
	cout << "  [3] 恢复所有登记过的窗口\n\n";
	SetColor(3, 0);
	cout << "  [S] 设置不透明度\n\n";
	SetColor(8, 0);
	cout << "  [Esc] 退出\n";
	SetColor(1, 0);
	cout << "            保存的窗口信息数：" << wnd_longs.size() << '\n';
	SetColor(2, 0);
	cout << "---------------------------------\n";
	
	constexpr int rui_left = 0;
	int y = 20;
	for (auto& o : outputs)
	{
		SetPos(rui_left, y);
		SetColor(o.first, 0);
		cout << o.second;
		++y;
	}
}
void UpdateWindowLongs()
{
	if (wnd_longs.empty())
		return;
	
	bool removed{false};
	do{
		removed = false;
		for (auto iter = wnd_longs.begin(); iter != wnd_longs.end(); ++iter)
			if (!IsWindowHandleValid(iter->first) 
				//|| GetWindowLongA(iter->first, GWL_EXSTYLE) == iter->second
				)
			{
				removed = true;
				wnd_longs.erase(iter);
				break;
			}
	}while(removed && !wnd_longs.empty());
}
void Read()
{
	FILE *fp = fopen("GhostWindow.dat", "r");
	if (!fp || feof(fp))
		return;
	printf("reading...\n");
	fscanf(fp, "%d", &alpha);
	LimitAlpha();
	
	int i{0};
	intptr_t val1;
	LONG val2;
	
	wnd_longs.clear();
	
	while (!feof(fp))
	{
		if (i == 0)
		{
			if (!fscanf(fp, "%Ld", &val1))
				break;
		}
		else
		{
			if (!fscanf(fp, "%ld", &val2))
				break;
			wnd_longs.insert(make_pair((HWND)val1, val2));
		}
		i = (i + 1) % 2;
	}
	fclose(fp);
	printf("reading completed\n");
	UpdateWindowLongs();
}
bool Save()
{
	FILE *fp = fopen("GhostWindow.dat", "w");
	if (!fp)
	{
		Output(12, "无法打开文件保存不透明度\n");
		return false;
	}
	LimitAlpha();
	fprintf(fp, "%d\n", alpha);
	for (auto& pr : wnd_longs)
	{
		if (IsWindowHandleValid(pr.first))
			fprintf(fp, "%Ld %ld\n", pr.first, pr.second);
	}
	fclose(fp);
	return true;
}
void CaptureHere()
{
	Output(11, "请将鼠标放在目标窗口上，五秒后将进行窗口捕获\n");
	DrawUI();
	Sleep(5000);
	
	POINT pt;
	GetCursorPos(&pt);
	hwnd = WindowFromPoint(pt);
	
	if (!IsWindowHandleValid(hwnd))
	{
		MessageBeep(MB_ICONERROR);
		Output(12, "窗口捕获失败!\n");
	}else{
		MessageBeep(MB_ICONEXCLAMATION);
		Output(10, "捕获成功！\n");
		DisplayWindowInfo();
	}
}
void CaptureFore()
{
	Output(14, "请切换到目标窗口，五秒后进行捕获\n");
	DrawUI();
	Sleep(5000);
	if (!IsWindowHandleValid(hwnd = GetForegroundWindow()))
	{
		MessageBeep(MB_ICONERROR);
		Output(12, "窗口捕获失败！\n");
	}else{
		MessageBeep(MB_ICONEXCLAMATION);
		Output(10, "窗口捕获成功！\n");
		DisplayWindowInfo();
	}
}
void GhostifyWindow(HWND _hwnd = hwnd)
{
	if (!IsWindowHandleValid(_hwnd))
	{
		Output(12, "你还没有成功捕获窗口，请先捕获窗口！\n");
		return;
	}
	Read();
	Output(7, ("当前不透明度：" + to_string(alpha) + '\n').c_str());
	LONG t = GetWindowLong(_hwnd, GWL_EXSTYLE);
	LONG t0 = t;
	
	t |= WS_EX_LAYERED;
	t |= WS_EX_TRANSPARENT;
	
	SetWindowLong(_hwnd, GWL_EXSTYLE, t);
	
	MessageBeep(MB_ICONINFORMATION);
	SetLayeredWindowAttributes(_hwnd, 0, alpha, LWA_ALPHA);
	SetWindowPos(_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);
	SetForegroundWindow(_hwnd);
	
	wnd_longs[_hwnd] = t0;
	Save();
	Output(10, ("已对目标窗口 " + to_string(intptr_t(_hwnd)) + " 应用操作\n").c_str());
}
void DeghostifyWindow(HWND _hwnd = hwnd)
{
	Read();
	
	if (wnd_longs.find(_hwnd) == wnd_longs.end())
	{
		Output(14, "错误：该窗口没有被本程序登记过，无法恢复原状态，请考虑重新启动目标窗口\n");
		return;
	}
	
	MessageBeep(MB_ICONINFORMATION);
	LONG t0 = wnd_longs[_hwnd];
	SetLayeredWindowAttributes(_hwnd, 0, 255, LWA_ALPHA);
	SetWindowPos(hwnd, (HWND_NOTOPMOST), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);
	SetWindowLong(_hwnd, GWL_EXSTYLE, t0);
	UpdateWindow(_hwnd);
	
	Output(10, ("已对目标窗口 " + to_string(intptr_t(_hwnd)) + " 应用恢复操作\n").c_str());
}
void DeghostifyRegisteredWindows()
{
	if (wnd_longs.empty())
	{
		Output(13, "没有窗口需要恢复");
		return;
	}
	for (auto& pr : wnd_longs)
		DeghostifyWindow(pr.first);
	Output(15, ("成功对 " + to_string(wnd_longs.size()) + " 个窗口应用恢复操作").c_str());
}
void SetOpacity()
{
	system("cls");
	SetColor(15, 0);
	cout << "输入不透明度(10~255)：";
	Output(15, "输入不透明度(10~255)：");
	cin >> alpha;
	if (Save())
	{
		MessageBeep(MB_ICONEXCLAMATION);
		SetColor(10, 0);
		cout << "设置成功，将在下一次应用时生效\n";
		Output(10, "设置成功，将在下一次应用时生效");
	}
	Sleep(1500);
}

int main()
{
	SetConsoleTitleA("GhostWindow - by Wormwaker");
	while(1)
	{
		Read();
		DrawUI();
		
		int ch = getch();
		
		switch(ch)
		{
		case ' ':{
			CaptureFore();
			break;
		}
		case 13:{
			CaptureHere();
			break;
		}
		case '1':{
			GhostifyWindow();
			break;
		}
		case '2':{
			DeghostifyWindow();
			break;
		}
		case '3':{
			DeghostifyRegisteredWindows();
			break;
		}
		case 's': case 'S':{
			SetOpacity();
			break;
		}
		case 27:{
			exit(0);
			break;
		}
		}
	}
	return 0;
}
