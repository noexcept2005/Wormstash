#define USINGSTD
#include <stdcjz.h>
void Put(LPCSTR lpText)
{
	int mark=1,shift,inc2;
	FILE *fp;
	char inc[10];
	for(int j=0;j<strlen(lpText);j++)
    {
    Sleep(1);
    inc[0]=lpText[j];
    mark=1;
    if(inc[0]>96&&inc[0]<123)
	inc[0]=inc[0]-32; 
	else if(inc[0]>64&&inc[0]<91)
	{
	mark=4;
	shift=VK_SHIFT;
	}
	else if(inc[0]=='#')
	{
	mark=2;
	shift=VK_SHIFT;  
	inc2=VK_NUMPAD3; 
    }else if(inc[0]==' ')
    {
    mark=3;
    inc2=VK_SPACE;
	}else if(inc[0]=='\n')
    {
    mark=3;
    inc2=VK_RETURN;
	}else if(inc[0]=='.')
	{
		mark=3;
		inc2=VK_DECIMAL;
	}/*else if(inc[0]='<')
	{
		fp=fopen("D:\\cstemp.tmp","w");
		fprintf("")
	}*/
    if(mark==1)
    {
    keybd_event(inc[0], 0, 0, 0); //按下  
    keybd_event(inc[0], 0, 2, 0); //松开  
    }else if(mark==2)
    {
    keybd_event(shift, 0, 0, 0); //按下  
    keybd_event(inc2, 0, 0, 0); 
	keybd_event(inc2, 0, 2, 0);  
	keybd_event(shift, 0, 2, 0); 	
	}else if(mark==3)
	{
	keybd_event(inc2, 0, 0, 0); //按下  
    keybd_event(inc2, 0, 2, 0); //松开  
	}else if(mark==4)
	{
	keybd_event(shift, 0, 0, 0); //按下  
    keybd_event(inc[0], 0, 0, 0); 
	keybd_event(inc[0], 0, 2, 0);  
	keybd_event(shift, 0, 2, 0); 
	}
	}
}
void Put2(LPCSTR lpText)
{
	FILE *fp;
	fp=fopen("D:\\cstemp.tmp","w");
	fprintf(fp,lpText);
	fclose(fp);
	Execute("Notepad.exe","D:\\cstemp.tmp");
	Sleep(50);
	Keys(KS_CTRL_A);
	Keys(KS_CTRL_C);
	Sleep(100);
	Taskkill("/im notepad.exe /f");
	Click();
	Keys(KS_CTRL_V);
	Sleep(100);
	system("del D:\\cstemp.tmp");
	return;
}
int main()
{
	LPSTR dtpath,dtdpath;
	bool iscon=false,isu=true,ism=false;
	CHAR hr[33],mac[65],maci[513];
	LPSTR macro;
	HWND hwndd=FindWindow("TMainForm",NULL);
	HWND hwnd=GetConsoleWindow();
	system("mode con cols=40 lines=10");//窗口宽度高度
	SetColor(14,0);
	SetTitle("DevCpp代码生成程序- by cjz");
	DrawRow1D(0,0,39,'*',10);
	MessageBeep(MB_ICONEXCLAMATION);
	if(hwndd==NULL)
	{
	SetColor(10,0);
	cout<<"正在打开DevCpp编译器...";
	Sleep(100);
	/*GetDesktopPath(dtpath);
	strcpy(dtdpath,dtpath);
	strcat(dtdpath,"\\Dev-C++.lnk");
	//cout<<dtdpath;Pause();
	//Execute(dtdpath,"");*/
	Execute("D:\\Program Files\\Dev-C++\\Dev-Cpp\\devcpp.exe","");
	while(( hwndd = FindWindow("TMainForm",NULL) )==NULL)
		; 
	cout<<"\r打开成功！                           \n";
	}
	Sleep(200);
	SetColor(11,0);
	cout<<"新建源文件...";
	Sleep(200);
	SetCursorPos(717,646); 
	Click();
	Keys(KS_CTRL_N);
	HideWindow(hwnd);
    ShowWindow(hwnd,SW_SHOW);
    Sleep(200);
    FlashWindow(hwnd,true);
    Sleep(150);
    FlashWindow(hwnd,false);
	cout<<"\r新建成功！         \n";
	MessageBeep(MB_ICONEXCLAMATION);
	Sleep(100);
	Cls();
	SetColor(14,0);
	SetTitle("代码生成");
	DrawRow1D(0,0,39,'*',3);
	UINT headercnt=0;
	do{
	SetColor(11,0);
	cout<<"输入头文件:\t";
	KeyPR(VK_SHIFT);
	cin>>hr;
	SetCursorPos(1413,616); 
	Click();
	/*if(headercnt==0)
	Put2("#include <");
	else
	Put2("\n#include <");*//*
	Sleep(100);
	Put2((LPCSTR)hr);
	Sleep(100);
	Put2(">"); */
	FILE *fp;
	fp=fopen("D:\\cstemp1.tmp","w");
	if(headercnt==0)
	fprintf(fp,"#include <%s>",hr);
	else
	fprintf(fp,"\n#include <%s>",hr);
	fclose(fp);
	Execute("Notepad.exe","D:\\cstemp1.tmp");
	Sleep(50);
	Keys(KS_CTRL_A);
	Keys(KS_CTRL_C);
	Sleep(100);
	Taskkill("/im notepad.exe /f");
	Click();
	Keys(KS_CTRL_V);
	Sleep(100);
	system("del D:\\cstemp1.tmp");
	headercnt++;
	HideWindow(hwnd);
    ShowWindow(hwnd,SW_SHOW);
    SetColor(10,0);
    cout<<"头文件输入完毕！是否继续输入头文件？[Space/Enter:继续,Esc:不]\t";
    MessageBeep(MB_ICONINFORMATION);
    Sleep(200);
    FlashWindow(hwnd,true);
    Sleep(100);
    FlashWindow(hwnd,false);
    inh://input the header choice
    int chh=getch();
    if(chh==32||chh==13)iscon=true;
    else if(chh==27)iscon=false;
    else goto inh;
    cout<<"\n";
	}while(iscon);
	SetColor(14,0);
	Cls();
	DrawRow1D(0,0,39,'*',2);
	SetColor(11,0);
	cout<<"是否输出\"using namespace std;\"?[Space/Enter:是,Esc:否]\t";
	inu:
		int chu=getch();
		if(chu==32||chu==13)isu=true;
    else if(chu==27)isu=false;
    else goto inu;
    cout<<"\n";
    if(isu)
    {	SetCursorPos(1413,616); 
		Click();
    	Put2("\nusing namespace std;\n");
    	cout<<"\r输出成功!                                              \n";
    	MessageBeep(MB_ICONEXCLAMATION);
	}
	SetColor(14,0);
	cout<<"是否编译宏？[Space/Enter:是,Esc:否]\t";
	inm:
		int chm=getch();
		if(chm==32||chm==13)ism=true;
    else if(chm==27)ism=false;
    else goto inm;
    cout<<"\n";
    if(ism)
    {
    	do	{
    		iscon=false;
    		SetColor(11,0);
    		cout<<"输入宏名称：\t";
    		SetColor(10,0);
    		cin>>mac;
    		SetColor(11,0);
    		cout<<"输入代替对象：\t";
    		SetColor(10,0);
    		cin>>maci;
    		Sleep(100);
    		SetCursorPos(1413,616); 
			Click();
			//sprintf(macro,"#define %s %s\n",mac,maci);
			//Put2((LPCSTR)macro);
			FILE *fp1=fopen("D:\\cstemp2.tmp","w");
			fprintf(fp1,"#define %s %s\n",mac,maci);
			fclose(fp1);
			Execute("Notepad.exe","D:\\cstemp2.tmp");
			Sleep(50);
			Keys(KS_CTRL_A);
			Keys(KS_CTRL_C);
			Sleep(100);
			Taskkill("/im notepad.exe /f");
			Click();
			Keys(KS_CTRL_V);
			Sleep(100);
			system("del D:\\cstemp2.tmp");
			HideWindow(hwnd);
    		ShowWindow(hwnd,SW_SHOW);
    		Sleep(200);
    		FlashWindow(hwnd,true);
    		Sleep(100);
    		FlashWindow(hwnd,false);
    		SetColor(11,0);
    		MessageBeep(MB_ICONINFORMATION);
    		cout<<"\n输出完成！！\n是否继续编译宏？[Space/Enter:是,Esc:否]\t";
    		inmc://input the macro choice
    		int chm=getch();
    		if(chm==32||chm==13)iscon=true;
    		else if(chm==27)iscon=false;
    		else goto inmc;
    		cout<<"\n";
		}while(iscon);
	}
	SetColor(14,0);
	Pause();
	return 1;
}

