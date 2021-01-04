
//#define _CRTDBG_MAP_ALLOC

#include "stdafx.h"


#include "URLEncDec.h"

//#define MAX_BUFFER_SIZE 4096

/*
#include <stdlib.h>
#include <crtdbg.h>
*/



namespace Internet
{

	// HEX Values array
	char hexVals[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	// UNSAFE String
	CStringA CURLEncDec::csUnsafeString= " \"<>%\\^[]`+$,@:;/!#?=&";

	// PURPOSE OF THIS FUNCTION IS TO CONVERT A GIVEN CHAR TO URL HEX FORM
	inline CStringA CURLEncDec::Convert(const char val) 
	{
		CStringA csRet;
		csRet += "%";
		csRet += DecToHex(val, 16);	
		return  csRet;
	}

	// THIS IS A HELPER FUNCTION.
	// PURPOSE OF THIS FUNCTION IS TO GENERATE A HEX REPRESENTATION OF GIVEN CHARACTER
	CStringA CURLEncDec::DecToHex(const char num, int radix)
	{	
		int temp=0;	
		CStringA csTmp;
		unsigned int num_char = (unsigned int) ((unsigned char)num);

		/*
		// ISO-8859-1 
		// IF THE IF LOOP IS COMMENTED, THE CODE WILL FAIL TO GENERATE A 
		// PROPER URL ENCODE FOR THE CHARACTERS WHOSE RANGE IN 127-255(DECIMAL)
		if (num_char < 0)	// that is, the unsigned char value greater than 127
		num_char = 256 + num_char;
		*/

		while (num_char >= radix)
		{
			temp = num_char % radix;
			num_char = (unsigned int) ((num_char-temp) / radix);
			csTmp += hexVals[temp];
		}

		csTmp += hexVals[num_char];

		if(csTmp.GetLength() < 2)
		{
			csTmp += '0';
		}

		/*
		CStringA strdecToHex(csTmp);
		// Reverse the String
		strdecToHex.MakeReverse(); 
		*/

		csTmp.MakeReverse();
		return csTmp;
	}

	// PURPOSE OF THIS FUNCTION IS TO CHECK TO SEE IF A CHAR IS URL UNSAFE.
	// TRUE = UNSAFE, FALSE = SAFE
	bool CURLEncDec::IsUnsafe(const char compareChar)
	{
		// this check will bost performance if there are a lot of non-ascii char, e.g. UTF-8 char
		unsigned int char_ascii_value = (unsigned int)((unsigned char) compareChar);
		if (char_ascii_value <= 31 || char_ascii_value >= 123)
			return true;

		return false;

		//
		// now : char_ascii_value > 31 && char_ascii_value < 123
		//

		bool bcharfound = false;
		char tmpsafeChar;
		int m_strLen = 0;

		m_strLen = csUnsafeString.GetLength();
		for(int ichar_pos = 0; ichar_pos < m_strLen ;ichar_pos++)
		{
			tmpsafeChar = csUnsafeString.GetAt(ichar_pos); 
			if(tmpsafeChar == compareChar)
			{ 
				bcharfound = true;
				break;
			} 
		}

		return bcharfound;
	}
	// PURPOSE OF THIS FUNCTION IS TO CONVERT A STRING 
	// TO URL ENCODE FORM.
	CStringA CURLEncDec::URLEncodeA(const char* lpszCharStream)
	{	
		CStringA csEncoded;	

		while (*lpszCharStream)
		{
			unsigned char ch = (unsigned char) (*lpszCharStream);
			if(IsUnsafe((char)ch))
				// get Hex Value of the Character
				csEncoded += Convert((char)ch);
			else
				// Safe Character				
				csEncoded += (char)ch;

			lpszCharStream++;
		}

		return csEncoded;
	}


	inline char CURLEncDec::Hex2Ch (const char* pszHex)
	{
		unsigned char digit;
		unsigned char* pIn = (unsigned char*) pszHex; 

		//digit += ((*pIn) >= 'A' ? (((*pIn) & 0xdf) - 'A')+10 : ((*pIn) - '0'));
		digit = ( (*pIn) >= 'A' ? ( (*pIn) - 'A')+10 : ((*pIn) - '0'));
		digit *= 16;

		pIn++;

		//digit += ((*pIn) >= 'A' ? (((*pIn) & 0xdf) - 'A')+10 : ((*pIn) - '0'));
		digit += ((*pIn) >= 'A' ? ((*pIn) - 'A')+10 : ((*pIn) - '0'));

		return digit;
	}

	/*
	The real question now is where you have your
	URLencoded string from. Could it be encoded in
	UTF-8, as URL string nowadays tend to, e.g. if send from browser to server? 

	Hope your App is UTF-8 aware ;) 
	*/
	CStringA CURLEncDec::URLDecodeA(const char* lpszCharStream)
	{
		CStringA sRet, sRetEmpty;

		int nLen = strlen(lpszCharStream) + 1;
		LPSTR lpszBuf = sRet.GetBuffer(nLen);
		::ZeroMemory(lpszBuf, nLen);

		while( *lpszCharStream )
		{
			if ( *lpszCharStream == '%' )
			{
				lpszCharStream ++;

				if (0 == *lpszCharStream ||
					0 == *(lpszCharStream+1) )
				{
					sRet.ReleaseBuffer();
					return sRetEmpty;	// there must be some error in input char stream
				}

				(*lpszBuf) = Hex2Ch(lpszCharStream);
				lpszCharStream ++;
			}
			else if (*lpszCharStream == '+')	// space char maybe replaced to '+'
				(*lpszBuf) = ' ';
			else
				(*lpszBuf) = (*lpszCharStream);

			lpszCharStream ++;
			lpszBuf ++;
		}

		sRet.ReleaseBuffer();
		return sRet;
	}

	CString CURLEncDec::UTF8ToUnicode(LPCSTR szText)
	{
		int size = MultiByteToWideChar(
			CP_UTF8,
			MB_ERR_INVALID_CHARS,
			szText,
			-1,
			NULL,
			0);
		CString rText;

		if (size > 0)
		{
			TCHAR *buffer = (TCHAR *)malloc(size * sizeof(TCHAR));
			if (MultiByteToWideChar(
				CP_UTF8,
				MB_ERR_INVALID_CHARS,
				szText,
				-1,
				buffer,
				size) > 0)
			{
				rText = buffer;
			}

			free(buffer);
		}

		return rText;
	}

	CStringA CURLEncDec::UnicodeToUTF8(LPCWSTR szText)
	{
		int size = WideCharToMultiByte(
			CP_UTF8,
			0,
			szText,
			-1,
			NULL,
			0,
			NULL,
			NULL);

		CStringA rText;
		if (size > 0)
		{
			char *buffer = (char *)malloc(size);
			if (WideCharToMultiByte(
				CP_UTF8,
				0,
				szText,
				-1,
				buffer,
				size,
				NULL,
				NULL) > 0)
			{
				rText = buffer;
			}

			free(buffer);
		}

		return rText;
	}


	CString CURLEncDec::URLEncodeToUTF8(const TCHAR* lpszCharStream)
	{	
		CStringA strIn;
		CStringA strOut;

		if (sizeof(TCHAR) == 1)
		{
			strIn = lpszCharStream;
		}
		else
		{
			strIn = UnicodeToUTF8(lpszCharStream);
		}

		strOut = URLEncodeA(strIn);


		if (sizeof(TCHAR) == 1)
		{
			return CString(strOut);
		}
		else
		{
			return UTF8ToUnicode(strOut);
		}
	}


	CString CURLEncDec::URLDecodeFromUTF8(const TCHAR* lpszCharStream)
	{
		CStringA strIn;
		CStringA strOut;

		if (sizeof(TCHAR) == 1)
		{
			strIn = lpszCharStream;
		}
		else
		{
			strIn = UnicodeToUTF8(lpszCharStream);
		}

		strOut = URLDecodeA(strIn);


		if (sizeof(TCHAR) == 1)
		{
			return CString(strOut);
		}
		else
		{
			return UTF8ToUnicode(strOut);
		}
	}


}