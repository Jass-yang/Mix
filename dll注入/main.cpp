#include <iostream>  
#include <Windows.h>  
#include <string>  

int main() {
    const char* dllPath = "d:\\proj.dll"; // 使用双反斜杠或原始字符串字面量  
    SIZE_T buffSize = strlen(dllPath) + 1; // 字符串长度加1（空字符）  
    SIZE_T realWrite = 0;

    // 1. 打开目标进程  
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 5208); // 确保进程ID正确  
    if (hProcess == NULL) {
        std::cerr << "无法打开目标进程" << std::endl;
        return 1;
    }

    // 2. 申请远程内存空间  
    char* remoteStr = (char*)VirtualAllocEx(hProcess, NULL, buffSize, MEM_COMMIT, PAGE_READWRITE);
    if (remoteStr == NULL) {
        std::cerr << "无法分配远程内存" << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    // 3. 将dll文件路径写入到内存  
    // 先将dll路径复制到本地缓冲区  
    char* localStr = new char[buffSize];
    strcpy_s(localStr, buffSize, dllPath);

    // 再将本地缓冲区的内容写入远程进程的内存  
    BOOL success = WriteProcessMemory(hProcess, remoteStr, localStr, buffSize, &realWrite);
    delete[] localStr; // 释放本地缓冲区  
    if (!success || realWrite != buffSize) {
        std::cerr << "写入远程内存失败" << std::endl;
        VirtualFreeEx(hProcess, remoteStr, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 4. 创建远程线程  
    typedef HMODULE(WINAPI* LoadLibraryType)(LPCSTR lpLibFileName);
    LoadLibraryType pLoadLibrary = (LoadLibraryType)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if (pLoadLibrary == NULL) {
        std::cerr << "无法获取LoadLibraryA地址" << std::endl;
        VirtualFreeEx(hProcess, remoteStr, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, remoteStr, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "创建远程线程失败" << std::endl;
        VirtualFreeEx(hProcess, remoteStr, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 等待远程线程结束  
    WaitForSingleObject(hThread, INFINITE);

    // 释放远程内存和线程句柄  
    VirtualFreeEx(hProcess, remoteStr, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}