
// DownloadPic.cpp : ����Ӧ�ó��������Ϊ��
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


// CDownloadPicApp ����

CDownloadPicApp::CDownloadPicApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	m_gdiplusToken = 0;
}


// Ψһ��һ�� CDownloadPicApp ����

CDownloadPicApp theApp;


// CDownloadPicApp ��ʼ��

BOOL CDownloadPicApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	GdiplusStartupInput gdiplusStartupInput;
	::GdiplusStartup( &m_gdiplusToken, &gdiplusStartupInput, NULL );

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CString sError;
	m_strHarddiskId = CGetHardDiskID::GetID(sError);

	CDownloadPicDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
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
