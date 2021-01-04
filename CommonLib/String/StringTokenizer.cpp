#include "stdafx.h"

#include "StringTokenizer.h"

CStringTokenizer::CStringTokenizer()
	: m_cDelim(_T("")), m_cQuoteChar(_T(""))
{
}

CStringTokenizer::CStringTokenizer(const CString& cStr, const CString& cDelim)
	: m_cDelim(cDelim), m_cQuoteChar(_T(""))
{
	Tokenize(cStr);
}

CStringTokenizer::CStringTokenizer(const CString& cStr, const CString& cDelim, const CString& cQuoteChar)
	: m_cDelim(cDelim), m_cQuoteChar(cQuoteChar)
{
	Tokenize(cStr);
}

CStringTokenizer::CStringTokenizer(const CStringTokenizer& src)
{
	CopyFrom(src);
}

CStringTokenizer::~CStringTokenizer()
{
}

CStringTokenizer& CStringTokenizer::operator=(const CStringTokenizer& src)
{
	if (this == &src) {
		return *this;
	}
	CopyFrom(src);
	return *this;
}

void CStringTokenizer::CopyFrom(const CStringTokenizer& src)
{
	m_cStr = src.m_cStr;
	m_nStrLen = src.m_nStrLen;
	m_cDelim = src.m_cDelim;
	m_cQuoteChar = src.m_cQuoteChar;

	m_cTokenArray.Copy(src.m_cTokenArray);
}

bool CStringTokenizer::IsDelimiter(const _TCHAR chCh)
{
	if (m_cDelim.Find(chCh) == -1) {
		return false;
	}
	return true;
}

bool CStringTokenizer::IsQuoteChar(const _TCHAR chCh)
{
	if (m_cQuoteChar.Find(chCh) == -1) {
		return false;
	}
	return true;
}

int CStringTokenizer::SkipToChar(const int nStartPos, const _TCHAR chCh)
{
	int nPos = nStartPos;
	for (; nPos < m_nStrLen; nPos++) {
		if (m_cStr[nPos] == chCh) {
			return nPos;
		}
	}
	return nPos;
}

int CStringTokenizer::SkipToDelimiter(const int nStartPos)
{
	int nPos = nStartPos;
	for (; nPos < m_nStrLen; nPos++) {
		_TCHAR chChar = m_cStr[nPos];
		if (IsDelimiter(chChar)) {
			return nPos;
		}

		if (IsQuoteChar(chChar)) {		
			nPos++;	
			if (nPos == m_nStrLen) {
				return nPos;
			}
			nPos = SkipToChar(nPos, chChar);
			if (nPos == m_nStrLen) {
				return nPos;
			}
		}
	}
	return nPos;
}

void CStringTokenizer::SetDelimiter(const CString& cDelim)
{
	m_cDelim = cDelim;
}

void CStringTokenizer::SetQuoteChar(const CString& cQuoteChar)
{
	m_cQuoteChar = cQuoteChar;
}

void CStringTokenizer::Tokenize(const CString& cStr)
{
	m_cStr = cStr;
	m_nStrLen = cStr.GetLength();
	m_cTokenArray.SetSize(0, 10);

	for (int nPos = 0; nPos < m_nStrLen;) {
		int nDelimPos = SkipToDelimiter(nPos);
		int nTokenLen = nDelimPos - nPos;
		CString cToken = m_cStr.Mid(nPos, nTokenLen);
		m_cTokenArray.Add(cToken);

		if (nDelimPos >= m_nStrLen) {
			break;
		}

		nPos = nDelimPos + 1;

		if (nPos == m_nStrLen) {
			m_cTokenArray.Add(_T(""));
			break;
		}
	}
}

CString CStringTokenizer::GetToken(const int nTokenIndex) const
{
	return m_cTokenArray.GetAt(nTokenIndex);
}

