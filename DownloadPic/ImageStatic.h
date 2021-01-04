#pragma once


// CImageStatic

class CImageStatic : public CStatic
{
	DECLARE_DYNAMIC(CImageStatic)

public:
	CImageStatic();
	virtual ~CImageStatic();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void SetFileName(const CString& strFileName);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
	void CreateMemeryDC();
	void ClearMemeryDC();


private:
	CDC						m_MemDC;
	CBitmap					m_MemBmp;
	CBitmap*				m_pMemBmpOld;
	CRect					m_rectClient;
protected:
	virtual void PreSubclassWindow();
};


