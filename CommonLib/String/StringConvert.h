#pragma once

class CStringConvert
{
public:
	static CString GetWideCharString(LPCSTR szText);
	static CStringA GetMultiByteString(LPCWSTR szText);
private:
	CStringConvert(void);
	~CStringConvert(void);
};
