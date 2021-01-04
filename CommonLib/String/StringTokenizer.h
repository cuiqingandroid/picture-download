#ifndef STRINGTOKENIZER_H
#define STRINGTOKENIZER_H

#include <afx.h>
#include <afxcoll.h>

class CStringTokenizer
{
private:
	void CopyFrom(const CStringTokenizer& src);

	bool IsDelimiter(const _TCHAR chCh);
	bool IsQuoteChar(const _TCHAR chCh);
	int SkipToChar(const int nStartPos, const _TCHAR chCh);
	int SkipToDelimiter(const int nStartPos);

protected:
	CString m_cDelim;
	CString m_cQuoteChar;

	CString m_cStr;
	int m_nStrLen;

	CStringArray m_cTokenArray;

public:
	CStringTokenizer();
	CStringTokenizer(const CString& cStr, const CString& chDelim);
	CStringTokenizer(const CString& cStr, const CString& chDelim, const CString& chQuoteChar);
	CStringTokenizer(const CStringTokenizer& src);
	virtual ~CStringTokenizer();

	CStringTokenizer& operator=(const CStringTokenizer& src);

	void SetDelimiter(const CString& cDelim);
	void SetQuoteChar(const CString& cQuoteChar);
	void Tokenize(const CString& cStr);

	int GetTokenCount() const { return (int)m_cTokenArray.GetSize(); };

	virtual CString GetToken(const int nTokenIndex) const;
};

#endif // STRINGTOKENIZER_H