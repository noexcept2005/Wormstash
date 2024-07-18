#include <windows.h>
#include <stdio.h>
int main()
{
	POINT posp={0,0};
	FILE *fp; 
	LPPOINT pos=&posp;
	HWND hwnd;
	fp=fopen("D:\\mm.vbs","w");
	fprintf(fp,"msgbox\"HELLO!\",vbInformation+vbSystemModal,\"MOUSE-MESSAGE\"");
	fclose(fp);
	//MessageBox(NULL,"HELLO!","MOUSE-MESSAGE",MB_ICONINFORMATION|MB_OK); //问题：无法同步进行 
	ShellExecute(NULL,"open","wscript.exe","D:\\mm.vbs","",SW_SHOW);
	hwnd=FindWindow("#32770",NULL);
	while(1)
	{
		//Sleep(10);
		GetCursorPos(pos);
		MoveWindow(hwnd,pos->x,pos->y,300,200,true);
	}
	return 0;
}
