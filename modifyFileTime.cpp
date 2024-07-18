#include <cstdio>
#include <windows.h>
#include <fstream>
#include <iostream>
using namespace std;

#define SET_DATE(v) \
		v.wDay = day; \
		v.wMonth = month;\
		v.wYear = year;

int main(int argc, char** argv) 
{
	while(1)
	{
		int year, month, day;
		char path[MAX_PATH] {0};
		printf("输入文件路径：");
		cin.getline(path, MAX_PATH);
		cout << "path=" << path << endl;
		cout << "\n输入年 月 日：";
		cin >> year >> month >> day;
		cin.get();
		
		SYSTEMTIME createTime; //创建时间
		GetSystemTime(&createTime);
		SET_DATE(createTime)
		createTime.wHour = 1;
		createTime.wMinute = 1;
		createTime.wSecond = 7;
		
		SYSTEMTIME lastWriteTime; //修改时间
		GetSystemTime(&lastWriteTime);
		SET_DATE(lastWriteTime)
		lastWriteTime.wHour = 9; 
		lastWriteTime.wMinute = 37; 
		lastWriteTime.wSecond = 23; 
		
		SYSTEMTIME lastAccessTime; //访问时间
		GetSystemTime(&lastAccessTime);
		SET_DATE(lastAccessTime)
		lastAccessTime.wHour = 15; 
		lastAccessTime.wMinute = 31;
		lastAccessTime.wSecond = 8; 
		
		FILETIME lastWriteFiletime;
		SystemTimeToFileTime(&lastWriteTime, &lastWriteFiletime);
		FILETIME createFiletime;
		SystemTimeToFileTime(&createTime, &createFiletime);
		FILETIME lastAccessFileTime;
		SystemTimeToFileTime(&lastAccessTime, &lastAccessFileTime);
		HANDLE filename = CreateFileA(path, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		// 设置文件的时间属性为之前设定的创建时间、最后访问时间和最后修改时间  
		if (!SetFileTime(filename, &createFiletime, &lastAccessFileTime, &lastWriteFiletime))
		{
			cout << "失败：" << GetLastError()<< endl;
		}else
			cout << "操作成功完成。\n";
		CloseHandle(filename);
	}
	return 0;	
}
//modified from https://blog.csdn.net/WangPaiFeiXingYuan/article/details/133755221
