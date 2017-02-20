#pragma comment(lib, "detours.lib")

#undef UNICODE
#include <windows.h>
#include "detours.h"
#include <fstream>  
#include <string>
#include <direct.h>
#include <strsafe.h>

extern __declspec(dllexport) void foo();

#define DIR_PATH "C:\\temp\\"

BOOL(WINAPI * Real_CreateDirectory) (
	LPCWSTR lpPathName,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateDirectoryW;

BOOL WINAPI Routed_CreateDirectory(
	LPCWSTR lpPathName,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{

	DWORD pid = GetCurrentProcessId();
	char name[500];
	char fullPIDLogFilePath[1024];
	char logger[1024];
	size_t i;
	HANDLE hFile;
	char DataBuffer[500];
	DWORD dwBytesToWrite;
	DWORD dwBytesWritten = 0;
	BOOL bErrorFlag = FALSE;
	wchar_t wtext[1024];


	//sprintf_s(logger, "Entered hook => file name: %s, log file: %s\n", DataBuffer, fullPIDLogFilePath);
	//OutputDebugString(TEXT(logger));

	//converting full log file path to LPWSTR
	sprintf_s(fullPIDLogFilePath, "%s%d.txt", DIR_PATH, pid);
	mbstowcs(wtext, fullPIDLogFilePath, strlen(fullPIDLogFilePath) + 1);//Plus null
	LPWSTR ptr = wtext;
	hFile = CreateFileW(ptr, FILE_APPEND_DATA, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		OutputDebugString(TEXT("Unable to open file\n"));

	wcstombs_s(&i, DataBuffer, (size_t)500, lpPathName, 500);
	sprintf_s(DataBuffer, "%s\r\n", DataBuffer);
	dwBytesToWrite = (DWORD)strlen(DataBuffer);
	//sprintf_s(logger, "length: %d\n", dwBytesToWrite);
	//OutputDebugString(logger);

	bErrorFlag = WriteFile(
		hFile,           // open file handle
		DataBuffer,      // start of data to write
		dwBytesToWrite,  // number of bytes to write
		&dwBytesWritten, // number of bytes that were written
		NULL);            // no overlapped structure

	if (FALSE == bErrorFlag)
		OutputDebugString(TEXT("write the requested file in our log\n"));
	else
		if (dwBytesWritten != dwBytesToWrite)
			OutputDebugString(TEXT("number of written bytes not equal to requested one\n"));
		else
			OutputDebugString(TEXT("documented succefully\n"));

	CloseHandle(hFile);

	//wcstombs_s(&i, name, (size_t)500, lpFileName, (size_t)500);
	//sprintf_s(fullPIDLogFilePath, "PID %d Tried to create a file named %s\n", pid, name);
	//OutputDebugString(TEXT(fullPIDLogFilePath));

	return Real_CreateDirectory(lpPathName, lpSecurityAttributes);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	LONG Error;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		OutputDebugString(TEXT("Attaching HookCreateDirectoryDll.dll"));
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)Real_CreateDirectory, Routed_CreateDirectory);
		Error = DetourTransactionCommit();

		if (Error == NO_ERROR)
			OutputDebugString(TEXT("Hooked Success"));
		else
			OutputDebugString(TEXT("Hook Error"));

		break;
	case DLL_PROCESS_DETACH:
		OutputDebugString(TEXT("De-Attaching HookCreateDirectoryDll.dll"));
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)Real_CreateDirectory, Routed_CreateDirectory);
		Error = DetourTransactionCommit();

		if (Error == NO_ERROR)
			OutputDebugString(TEXT("Un-Hooked Success"));
		else
			OutputDebugString(TEXT("Un-Hook Error"));
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}


