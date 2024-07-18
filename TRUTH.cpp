//UTF-8
#define UNICODE
#define _UNICODE
#include <iostream>
#include <Windows.h>
#include <string>
#include <sstream>
#include <vector>
#include <io.h>
#include <tlhelp32.h>
using namespace std;

#define CJZAPI __stdcall
#define KEY_DOWN(vk) 	(GetAsyncKeyState(vk) & 0x8000 ? 1 : 0)
#define DEF_CAPTION		L"TRUTH v1.0 - by Wormwaker"

int scr_w = 1920, scr_h = 1080;
HWND hwnd{nullptr};

inline int GetScreenHeight(void) //获取屏幕高度
{
	return GetSystemMetrics(SM_CYSCREEN);
}
inline int GetScreenWidth(void) //获取屏幕宽度
{
	return GetSystemMetrics(SM_CXSCREEN);
}
bool CJZAPI ExistProcess(LPCWSTR lpName)	//判断是否存在指定进程
{	//******警告！区分大小写！！！！******// 
	//*****警告！必须加扩展名！！！！*****// 
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);	
	if (INVALID_HANDLE_VALUE == hSnapshot) 	
	{		
		return false;	
	}	
	PROCESSENTRY32 pe = { sizeof(pe) };	
	BOOL fOk;	
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) 	
	{		
		if (! wcsicmp(pe.szExeFile, lpName)) 		
		{			
			CloseHandle(hSnapshot);			
			return true;		
		}	
	}	
	return false;
}
void CJZAPI SetColor(UINT uFore,UINT uBack) 
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, uFore + uBack * 0x10);
}
template <typename T>
string CJZAPI str(const T& value)
{
	stringstream ss;
	ss << value;
	string s;
	ss >> s;
	return s;
}
//wchar_t* CJZAPI strrpc(wchar_t* str, const wchar_t* oldstr, const wchar_t* newstr)
//{    		//字符串替换 
//	wchar_t bstr[65535];//转换缓冲区    
//	memset(bstr, 0, sizeof(bstr));
//	for (size_t i = 0; i < wcslen(str); i++)
//	{
//		if (!wcsncmp(str + i, oldstr, wcslen(oldstr)))
//		{//查找目标字符串            
//			wcscat_s(bstr, newstr);
//			i += wcslen(oldstr) - 1;
//		}
//		else {
//			wcsncat(bstr, str + i, sizeof(wchar_t));	    
//		}
//	}
//	wcscpy(str, bstr);
//	return str;
//}
wchar_t* strrpc(wchar_t* str, const wchar_t* oldstr, const wchar_t* newstr) {
	// 转换缓冲区    
	wchar_t bstr[65535];
	memset(bstr, 0, sizeof(bstr));
	
	// 保存字符串长度
	size_t len = wcslen(str);
	
	size_t i = 0;
	size_t bstr_index = 0;
	while (i < len) {
		// 查找目标字符串
		if (!wcsncmp(str + i, oldstr, wcslen(oldstr))) {
			// 如果找到了目标字符串，替换为新字符串
			wcscpy(bstr + bstr_index, newstr);
			bstr_index += wcslen(newstr);
			i += wcslen(oldstr);
		} else {
			// 如果没有找到目标字符串，直接拷贝到新字符串中
			bstr[bstr_index++] = str[i++];
		}
	}
	
	// 将新字符串拷贝回原字符串
	wcscpy(str, bstr);
	
	return str;
}
string CJZAPI sprintf2(const char* szFormat, ...)
{
	va_list _list;
	va_start(_list, szFormat);
	char szBuffer[1024] = "\0";
	_vsnprintf(szBuffer, 1024, szFormat, _list);
	va_end(_list);
	return string{szBuffer};
}
template <typename T>
int CJZAPI ToInt(const T& value)
{
	stringstream ss;
	ss << value;
	int i;
	ss >> i;
	return i;
}
inline bool CJZAPI strequ(char *str, const char *obj)
{	//比较是否一样 
	return (strcmp((const char *)str, obj) == 0 ? true : false);
}
inline bool CJZAPI sequ(const string& s1, const string& s2)
{
	return _stricmp(s1.c_str(), s2.c_str()) == 0;
}
string CJZAPI strtail(const string& s, int cnt = 1) {
	//012 len=3
	//abc   s.substr(2,1)
	if (cnt > s.size())
		return s;
	return s.substr(s.size() - cnt, cnt);
}
string CJZAPI strhead(const string& s, int cnt = 1) {
	if (cnt > s.size())
		return s;
	return s.substr(0, cnt);
}
string CJZAPI strxtail(const string& s, int cnt = 1) {
	if (cnt > s.size())
		return "";
	return s.substr(0, s.size() - cnt);
}
string CJZAPI strxhead(const string& s, int cnt = 1) {
	if (cnt > s.size())
		return "";
	return s.substr(cnt, s.size() - cnt);
}
string CJZAPI strip(const string& s)
{
	string res = s;
	while(!res.empty() && (res.at(0) == '\r' || res.at(0) == '\n' || res.at(0) == '\0'))
		res = res.substr(1, res.size() - 1);
	while(!res.empty() && (res.at(res.size()-1) == '\r' || res.at(res.size()-1) == '\n' || res.at(0) == '\0'))
		res = res.substr(0, res.size() - 1);
	return res;
}
// wstring=>string
std::string WString2String(const std::wstring &ws)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const wchar_t *wchSrc = ws.c_str();
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	char *chDest = new char[nDestSize];
	memset(chDest, 0, nDestSize);
	wcstombs(chDest, wchSrc, nDestSize);
	std::string strResult = chDest;
	delete[] chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;
}

