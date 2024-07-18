#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
using namespace std;
int mday[12]={31,28,31,30,31,30,31,31,30,31,30,31};
void colors(UINT fore,UINT back)
{
	HANDLE handle=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle,fore+back*0x10);
 }   //变色
 
int dayofweek(int y,int m,int d)
{
	if(m==1||m==2)
	{
		y--;
		m+=12;
	}
	return (y+y/4-y/100+y/400+(13*m+8)/5+d)%7;
}
int is_leap(int y)
{
	return y % 4==0 && y %100 != 0 || y % 400 == 0;
}
int monthdays(int y,int m)
{
	if(m--!=2)
	return mday[m];
	return mday[m]+is_leap(y);
}
void put_ca(int y,int m,int d)
{
	int i;
	int wd=dayofweek(y,m,1);
	int mdays=monthdays(y,m);
	colors(10,0);
	cout<<"       "<<y<<"."<<m<<"   \n\n";
	cout<<" 日 一 二 三 四 五 六 \n";
	cout<<"======================\n";
	printf("%*s",3*wd," ");
	for(i=1;i <= mdays;i++)
	{
		colors(10,0);
		if(i==d)
		colors(12,0);
		printf("%3d",i);
		colors(10,0);
		if(++wd%7==0)
		putchar('\n');
	}
	if(wd%7!=0)
	putchar('\n');
 } 
 int main()
 {
 	int y,m,d;
 	char *dmsg;
 	time_t current;
	struct tm *timer;
	time(&current);
	timer=gmtime(&current);
	y=timer->tm_year+1900;
	m=timer->tm_mon+1; 
	d=timer->tm_mday;
	system("mode con cols=31 lines=20");	//窗口宽度高度
	 system("color 0E");
	system("title 日历");
	cout<<"当  前 日 期：\t";
	system("date /t");
	cout<<"当  前 时 间：\t";
	system("time /t");
	cout<<"\n";
 	put_ca(y,m,d);
 	cout<<"\n";
 	system("pause"); 
 	return 0;
 }
