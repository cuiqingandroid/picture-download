
// DownloadPicDlg.h : 头文件
//

#pragma once

#include <vector>
#include "afxwin.h"
#include "afxcmn.h"
#include "ImageStatic.h"
// CDownloadPicDlg 对话框
class CDownloadPicDlg : public CDialog, public FCHttpRequestManager
{
// 构造
public:
	CDownloadPicDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DOWNLOADPIC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	int   m_task_id ;
	std::string   m_receive ;

	HTTP_REQUEST_HEADER   m_header ;

	int m_nStart;

	std::vector<CString> m_vectUrl;

	int m_nImageIdx;

	virtual void OnAfterRequestSend (FCHttpRequest& rTask) ;
	virtual void OnAfterRequestFinish (FCHttpRequest& rTask) ;
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
