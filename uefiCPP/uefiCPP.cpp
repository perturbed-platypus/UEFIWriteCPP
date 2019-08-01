#include "pch.h"
#include <iostream>
#include <Windows.h>

// https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-setfirmwareenvironmentvariableexa
#define VARIABLE_ATTRIBUTE_NON_VOLATILE 0x00000001

struct CloseHandleHelper
{
	void operator()(void *p) const
	{
		CloseHandle(p);
	}
};

// Taken from https://superuser.com/questions/1045279/use-bcdedit-to-configure-pxe-boot-as-default-boot-option
BOOL SetPrivilege(HANDLE process, LPCWSTR name, BOOL on)
{
	HANDLE token;
	if (!OpenProcessToken(process, TOKEN_ADJUST_PRIVILEGES, &token))
		return FALSE;
	std::unique_ptr<void, CloseHandleHelper> tokenLifetime(token);
	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	if (!LookupPrivilegeValueW(NULL, name, &tp.Privileges[0].Luid))
		return FALSE;
	tp.Privileges[0].Attributes = on ? SE_PRIVILEGE_ENABLED : 0;
	return AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), NULL, NULL);
}

int main()
{
	unsigned char PTR[] =
		"\xcc\xcc\xc3";

	const int PTR_SIZE = sizeof(PTR);
	DWORD returnValue;
	const LPCSTR variableName = ("Testing-cpp");
	const LPCSTR GUID = ("{E660597E-B94D-4209-9C80-1805B5D19B69}");

	SetPrivilege(GetCurrentProcess(), SE_SYSTEM_ENVIRONMENT_NAME, TRUE);

	returnValue = SetFirmwareEnvironmentVariableExA(variableName, GUID, PTR, PTR_SIZE, VARIABLE_ATTRIBUTE_NON_VOLATILE);

	if (returnValue == 0)
	{
		return -1;
	}
	return 0;
}
