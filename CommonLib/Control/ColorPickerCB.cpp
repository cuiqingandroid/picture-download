/*|*\
|*|  File:      FocusEditCtrl.h
|*|  
|*|  By:        James R. Twine
|*|             Copyright 1998, James R. Twine
|*|             Copyright 1999-2000, TransactionWorks, Inc.
|*|  Date:      xxxx, xxxx, 1998
|*|             
|*|             This implementes a ComboBox control that can be
|*|             used to display and provide selection for a specific
|*|             set of colors.  The standard set of colors provided
|*|             by the control are a subset of the X11 colorset, and
|*|             are the ones available in (and the color names 
|*|				recognized by) Internet Explorer.
|*|             
|*|				The ComboBox that is subclassed must have the 
|*|             Owner Draw Fixed and Has Strings styles.
|*|             
|*|             This is based on code that was originally found on
|*|             CodeGuru, and was ?1997 Baldvin Hansson.
|*|             
|*|             A bug fix by Marcel Galem was added to provide
|*|             correct masking of the COLORREF values.
|*|             
|*|             DDX routines were provided by Paul Wardle
|*|             (paul.wardle@siemenscomms.co.uk)
|*|             
|*|             This Code May Be Freely Incorporated Into 
|*|             Projects Of Any Type Subject To The Following 
|*|             Conditions:
|*|             
|*|             o This Header Must Remain In This File, And Any
|*|               Files Derived From It
|*|             o Do Not Misrepresent The Origin Of Any Parts Of 
|*|               This Code (IOW, Do Not Claim You Wrote It)
|*|             
|*|             A "Mention In The Credits", Or Similar Acknowledgement,
|*|             Is *NOT* Required.  It Would Be Nice, Though! :)
|*|	
\*|*/
#pragma warning (disable : 4127)	// conditional expression is constant

#include "stdafx.h"
#include "ColorPickerCB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Uncomment This To Help Debugging The Color Values...
//
//#define		_INCLUDE_COLOR_INFO


//
//	Load/Create Standard Colors...
//
SColorAndName	CColorPickerCB::ms_pColors[] =
{
	//
	//	Standard IE Colors (Subset Of X11 Colorset)...
	//
	SColorAndName( RGB( 0x88, 0x88, 0x88 ), _T( "随机" ) ),
	SColorAndName( RGB( 0xFF, 0xFF, 0xFF ), _T( "白色" ) ),
	SColorAndName( RGB( 0xFF, 0x00, 0x00 ), _T( "红色" ) ),
	SColorAndName( RGB( 0x00, 0x00, 0xFF ), _T( "蓝色" ) ),
	SColorAndName( RGB( 0xAD, 0xD8, 0xE6 ), _T( "淡蓝色" )),
	SColorAndName( RGB( 0x80, 0x00, 0x80 ), _T( "紫色" ) ),
	SColorAndName( RGB( 0xFF, 0xFF, 0x00 ), _T( "黄色" ) ),
	SColorAndName( RGB( 0xFF, 0xA5, 0x00 ), _T( "橘色" ) ),
	SColorAndName( RGB( 0x00, 0xFF, 0x00 ), _T( "绿色" ) ),
	SColorAndName( RGB( 0xFF, 0xC0, 0xCB ), _T( "粉色" ) ),
	SColorAndName( RGB( 0x00, 0x00, 0x8B ), _T( "深蓝色")),
	SColorAndName( RGB( 0x80, 0x80, 0x80 ), _T( "灰色" ) ),
	SColorAndName( RGB( 0x00, 0x64, 0x00 ), _T( "深绿色" )),
	SColorAndName( RGB( 0xA5, 0x2A, 0x2A ), _T( "褐色" ) ),
	SColorAndName( RGB( 0x90, 0xEE, 0x90 ), _T( "淡绿色" )),
	SColorAndName( RGB( 0x00, 0x00, 0x00 ), _T( "黑色" )),
	SColorAndName( RGB( 0xCB, 0xA5, 0xFB ), _T( "淡紫色" ) )
};