// string => wstring
std::wstring String2WString(const std::string &s)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const char *chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t *wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);
	std::wstring wstrResult = wchDest;
	delete[] wchDest;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;
}
//from https://blog.csdn.net/qq_30386941/article/details/126814596
bool CJZAPI ExistFile(const string& sfile)
{	//文件或文件夹都可以 
	return !_access(sfile.c_str(), S_OK);//S_OK表示只检查是否存在 
}
int CJZAPI RandomRange(int Min, int Max, bool rchMin, bool rchMax)
{ //随机数值区间 
	int a;	
	a = rand();
	if(rchMin && rchMax)	//[a,b]
		return (a%(Max - Min + 1)) + Min;
	else if(rchMin && !rchMax)		//[a,b)
		return (a%(Max - Min)) + Min;
	else if(!rchMin && rchMax)		//(a,b]
		return (a%(Max - Min)) + Min + 1;
	else							//(a,b)
		return (a%(Max - Min - 1)) + Min + 1;
}
int CJZAPI Randint(int Min, int Max)
{	//闭区间随机值
	return RandomRange(Min, Max, true, true);
}
template <typename T>
T Choice(const vector<T>& _list)
{
	return _list.at(Randint(0, _list.size() - 1));
}

void FocusWindow(HWND _hwnd)
{
	DWORD dwCurrentThread = GetCurrentThreadId();
	DWORD dwFGThread      = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
	
	AttachThreadInput(dwCurrentThread, dwFGThread, TRUE);
	
	// Possible actions you may wan to bring the window into focus.
	SetForegroundWindow(_hwnd);
	SetCapture(_hwnd);
	SetFocus(_hwnd);
	SetActiveWindow(_hwnd);
	EnableWindow(_hwnd, TRUE);
	
	AttachThreadInput(dwCurrentThread, dwFGThread, FALSE);
}	//from https://www.coder.work/article/555749

void KeyP(BYTE bVk)
{
	keybd_event(bVk, 0, 0, 0);
}
void KeyR(BYTE bVk)
{
	keybd_event(bVk, 0, 2, 0);
}
std::wstring PasteFromClipboard() {
	std::wstring result;
	if (!OpenClipboard(NULL)) return result;
	
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData == NULL) {
		CloseClipboard();
		return result;
	}
	
	wchar_t* pText = (wchar_t*)GlobalLock(hData);
	if (pText != NULL) {
		result = pText;
		GlobalUnlock(hData);
	}
	CloseClipboard();
	
	return result;
}

// 将std::wstring内容复制到剪贴板
void CopyToClipboard(const std::wstring& text) {
	if (!OpenClipboard(NULL)) return;
	EmptyClipboard();
	
	size_t len = (text.size() + 1) * sizeof(wchar_t);
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	if (hMem == NULL) {
		CloseClipboard();
		return;
	}
	
	wchar_t* memPtr = (wchar_t*)GlobalLock(hMem);
	wcscpy_s(memPtr, len / sizeof(wchar_t), text.c_str());
	GlobalUnlock(hMem);
	
	SetClipboardData(CF_UNICODETEXT, hMem);
	CloseClipboard();
}
/////////////////////////////////////////////////////////////////
std::wstring getIthUnicodeChar(const std::wstring& utf8Str, size_t i) {
	size_t byteIndex = 0; // 当前字节索引
	size_t charCount = 0; // 当前已处理的Unicode字符数量
	
	while (byteIndex < utf8Str.size()) {
		
		
		
		if ((utf8Str[byteIndex] & 0x80) == 0) { // 1字节字符
			byteIndex += 1;
		} else if ((utf8Str[byteIndex] & 0xE0) == 0xC0) { // 2字节字符
			byteIndex += 2;
		} else if ((utf8Str[byteIndex] & 0xF0) == 0xE0) { // 3字节字符
			byteIndex += 3;
		} else if ((utf8Str[byteIndex] & 0xF8) == 0xF0) { // 4字节字符
			byteIndex += 4;
		} else {
			throw std::invalid_argument("Invalid UTF-8 sequence");
		}
		
		if (charCount == i) {
			return utf8Str.substr(byteIndex - utf8Str.size(), byteIndex - charCount);
		}
		charCount++;
	}
	
	throw std::out_of_range("Index out of range");
}
#include <string>
#include <stdexcept>
#include <cstdint> // for uint32_t

