
// DownloadPic.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "DownloadPic.h"
#include "DownloadPicDlg.h"
#include "GetHardDiskID1.h"
#include "../CommonLib/File/FFileOperator.h"
#include "md5.h"
#include "CNUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDownloadPicApp

BEGIN_MESSAGE_MAP(CDownloadPicApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDownloadPicApp 构造

CDownloadPicApp::CDownloadPicApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_gdiplusToken = 0;
}


// 唯一的一个 CDownloadPicApp 对象

CDownloadPicApp theApp;


// CDownloadPicApp 初始化

BOOL CDownloadPicApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	GdiplusStartupInput gdiplusStartupInput;
	::GdiplusStartup( &m_gdiplusToken, &gdiplusStartupInput, NULL );

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CString sError;
	m_strHarddiskId = CGetHardDiskID::GetID(sError);

	CDownloadPicDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CDownloadPicApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	::GdiplusShutdown(m_gdiplusToken);
	return CWinAppEx::ExitInstance();
}

CString CDownloadPicApp::GetHarddiskId()
{
	return m_strHarddiskId;
}

CString CDownloadPicApp::GetRegId(const CString& strDiskId)
{
	CString strKey = strDiskId;
	if (strDiskId.GetLength() > 0)
	{
		USES_CONVERSION;
		for (int i = 0; i < 10; i++)
		{
			char *md5 = MD5String(T2A(strKey));
			if (md5)
			{
				strKey = A2T(md5);
				free(md5);
			}
		}
	}

	return strKey;
}

BOOL CDownloadPicApp::IsReged()
{
	CString strKey = GetRegId(m_strHarddiskId);
	if (strKey.GetLength() == 0)
	{
		return TRUE;
	}

	CString strFilePath = CUtility::GetAppDataPath(TRUE);
	strFilePath = CFFileOperator::CombinePath(strFilePath, strKey);
	if (CFFileOperator::IsFileExists(strFilePath) == false)
	{
		return FALSE;
	}	

	return TRUE;
}
