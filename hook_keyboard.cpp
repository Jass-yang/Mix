#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define WIN32_LEAN_AND_MEAN
#define NOCOMM

#include <Windows.h>

LPCWSTR AppName = L"SetRWin";
static HHOOK hHook = nullptr;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK HookProc(int code, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
    CreateMutexW(NULL, FALSE, AppName);//������Ϊappname�Ļ��������
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBoxW(nullptr, L"SetRWin is running.", AppName, MB_OK);
        return 0;
    }

    WNDCLASSEXW wc = { 0 };//��������Ϣ
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.hInstance = hInstance;
    wc.lpszClassName = AppName;
    wc.lpfnWndProc = WndProc;

    RegisterClassExW(&wc);//ע��һ��������

    HWND hwnd = CreateWindowExW(0, AppName, AppName, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);

    MSG msg = { 0 };//���������̵߳���Ϣ���е���Ϣ��Ϣ��
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);//��������Կ��Ϣת��Ϊ�ַ���Ϣ��
        DispatchMessageW(&msg);//����Ϣ���ȵ����ڹ��̡�
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            if (hHook == nullptr)
            {
                hHook = SetWindowsHookExW(WH_KEYBOARD_LL, HookProc, nullptr, 0);
            }

            break;
        }
        case WM_DESTROY:
        {
            UnhookWindowsHookEx(hHook);
            hHook = nullptr;

            PostQuitMessage(0);
            break;
        }
        default:
            break;
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK HookProc(int code, WPARAM wParam, LPARAM lParam)
{
    if (code == HC_ACTION)
    {
        KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;
        if (kbd->vkCode == VK_APPS)
        {
            INPUT input[1] = { 0 };
            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = VK_RWIN;

            if (wParam == WM_KEYUP)
            {
                input[0].ki.dwFlags = KEYEVENTF_KEYUP;
            }

            SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
            return 1;
        }
    }

    return CallNextHookEx(hHook, code, wParam, lParam);
}