#include <iostream>
#include <windows.h>

int main() {
	// 初始化计时器
	DWORD startTime = GetTickCount();
	int leftClicks = 0, rightClicks = 0;
	bool leftButtonDown = false, rightButtonDown = false;
	
	while (true) {
		// 检测鼠标左键状态
		bool leftButtonPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8001) != 0;
		if (leftButtonPressed && !leftButtonDown) {
			leftClicks++;
		}
		leftButtonDown = leftButtonPressed;
		
		// 检测鼠标右键状态
		bool rightButtonPressed = (GetAsyncKeyState(VK_RBUTTON) & 0x8001) != 0;
		if (rightButtonPressed && !rightButtonDown) {
			rightClicks++;
		}
		rightButtonDown = rightButtonPressed;
		
		// 计算经过的时间
		DWORD currentTime = GetTickCount();
		DWORD elapsedTime = currentTime - startTime;
		
		// 每秒输出一次CPS
		if (elapsedTime >= 1000) {
			std::cout << "\r" << leftClicks << " / "
					  << rightClicks;
			
			// 重置计时器和点击次数
			startTime = currentTime;
			leftClicks = 0;
			rightClicks = 0;
		}
		
		// 稍微延时，减轻CPU负担
		Sleep(10);
	}
	
	return 0;
}

