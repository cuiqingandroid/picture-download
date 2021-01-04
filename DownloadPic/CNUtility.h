#pragma once

class CUtility
{
public:
	CUtility(void);
	~CUtility(void);

	typedef enum
	{
		Action_Update = 0,
		Action_Unintall
	}Action;

	static bool IsX64Architecture();

	// Â·¾¶Ïà¹Ø
	static CString GetLocalPath(void);
	static CString GetTempPath();
	static CString GetAppDataPath(BOOL bCreate = FALSE);
	static CString GetWindowsPath();
	static CString GetLastDirver();
	static CString GetMaxDirver();

	static BOOL CloseProcess(const CString& strProcessName, int nExitCode = 0);
	
	static BOOL IsEntirelyCovered(HWND hWnd);
	static BOOL IsPartialyCovered(HWND hwnd);
	static BOOL IsFullScreen(HWND hWnd);
	static BOOL IsFullScreenWndCovered(HWND hWnd);
};