// 返回UTF-8字符串中第i个Unicode字符的wchar_t表示（BMP平面内）
wchar_t getIthUnicodeCharBMP(const std::wstring& utf8Str, size_t i) {
	size_t byteIndex = 0; // 当前字节索引
	size_t charCount = 0; // 当前已处理的Unicode字符数量
	
	while (byteIndex < utf8Str.size()) {
		uint32_t codePoint = 0;
		int byteCount = 0;
		
		// 解析当前字符的字节数和Unicode码点
		if ((utf8Str[byteIndex] & 0x80) == 0) { // 1字节字符
			codePoint = utf8Str[byteIndex];
			byteCount = 1;
		} else if ((utf8Str[byteIndex] & 0xE0) == 0xC0) { // 2字节字符
			codePoint = (utf8Str[byteIndex] & 0x1F) << 6;
			codePoint |= (utf8Str[byteIndex + 1] & 0x3F);
			byteCount = 2;
		} else if ((utf8Str[byteIndex] & 0xF0) == 0xE0) { // 3字节字符，但这里只处理BMP平面内字符
			codePoint = (utf8Str[byteIndex] & 0x0F) << 12;
			codePoint |= (utf8Str[byteIndex + 1] & 0x3F) << 6;
			codePoint |= (utf8Str[byteIndex + 2] & 0x3F);
			byteCount = 3;
		} else {
			throw std::invalid_argument("Invalid or non-BMP Unicode sequence encountered");
		}
		
		// 检查是否超出了BMP平面
		if (codePoint > 0xFFFF) {
			throw std::invalid_argument("Character outside BMP plane detected");
		}
		
		byteIndex += byteCount;
		
		if (charCount == i) {
			// 确保转换为wchar_t不会丢失信息（对于BMP平面内的字符是安全的）
			return static_cast<wchar_t>(codePoint);
		}
		charCount++;
	}
	
	throw std::out_of_range("Index out of range");
}

// 计算UTF-8字符串中的总Unicode字符数量
size_t countUnicodeChars(const std::wstring& utf8Str) {
	size_t count = 0;
	for (size_t i = 0; i < utf8Str.size();) {
		if ((utf8Str[i] & 0x80) == 0) {
			i++; // 1字节
		} else if ((utf8Str[i] & 0xE0) == 0xC0) {
			i += 2; // 2字节
		} else if ((utf8Str[i] & 0xF0) == 0xE0) {
			i += 3; // 3字节
		} else if ((utf8Str[i] & 0xF8) == 0xF0) {
			i += 4; // 4字节
		} else {
			throw std::invalid_argument("Invalid UTF-8 sequence");
		}
		count++;
	}
	return count;
}

//std::wstring Encode(const std::wstring& vis, const std::wstring& hid) {
//	const std::wstring str = L"\u200e\u200f\u200c\u200d\ufeff";
//	std::wstring result = L"";
//	
//	for (size_t i = 0; i < hid.length(); i++) {
//		int tmp = static_cast<int>(hid[i]);
//		std::wstring oneLetter = L"";
//		
//		while (tmp > 0) {
//			oneLetter = str[tmp % 5] + oneLetter;
//			tmp = (tmp - tmp % 5) / 5;
//		}
//		
//		result += oneLetter;
//		if (i != hid.length() - 1) {
//			result += L"\u200b";
//		}
//	}
//	
//	return vis.substr(0, 1) + result + vis.substr(1);
//}

