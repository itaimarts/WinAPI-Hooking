#include "stdafx.h"
#undef UNICODE
#include <vector>
#include <string>
#include <windows.h>
#include <Tlhelp32.h>
using std::vector;
using std::string;

#define DLL_NAME "HookCreateFileDLL.dll"
#define DIR_PATH "C:\\temp"

int main(void)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hTool32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	BOOL bProcess = Process32First(hTool32, &pe32);
	CreateDirectory(DIR_PATH, NULL);
	
	if (bProcess == TRUE) {
		while ((Process32Next(hTool32, &pe32)) == TRUE) {
			//if (strcmp(pe32.szExeFile, "notepad.exe") == 0)
			//{
			char* DirPath = new char[MAX_PATH];
			char* FullPath = new char[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, DirPath);
			sprintf_s(FullPath, MAX_PATH, "%s\\%s", DirPath, DLL_NAME);

			HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pe32.th32ProcessID);

			if (hProcess == NULL)
				continue;

			LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");

			LPVOID LLParam = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(FullPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			WriteProcessMemory(hProcess, LLParam, FullPath, strlen(FullPath), NULL);
			CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddr,
				LLParam, NULL, NULL);
			CloseHandle(hProcess);
			delete[] DirPath;
			delete[] FullPath;
			//}
		}
	}
	CloseHandle(hTool32);
	return 0;
}