CColorPickerCB::CColorPickerCB()
{
	return;													// Done!
}


CColorPickerCB::~CColorPickerCB()
{
	return;													// Done!
}


BEGIN_MESSAGE_MAP(CColorPickerCB, CComboBox)
	//{{AFX_MSG_MAP(CColorPickerCB)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPickerCB message handlers

int CColorPickerCB::OnCreate( LPCREATESTRUCT pCStruct ) 
{
	if( CComboBox::OnCreate( pCStruct ) == -1 )				// If Create Failed
	{
		return( -1 );										// Return Failure
	}
	_ASSERTE( GetStyle() & CBS_OWNERDRAWFIXED );			// Assert Proper Style Set
//	_ASSERTE( GetStyle() & CBS_OWNERDRAWVARIABLE );			// Assert Proper Style Set
	_ASSERTE( GetStyle() & CBS_DROPDOWNLIST );				// Assert Proper Style Set
	_ASSERTE( GetStyle() & CBS_HASSTRINGS );				// Assert Proper Style Set

	return( 0 );											// Done!
}


void CColorPickerCB::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();							// Subclass Control

	_ASSERTE( GetStyle() & CBS_OWNERDRAWFIXED );			// Assert Proper Style Set
//	_ASSERTE( GetStyle() & CBS_OWNERDRAWVARIABLE );			// Assert Proper Style Set
	_ASSERTE( GetStyle() & CBS_DROPDOWNLIST );				// Assert Proper Style Set
	_ASSERTE( GetStyle() & CBS_HASSTRINGS );				// Assert Proper Style Set

	CComboBox::PreSubclassWindow();
	if (GetWindowLong(m_hWnd, GWL_STYLE) & CBS_HASSTRINGS)
	{
		if (GetWindowLong(m_hWnd, GWL_STYLE) & CBS_OWNERDRAWFIXED)
		{
			::SendMessage(m_hWnd, CB_SETITEMHEIGHT, (WPARAM)-1, 15L);
		}
	}

	return;													// Done!
}


void	CColorPickerCB::InitializeDefaultColors( void )
{
	_ASSERTE( m_hWnd );										// We Must Be Created First...

#if	defined( _INCLUDE_COLOR_INFO )
	COLORREF	crColor;
	TCHAR		caColor[ 256 ];
#endif
	int			iAddedItem = -1;
	int			iColors = COUNTOF( ms_pColors );			// Get Number Of Colors Set

	ResetContent();											// Clear All Colors

#if	defined( _INCLUDE_COLOR_INFO )
	SetDroppedWidth( 400 );									// Increase Drop Width
#endif

	for( int iColor = 0; iColor < iColors; iColor++ )		// For All Colors
	{
#if	defined( _INCLUDE_COLOR_INFO )
		crColor = ms_pColors[ iColor ].m_crColor;			// Store The Color Value

		_ASSERTE( _stprintf( caColor, 
				_T( "%s\t\t%02X%02X%02X" ), ms_pColors[ 
				iColor ].m_cColor, GetRValue( crColor ), 
				GetGValue( crColor ), GetBValue( crColor ) ) 
				 < 255 );									// Build The Debug String
		iAddedItem = AddString(	caColor );					// Set Color Name/Text
#else
		iAddedItem = AddString(	ms_pColors[
					iColor ].m_cColor );					// Set Color Name/Text
#endif
		if( iAddedItem == CB_ERRSPACE )						// If Not Added
		{
#pragma warning (disable : 4127)
			ASSERT( FALSE );								// Let 'Em Know What Happened...
#pragma warning (default : 4127)
			break;											// Stop
		}
		else												// If Added Successfully
		{
			SetItemData( iAddedItem, ms_pColors[
					iColor ].m_crColor );					// Set Color RGB Value
		}
	}
	return;													// Done!
}


