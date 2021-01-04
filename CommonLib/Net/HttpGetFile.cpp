#include "StdAfx.h"
#include "HttpGetFile.h"

CHttpGetFile::CHttpGetFile(void)
{
	m_pSession = NULL;
	m_bUseNewConnection = FALSE;
	m_handleThread = NULL;

	m_nRet = 0;
	m_dwHttpStatus = 200;

	m_bZipDownload = FALSE;
}

CHttpGetFile::~CHttpGetFile(void)
{
	//EndDowLoadThread();
	CloseSocket();
}

BOOL CHttpGetFile::IsBadRequest(DWORD dwStatus)
{
	if (dwStatus >= HTTP_STATUS_BAD_REQUEST && dwStatus != 499)
	{
		return TRUE;
	}

	return FALSE;
}

CInternetSession * CHttpGetFile::GetInternetSession(void)
{
	if ( !m_pSession )
	{
		m_pSession = new CInternetSession(
			0,
			1,
#ifdef SUPPORT_PROXY
			PRE_CONFIG_INTERNET_ACCESS,
#else
			INTERNET_OPEN_TYPE_DIRECT,
#endif
			0,
			0,
			INTERNET_FLAG_DONT_CACHE);
		m_pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, SESSION_TIMEOUT);
		m_pSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, SESSION_TIMEOUT);
		m_pSession->SetOption(INTERNET_OPTION_SEND_TIMEOUT, SESSION_TIMEOUT);
	}

	return m_pSession;
}

CHttpConnection * CHttpGetFile::GetHttpConnection(LPCTSTR rServer, INTERNET_PORT nPort)
{
	tstring key = rServer;
	CHttpConnection *pConnection = NULL;

	if ( m_mConnections.find(key) == m_mConnections.end() )
	{
		CInternetSession *pSession = GetInternetSession();
		pConnection = pSession->GetHttpConnection(rServer, nPort);
		if ( pConnection )
		{
			m_mConnections[key] = pConnection;
		}
	}
	else
	{
		pConnection = m_mConnections[key];
	}

	return pConnection;
}

