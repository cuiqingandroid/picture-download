#include "StdAfx.h"
#include "StringConvert.h"

CStringConvert::CStringConvert(void)
{
}

CStringConvert::~CStringConvert(void)
{
}

CString CStringConvert::GetWideCharString(LPCSTR szText)
{
	int size = MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		szText,
		-1,
		NULL,
		0);
	CString rText;

	if (size > 0)
	{
		TCHAR *buffer = (TCHAR *)malloc(size * sizeof(TCHAR));
		if (MultiByteToWideChar(
			CP_UTF8,
			MB_ERR_INVALID_CHARS,
			szText,
			-1,
			buffer,
			size) > 0)
		{
			rText = buffer;
		}

		free(buffer);
	}

	return rText;
}

CStringA CStringConvert::GetMultiByteString(LPCWSTR szText)
{
	int size = WideCharToMultiByte(
		CP_UTF8,
		0,
		szText,
		-1,
		NULL,
		0,
		NULL,
		NULL);

	CStringA rText;
	if (size > 0)
	{
		char *buffer = (char *)malloc(size);
		if (WideCharToMultiByte(
			CP_UTF8,
			0,
			szText,
			-1,
			buffer,
			size,
			NULL,
			NULL) > 0)
		{
			rText = buffer;
		}

		free(buffer);
	}

	return rText;
}