void	CColorPickerCB::OnLButtonDown( UINT nFlags, CPoint ptPoint ) 
{
	if( GetFocus() != this )								// If Not Focused
	{
		SetFocus();											// Gain Focus
	}
	CComboBox::OnLButtonDown( nFlags, ptPoint );			// Do Default

	return;													// Done!
}


void	CColorPickerCB::DrawItem( LPDRAWITEMSTRUCT pDIStruct )
{
	COLORREF	crColor = 0;
	COLORREF	crNormal = GetSysColor( COLOR_WINDOW );
	COLORREF	crSelected = GetSysColor( COLOR_HIGHLIGHT );
	COLORREF	crText = GetSysColor( COLOR_WINDOWTEXT );
	CBrush		brFrameBrush;
	TCHAR		cColor[ CCB_MAX_COLOR_NAME ];				// Color Name Buffer
	CRect		rItemRect( pDIStruct -> rcItem );
	CRect		rBlockRect( rItemRect );
	CRect		rTextRect( rBlockRect );
	CDC			dcContext;
	int			iFourthWidth = 0;
	int			iItem = pDIStruct -> itemID;
	int			iState = pDIStruct -> itemState;

	if (pDIStruct->itemID != 0)
	{
		int test = 0;
	}

	if( !dcContext.Attach( pDIStruct -> hDC ) )				// Attach CDC Object
	{
		return;												// Stop If Attach Failed
	}
	iFourthWidth = ( rBlockRect.Width() / 4 );				// Get 1/4 Of Item Area
	brFrameBrush.CreateStockObject( BLACK_BRUSH );			// Create Black Brush

	if( iState & ODS_SELECTED )								// If Selected
	{														// Set Selected Attributes
		dcContext.SetTextColor( ( 0x00FFFFFF & ~( 
				crText ) ) );								// Set Inverted Text Color (With Mask)
		dcContext.SetBkColor( crSelected );					// Set BG To Highlight Color
		dcContext.FillSolidRect( &rBlockRect, crSelected );	// Erase Item
	}
	else													// If Not Selected
	{														// Set Standard Attributes
		dcContext.SetTextColor( crText );					// Set Text Color
		dcContext.SetBkColor( crNormal );					// Set BG Color
		dcContext.FillSolidRect( &rBlockRect, crNormal );	// Erase Item
	}
	if( iState & ODS_FOCUS )								// If Item Has The Focus
	{
		dcContext.DrawFocusRect( &rItemRect );				// Draw Focus Rect
	}
	//
	//	Calculate Text Area...
	//
	rTextRect.left += ( iFourthWidth + 2 );					// Set Start Of Text
	rTextRect.top += 2;										// Offset A Bit

	//
	//	Calculate Color Block Area..
	//
	rBlockRect.DeflateRect( CSize( 2, 2 ) );				// Reduce Color Block Size
	rBlockRect.right = iFourthWidth;						// Set Width Of Color Block

	//
	//	Draw Color Text And Block...
	//
	if( iItem != -1 )										// If Not An Empty Item
	{
		int		iChars = GetLBText( iItem, cColor );		// Get Color Text
		int		iaTabStops[ 1 ] = { 50 };

		_ASSERTE( iChars != LB_ERR );						// Sanity Check

		if( iState & ODS_DISABLED )							// If Disabled
		{
			crColor = ::GetSysColor( COLOR_GRAYTEXT );		// Get Inactive Text Color
			dcContext.SetTextColor( crColor );				// Set Text Color
		}
		else												// If Normal
		{
			crColor = GetItemData( iItem );					// Get Color Value
		}
		dcContext.SetBkMode( TRANSPARENT );					// Transparent Background
		dcContext.TabbedTextOut( rTextRect.left, 
				rTextRect.top - 1, cColor, iChars, 1, 
				iaTabStops, 0 );							// Draw Color Name

		if (iItem == 0)
		{
			double nWidth = ((double)rBlockRect.Width() / 4);
			double nHeight = ((double)rBlockRect.Height() / 4);

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					CRect rectTemp;
					rectTemp.left = rBlockRect.left + i * nWidth + 0.5;
					rectTemp.top = rBlockRect.top + j * nHeight + 0.5;
					rectTemp.right = rBlockRect.left + (i + 1) * nWidth + 0.5;
					rectTemp.bottom = rBlockRect.top + (j + 1) * nHeight + 0.5;

					if (i == 3)
					{
						rectTemp.right = rBlockRect.right;
					}

					if (j == 3)
					{
						rectTemp.bottom = rBlockRect.bottom;
					}
					COLORREF colorTemp = ms_pColors[i * 4 + j + 1].m_crColor;
					dcContext.FillSolidRect( &rectTemp, colorTemp );
				}

			}
		}
		else
		{
			dcContext.FillSolidRect( &rBlockRect, crColor );
		}
			// Draw Color
				
		dcContext.FrameRect( &rBlockRect, &brFrameBrush );	// Draw Frame
	}
	dcContext.Detach();										// Detach DC From Object
	
	return;													// Done!
}


