
// DownloadPicDlg.h : ͷ�ļ�
//

#pragma once

#include <vector>
#include "afxwin.h"
#include "afxcmn.h"
#include "ImageStatic.h"
// CDownloadPicDlg �Ի���
class CDownloadPicDlg : public CDialog, public FCHttpRequestManager
{
// ����
public:
	CDownloadPicDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DOWNLOADPIC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	int   m_task_id ;
	std::string   m_receive ;

	HTTP_REQUEST_HEADER   m_header ;

	int m_nStart;

	std::vector<CString> m_vectUrl;

	int m_nImageIdx;

	virtual void OnAfterRequestSend (FCHttpRequest& rTask) ;
	virtual void OnAfterRequestFinish (FCHttpRequest& rTask) ;
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnLvnItemchangedListPicName(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnPath();
	afx_msg void OnBnClickedButton1();
public:
	void DownloadImage();
private:
	CComboBox m_ctrlComboImageType;
	CComboBox m_ctrlComboImageSize;
	CString m_strImageSavePath;
	int m_nDownImageCount;
	int m_nDownImageStartIndex;
	CString m_strInfo;
	CListCtrl m_ctrlPicName;
	CImageStatic m_ctrlPicView;
	
public:
	afx_msg void OnBnClickedBtnReg();

	void CheckReg();
};
