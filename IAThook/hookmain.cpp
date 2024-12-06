#include"hookmain.h"
BOOL InstallHook()
{
	DWORD dwOldProtect = 0;
	VirtualProtect(g_iatAddr, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*g_iatAddr = (DWORD)HookMessageBoxW;
	VirtualProtect(g_iatAddr, 4, dwOldProtect, &dwOldProtect);
	MessageBoxA(0, "HOOk成功", "提示", MB_OK);
	return TRUE;
}
BOOL UnstallHook()
{
	DWORD dwOldProtect = 0;
	VirtualProtect(g_iatAddr, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*g_iatAddr = (DWORD)g_UnhookFuncAddr;
	VirtualProtect(g_iatAddr, 4, dwOldProtect, &dwOldProtect);
	return TRUE;
}

int WINAPI HookMessageBoxW(
	HWND    hWnd,
	LPCWSTR lpText,
	LPCWSTR lpCaption,
	UINT    uType
)
{
	UnstallHook();
	DWORD result = MessageBoxW(hWnd, L"51hook", lpCaption, uType);
	InstallHook();
	return result;
}
DWORD* getIatAddr(const char* dllname, const char* funName)
{

	HMODULE hModule = GetModuleHandleA(0);
	PIMAGE_DOS_HEADER pdosHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pdosHeader->e_lfanew + (char*)hModule);
	PIMAGE_OPTIONAL_HEADER pOptionHeader = &(pNtHeader->OptionalHeader);
	IMAGE_DATA_DIRECTORY dataDirectory = pOptionHeader->DataDirectory[1];
	PIMAGE_IMPORT_DESCRIPTOR pImprotTable =
		(PIMAGE_IMPORT_DESCRIPTOR)(dataDirectory.VirtualAddress + (DWORD)hModule);

	while (pImprotTable->Name)
	{
		char* dllName = (char*)pImprotTable->Name + (DWORD)hModule;
		if (_stricmp(dllName, dllname) == 0)
		{
			MessageBoxA(0, dllName, "333", MB_OK);
			PIMAGE_THUNK_DATA pINT =
				(PIMAGE_THUNK_DATA)(pImprotTable->OriginalFirstThunk + (DWORD)hModule);
			PIMAGE_THUNK_DATA pIAT =
				(PIMAGE_THUNK_DATA)(pImprotTable->FirstThunk + (DWORD)hModule);
			while (pINT->u1.Function)
			{
				if ((pINT->u1.Ordinal & 0x80000000) == 0)
				{

					PIMAGE_IMPORT_BY_NAME pImportName =
						(PIMAGE_IMPORT_BY_NAME)(pINT->u1.Ordinal + (DWORD)hModule);
					MessageBoxA(0, pImportName->Name, "333", MB_OK);
					if (strcmp(pImportName->Name, funName) == 0)
					{
						MessageBoxA(0, pImportName->Name, "333", MB_OK);
						return (DWORD*)pIAT;
					}
				}
				pINT++;
				pIAT++;
			}
		}
		pImprotTable++;

	}
	MessageBoxA(0, "并没找到次函数", "333", MB_OK);
	return NULL;
}
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwCallReason, LPVOID lpReserved)
{

	if (dwCallReason == DLL_PROCESS_ATTACH)
	{
		MessageBoxA(0, "DLL加载", "提示", MB_OK);
		g_iatAddr = getIatAddr("user32.dll", "MessageBoxW");
		g_UnhookFuncAddr = (DWORD*)(*g_iatAddr);
		BOOL result = InstallHook();
		MessageBoxA(0, "DLL加载完毕", "提示", MB_OK);
	}
	else if (dwCallReason == DLL_PROCESS_DETACH)
	{
		UnstallHook();
	}
	return TRUE;
}