/* antiAutoRun.cpp *
 * Author:noexcept *
 */
//C++
#include <Windows.h>
#include <io.h>
#include <iostream>
#include <string>	
using namespace std;
#define AUTORUN "autorun.inf"	//同名文件夹 
#define ANTIDELETE "\\con\\"	//这个是无法删除的文件夹的名称,还有其他选择 

bool Immune(string szDisk)	//如"C:\"
{	//定向免疫 
	cout<<"\n试图免疫...";
	if(access(szDisk.c_str(),S_OK))	//不存在此盘 
	{
		cout<<"\nERROR: 不存在磁盘"<<szDisk;
		return false;
	}
	
	string dir;
	dir+=szDisk;
	dir+=AUTORUN;
	BOOL bRet = CreateDirectory(dir.c_str(),nullptr);	//创建文件夹 
	if(!bRet)
	{	//failed
		cout<<"\nERROR: 无法免疫此盘，可能有写保护或已经免疫。";
		return false;
	}
	
	dir+=ANTIDELETE;
	bRet = CreateDirectory(dir.c_str(),nullptr);
	if(!bRet)
	{	//failed
		cout<<"\nERROR: 无法免疫此盘，可能有写保护或已经免疫。";
		return false;
	}
	return true;
}
int main()
{
	string szDisk;
	cout<<"输入想要免疫的磁盘盘符(like C:\\)： ";
	cin>>szDisk;
	if(Immune(szDisk))
		cout<<"\n操作成功完成.";
	else
		cout<<"\n免疫失败!";
	return 0;
}
