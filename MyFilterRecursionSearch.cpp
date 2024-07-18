#include <iostream>
#include <sys\\stat.h>
#include <Windows.h>
#include <io.h>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
using namespace std; 
#define CJZAPI __cdecl
inline BOOL CJZAPI ExistFile(string strFile) {
	//文件或文件夹都可以
	return !_access(strFile.c_str(), S_OK);//S_OK表示只检查是否存在
}
bool CJZAPI strequ(char *str, const char *obj)
{	//比较是否一样 
	return (strcmp((const char *)str, obj) == 0 ? true : false);
}
BOOL CJZAPI IsDir(const string& lpPath)
{	//是否是文件夹 
	struct _stat buf;
	int res;
	res = _stat(lpPath.c_str(), &buf);
	return (buf.st_mode & _S_IFDIR);
}
BOOL CJZAPI IsFile(const string& lpPath)
{	//是否是文件 
	struct _stat buf;
	int res;
	res = _stat(lpPath.c_str(), &buf);
	return (buf.st_mode & _S_IFREG);
}
vector<string> CJZAPI GetDirFiles(const string& dir, const string& filter = "*.*")
{
	if (dir.empty() || (filter != "" && !ExistFile(dir)))
	{
		return vector<string>();
	}
	_finddata_t fileDir;
	long lfDir;
	vector<string> files{};
	string dirp = dir + filter;	//它是查找式的

	if ((lfDir = _findfirst(dirp.c_str(), &fileDir)) == -1l)
	{
		return vector<string>();
	}
	else {
		do {	//遍历目录
			if (!strequ(fileDir.name, ".") && !strequ(fileDir.name, ".."))	//排除这两个狗东西
				files.push_back(string(fileDir.name));
		} while (_findnext(lfDir, &fileDir) == 0);
	}
	_findclose(lfDir);
	return files;
}
void GetDirFilesR_Proc(vector<string>* result, const string& odir /*backslashed*/, const string& childDir, const string& filter)
{
	vector<string> matchedFiles = GetDirFiles(odir+childDir, filter);
	for (auto& f : matchedFiles)
		if (IsFile(odir+childDir+f))
		{
			cout<<childDir+f<<endl;
			result->push_back(childDir + f);
			cout<<"---Result size="<<result->size()<<endl;
		}
	matchedFiles.clear();
	vector<string> all = GetDirFiles(odir + childDir, "*");
	for (auto& ele : all)
		if (IsDir(odir+childDir+ele))
		{
			cout<<"   * into: "<<childDir+ele<<endl;
			GetDirFilesR_Proc(result, odir, childDir + ele + "\\", filter);
		}
			
}
vector<string> CJZAPI GetDirFilesR(const string& dir /*backslashed*/, const string& filter = "*.*")
{
	vector<string> result;
	cout<<"\nDIR="<<dir;
	cout<<"\nFILTER="<<filter;
	cout<<"\n\n==============================================";
	GetDirFilesR_Proc(&result, dir, "", filter);
	return result;
}
int main()
{
	auto res = GetDirFilesR(R"(D:\Wormwaker\PROGRAMS\)", "death*.mp3");
	cout<<"\n\n\n\n\n===============================================";
	cout<<"\nTotal Count="<<res.size()<<endl;
	cout<<"\nFINAL RESULT:"<<endl<<endl;
	if(res.size())
	{
		for( auto& ele : res)
			cout<<ele<<endl;
	}
	cout<<"\nTotal Count="<<res.size()<<endl;
	cout<<"Mission Completed";
	return 0;
}
