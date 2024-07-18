#include <Windows.h>
#include <stdio.h>

void PrintClipboard(void)
{
	if(!OpenClipboard(NULL))
	{
		printf("无法打开剪贴板！\n");
		return;
	}
	if(!IsClipboardFormatAvailable(CF_TEXT))
	{
		printf("剪贴板数据格式不是文本！\n");
		CloseClipboard();
		return;
	}
	HANDLE hMem = GetClipboardData(CF_TEXT);
	printf("%s", (char*)hMem);
	CloseClipboard();
	return;
}

int main()
{
	SetConsoleTitleA("剪贴板文本查看器");
	while(1)
	{
		system("cls");
		PrintClipboard();
		Sleep(200);
	}
	return 0;
}
//by Wormwaker