void CHttpGetFile::CloseSocket(void)
{
	std::map<tstring, CHttpConnection*>::iterator it;
	for (it = m_mConnections.begin(); it != m_mConnections.end(); it++)
	{
		CHttpConnection *pConnection = (*it).second;
		if ( pConnection )
		{
			pConnection->Close();
			delete pConnection;
		}
	}
	m_mConnections.erase(m_mConnections.begin(), m_mConnections.end());

	if ( m_pSession )
	{
		m_pSession->Close();
		delete m_pSession;
		m_pSession = NULL;
	}
}
UINT CHttpGetFile::GetHttpFile(LPCTSTR szURL, LPCTSTR szLocalFilePath, HttpFile::IDownloadCallback *pCallback/* = NULL*/,  BOOL bUseNewConnection)
{
	m_strURL = szURL;
	m_strLocalFilePath = szLocalFilePath;
	m_pCallback = pCallback;
	m_bUseNewConnection = bUseNewConnection;

	DWORD dwThreadId;
	TRACE(_T("the thread start\n"));
	m_handleThread = CreateThread(NULL, 0, BeginDownLoadThread, this, CREATE_SUSPENDED, &dwThreadId);
	::ResumeThread(m_handleThread);


	while (WaitForSingleObject(m_handleThread, 0) !=  WAIT_OBJECT_0)
	{
		MSG msg;
		while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return m_nRet;
}

BOOL CHttpGetFile::EndDowLoadThread()
{
	return TRUE;
}

DWORD WINAPI CHttpGetFile::BeginDownLoadThread(LPVOID lpParam)
{
	CHttpGetFile* pThis = (CHttpGetFile*)lpParam;

	pThis->m_nRet = pThis->GetHttpFileOriginal(pThis->m_strURL, pThis->m_strLocalFilePath, pThis->m_pCallback, pThis->m_bUseNewConnection);

	return 0;
}

UINT CHttpGetFile::GetHttpFileOriginal(LPCTSTR szURL, LPCTSTR szLocalFilePath, HttpFile::IDownloadCallback *pCallback/* = NULL*/,  BOOL bUseNewConnection)
{
	char httpbuff[512];
	CString rServer;
	DWORD dwServiceType;
	CString rObject;
	INTERNET_PORT nPort;
	DWORD dwRet;
	UINT iRet = 0;
	CHttpConnection *pConnection = NULL;

	CHttpFile *pFile = NULL;

	tstring key = tstring(szLocalFilePath);

	m_dwHttpStatus = 200;
	
	try
	{
		AfxParseURL(szURL, dwServiceType, rServer, rObject, nPort);

		if (rServer.IsEmpty() ||  rObject.IsEmpty())
		{
			iRet = -1;
			return iRet;
		}

		if (pCallback)
		{
			pCallback->OnStart(szURL);
		}

		if ( bUseNewConnection )
		{
			CInternetSession *pSession = GetInternetSession();
			pConnection = m_pSession->GetHttpConnection(rServer, nPort);
		}
		else
			pConnection = GetHttpConnection(rServer, nPort);


		pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, rObject);

		if (!m_strReferer.IsEmpty())
		{
			pFile->AddRequestHeaders(m_strReferer, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
		}
		
		pFile->AddRequestHeaders(_T("Cache-Control: no-cache\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);

		if (m_bZipDownload)
		{
			pFile->AddRequestHeaders(_T("Accept-Encoding: gzip\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
		}

		if (!m_strUserAgent.IsEmpty())
		{
			CString strAgent;

			strAgent.Format(_T("User-Agent: %s\r\n"), m_strUserAgent);

			pFile->AddRequestHeaders(strAgent);
		}

		if (!m_strLanguage.IsEmpty())
		{
			CString strLanguage;

			strLanguage.Format(_T("Accept-Language: %s\r\n"), m_strLanguage);

			pFile->AddRequestHeaders(strLanguage);
		}
		
		if (m_mETags.find(key) != m_mETags.end())
		{
			CString strETag;

			strETag.Format(TEXT("If-Modified-Since: %s\r\n"), m_mETags[key].c_str());

			pFile->AddRequestHeaders(strETag);
		}

		pFile->SendRequest();
		pFile->QueryInfoStatusCode(m_dwHttpStatus);

		if (CHttpGetFile::IsBadRequest(m_dwHttpStatus))
		{
			if (m_dwHttpStatus == 404)
			{
				iRet = HttpFile::HTTP_FILE_NOT_EXISTS;
			}
			else if (m_dwHttpStatus == 408)
			{
				iRet = HttpFile::HTTP_TIMED_OUT;
			}
			else if (m_dwHttpStatus >= 500)
			{
				iRet = HttpFile::HTTP_SERVER_ERROR;
			}
			else
			{
				iRet = HttpFile::HTTP_UNKNOW;
			}

			if (pCallback)
			{
				pCallback->OnError(szURL, iRet);
			}
			throw FALSE;
		}

		// Not Modified
		if (m_dwHttpStatus == 304)
		{
			throw TRUE;
		}

		int numbytes;
		DWORD nlengh;
		BOOL bCancel = FALSE;

		pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, nlengh);
		DWORD nReadByte = 0;

		TCHAR szTag[255] = {0};
		DWORD dwLength = 255;
		if (pFile->QueryInfo(HTTP_QUERY_LAST_MODIFIED, szTag, &dwLength, NULL) && dwLength > 0)
		{
			m_mETags[key] = tstring(szTag);
		}

		CFile myFile(szLocalFilePath, CFile::modeCreate|CFile::modeWrite);

		myFile.SeekToEnd();

		while (numbytes = pFile->Read(httpbuff, 512))
		{
			myFile.Write(httpbuff, numbytes);

			nReadByte += numbytes;

			if (pCallback && pCallback->IsCancel())
			{
				pCallback->OnCancel(szURL);
				bCancel = TRUE;
				break;
			}
			
			if (pCallback)
			{
				pCallback->OnUpdate(szURL, nReadByte, nlengh);
			}
		}
	
		myFile.Close();

		if (bCancel)
		{
			CFile::Remove(szLocalFilePath);
		}

		if (pCallback)
		{
			pCallback->OnComplete(szURL);
		}
	}
	catch(CInternetException *e)
	{
		e->Delete();

		if (pCallback)
		{
			pCallback->OnError(szURL, HttpFile::HTTP_CONNECT_FAILED);
		}
		iRet = -1;
	}
	catch(CFileException *e)
	{
		e->Delete();

		if (pCallback)
		{
			pCallback->OnError(szURL, HttpFile::HTTP_WRITE_FILE_ERROR);
		}
		iRet = HttpFile::HTTP_WRITE_FILE_ERROR;
	}
	catch(BOOL)
	{

	}

	if (pFile)
	{
		pFile->Close();
		delete pFile;
		pFile= NULL;
	}

	if ( bUseNewConnection )
	{
		m_mConnections.erase(szURL);
		if ( pConnection )
		{
			pConnection->Close();
			delete pConnection;
		}
	}

	return iRet;
}

BOOL CHttpGetFile::IsHttpFileExist(LPCTSTR szURL, LPCTSTR szReferer)
{
	BOOL bRet = FALSE;

	CInternetSession *pSession = NULL;
	CHttpConnection *pConnection = NULL;
	CHttpFile *pFile = NULL;

	CString rServer;
	DWORD dwServiceType;
	CString rObject;
	INTERNET_PORT nPort;
	DWORD dwRet;

	try
	{
		AfxParseURL(szURL, dwServiceType, rServer, rObject, nPort);

		pSession  = new CInternetSession(
			0,
			1,
			INTERNET_OPEN_TYPE_DIRECT,
			0,
			0,
			INTERNET_FLAG_DONT_CACHE);

		if (pSession == NULL)
		{
			throw FALSE;
		}

		pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, SESSION_TIMEOUT);
		pSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, SESSION_TIMEOUT);
		pSession->SetOption(INTERNET_OPTION_SEND_TIMEOUT, SESSION_TIMEOUT);

		pConnection = pSession->GetHttpConnection(rServer, nPort);
		if (pConnection == NULL)
		{
			throw FALSE;
		}

		pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, rObject);
		if (pFile == NULL)
		{
			throw FALSE;
		}

		if (szReferer != NULL)
		{
			pFile->AddRequestHeaders(szReferer, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
		}

		pFile->AddRequestHeaders(_T("Cache-Control: no-cache\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
		pFile->SendRequest();
		pFile->QueryInfoStatusCode(dwRet);

		if (!CHttpGetFile::IsBadRequest(dwRet))
		{
			bRet = TRUE;
		}
	}
	catch(BOOL)
	{

	}
	catch(...)
	{

	}
	
	if (pFile)
	{
		pFile->Close();
		delete pFile;
		pFile= NULL;
	}

	if ( pConnection )
	{
		pConnection->Close();
		delete pConnection;
	}

	if ( pSession )
	{
		pSession->Close();
		delete pSession;
		pSession = NULL;
	}
	

	return bRet;
}

UINT CHttpGetFile::UploadHttpFileOriginal(LPCTSTR szURL, LPCTSTR szLocalFilePath, HttpFile::IDownloadCallback *pCallback, BOOL bUseNewConnection)
{
	UINT iRet = 0;

	CHttpConnection *pConnection = NULL;
	CHttpFile *pHttpFile = NULL;
	CString rServer;
	DWORD dwServiceType;
	CString rObject;
	INTERNET_PORT nPort;

	try
	{
		AfxParseURL(szURL, dwServiceType, rServer, rObject, nPort);

		CFile sendFile(szLocalFilePath, CFile::modeRead | CFile::shareDenyWrite);
		CString strHeaders = _T("--BigFootBoundary\r\n");;
		strHeaders += _T("Content-Disposition: form-data; name=\"data\"; filename=\"config\"\r\n");
		strHeaders += _T("Content-Type: application/octet-stream\r\n");
		strHeaders += _T("Content-Transfer-Encoding: binary\r\n\r\n");

		CString strFooters = _T("\r\n--BigFootBoundary--\r\n");

		DWORD dwTotalLength = strHeaders.GetLength() + sendFile.GetLength() + strFooters.GetLength();
		USES_CONVERSION;
		BYTE *pPostData = new BYTE[dwTotalLength];
		BYTE *p = pPostData;
		memcpy(p, T2A(strHeaders), strHeaders.GetLength());
		p += strHeaders.GetLength();
		sendFile.Read(p, sendFile.GetLength());
		p += sendFile.GetLength();
		memcpy(p, T2A(strFooters), strFooters.GetLength());
		sendFile.Close();
		CMemFile memFile(pPostData, dwTotalLength);

		CInternetSession *pSession = GetInternetSession();
		pConnection = m_pSession->GetHttpConnection(rServer, nPort);

		pHttpFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, rObject);

		pHttpFile->AddRequestHeaders(_T("Cache-Control: no-cache\r\n"));
		pHttpFile->AddRequestHeaders(_T("Accept: */*\r\n"));
		pHttpFile->AddRequestHeaders(_T("Content-Type: multipart/form-data, boundary=BigFootBoundary\r\n"));
		pHttpFile->SendRequestEx(dwTotalLength);
		if (pCallback)
		{
			pCallback->OnStart(szURL);
		}

		CHAR buf[512];
		int nRead = 0;
		DWORD dwTotal = 0;
		BOOL bCancel = FALSE;
		while (nRead = memFile.Read(buf, 512))
		{
			if (pCallback && pCallback->IsCancel())
			{
				pCallback->OnCancel(szURL);
				bCancel = TRUE;
				break;
			}


			pHttpFile->Write(buf, nRead);
			dwTotal += nRead;

			if (pCallback)
			{
				pCallback->OnUpdate(szURL, dwTotal, dwTotalLength);
			}
		}
		pHttpFile->EndRequest();

		if (pCallback && !bCancel)
		{
			pCallback->OnComplete(szURL);
		}

		memFile.Close();

		delete []pPostData;

		iRet = 0;
	}
	catch(CInternetException *e)
	{
		if (pCallback)
		{
			pCallback->OnError(szURL, HttpFile::HTTP_CONNECT_FAILED);
		}
		iRet = -1;
	}
	catch(CFileException *e)
	{
		if (pCallback)
		{
			pCallback->OnError(szURL, HttpFile::HTTP_WRITE_FILE_ERROR);
		}
		iRet = HttpFile::HTTP_WRITE_FILE_ERROR;
	}
	catch(BOOL)
	{

	}

	if (pHttpFile)
	{
		pHttpFile->Close();
		delete pHttpFile;
	}
	if (pConnection)
	{
		pConnection->Close();
		delete pConnection;
	}

	return iRet;
}



