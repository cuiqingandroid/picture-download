// ImageStatic.cpp : implementation file
//

#include "stdafx.h"
#include "DownloadPic.h"
#include "ImageStatic.h"
#include "../CommonLib/File/FFileOperator.h"


// CImageStatic

IMPLEMENT_DYNAMIC(CImageStatic, CStatic)

CImageStatic::CImageStatic()
{
	m_pMemBmpOld  = NULL;
}

CImageStatic::~CImageStatic()
{
}


BEGIN_MESSAGE_MAP(CImageStatic, CStatic)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CImageStatic message handlers



void CImageStatic::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	CreateMemeryDC();

	// TODO: Add your message handler code here
}

void CImageStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	dc.BitBlt(
		0,
		0,
		m_rectClient.Width(),
		m_rectClient.Height(),
		&m_MemDC,
		0,
		0,
		SRCCOPY
		);

}

BOOL CImageStatic::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}

void CImageStatic::CreateMemeryDC()
{
	if (::IsWindow(CWnd::m_hWnd)) {
		ClearMemeryDC();

		CDC *pDC = GetDC();

		GetClientRect(&m_rectClient);

		m_MemDC.CreateCompatibleDC(pDC);
		m_MemBmp.CreateCompatibleBitmap(pDC, m_rectClient.Width(), m_rectClient.Height());
		m_pMemBmpOld = m_MemDC.SelectObject(&m_MemBmp);

		m_MemDC.FillSolidRect(&m_rectClient, RGB(128, 128, 128));

		if(pDC != NULL) {
			ReleaseDC(pDC);
			pDC = NULL;
		}
	}

}

void CImageStatic::ClearMemeryDC()
{
	if (m_MemDC.GetSafeHdc()) {
		if (m_pMemBmpOld != NULL) {
			m_MemDC.SelectObject(m_pMemBmpOld);
			m_pMemBmpOld = NULL;
		}

		m_MemBmp.DeleteObject();
		m_MemDC.DeleteDC();
	}
}

void CImageStatic::SetFileName( const CString& strFileName )
{
	if (CFFileOperator::IsFileExists(strFileName) == false)
	{
		return;
	}

	Bitmap* pBitmap = Bitmap::FromFile(strFileName);
	if (pBitmap == NULL)
	{
		return;
	}

	int nImageWidth = pBitmap->GetWidth();
	int nImageHeight = pBitmap->GetHeight();

	double dblScal = (double)nImageWidth / nImageHeight;

	int nRectWidth = nImageWidth;
	int nRectHeight = nImageHeight;


	if (nRectWidth > m_rectClient.Width() || nRectHeight > m_rectClient.Height())
	{
		if ((m_rectClient.Width()) * dblScal > (m_rectClient.Height()))
		{
			nRectHeight =m_rectClient.Height();
			nRectWidth = (int)(nRectHeight * dblScal);
		}
		else
		{
			nRectWidth = m_rectClient.Width();
			nRectHeight = (int)(nRectWidth / dblScal);
		}
	}


	int nStartX = m_rectClient.left + (m_rectClient.Width() - nRectWidth) / 2;
	if (nStartX < m_rectClient.left)
	{
		nStartX = m_rectClient.left;

		nRectWidth = m_rectClient.Width();
	}

	int nStartY = m_rectClient.top + (m_rectClient.Height() - nRectHeight) / 2;
	if (nStartY < m_rectClient.top)
	{
		nStartY = m_rectClient.top;

		nRectHeight = m_rectClient.Height();
	}

	m_MemDC.FillSolidRect(&m_rectClient, RGB(128, 128, 128));


	RectF rfDest(PointF((REAL)nStartX + 2, (REAL)nStartY + 2), SizeF((REAL)(nRectWidth - 4), (REAL)(nRectHeight - 4)));

	Graphics graph(m_MemDC.GetSafeHdc());
	graph.DrawImage(pBitmap, rfDest, (REAL)0, (REAL)0, (REAL)nImageWidth, (REAL)nImageHeight, UnitPixel, NULL);

	Invalidate();

	delete pBitmap;

}

void CImageStatic::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CStatic::PreSubclassWindow();

	CreateMemeryDC();
}
