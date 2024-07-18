// VirtualInput 模拟输入
// Author: Wormwaker
#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cstdarg>
#include <map>
using namespace std;
#define KEY_DOWN(vk) (GetAsyncKeyState(vk)&0x8000?1:0)
#define CJZAPI __stdcall

inline VOID CJZAPI KeyP(BYTE bVk) { //press
	keybd_event(bVk,0,0,0);
}
inline VOID CJZAPI KeyR(BYTE bVk) { //release
	keybd_event(bVk,0,2,0);
}
inline VOID CJZAPI KeyPR(BYTE bVk) { //press&release
	keybd_event(bVk,0,0,0);
	Sleep(5);
	keybd_event(bVk,0,2,0);
}
inline void Paste()
{
	KeyP(VK_LCONTROL);
	KeyPR('V');
	KeyR(VK_LCONTROL);
}

VOID CJZAPI SetColor(UINT uFore,UINT uBack) 
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle,uFore+uBack*0x10);
}   //变色
/*
  颜色属性由两个十六进制数字指定 --
  第一个对应于前景，第二个对应于背景。（对于↑函数） 
  每个数字 可以为以下任何值:
  0 = 黑色 8 = 深灰色
  1 = 深蓝色 9 = 蓝色
  2 = 深绿色 10 = 淡绿色
  3 = 深浅绿色 11 = 淡浅绿色(浅蓝色) 
  4 = 深红色 12 = 淡红色
  5 = 深紫色 13 = 淡紫色
  6 = 深黄色 14 = 淡黄色
  7 = 深白色 15 = 亮白色
 */
string CJZAPI strip(const string& s)
{
	string res = s;
	while (!res.empty() && (res.at(0) == '\r' || res.at(0) == '\n' || res.at(0) == '\0'))
		res = res.substr(1, res.size() - 1);
	while (!res.empty() && (res.at(res.size() - 1) == '\r' || res.at(res.size() - 1) == '\n' || res.at(0) == '\0'))
		res = res.substr(0, res.size() - 1);
	return res;
}
wstring CJZAPI wstrip(const wstring& s)
{
	wstring res = s;
	while (!res.empty() && (res.at(0) == L'\r' || res.at(0) == L'\n' || res.at(0) == L'\0'))
		res = res.substr(1, res.size() - 1);
	while (!res.empty() && (res.at(res.size() - 1) == L'\r' || res.at(res.size() - 1) == L'\n' || res.at(0) == L'\0'))
		res = res.substr(0, res.size() - 1);
	return res;
}
// string => wstring
wstring String2WString(const string &s)
{
	string strLocale = setlocale(LC_ALL, "");
	const char *chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t *wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);
	wstring wstrResult = wchDest;
	delete[] wchDest;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;
}
//from https://blog.csdn.net/qq_30386941/article/details/126814596
string CJZAPI sprintf2(const char* szFormat, ...)
{
	va_list _list;
	va_start(_list, szFormat);
	char szBuffer[1024] = "\0";
	_vsnprintf(szBuffer, 1024, szFormat, _list);
	va_end(_list);
	return string{ szBuffer };
}