// 加密函数
std::wstring Encode(const std::wstring& vis, const std::wstring& hid) {
	const std::wstring str = L"\u200e\u200f\u200c\u200d\ufeff";
	std::wstring result = L"";
	
	//std::string hid_a = WString2String(hid);
	
	size_t len = countUnicodeChars(hid);
	for (size_t i = 0; i < len; ++i) {
		wchar_t tmp = getIthUnicodeCharBMP(hid, i);
		std::wstring oneLetter = L"";
		
		while (tmp > 0) {
			oneLetter = str[tmp % 5] + oneLetter;
			tmp /= 5;
		}
		
		result += oneLetter;
		if (i != len - 1) {
			result += L'\u200b';
		}
	}
	
	return vis.front() + result + vis.substr(1);
}

//// 加密函数，优化后
//std::wstring Encode(const std::wstring& vis, const std::wstring& hid) {
//	// 定义用于编码的零宽度Unicode字符集
//	const std::wstring codeChars = L"\u200e\u200f\u200c\u200d\ufeff";
//	
//	std::wstring result = std::wstring{vis.front()}; // 保留明文的第一个字符
//	
//	// 遍历暗语的每个字符进行编码
//	for (wchar_t ch : hid) {
//		// 将暗语字符的Unicode编码值转化为五进制数，并构建对应的编码字符串
//		std::wstring encodedPart = L"";
//		unsigned int charCode = ch;
//		while (charCode > 0) {
//			encodedPart = codeChars[charCode % 5] + encodedPart;
//			charCode /= 5;
//		}
//		
//		// 将编码后的暗语字符添加到结果中
//		result += encodedPart;
//	}
//	
//	// 在暗语编码后添加一个零宽度空格作为分隔符（如果需要的话）
//	result += L'\u200b';
//	
//	// 将明文的剩余部分添加到编码后的暗语后面
//	result += vis.substr(1);
//	
//	return result;
//}
void ShrinkEmerge()
{
	constexpr int w = 550, h = 250;
	ShowWindow(hwnd, SW_RESTORE);
	FocusWindow(hwnd);
	SetWindowPos(hwnd, HWND_TOP, scr_w / 2 - w / 2, scr_h - h, w, h, 0);
	//system("mode con lines=5 cols=20");
}
void ApproachClipboardText(wstring& text)
{
	if (text.empty())
		return;
	if (text.at(text.size()-1) == L'\n')
		text = text.substr(0, text.size() - 1);
}
void ApproachTruthString(wstring& truth)
{
	if (truth.empty())
		return;
	//strrpc((wchar_t*)truth.c_str(), L" ", L"`");	//英文空格替换　ˋ
//	strrpc((wchar_t*)truth.c_str(), L"\n", L"↙");	//换行符替换↙
//	if (truth.at(truth.size()-1) == L'\n')
//		truth = truth.substr(0, truth.size() - 1);
}
void ApproachMixedString(wstring& mixed_string)
{
	//你
	mixed_string = wstring(strrpc((wchar_t*)mixed_string.c_str(), 
		L"\u200f\u200f\u200c\u200c\u200f\u200c\u200d",
		L"\u200f\u200f\u200c\u200c\u200c\ufeff\u200e"));
	
	if (mixed_string.length() >= 3
		&& mixed_string.substr(mixed_string.length() - 3, 3)
		   == L"\u200b\u200c\u200e")	//去除多余的末尾
		mixed_string = mixed_string.substr(0, mixed_string.length() - 3);
}

