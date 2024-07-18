//类似C语言中的try-catch

#include <stdio.h>
#include <windows.h>
#include <setjmp.h>

jmp_buf jmpBuffer;

// 自定义的异常处理函数
LONG WINAPI CustomUnhandledExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo) {
//	printf("Unhandled exception occurred!\n");
	printf("使用不死图腾！！\n");	
	// 尝试使用 longjmp 跳过异常引发的代码
	longjmp(jmpBuffer, 
		1	//这里可以传递错误信息，作为setjmp第二次返回值
		);
	
	// 如果 longjmp 成功执行，这里将不会被执行
	return EXCEPTION_CONTINUE_SEARCH;
}

int main() {
	// 设置自定义的未处理异常过滤器
	SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);
	int ret = 0;
	// 尝试使用 longjmp 跳过异常引发的代码
	if ((ret = setjmp(jmpBuffer)) == 0) {
		// 可能引发异常的代码
		printf("开始渡劫\n");
		
		//1.
		int* ptr = NULL;
		*ptr = 114514;	//NullPointerException
		
		//2.
		int* ptr2 = (int*)0x00000001;
		*ptr2 = 42; 	//Access Violation
		
		//3.
		int a[1] = {0};
		a[100]=1;		//Access Violation
		
		//abort直接崩溃，不是异常
	} else {
		//printf("Recovered from exception using longjmp.\n");
		printf("获得了成就 [超越生死]！\n");
		//printf("ret = %d\n", ret);	//你传递的错误信息
	}
	
	//printf("Program continues after exception.\n");
	printf("大难不死，劫后余生\n");
	
	return 0;
}

/*
注意事项：
使用 longjmp 跳过异常引发的代码属于非标准做法： 这种方法可能会导致程序状态不一致或未定义行为。因此，一般情况下不建议在生产环境中使用这种方式来处理异常。
更好的做法是预防异常的发生： 在编程中应当遵循良好的异常处理实践，通过合适的检查和错误处理机制来防止空指针访问等异常的发生。
总结来说，SetUnhandledExceptionFilter 可以用于捕获异常，但使用 longjmp 跳过异常引发的代码并不是一个推荐的做法，应该仅作为示例和理论演示。
*/