// 将宽字符文本放入剪贴板
void PutTextToClipboard(LPCWSTR text) {
	// 打开剪贴板
	if (OpenClipboard(NULL)) {
		// 清空剪贴板内容
		EmptyClipboard();
		
		// 获取文本长度（以字符数为单位）
		int textLength = lstrlenW(text);
		//printf("lstrlenW=%d  content=%ls\n", textLength, text);
		
		// 分配内存保存文本数据
		HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, (textLength + 1) * sizeof(WCHAR));
		if (hClipboardData != NULL) {
			// 锁定内存并将文本数据复制到其中
			LPWSTR pClipboardData = static_cast<LPWSTR>(GlobalLock(hClipboardData));
			if (pClipboardData != NULL) {
				lstrcpyW(pClipboardData, text);
				GlobalUnlock(hClipboardData);
				
				// 将数据放入剪贴板
				SetClipboardData(CF_UNICODETEXT, hClipboardData);
			}
		}
		
		// 关闭剪贴板
		CloseClipboard();
	}
}
// 从剪贴板获取宽字符文本
wstring GetTextFromClipboard(void) 
{
	wstring clipboardText;
	// 打开剪贴板
	if (OpenClipboard(NULL)) {
		// 检查剪贴板中是否包含 CF_UNICODETEXT 格式的数据
		if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
			// 获取剪贴板中 CF_UNICODETEXT 格式的数据
			HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
			if (hClipboardData != NULL) {
				// 锁定内存并获取文本数据
				LPCWSTR pClipboardText = static_cast<LPCWSTR>(GlobalLock(hClipboardData));
				if (pClipboardText != NULL) {
					// 将文本数据存入字符串
					clipboardText = pClipboardText;
					GlobalUnlock(hClipboardData);
				}
			}
		}
		// 关闭剪贴板
		CloseClipboard();
	}
	return clipboardText;
}
static const map<char, BYTE> normal_keys
{
	{' ', VK_SPACE}, {'\n', VK_RETURN}, {'\r', VK_NONAME},
	{',', VK_OEM_COMMA},{'.', VK_DECIMAL},{'*', VK_MULTIPLY},
	{'/', VK_DIVIDE}, {'-', VK_OEM_MINUS}, {'=', VK_OEM_PLUS},
	{'[', VK_OEM_4}, {']', VK_OEM_6}, {';', VK_OEM_1},
	{'\'', VK_OEM_7}, {'\\', VK_OEM_5}, {'`', VK_OEM_3},
	{'+', VK_ADD},  {'\t', VK_TAB}, {'\b', VK_BACK},
};
static const map<char, BYTE> shift_keys
{
	{'!', '1'}, {'?', VK_OEM_2}, {'(', '9'},
	{')', '0'}, {'&', '7'}, {'"', VK_OEM_7},
	{'<', VK_OEM_COMMA}, {'>', VK_OEM_PERIOD}, {':', VK_OEM_1},
	{'@', '2'}, {'#', '3'}, {'$', '4'},
	{'%', '5'}, {'^', '6'}, {'_', VK_OEM_MINUS},
	{'~', VK_OEM_3}, {'{', VK_OEM_4}, {'}', VK_OEM_6},
	{'|', VK_OEM_5},
};

HWND hwnd_target{nullptr};

