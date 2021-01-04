#include "StdAfx.h"
#include "CNUtility.h"
#include "..\CommonLib\File\FFileOperator.h"
#include "psapi.h"
#include <tlhelp32.h>
#include <vector>
using namespace std;

CUtility::CUtility(void)
{
}

CUtility::~CUtility(void)
{
}

bool CUtility::IsX64Architecture()
{
	SYSTEM_INFO systemInfo;
	memset(&systemInfo, 0, sizeof(SYSTEM_INFO));
	GetNativeSystemInfo(&systemInfo);

	return (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
}

CString CUtility::GetTempPath()
{
	CString strPath;

	TCHAR path[MAX_PATH];
	DWORD dwRet = ::GetTempPath(MAX_PATH, path);
	if (dwRet > 0)
	{
		strPath = path;
	}

	strPath = CFFileOperator::CombinePath(strPath, _T("CloudNest"));

	if (CFFileOperator::IsFileExists(strPath) == false)
	{
		CFFileOperator::CreateDirectories(strPath);
	}

	return strPath;
}

CString CUtility::GetAppDataPath(BOOL bCreate)
{
	CString strPath;
	TCHAR path[MAX_PATH];

	if (SHGetSpecialFolderPath(NULL, path, CSIDL_APPDATA, FALSE) == TRUE)
	{
		strPath = path;
		strPath.Trim(_T("\\"));
		strPath += _T("\\tutu");

		if (bCreate == TRUE)
		{
			if (CFFileOperator::IsFileExists(strPath) == false)
			{
				CFFileOperator::CreateDirectories(strPath);
			}
		}


		return strPath;
	}

	return strPath;
}


CString CUtility::GetWindowsPath()
{
	CString strPath;
	TCHAR path[MAX_PATH];

	if (SHGetSpecialFolderPath(NULL, path, CSIDL_WINDOWS, FALSE) == TRUE)
	{
		strPath = path;
		strPath.Trim(_T("\\"));
		return strPath;
	}

	return strPath;
}

CString CUtility::GetLastDirver()
{
	CString strPath;

	vector<TCHAR> vectDrives;

	DWORD dwDrives  = 0, dwMask = 1;
	TCHAR chDrive;
	dwDrives = GetLogicalDrives();
	for (UINT i = 0; i < 26;i++)
	{
		if(dwDrives & dwMask)
		{
			chDrive = 'A' + i;

			vectDrives.push_back(chDrive);
		}
		dwMask <<= 1;		
	}

	for (int i = 0; i < vectDrives.size(); i++)
	{
		CString strDriver(vectDrives[i]);

		strDriver += _T(":");

		if (GetDriveType(strDriver) == DRIVE_FIXED)
		{
			strPath = strDriver;
		}
	}

	return strPath;
}


typedef struct
{
	CString strDriver;
	int nSize;
}DriverInfo;

CString CUtility::GetMaxDirver()
{
	CString strPath;

	vector<TCHAR> vectDrives;
	DWORD dwDrives  = 0, dwMask = 1;
	TCHAR chDrive;
	dwDrives = GetLogicalDrives();
	for (UINT i = 0; i < 26;i++)
	{
		if(dwDrives & dwMask)
		{
			chDrive = 'A' + i;

			vectDrives.push_back(chDrive);
		}
		dwMask <<= 1;		
	}

	vector<DriverInfo> vectDriverInfo;

	for (int i = 0; i < vectDrives.size(); i++)
	{
		CString strDriver(vectDrives[i]);

		strDriver += _T(":");

		if (GetDriveType(strDriver) == DRIVE_FIXED)
		{
			DWORD dwSectorsPerCluster = 0;//每簇中扇区数
			DWORD dwBytesPerSector = 0;//每扇区中字节数
			DWORD dwFreeClusters = 0;//剩余簇数
			DWORD dwTotalClusters = 0;//总簇数

			BOOL bRet = GetDiskFreeSpace(
				strDriver,
				&dwSectorsPerCluster,
				&dwBytesPerSector,
				&dwFreeClusters,
				&dwTotalClusters
				);

			if (bRet == FALSE)
			{
				continue;;
			}

			double dblFree = dwFreeClusters * dwSectorsPerCluster;
			dblFree /= (1024 * 1024 * 1024);
			dblFree *= dwBytesPerSector;

			DriverInfo info;
			info.strDriver = strDriver;
			info.nSize = (int)dblFree;

			vectDriverInfo.push_back(info);
		}
	}

	if (vectDriverInfo.size() < 1)
	{
		return strPath;
	}

	if (vectDriverInfo.size() == 1)
	{
		DriverInfo& info = vectDriverInfo[0];
		strPath = info.strDriver;

		return strPath;
	}

	CString strSysDriver = GetWindowsPath();
	strSysDriver = strSysDriver.Left(2);

	int nMaxSize = 0;
	for (int i = (int)vectDriverInfo.size() - 1; i >= 0; i--)
	{
		DriverInfo& info = vectDriverInfo[i];
		if (info.strDriver.CompareNoCase(strSysDriver) == 0)
		{
			continue;
		}

		if (i == (int)vectDriverInfo.size() - 1)
		{
			if (info.nSize > 5)
			{
				strPath = info.strDriver;
				break;
			}
		}

		if (info.nSize > nMaxSize)
		{
			nMaxSize = info.nSize;
			strPath = info.strDriver;
		}
	}

	return strPath;
}

BOOL CUtility:: CloseProcess(const CString& strProcessName, int nExitCode)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	BOOL bRet = FALSE;


	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof( PROCESSENTRY32 );

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if( !Process32First( hProcessSnap, &pe32 ) )
	{
		CloseHandle( hProcessSnap );    // Must clean up the
		return FALSE;
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	vector<DWORD> vectProcessIds;
	do
	{
		CString strName = pe32.szExeFile;
		if (strName.CompareNoCase(strProcessName) == 0)
		{
			vectProcessIds.push_back(pe32.th32ProcessID);
		}

	} while( Process32Next( hProcessSnap, &pe32 ) );

	CloseHandle( hProcessSnap );

	for (int i = 0; i < vectProcessIds.size(); i++)
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, vectProcessIds[i]);
		if (NULL != hProcess )
		{	
			bRet = TerminateProcess(hProcess, nExitCode);
			CloseHandle(hProcess);
		}
	}

	return bRet;
}

