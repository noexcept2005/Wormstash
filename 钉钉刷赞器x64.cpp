#define USINGABB
#include <stdcjz.h>
Main{
    int i=0;
	int n=100;	//次数 
	int speed=64;	//每秒赞数 
	int ch;
	POINT pt,pt2;	//鼠标坐标 
	LONG maxDsc=30;	//鼠标偏移极限距离 
	SetTitle("钉钉刷赞器");
	SetConsoleSize(35,20); 
	/*while(!HaveWindow("StandardFrame_DingTalk",NULL) && ch == IDOK){
		ch = MessageBox(GetConsoleWindow(),"没有打开钉钉。\n请先打开然后按确定。","提示",MB_ICONEXCLAMATION|MB_OKCANCEL);
	}
	if(ch == IDCANCEL || ch == IDCLOSE)
	{
		SetColor(11,0);
		cout<<"已取消...";
		Sleep(500);
		exit(0);
	}*/
	while(!HaveWindow("StandardFrame",NULL) && ch == IDOK){
		ch = MessageBox(GetConsoleWindow(),"没有打开钉钉直播窗口。\n请先打开然后按确定。","提示",MB_ICONEXCLAMATION|MB_OKCANCEL);
	}
	if(ch == IDCANCEL || ch == IDCLOSE)
	{
		SetColor(11,0);
		cout<<"已取消...";
		Sleep(500);
		exit(0);
	}
	inp1:
	SetColor(14,0);
	cout<<"输入每秒赞数(推荐：20~60)：";
	cin>>speed;
	if(speed < 1 || speed > 1000){
		SetColor(11,0);
		cout<<"\n不符合范围(1~1000)!\n";
		goto inp1; 
	} 
	cout<<"输入次数：";
	cin>>n;
	SetColor(10,0);
	cout<<"\n\n准备就绪。\n马上隐藏此窗口，几秒后\n请在赞上单击鼠标开始刷赞...\n";
	Sleep(3000);
	HideConsole();
	if(HaveWindow("Msgbox",NULL))
	    KeyPR(VK_SPACE);
	Sleep(1000);
	waitfor(LEFT_CLICK);
	GetCursorPos(&pt);
	//MsgSndTip(); 
	 do{
	 	if(!HaveWindow("Msgbox",NULL))
	 	   Click();
        else
	 	   KeyPR(VK_SPACE);
	 	Sleep((DWORD)1000.0 / speed);
	 	i++;
	 	/*if(kbhit())
	 			   break;*/
        if((i * 5) % speed == 0)	//每秒检测鼠标五次 
        {
        	GetCursorPos(&pt2);
        	if(Distance(pt,pt2) > maxDsc)	//临时取消
			{
				//就不跳出控制台了，直接退出，这样安全。:)
				exit(-1); 
			 } 
		}
	 }while(i <= n);
	 Sleep(1000);
	 if(HaveWindow("Msgbox",NULL))
	    KeyPR(VK_SPACE);
     FlauntWindow();
     SetColor(11,0);
     cout<<"\n刷赞成功！\n\n";
     Sleep(600);
     SetColor(5,0);
	 return 0;
} 