#define VIRTUAL_INPUT_INTERNAL_CD 0
void VirtualInput(const string& s)
{
	clock_t start = clock();
	for(long i = 0; i < s.length(); )
	{
		if(KEY_DOWN(VK_RCONTROL))
		{
			MessageBeep(MB_ICONINFORMATION);
			FlashWindow(GetConsoleWindow(), TRUE);
			SetColor(13, 0);
			cout << "\n已暂停！";
			SetColor(2, 0);
			printf("(已经输入：%zu / %zu  %.1lf%%)\n\n",
				i, s.length(), 100.0*i/s.length());
			while(KEY_DOWN(VK_RCONTROL));
			SetColor(7, 0);
			cout << "[右Ctrl] 继续输入\n";
			cout << "[右Alt] 终止输入\n";
			while(1)
			{
				if(KEY_DOWN(VK_RCONTROL))
				{
					SetColor(10, 0);
					cout << "继续！\n";
					while(KEY_DOWN(VK_RCONTROL));
					MessageBeep(MB_ICONINFORMATION);
					break;
				}
				if(KEY_DOWN(VK_RMENU))
				{
					SetColor(12, 0);
					cout << "终止输入！\n";
					exit(0);
				}
				Sleep(5);
			}
		}
		while(GetForegroundWindow() != hwnd_target)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			FlashWindow(GetConsoleWindow(), TRUE);
			SetColor(12, 0);
			printf("[防误触] 检测到原输入窗口不在前端，请将原输入窗口切换为前端窗口！ (已经输入：%zu / %zu  %.1lf%%)\n",
					i, s.length(), 100.0*i/s.length());
			SetColor(7, 0);
			printf("-->      [右Alt] 终止输入\n");
			while(1)
			{
				if(hwnd_target == GetForegroundWindow())
					break;
				if(KEY_DOWN(VK_RMENU))
				{
					SetColor(12, 0);
					cout << "终止输入！";
					exit(0);
				}
				Sleep(5);
			}
			SetColor(15, 0);
			printf("-->      [右Ctrl] 继续输入剩余内容\n" );
			while(1)
			{
				if(KEY_DOWN(VK_RCONTROL))
				{
					SetColor(14, 0);
					cout << "继续输入！\n";
					while(KEY_DOWN(VK_RCONTROL));
					MessageBeep(MB_ICONINFORMATION);
					break;
				}
				if(KEY_DOWN(VK_RMENU))
				{
					SetColor(12, 0);
					cout << "终止输入！";
					exit(0);
				}
				Sleep(5);
			}
		}
		if(KEY_DOWN(VK_RMENU))
			exit(0);
//		printf("%d = '%c'\n", s.at(i), s.at(i));
		if((unsigned char)	//不能漏
			(s.at(i)) > 0x7F)
		{
			string substr = s.substr(i,2);
			wchar_t wsubstr[6] = L"\0";
			lstrcpynW(wsubstr, String2WString(substr).c_str(), 6);
			PutTextToClipboard(wsubstr);
			
			wstring wstr = wstring{wsubstr};
			if(!wstrip(wstr).empty())
			{
				wstring cbwstr;
				while((cbwstr = GetTextFromClipboard()) != wstr)
				{
					PutTextToClipboard(wsubstr);
					//printf("cbwstr=\"%ls\", substr=\"%ls\" len_cbwstr=%zu, len_substr=%zu cbwstr_char=%d substr_char=%d\n", 
					//	cbwstr.c_str(), substr, cbwstr.length(), wstr.length(), int(cbwstr[0]), int(wstr[0]));
					Sleep(50);	//防止出现恶心人的叠字现象，讨厌厌
				}
				Paste();
			}
			Sleep(VIRTUAL_INPUT_INTERNAL_CD);
			i += 2;
			continue;
		}
		if(isdigit(s.at(i)))
		{
			KeyPR(s.at(i));
		}else if(islower(s.at(i)))
		{
			KeyPR(toupper(s.at(i)));
		}
		else if(isupper(s.at(i)))
		{
			KeyP(VK_LSHIFT);
			KeyPR(s.at(i));
			KeyR(VK_LSHIFT);
		}else{
			bool yes = false;
			for(const auto& pr : normal_keys)
				if(s.at(i) == pr.first)
				{
					KeyPR(pr.second);
					yes = true;
				}
					
			for(const auto& pr : shift_keys)
				if(s.at(i) == pr.first)
				{
					KeyP(VK_LSHIFT);
					KeyPR(pr.second);
					KeyR(VK_LSHIFT);
					yes = true;
				}
			if(!yes)
			{
				wchar_t wsubstr[3] = L"";
				lstrcpynW(wsubstr, String2WString(s.substr(i,1)).c_str(), 3);
				PutTextToClipboard(wsubstr);
				wstring wstr = wstring{wsubstr};
				if(!wstrip(wstr).empty())
				{
					while(GetTextFromClipboard() != wstr)
					{
						PutTextToClipboard(wsubstr);
						Sleep(50);	//防止出现恶心人的叠字现象，讨厌厌
					}
					Paste();
				}
			}
		}
		++i;
		if(KEY_DOWN(VK_RMENU))
			exit(0);
		Sleep(VIRTUAL_INPUT_INTERNAL_CD);
	}
	MessageBeep(MB_ICONINFORMATION);
	SetColor(15, 0);
	printf("模拟输入完毕！总耗时：%.2lf ms (包括暂停时间)\n\n", (clock() - start) / (double)CLOCKS_PER_SEC);
}
int main()
{
	SetConsoleTitleA("模拟输入 Virtual Input - by Wormwaker");
	SetColor(10, 0);
	cout << "====================================================\n";
	SetColor(14, 0);
	cout << "             模拟输入器 - 应对无法粘贴的情况 \n";
	SetColor(3, 0);
	cout << "                            by Wormwaker  \n";
	SetColor(15, 0);
	cout << "输入你需要打字的内容(可以粘贴)，按下 [Ctrl+Z] 结束输入：\n";
	
	SetColor(7, 0);
	string content{""};
	char ch;
	while((ch = fgetc(stdin)) != EOF)
	{
		content += ch;
	}
	SetColor(10, 0);
	cout << "\n====================================================\n";
	SetColor(3, 0);
	cout << "                   总长度: ";
	SetColor(6, 0);
	cout << content.length() << endl;
	SetColor(14, 0);
	cout << "按下 [右Ctrl] 开始输入！\n";
	SetColor(15, 0);
	cout << "[提示] 输入的中途按下";
	SetColor(3, 0);
	cout << " [右Ctrl] ";
	SetColor(15, 0);
	cout << "可以暂停输入，按下";
	SetColor(13, 0);
	cout << " [右Alt] ";
	SetColor(15, 0);
	cout << "可以中断输入！\n";
	while(!KEY_DOWN(VK_RCONTROL));
	while(KEY_DOWN(VK_RCONTROL));
	hwnd_target = GetForegroundWindow();
	VirtualInput(content);
	return 0;
}