BOOL CUtility::IsPartialyCovered(HWND hWnd)
{
	BOOL bCovered = FALSE;

	if (hWnd == NULL)
	{
		return bCovered;
	}

	if ((::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) == WS_CHILD)
	{
		return bCovered;
	}

	CRect rcTarget;
	::GetWindowRect(hWnd, &rcTarget);
	CRgn rgnAboveWnds;
	rgnAboveWnds.CreateRectRgn(0, 0, 0, 0);
	HWND hAboveWnd = hWnd;

	while((hAboveWnd = ::GetNextWindow(hAboveWnd, GW_HWNDPREV)) != NULL)
	{
		if (!::IsWindowVisible(hAboveWnd))
		{
			continue;
		}

		CRect rcAboveWnd;
		CRgn rgnAboveWnd;
		::GetWindowRect(hAboveWnd, &rcAboveWnd);
		rgnAboveWnd.CreateRectRgn(rcAboveWnd.left, rcAboveWnd.top, rcAboveWnd.right, rcAboveWnd.bottom);
		rgnAboveWnds.CombineRgn(&rgnAboveWnds, &rgnAboveWnd, RGN_OR);

		CRgn rgnTarget;
		rgnTarget.CreateRectRgn(rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
		int nCombineResult = rgnTarget.CombineRgn(&rgnTarget, &rgnAboveWnds, RGN_AND);

		if (nCombineResult == COMPLEXREGION || nCombineResult == SIMPLEREGION)
		{
			bCovered =  TRUE;
			break;
		}
	}

	return bCovered;
}

BOOL CUtility::IsEntirelyCovered(HWND hWnd)
{
	BOOL bCovered = FALSE;

	if (hWnd == NULL)
	{
		return bCovered;
	}

	if ((::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) == WS_CHILD)
	{
		return bCovered;
	}

	CRect rcTarget;
	::GetWindowRect(hWnd, &rcTarget);
	CRgn rgnAboveWnds;
	rgnAboveWnds.CreateRectRgn(0, 0, 0, 0);
	HWND hAboveWnd = hWnd;

	while((hAboveWnd = ::GetNextWindow(hAboveWnd, GW_HWNDPREV)) != NULL)
	{
		if (!::IsWindowVisible(hAboveWnd))
		{
			continue;
		}

		CRect rcAboveWnd;
		CRgn rgnAboveWnd;
		::GetWindowRect(hAboveWnd, &rcAboveWnd);
		rgnAboveWnd.CreateRectRgn(rcAboveWnd.left, rcAboveWnd.top, rcAboveWnd.right, rcAboveWnd.bottom);
		rgnAboveWnds.CombineRgn(&rgnAboveWnds, &rgnAboveWnd, RGN_OR);

		CRgn rgnTarget;
		rgnTarget.CreateRectRgn(rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
		int nCombineResult = rgnTarget.CombineRgn(&rgnTarget, &rgnAboveWnds, RGN_DIFF);

		if (nCombineResult == NULLREGION)
		{
			bCovered =  TRUE;
			break;
		}
	}

	return bCovered;
}

BOOL CUtility::IsFullScreen(HWND hWnd)
{
	if (hWnd == NULL)
	{
		return FALSE;
	}

	if (!::IsWindowVisible(hWnd))
	{
		return FALSE;
	}

	int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	CRect rcWnd;
	GetWindowRect(hWnd, &rcWnd);

	if (rcWnd.top == 0 && rcWnd.left == 0 
		&& rcWnd.Width() == nScreenWidth
		&& rcWnd.Height() == nScreenHeight)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CUtility::IsFullScreenWndCovered(HWND hWnd)
{
	HWND hAboveWnd = NULL;

	while((hAboveWnd = ::GetNextWindow(hAboveWnd, GW_HWNDPREV)) != NULL)
	{
		if (!::IsWindowVisible(hAboveWnd))
		{
			continue;
		}

		if (IsFullScreen(hAboveWnd))
		{
			return TRUE;
		}
	}

	return FALSE;
}