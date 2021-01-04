
#ifndef __CURLENCDEC_H_
#define __CURLENCDEC_H_

// Does it need to support wide char string input?????
namespace Internet
{

	class CURLEncDec
	{
	private:
		static CStringA csUnsafeString;	// URL encoding operating on bytes stream
		static CStringA DecToHex(const char num, int radix); // URL encoding operating on bytes stream
		static bool IsUnsafe(const char compareChar);
		static CStringA Convert(const char val); // URL encoding operating on bytes stream

		// helper to convert %XX to chars
		static char Hex2Ch (const char* pszHex);

		// can't construct a object
		CURLEncDec() { };
		virtual ~CURLEncDec() { };

		// Those need URL encoding string must either MBCS or UTF-8 char stream, and encoded result must be a ascii char stream
		static CStringA URLEncodeA(const char* lpszCharStream); 
		static CStringA URLDecodeA(const char* lpszCharStream);


	public:
		static CString UTF8ToUnicode(LPCSTR szText);
		static CStringA UnicodeToUTF8(LPCWSTR szText);

		static CString URLEncodeToUTF8(const TCHAR* lpszCharStream); 
		static CString URLDecodeFromUTF8(const TCHAR* lpszCharStream);

	};

};

#endif //__CURLENCODE_H_