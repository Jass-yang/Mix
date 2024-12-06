#include <iostream>  
#include <Windows.h>  
#include <string>  

int main() {
    const char* dllPath = "d:\\proj.dll"; // ʹ��˫��б�ܻ�ԭʼ�ַ���������  
    SIZE_T buffSize = strlen(dllPath) + 1; // �ַ������ȼ�1�����ַ���  
    SIZE_T realWrite = 0;

    // 1. ��Ŀ�����  
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 5208); // ȷ������ID��ȷ  
    if (hProcess == NULL) {
        std::cerr << "�޷���Ŀ�����" << std::endl;
        return 1;
    }

    // 2. ����Զ���ڴ�ռ�  
    char* remoteStr = (char*)VirtualAllocEx(hProcess, NULL, buffSize, MEM_COMMIT, PAGE_READWRITE);
    if (remoteStr == NULL) {
        std::cerr << "�޷�����Զ���ڴ�" << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    // 3. ��dll�ļ�·��д�뵽�ڴ�  
    // �Ƚ�dll·�����Ƶ����ػ�����  
    char* localStr = new char[buffSize];
    strcpy_s(localStr, buffSize, dllPath);

    // �ٽ����ػ�����������д��Զ�̽��̵��ڴ�  
    BOOL success = WriteProcessMemory(hProcess, remoteStr, localStr, buffSize, &realWrite);
    delete[] localStr; // �ͷű��ػ�����  
    if (!success || realWrite != buffSize) {
        std::cerr << "д��Զ���ڴ�ʧ��" << std::endl;
        VirtualFreeEx(hProcess, remoteStr, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 4. ����Զ���߳�  
    typedef HMODULE(WINAPI* LoadLibraryType)(LPCSTR lpLibFileName);
    LoadLibraryType pLoadLibrary = (LoadLibraryType)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if (pLoadLibrary == NULL) {
        std::cerr << "�޷���ȡLoadLibraryA��ַ" << std::endl;
        VirtualFreeEx(hProcess, remoteStr, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, remoteStr, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "����Զ���߳�ʧ��" << std::endl;
        VirtualFreeEx(hProcess, remoteStr, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // �ȴ�Զ���߳̽���  
    WaitForSingleObject(hThread, INFINITE);

    // �ͷ�Զ���ڴ���߳̾��  
    VirtualFreeEx(hProcess, remoteStr, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}