COLORREF	CColorPickerCB::GetSelectedColorValue( void )
{
	int		iSelectedItem = GetCurSel();					// Get Selected Item

	if( iSelectedItem == CB_ERR )							// If Nothing Selected
	{
		return( RGB( 0, 0, 0 ) );							// Return Black
	}
	return( GetItemData( iSelectedItem ) );					// Return Selected Color
}


LPCTSTR	CColorPickerCB::GetSelectedColorName( void )
{
	int		iSelectedItem = GetCurSel();					// Get Selected Item

	if( iSelectedItem != CB_ERR )							// If Something Selected
	{
		GetLBText( iSelectedItem, m_cColorName );			// Store Name Of Color
	}
	else													// If Nothing Selected
	{
		m_cColorName[ 0 ] = _T( '\0' );						// Terminate Color Name Buffer (Zero Length String)
	}
	return( m_cColorName );									// Return Selected Color Name
}


void		CColorPickerCB::SetSelectedColorValue( COLORREF crClr )
{
	int		iItems = GetCount();
	
	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		if( crClr == GetItemData( iItem ) )					// If Match Found
		{
			SetCurSel( iItem );								// Select It
			break;											// Stop Looping
		}
	}
	return;													// Done!
}


void	CColorPickerCB::SetSelectedColorName( LPCTSTR cpColor )
{
	int		iItems = GetCount();
	TCHAR	cColor[ CCB_MAX_COLOR_NAME ];

	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		GetLBText( iItem, cColor );							// Get Color Name
		if( !_tcsicmp( cColor, cpColor ) )					// If Match Found
		{
			SetCurSel( iItem );								// Select It
			break;											// Stop Looping
		}
	}
	return;													// Done!
}


int		CColorPickerCB::AddColor( LPCTSTR cpColor, COLORREF crColor )
{
	int		iIndex = CB_ERR;

	_ASSERTE( cpColor );									// Need This!

#if	defined( _INCLUDE_COLOR_INFO )
	TCHAR	caColor[ 256 ];

	_ASSERTE( _stprintf( caColor, 
			_T( "%s\t\t%02X%02X%02X" ), cpColor, GetRValue( 
				crColor ), GetGValue( crColor ), GetBValue( 
				crColor ) ) < 255 );						// Build The Debug String
		iIndex = AddString(	caColor );						// Set Color Name/Text
#else
		iIndex = AddString(	cpColor );						// Insert Just The Color
#endif
	if( iIndex != CB_ERR )									// If Inserted
	{
		SetItemData( iIndex, (DWORD)crColor );				// Set The Color Value
	}
	return( iIndex );										// Return Insertion Locatiom Or Failure Code
}


