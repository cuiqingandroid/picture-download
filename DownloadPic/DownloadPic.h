
// DownloadPic.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDownloadPicApp:
// �йش����ʵ�֣������ DownloadPic.cpp
//

class CDownloadPicApp : public CWinAppEx
{
public:
	CDownloadPicApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()

public:
	ULONG_PTR m_gdiplusToken;
	virtual int ExitInstance();

	CString GetHarddiskId();
	CString GetRegId(const CString& strDiskId);
	BOOL IsReged();

private:
	CString m_strHarddiskId;

};

extern CDownloadPicApp theApp;