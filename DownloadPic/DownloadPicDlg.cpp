
// DownloadPicDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DownloadPic.h"
#include "DownloadPicDlg.h"
#include "FolderDlg.h"
#include "../libjson/libjson.h"
#include "../libjson/_internal/Source/JSONNode.h"
#include "../CommonLib/Net/HttpGetFile.h"
#include "../CommonLib/File/FFileOperator.h"
#include "DlgReg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define __TITLE _T("图片下载")


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDownloadPicDlg 对话框




CDownloadPicDlg::CDownloadPicDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDownloadPicDlg::IDD, pParent)
	, m_strImageSavePath(_T(""))
	, m_nDownImageCount(0)
	, m_nDownImageStartIndex(0)
	, m_strInfo(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_header.m_header += L"Referer: http://wallpaper.apc.360.cn/\r\n" ;
	m_header.m_header += L"Accept: */*\r\n" ;

	m_nStart = -1;
	m_nImageIdx = 0;

	m_strImageSavePath = _T("D:\\Image");
	m_nDownImageStartIndex = 0;
	m_nDownImageCount = 10;
}

void CDownloadPicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_IMAGE_TYPE, m_ctrlComboImageType);
	DDX_Control(pDX, IDC_COMBO_IMAGE_SIZE, m_ctrlComboImageSize);
	DDX_Text(pDX, IDC_EDIT_SAVE_PATH, m_strImageSavePath);
	DDX_Text(pDX, IDC_EDIT_IMAGE_COUNT, m_nDownImageCount);
	DDX_Text(pDX, IDC_EDIT_IMAGE_START_INDEX, m_nDownImageStartIndex);

	DDX_Text(pDX, IDC_STC_INFO, m_strInfo);

	DDX_Control(pDX, IDC_LIST_PIC_NAME, m_ctrlPicName);
	DDX_Control(pDX, IDC_STATIC_PIC, m_ctrlPicView);
}

BEGIN_MESSAGE_MAP(CDownloadPicDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_GETFILE, &CDownloadPicDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_PATH, &CDownloadPicDlg::OnBnClickedBtnPath)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PIC_NAME, &CDownloadPicDlg::OnLvnItemchangedListPicName)
	ON_BN_CLICKED(IDC_BTN_REG, &CDownloadPicDlg::OnBnClickedBtnReg)
END_MESSAGE_MAP()


// CDownloadPicDlg 消息处理程序

BOOL CDownloadPicDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CheckReg();

	m_ctrlComboImageType.InsertString(0, _T("美女"));
	m_ctrlComboImageType.InsertString(1, _T("爱情"));
	m_ctrlComboImageType.InsertString(2, _T("风景"));
	m_ctrlComboImageType.InsertString(3, _T("小清新"));
	m_ctrlComboImageType.InsertString(4, _T("动漫"));
	m_ctrlComboImageType.InsertString(5, _T("明星"));
	m_ctrlComboImageType.InsertString(6, _T("萌宠"));
	m_ctrlComboImageType.InsertString(7, _T("游戏"));
	m_ctrlComboImageType.InsertString(8, _T("汽车"));
	m_ctrlComboImageType.SetCurSel(0);

	m_ctrlComboImageType.SetItemData(0, 6);
	m_ctrlComboImageType.SetItemData(1, 30);
	m_ctrlComboImageType.SetItemData(2, 9);
	m_ctrlComboImageType.SetItemData(3, 15);
	m_ctrlComboImageType.SetItemData(4, 26);
	m_ctrlComboImageType.SetItemData(5, 11);
	m_ctrlComboImageType.SetItemData(6, 14);
	m_ctrlComboImageType.SetItemData(7, 5);
	m_ctrlComboImageType.SetItemData(8, 12);



	m_ctrlComboImageSize.InsertString(0, _T("1024_768"));
	m_ctrlComboImageSize.InsertString(1, _T("1280_1024"));
	m_ctrlComboImageSize.InsertString(2, _T("1280_800"));
	m_ctrlComboImageSize.InsertString(3, _T("1366_768"));
	m_ctrlComboImageSize.InsertString(4, _T("1400_900"));
	m_ctrlComboImageSize.InsertString(5, _T("1600_900"));
	m_ctrlComboImageSize.SetCurSel(0);

	CRect rectList;
	m_ctrlPicName.GetWindowRect(&rectList);
	int nWidth = rectList.Width() - 22;
	m_ctrlPicName.InsertColumn(0, _T("图片名称"), 0, nWidth);
	m_ctrlPicName.InsertColumn(1, _T("type"), 0, 0);
	m_ctrlPicName.SetExtendedStyle(m_ctrlPicName.GetExtendedStyle() | LVS_EX_FULLROWSELECT);


	UpdateData(FALSE);
	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDownloadPicDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDownloadPicDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDownloadPicDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDownloadPicDlg::OnAfterRequestSend( FCHttpRequest& rTask )
{

}