bool	CColorPickerCB::RemoveColor( LPCTSTR cpColor )
{
	TCHAR	cColor[ CCB_MAX_COLOR_NAME ];
	bool	bRemoved = false;
	int		iItems = GetCount();

	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		GetLBText( iItem, cColor );							// Get Color Name
		if( !_tcsicmp( cColor, cpColor ) )					// If Match Found
		{
			if( DeleteString( iItem ) != CB_ERR )			// Remove It
			{
				bRemoved = true;							// Set Flag If Removed
				break;										// Stop Checking
			}
		}
	}
	return( bRemoved );										// Done!
}


bool	CColorPickerCB::RemoveColor( COLORREF crClr )
{
	bool	bRemoved = false;
	int		iItems = GetCount();

	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		if( crClr == GetItemData( iItem ) )					// If Desired Color Found
		{
			if( DeleteString( iItem ) != CB_ERR )			// Remove It
			{
				bRemoved = true;							// Set Flag If Removed
				break;										// Stop Checking
			}
		}
	}
	return( bRemoved );										// Done!
}


void	CColorPickerCB::DDX_ColorPicker( CDataExchange *pDX, int iIDC, COLORREF &crColor )
{
	CColorPickerCB	*pPicker = NULL;
	HWND			hWndCtrl = pDX -> PrepareCtrl( iIDC );
	
	_ASSERTE( hWndCtrl );									// (In)Sanity Check

	pPicker = (CColorPickerCB*)CWnd::FromHandle( hWndCtrl );// Get Actual Control
	
	_ASSERTE( pPicker );									// (In)Sanity Check

	if( !( pDX -> m_bSaveAndValidate ) )					// If Setting The Color Value
	{
		pPicker -> SetSelectedColorValue( crColor );		// Set It
	}
	else													// If Getting The Color Value
	{
		crColor = pPicker -> GetSelectedColorValue();		// Get It
	}
	return;													// Done!
}


void	CColorPickerCB::DDX_ColorPicker( CDataExchange *pDX, int iIDC, CString &sName )
{
	CColorPickerCB	*pPicker = NULL;
	HWND			hWndCtrl = pDX -> PrepareCtrl( iIDC );
	
	_ASSERTE( hWndCtrl );									// (In)Sanity Check

	pPicker = (CColorPickerCB*)CWnd::FromHandle( hWndCtrl );// Get Actual Control
	
	_ASSERTE( pPicker );									// (In)Sanity Check

	if( !( pDX -> m_bSaveAndValidate ) )					// If Setting The Color Name
	{
		pPicker -> SetSelectedColorName( sName );			// Set It
	}
	else													// If Getting The Color Name
	{
		sName = pPicker -> GetSelectedColorName();			// Get It
	}
	return;													// Done!
}


#pragma warning (disable : 4018)	// '<':  signed/unsigned mismatch
#pragma warning (disable : 4100)	// unreferenced formal parameter
#pragma warning (disable : 4127)	// conditional expression is constant
#pragma warning (disable : 4244)	// conv from X to Y, possible loss of data
#pragma warning (disable : 4310)	// cast truncates constant value
#pragma warning (disable : 4505)	// X: unreference local function removed
#pragma warning (disable : 4510)	// X: default ctor could not be generated
#pragma warning (disable : 4511)	// X: copy constructor could not be generated
#pragma warning (disable : 4512)	// assignment operator could not be generated
#pragma warning (disable : 4514)	// debug symbol exceeds 255 chars
#pragma warning (disable : 4610)	// union X can never be instantiated
#pragma warning (disable : 4663)	// to explicitly spec class template X use ...
#pragma warning (disable : 4710)	// function 'XXX' not expanded
#pragma	warning	(disable : 4786)	// X: identifier truncated to '255' chars



void CColorPickerCB::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
}

int CColorPickerCB::GetColorCount()
{
	return COUNTOF(ms_pColors);
}
