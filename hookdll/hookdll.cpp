#include"hookdll.h"


HINSTANCE g_hinstance = NULL;
HHOOK  g_hhook = NULL;
LRESULT CALLBACK KeyboardProc(
	_In_ int    code,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	if (code == HC_ACTION)
	{

		BYTE KeyState[256]{ 0 };
		if (GetKeyboardState(KeyState))
		{
			LONG keyinfo = lParam;
			UINT keyCode = (keyinfo >> 16) & 0x00ff;
			WCHAR wkeyCode = 0;
			ToAscii((UINT)wParam, keyCode, KeyState, (LPWORD)&wkeyCode, 0);
			CHAR strinfo[12] = { 0 };
			sprintf_s(strinfo, _countof(strinfo), "51hook_%c", wkeyCode);
			//OutputDebugStringA(strinfo);
			MessageBoxA(0, strinfo, 0, MB_OK);
			return 0;

		}
		else
		{
			MessageBox(0, 0, 0, MB_OK);
		}
		
	}
	return CallNextHookEx(g_hhook, code, wParam, lParam);

}
BOOL InstallHook()
{
	g_hhook = SetWindowsHookExA(WH_KEYBOARD, KeyboardProc, g_hinstance, 0);
	if (g_hhook == NULL)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL UnInstallHook()
{
	if (g_hhook != NULL)
	{
		return UnhookWindowsHookEx(g_hhook);
	}
	return FALSE;
}
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_hinstance = hInstance;
	}
	return TRUE;
}