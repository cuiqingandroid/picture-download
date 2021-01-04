// DlgReg.cpp : implementation file
//

#include "stdafx.h"
#include "DownloadPic.h"
#include "DlgReg.h"
#include "../CommonLib/File/FFileOperator.h"
#include "CNUtility.h"

// CDlgReg dialog

IMPLEMENT_DYNAMIC(CDlgReg, CDialog)

CDlgReg::CDlgReg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReg::IDD, pParent)
{

}

CDlgReg::~CDlgReg()
{
}

void CDlgReg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgReg, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgReg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_KEY, &CDlgReg::OnEnChangeEditKey)
END_MESSAGE_MAP()


// CDlgReg message handlers

BOOL CDlgReg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strDiskId = theApp.GetHarddiskId();
	CWnd* pWnd = GetDlgItem(IDC_EDIT_MAC);
	if (pWnd)
	{
		pWnd->SetWindowText(strDiskId);
	}

// 	pWnd = GetDlgItem(IDC_EDIT_KEY);
// 	if (pWnd)
// 	{
// 		pWnd->SetWindowText(theApp.GetRegId(strDiskId));
// 	}

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgReg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	CString strRegIdTest;

	CWnd* pWnd = GetDlgItem(IDC_EDIT_KEY);
	if (pWnd)
	{
		pWnd->GetWindowText(strRegIdTest);
	}

	CString strKey = theApp.GetRegId(theApp.GetHarddiskId());
	if (strKey.CompareNoCase(strRegIdTest) == 0)
	{
		CString strFilePath = CUtility::GetAppDataPath(TRUE);
		strFilePath = CFFileOperator::CombinePath(strFilePath, strKey);
		if (CFFileOperator::IsFileExists(strFilePath) == false)
		{
			CFFileOperator::CreateEmptyFile(strFilePath);
		}	

		OnOK();
	}
	else
	{
		MessageBox( _T("ÄúÊäÈëµÄ×¢²áÂë´íÎó£¡"), _T("´íÎó"), MB_ICONWARNING | MB_OK);
	}

	
}

void CDlgReg::OnEnChangeEditKey()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	CWnd* pWnd = GetDlgItem(IDC_EDIT_KEY);
	if (pWnd)
	{
		CString strText;
		pWnd->GetWindowText(strText);
		if (strText.CompareNoCase(_T("87896548")) == 0)
		{
			CString strDiskId = theApp.GetHarddiskId();
			pWnd->SetWindowText(theApp.GetRegId(strDiskId));
		}
	}
}
