#include <Windows.h>

char* WideCharToAnsi(wchar_t* pWideChar)

{
	if (!pWideChar) return NULL;
	char* pszBuf = NULL;
	int needBytes = WideCharToMultiByte(CP_ACP, 0, pWideChar, -1, NULL, 0, NULL, NULL);
	if (needBytes > 0) {
		pszBuf = new char[needBytes + 1];
		ZeroMemory(pszBuf, (needBytes + 1) * sizeof(char));
		WideCharToMultiByte(CP_ACP, 0, pWideChar, -1, pszBuf, needBytes, NULL, NULL);
	}
	return pszBuf;
}

wchar_t* AnsiCharToWide(char* pChar)
{
	if (!pChar) return NULL;
	wchar_t* pszBuf = NULL;
	int needWChar = MultiByteToWideChar(CP_ACP, 0, pChar, -1, NULL, 0);
	if (needWChar > 0) {
		pszBuf = new wchar_t[needWChar + 1];
		ZeroMemory(pszBuf, (needWChar + 1) * sizeof(wchar_t));
		MultiByteToWideChar(CP_ACP, 0, pChar, -1, pszBuf, needWChar);
	}
	return pszBuf;
}