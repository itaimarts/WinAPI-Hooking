#include "stdafx.h"
#include <string>
#include <windows.h>
#include <stdio.h>
#include "imagehlp.h"
#include <vector>
#include <winnt.h>


using namespace std;


int main()
{

	HMODULE lib = LoadLibraryEx(TEXT("KERNEL32.DLL"), NULL, DONT_RESOLVE_DLL_REFERENCES);
	PIMAGE_NT_HEADERS header = (PIMAGE_NT_HEADERS)((BYTE *)lib + ((PIMAGE_DOS_HEADER)lib)->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)((BYTE *)lib + header->
		OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	BYTE** names = (BYTE**)((int)lib + exports->AddressOfNames);


	while (true)
	{
		printf("********************************************************************************\n");
		for (int i = 0; i < exports->NumberOfNames; i++) {
			DWORD nameRVA = ((PDWORD)names)[i];
			const char* nameAddress = (char*)lib + nameRVA;
			char* LoadLibraryAddr = (char*)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),nameAddress);
			BYTE Byte1 = LoadLibraryAddr[0];
			BYTE Byte2 = LoadLibraryAddr[1];
			BYTE Byte3 = LoadLibraryAddr[2];
			BYTE Byte4 = LoadLibraryAddr[3];
			BYTE Byte5 = LoadLibraryAddr[4];
			if (Byte1 != 0X8b)
				printf("The function %s Hooked!!!!!\n", nameAddress);
		}
		Sleep(10000);
	}

	
    return 0;
}

