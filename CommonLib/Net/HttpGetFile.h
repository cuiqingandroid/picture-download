#ifndef HTTPGETFILE
#define  HTTPGETFILE

#include <afxinet.h>
#include <map>
#include "../String/tstring.h"

namespace HttpFile
{
	typedef enum HTTPERRORTYPE
	{
		HTTP_NOERROR  = 0,		  //��ȷ
		HTTP_CONNECT_FAILED  = 1,     //����ʧ��
		HTTP_FILE_NOT_EXISTS = 2,     //�ļ�������
		HTTP_TIMED_OUT = 3,          //��ʱ
		HTTP_SERVER_ERROR  = 4,       //����������
		HTTP_WRITE_FILE_ERROR = 5,   //�ļ�д�����
		HTTP_UNKNOW  = 6            //��������
	} HttpErrorType;

	class IDownloadCallback
	{
	public:
		virtual void OnStart(LPCTSTR szFileName) = 0;                                //��ʼ����
		virtual void OnUpdate(LPCTSTR szFileName, UINT nDownLoadSize, UINT nFileSize) = 0;                   //����״̬���£�nStepΪ������ɰٷֱȣ�0��100��
		virtual void OnComplete(LPCTSTR szFileName) = 0;                             //�������
		virtual void OnError(LPCTSTR szFileName, int nErrorCode) = 0;                //����
		virtual void OnCancel(LPCTSTR szFileName) = 0;                               //ȡ��

		virtual BOOL IsCancel() = 0;                                                   //���Cancel����TRUE����ȡ����ǰ����
	};
}

#define SESSION_TIMEOUT			480000

class CHttpGetFile
{
public:
	CHttpGetFile(void);
	~CHttpGetFile(void);

	void AddReferer(const CString& strReferer)
	{
		m_strReferer = strReferer;
	}
	void SetUserAgent(const CString& strAgent)
	{
		m_strUserAgent = strAgent;
	}

	void SetLanguage(const CString& strLanuage)
	{
		m_strLanguage = strLanuage;
	}

	void SetZipDownload(BOOL bZip)
	{
		m_bZipDownload = bZip;
	}

	DWORD GetHttpStatusCode()
	{
		return m_dwHttpStatus;
	}

	// �ڹ����߳��������ļ�
	UINT GetHttpFile(LPCTSTR szURL, LPCTSTR szLocalFilePath, HttpFile::IDownloadCallback *pCallback = NULL, BOOL bUseNewConnection = FALSE);

	// ֱ�������ļ�
	UINT GetHttpFileOriginal(LPCTSTR szURL, LPCTSTR szLocalFilePath, HttpFile::IDownloadCallback *pCallback = NULL, BOOL bUseNewConnection = FALSE);

	UINT UploadHttpFileOriginal(LPCTSTR szURL, LPCTSTR szLocalFilePath, HttpFile::IDownloadCallback *pCallback = NULL, BOOL bUseNewConnection = FALSE);

	static BOOL IsHttpFileExist(LPCTSTR szURL, LPCTSTR szReferer = NULL);
protected:
	CInternetSession *m_pSession;
	std::map<tstring, CHttpConnection*> m_mConnections;
	std::map<tstring, tstring> m_mETags;
	CString m_strReferer;
	CString m_strUserAgent;
	BOOL m_bZipDownload;

	CString m_strURL;
	CString m_strLocalFilePath;
	CString m_strLanguage;
	HttpFile::IDownloadCallback * m_pCallback;
	BOOL m_bUseNewConnection;

	UINT m_nRet;

	DWORD m_dwHttpStatus;

	HANDLE m_handleThread;

	static DWORD WINAPI BeginDownLoadThread(LPVOID lpParam);

	BOOL EndDowLoadThread();

	static BOOL IsBadRequest(DWORD dwStatus);

	void CloseSocket(void);
	CInternetSession * GetInternetSession(void);
	CHttpConnection * GetHttpConnection(LPCTSTR rServer, INTERNET_PORT nPort);

};

#endif
