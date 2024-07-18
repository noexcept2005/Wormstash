#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <iostream>
#include <string>

std::wstring GetDefaultBrowser()
{
	HKEY hKey;
	LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice", 0, KEY_READ, &hKey);
	if (status != ERROR_SUCCESS)
	{
		return L"";
	}
	
	wchar_t progId[MAX_PATH];
	DWORD dataSize = sizeof(progId);
	status = RegQueryValueEx(hKey, L"ProgId", nullptr, nullptr, reinterpret_cast<LPBYTE>(progId), &dataSize);
	RegCloseKey(hKey);
	if (status != ERROR_SUCCESS)
	{
		return L"";
	}
	
	HKEY hKeyProgId;
	status = RegOpenKeyEx(HKEY_CLASSES_ROOT, progId, 0, KEY_READ, &hKeyProgId);
	if (status != ERROR_SUCCESS)
	{
		return L"";
	}
	
	wchar_t defaultBrowser[MAX_PATH];
	dataSize = sizeof(defaultBrowser);
	status = RegQueryValueEx(hKeyProgId, nullptr, nullptr, nullptr, reinterpret_cast<LPBYTE>(defaultBrowser), &dataSize);
	RegCloseKey(hKeyProgId);
	if (status != ERROR_SUCCESS)
	{
		return L"";
	}
	
	return std::wstring(defaultBrowser);
}

//std::wstring GetDefaultBrowserPath()
//{	//"D:\program files\360browsers\360ChromeX\Chrome\Application\360ChromeX.exe" -- "%1"
//	HKEY hKey;
//	LSTATUS status = RegOpenKeyEx(HKEY_CLASSES_ROOT, L"http\\shell\\open\\command", 0, KEY_READ, &hKey);
//	if (status != ERROR_SUCCESS)
//	{
//		return L"";
//	}
//	
//	wchar_t browserPath[MAX_PATH];
//	DWORD dataSize = sizeof(browserPath);
//	status = RegQueryValueEx(hKey, nullptr, nullptr, nullptr, reinterpret_cast<LPBYTE>(browserPath), &dataSize);
//	RegCloseKey(hKey);
//	if (status != ERROR_SUCCESS)
//	{
//		return L"";
//	}
//	
//	std::wstring path(browserPath);
//	// The path may contain additional parameters, so we need to extract the executable path.
//	size_t pos = path.find_first_of(L"\"");
//	if (pos != std::wstring::npos)
//	{
//		path = path.substr(1, pos - 1); // Extracting the executable path
//	}
//	
//	return path;
//}

std::wstring GetDefaultBrowserPath()
{
	std::wstring defaultBrowserPath;
	HKEY hKey;
	LSTATUS status;
	
//	// Check HKEY_CLASSES_ROOT\http\shell\open\command
//	status = RegOpenKeyEx(HKEY_CLASSES_ROOT, L"http\\shell\\open\\command", 0, KEY_READ, &hKey);
//	if (status == ERROR_SUCCESS)
//	{
//		wchar_t browserPath[MAX_PATH];
//		DWORD dataSize = sizeof(browserPath);
//		status = RegQueryValueEx(hKey, nullptr, nullptr, nullptr, reinterpret_cast<LPBYTE>(browserPath), &dataSize);
//		RegCloseKey(hKey);
//		if (status == ERROR_SUCCESS)
//		{
//			defaultBrowserPath = std::wstring(browserPath);
//			
//			// Extracting the executable path
//			size_t pos = defaultBrowserPath.find_first_of(L"\"");
//			if (pos != std::wstring::npos)
//			{
//				defaultBrowserPath = defaultBrowserPath.substr(1, pos - 1);
//			}
//			
//			return defaultBrowserPath;
//		}
//	}
	
	// Check HKEY_LOCAL_MACHINE\SOFTWARE\Classes\http\shell\open\command
	status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\http\\shell\\open\\command", 0, KEY_READ, &hKey);
	if (status == ERROR_SUCCESS)
	{
		wchar_t browserPath[MAX_PATH];
		DWORD dataSize = sizeof(browserPath);
		status = RegQueryValueEx(hKey, nullptr, nullptr, nullptr, reinterpret_cast<LPBYTE>(browserPath), &dataSize);
		RegCloseKey(hKey);
		if (status == ERROR_SUCCESS)
		{
			defaultBrowserPath = std::wstring(browserPath);
			
			// Extracting the executable path
			size_t pos = defaultBrowserPath.find_first_of(L"\"");
			if (pos != std::wstring::npos)
			{
				defaultBrowserPath = defaultBrowserPath.substr(1, pos - 1);
			}
			
			return defaultBrowserPath;
		}
	}
	
	return L"";
}


std::wstring GetDefaultBrowserPath2()
{
	HKEY  hKetRoot, hkeySub;
	TCHAR ValueName[MAX_PATH];
	
	
	ULONG cbValueName = MAX_PATH;
	ULONG cbDataValue = MAX_PATH;
	
	std::wstring s = L"";
	DWORD dwType;
	if (RegOpenKey(HKEY_CLASSES_ROOT, NULL, &hKetRoot) == ERROR_SUCCESS)
	{
		if (RegOpenKeyEx(hKetRoot, L"http\\shell\\open\\command", 0, KEY_ALL_ACCESS, &hkeySub) == ERROR_SUCCESS)
		{   
			
			wchar_t szPath[MAX_PATH];
			RegEnumValueW(hkeySub, 0, ValueName, &cbValueName, NULL, &dwType, (LPBYTE)szPath, &cbDataValue);
			s = std::wstring(szPath);
			RegCloseKey(hkeySub);
		}
		
		RegCloseKey(hKetRoot);
	}
	return s;
}

int main()
{
	std::wstring defaultBrowser = GetDefaultBrowserPath();
	if (!defaultBrowser.empty())
	{
		std::wcout << L"The default browser is: " << defaultBrowser << std::endl;
	}
	else
	{
		std::wcout << L"Failed to retrieve default browser." << std::endl;
	}
	
	return 0;
}

