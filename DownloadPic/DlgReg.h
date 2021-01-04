#pragma once


// CDlgReg dialog

class CDlgReg : public CDialog
{
	DECLARE_DYNAMIC(CDlgReg)

public:
	CDlgReg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgReg();

// Dialog Data
	enum { IDD = IDD_DLG_REG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEditKey();
};
