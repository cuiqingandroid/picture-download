// MiImageList.cpp : implementation file
//

#include "stdafx.h"
#include "MiImageList.h"


// CMiImageList

CMiImageList::CMiImageList()
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup( &m_gdiplusToken, &gdiplusStartupInput, NULL );
}

CMiImageList::~CMiImageList()
{
	GdiplusShutdown(m_gdiplusToken);
}


// CMiImageList member functions
BOOL CMiImageList::Create(LPCTSTR lpszImageID, int cx, LPCTSTR lpszType, HMODULE hModule)
{
	BOOL ret = FALSE;
	Bitmap *pBitmap = NULL;

	HRSRC hResource = ::FindResource(hModule, lpszImageID, lpszType);
	if (!hResource)
	{
		return false;
	}

	DWORD imageSize = ::SizeofResource(hModule, hResource);
	if (!imageSize)
	{
		return false;
	}

	const void* pResourceData = ::LockResource(::LoadResource(hModule, hResource));
	if (!pResourceData)
	{
		return false;
	}

	HGLOBAL hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (hBuffer)
	{
		void* pBuffer = ::GlobalLock(hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);

			IStream* pStream = NULL;
			if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
			{
				pBitmap = Gdiplus::Bitmap::FromStream(pStream);
				pStream->Release();
			}
			else
			{
				pBitmap = NULL;
			}
			::GlobalUnlock(hBuffer);
		}
		::GlobalFree(hBuffer);
	}

	if (pBitmap == NULL)
	{
		return ret;
	}

	UINT width = pBitmap->GetWidth();
	UINT height = pBitmap->GetHeight();


	BITMAPINFO bi = {0};
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = width;
	bi.bmiHeader.biHeight = -height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = 0;
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	Rect rect(0, 0, width, height);
	BitmapData* bitmapData = new BitmapData;
	pBitmap->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, bitmapData);
	UINT* pixels = (UINT*)bitmapData->Scan0;
	LPBYTE buffer;
	HWND hwndDesktop = GetDesktopWindow();
	HDC hdcDesktop = GetDC(hwndDesktop);
	HBITMAP hbmImage = CreateDIBSection(hdcDesktop, &bi, DIB_RGB_COLORS, (void **)&buffer, NULL, 0);
	memcpy(buffer, pixels, width * height * 4);
	ReleaseDC(hwndDesktop, hdcDesktop);
	if (hbmImage)
	{
		ret = CImageList::Create(cx, height, ILC_COLOR32, 0, 1);
		ImageList_Add(m_hImageList, hbmImage, NULL);
		DeleteObject(hbmImage);
	}
	pBitmap->UnlockBits(bitmapData);
	delete bitmapData;

	return ret;
}