#include <stdio.h>
#include <windows.h>
#include <imagehlp.h>  // 需要链接 ImageHlp.lib

#pragma comment(lib, "ImageHlp.lib")

// 打印错误信息
void PrintError(const char* message) {
    DWORD error = GetLastError();
    LPVOID errorMessage;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&errorMessage,
        0, NULL
    );

    printf("%s Error: 0x%08X - %s\n", message, error, (char*)errorMessage);
    LocalFree(errorMessage);
}

// 列出 DLL 导出的所有函数
void ListExportedFunctions(HMODULE hModule) {
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)hModule;
    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)hModule + dosHeader->e_lfanew);

    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        printf("Invalid NT headers signature.\n");
        return;
    }

    IMAGE_EXPORT_DIRECTORY* exportDir = (IMAGE_EXPORT_DIRECTORY*)
        ((BYTE*)hModule + ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    DWORD* nameRVAs = (DWORD*)((BYTE*)hModule + exportDir->AddressOfNames);
    WORD* nameOrdinals = (WORD*)((BYTE*)hModule + exportDir->AddressOfNameOrdinals);

    printf("\n导出函数列表:\n");
    printf("------------------------\n");

    for (DWORD i = 0; i < exportDir->NumberOfNames; i++) {
        char* functionName = (char*)((BYTE*)hModule + nameRVAs[i]);
        printf("%s\n", functionName);
    }

    printf("------------------------\n\n");
}

int main() {
    char dllPath[MAX_PATH];
    printf("请输入 DLL 路径 (默认: fscan.dll): ");
    fgets(dllPath, MAX_PATH, stdin);

    // 移除换行符
    if (strlen(dllPath) > 0 && dllPath[strlen(dllPath) - 1] == '\n') {
        dllPath[strlen(dllPath) - 1] = '\0';
    }

    // 使用默认值
    if (strlen(dllPath) == 0) {
        strcpy_s(dllPath, MAX_PATH, "fscan.dll");
    }

    printf("尝试加载 DLL: %s\n", dllPath);

    HMODULE hDLL = LoadLibrary(dllPath);
    if (hDLL == NULL) {
        PrintError("无法加载 DLL");
        return 1;
    }

    printf("DLL 加载成功！\n");

    // 列出所有导出函数
    ListExportedFunctions(hDLL);

    // 交互式测试函数名
    char functionName[256];
    while (1) {
        printf("请输入要测试的函数名 (输入 'exit' 退出): ");
        fgets(functionName, 256, stdin);

        // 移除换行符
        if (strlen(functionName) > 0 && functionName[strlen(functionName) - 1] == '\n') {
            functionName[strlen(functionName) - 1] = '\0';
        }

        // 检查是否退出
        if (strcmp(functionName, "exit") == 0) {
            break;
        }

        // 获取函数地址
        FARPROC functionAddr = GetProcAddress(hDLL, functionName);
        if (functionAddr == NULL) {
            PrintError("无法获取函数地址");
        } else {
            printf("函数 '%s' 地址: 0x%p\n", functionName, functionAddr);
        }
    }

    // 卸载 DLL
    FreeLibrary(hDLL);
    printf("DLL 已卸载。\n");

    return 0;
}