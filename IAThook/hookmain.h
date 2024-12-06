#pragma once
#include<Windows.h>
#include<iostream>

DWORD* g_iatAddr = NULL;
DWORD* g_UnhookFuncAddr = NULL;
BOOL InstallHook();
BOOL UnstallHook();
int WINAPI HookMessageBoxW(
	HWND    hWnd,
	LPCWSTR lpText,
	LPCWSTR lpCaption,
	UINT    uType
);
DWORD* getIatAddr(const char* dllname, const char* funName);


