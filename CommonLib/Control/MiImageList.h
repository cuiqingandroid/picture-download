#pragma once

// CMiImageList command target
#include <gdiplus.h>

using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")

class CMiImageList : public CImageList
{
	ULONG_PTR						m_gdiplusToken;
public:
	BOOL Create(LPCTSTR lpszImageID, int cx, LPCTSTR lpszType, HMODULE hModule = NULL);

public:
	CMiImageList();
	virtual ~CMiImageList();
};