void Start()
{
	wstring clipboard_text{L""};
	wstring truth_string{L"没啥"};
	wstring mixed_string{L""};
	
	vector<wstring> truth_prefabs {
		L"nothing",
		L"没啥",
		L"hh",
		L"没有内容。",
		L"这是程序自行添加的。",
	};
	
	SetColor(3, 0);
	cout << "----------------------------\n";
	SetColor(10, 0);
	cout << "         服务已启动     \n\n";
	SetColor(12, 0);
	cout << "[LAlt+Enter] 混合默认暗语加密并发送\n\n";
	cout << "[RAlt+Enter] 混合自定义暗语加密并发送\n\n";
	while (1)
	{
		if (KEY_DOWN(VK_MENU) && KEY_DOWN(VK_RETURN))
		{
			bool custom = KEY_DOWN(VK_RMENU);
			//等待释放：
			while (KEY_DOWN(VK_MENU) && KEY_DOWN(VK_RETURN));
			
			KeyR(VK_MENU);
			
//			//添加空格（防止微信被吞一个字符）:
//			KeyP(VK_SPACE);
//			Sleep(5);
//			KeyR(VK_SPACE);
				
//			Sleep(5);
			
//			//去除空行：
//			KeyP(VK_BACK);
//			Sleep(5);
//			KeyR(VK_BACK);
			
			//全选复制：
			KeyP(VK_LCONTROL);
			KeyP('A');
			Sleep(5);
			KeyR('A');
			
			Sleep(5);
			
			KeyP('C');
			Sleep(5);
			KeyR('C');
			KeyR(VK_LCONTROL);
			
			clipboard_text = PasteFromClipboard();
			
			ApproachClipboardText(clipboard_text);
			
			if (clipboard_text.empty())
			{
				MessageBeep(MB_ICONERROR);
				continue;
			}
			
			if (!custom)
				truth_string = L"@Wormwaker\n";//Choice(truth_prefabs);
			else{
				HWND hwnd_that = GetForegroundWindow();
				
				system("cls");
				ShrinkEmerge();
				SetConsoleTitleA("输入暗语");
				
				SetColor(15, 0);
				cout << "INPUT (Press Ctrl+Z+Enter to end)" << '\n';
				SetColor(14, 0);
				cout << "$> ";
				SetColor(13, 0);
//				wcin >> truth_string;
				
				truth_string.clear();
				wchar_t ch;
				while((ch = fgetwc(stdin)) != WEOF)
				{
					truth_string += ch;
				}
				
				//“你”这个字不行
				FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
				SetConsoleTitleW(DEF_CAPTION);
				ShowWindow(hwnd, SW_HIDE);
				
				FocusWindow(hwnd_that);
				Sleep(500);
			}
			
			ApproachTruthString(truth_string);
			
			mixed_string = Encode(clipboard_text, truth_string);
			
			ApproachMixedString(mixed_string);
			
			SetColor(2, 0);
			cout << "成功添加暗语 \"";
			SetColor (12, 0);
			//wcout << truth_string;
			printf("%ls", truth_string.c_str());
			SetColor(2, 0);
			cout << "\"\n";
//			SetColor(3, 0);
//			cout << "合并后的字符串为：\"";
//			SetColor(8, 0);
////			wcout << mixed_string;
//			printf("%ls", mixed_string.c_str());
//			SetColor(3, 0);
//			cout << "\"\n" << endl;
			
			CopyToClipboard(mixed_string);
			
			//粘贴：
			KeyP(VK_LCONTROL);
			KeyP('A');
			Sleep(5);
			KeyR('A');
			
			Sleep(5);
			
			KeyP('V');
			Sleep(5);
			KeyR('V');
			KeyR(VK_LCONTROL);
			
			//发送：
			KeyP(VK_LCONTROL);
			KeyP(VK_RETURN);
			Sleep(5);
			KeyR(VK_RETURN);
			KeyR(VK_LCONTROL);

			MessageBeep(MB_ICONINFORMATION);
			
			clipboard_text.clear();
			truth_string.clear();
			mixed_string.clear();
		}
	}
	
}
bool TerminalCheck(DWORD dwPid, HWND _hwnd)
{	//检查是否为win11新终端
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);	
	if (INVALID_HANDLE_VALUE == hSnapshot) 	
	{		
		return false;	
	}	
	PROCESSENTRY32 pe = { sizeof(pe) };	
	BOOL fOk;	
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) 	
	{		
		if (! wcsicmp(pe.szExeFile, L"WindowsTerminal.exe")
			&& pe.th32ProcessID == dwPid) 		
		{			
			CloseHandle(hSnapshot);			
			{
				wchar_t title[MAX_PATH]{0};
				GetWindowTextW(_hwnd, title, MAX_PATH);
			//	if(wcsicmp(title, _wpgmptr) && wcsicmp(title, DEF_CAPTION))
			//		return false;
				return true;
			}		
		}	
	}	
	return false;
}
BOOL CALLBACK EnumWindowsProc(HWND _hwnd, LPARAM lParam)
{
	DWORD pid;
	GetWindowThreadProcessId(_hwnd, &pid);
	if(TerminalCheck(pid, _hwnd))
	{
		hwnd = _hwnd;
		return FALSE;
	}
	return TRUE;
}
int main()
{
	scr_w = GetScreenWidth();
	scr_h = GetScreenHeight();
	
	if(ExistProcess(L"WindowsTerminal.exe"))
	{	//win11电脑且使用新版终端
		EnumWindows(EnumWindowsProc, 0);
	}else{	//旧版控制台主机
		hwnd = GetConsoleWindow();
	}
	if(!hwnd || hwnd == INVALID_HANDLE_VALUE)
	{
		hwnd = GetForegroundWindow();
	}
	ShowWindow(hwnd, SW_HIDE);
	
	SetConsoleTitleW(DEF_CAPTION);
	system("chcp 65001");
	SetColor(11, 0);
	cout << "控制台窗口句柄：";
	SetColor(13, 0);
	cout << reinterpret_cast<intptr_t>(hwnd) << endl;
	
	Start();
	
	return 0;
}