void CDownloadPicDlg::OnAfterRequestFinish( FCHttpRequest& rTask )
{
	rTask.PopReceived (m_receive) ;

	if(libjson::is_valid(m_receive)==false)  
	{
		GetDlgItem(IDC_BTN_GETFILE)->EnableWindow(TRUE);

		return;
	}

	JSONNode rn=libjson::parse(m_receive);

	CString strImageSize = _T("1600_900");
	m_ctrlComboImageSize.GetWindowText(strImageSize);
	strImageSize = _T("img_") + strImageSize;

	USES_CONVERSION;

	int nSize = rn.size();
	for (int i=0;i<rn.size();i++)
	{
		JSONNode temp=rn[i];

		CString strName = A2T(temp.name().c_str());

		if (strName.Compare(_T("data")) == 0)
		{
			for(int j=0;j<temp.size();j++)
			{
				JSONNode nodeData=temp[j];
				for(int nIdx = 0;nIdx < nodeData.size();nIdx++)
				{
					CString strSubName = A2T(nodeData[nIdx].name().c_str());
					if (strSubName.CompareNoCase(strImageSize) == 0)
					{
						CString strUrl =  A2T(nodeData[nIdx].as_string().c_str());

						m_vectUrl.push_back(strUrl);
					}
				}
			}

			break;
		}
	}
	CString strType;
	m_ctrlComboImageType.GetWindowText(strType);

	CString strRootPath = CFFileOperator::CombinePath(m_strImageSavePath, strType);
	if (CFFileOperator::IsFileExists(strRootPath) == false)
	{
		CFFileOperator::CreateDirectories(strRootPath);
	}

	for (int i = 0; i < m_vectUrl.size(); i++)
	{
		CHttpGetFile httpFile;
		CString strUrl = m_vectUrl[i];

		CString strName;
		strName.Format(_T("Image_%d_%d.jpg"), m_nStart, m_nImageIdx);
		CString strPath = CFFileOperator::CombinePath(strRootPath, strName);

		httpFile.GetHttpFile(strUrl, strPath);

		m_nImageIdx++;

		int nIdx = m_ctrlPicName.GetItemCount();
		m_ctrlPicName.InsertItem(nIdx, strName);

		m_strInfo.Format(_T("已下载%d个图片"), m_nImageIdx);
		UpdateData(FALSE);
	}

	if (m_vectUrl.size() > 0 && m_nImageIdx < m_nDownImageCount)
	{
		m_nStart += 50;
		m_vectUrl.clear();
		DownloadImage();
	}
	else
	{
		GetDlgItem(IDC_BTN_GETFILE)->EnableWindow(TRUE);
		m_nImageIdx = 0;
		m_nStart = -1;
		m_vectUrl.clear();
	}

}
void CDownloadPicDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	DownloadImage();

	GetDlgItem(IDC_BTN_GETFILE)->EnableWindow(FALSE);
	m_ctrlPicName.DeleteAllItems();
}


void CDownloadPicDlg::DownloadImage()
{
	int nCount = min(m_nDownImageCount, 50);
	if (m_nStart == -1)
	{
		m_nStart = m_nDownImageStartIndex;
	}

	int nType = (int)m_ctrlComboImageType.GetItemData(m_ctrlComboImageType.GetCurSel());

	m_header.m_url.Format(_T("http://wallpaper.apc.360.cn/index.php?c=WallPaper&a=getAppsByCategory&from=360desktop&cid=%d&start=%d&count=%d"), nType, m_nStart, nCount);
	if (!UrlIs(m_header.m_url, URLIS_URL))
	{

		return ;
	}

	m_task_id = AddRequest(m_header) ;
	m_receive.clear() ;
}

void CDownloadPicDlg::OnBnClickedBtnPath()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	LPCTSTR lpszTitle = _T( "请选择图片保存文件夹:" );
	UINT	uFlags	  = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;

	CFolderDialog dlg( lpszTitle, m_strImageSavePath, this, uFlags);

	if( dlg.DoModal() == IDOK )
	{
		m_strImageSavePath = dlg.GetFolderPath();
	}	

	UpdateData(FALSE);

}

void CDownloadPicDlg::OnLvnItemchangedListPicName(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
 
	if(LVIF_STATE == pNMLV->uChanged && (pNMLV->uNewState & LVIS_SELECTED))  
	{  
		int nItem = pNMLV->iItem;  
		if(-1 != nItem)  
		{  
			CString strName = m_ctrlPicName.GetItemText(nItem, 0);
			CString strType;
			m_ctrlComboImageType.GetWindowText(strType);

			CString strRootPath = CFFileOperator::CombinePath(m_strImageSavePath, strType);
			CString strPath = CFFileOperator::CombinePath(strRootPath, strName);

			m_ctrlPicView.SetFileName(strPath);
		}  
	}  
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDownloadPicDlg::OnBnClickedBtnReg()
{
	// TODO: Add your control notification handler code here
	CDlgReg dlg;
	dlg.DoModal();

	CheckReg();
}

void CDownloadPicDlg::CheckReg()
{
	CString strTitle = __TITLE;
	if (theApp.IsReged() == FALSE)
	{
		strTitle += _T("（试用版）");

		CWnd* pWnd = GetDlgItem(IDC_BTN_REG);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_SHOW);
		}

		pWnd = GetDlgItem(IDC_EDIT_IMAGE_COUNT);
		if (pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}

		pWnd = GetDlgItem(IDC_EDIT_IMAGE_START_INDEX);
		if (pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
	}
	else
	{
		CWnd* pWnd = GetDlgItem(IDC_BTN_REG);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
		}

		pWnd = GetDlgItem(IDC_EDIT_IMAGE_COUNT);
		if (pWnd)
		{
			pWnd->EnableWindow(TRUE);
		}

		pWnd = GetDlgItem(IDC_EDIT_IMAGE_START_INDEX);
		if (pWnd)
		{
			pWnd->EnableWindow(TRUE);
		}
	}

	SetWindowText(strTitle);
}
