// FileScanner.cpp : implementation file
//

#include "stdafx.h"
#include <afxinet.h>
#include "MusicApp1.h"
#include "FileScanner.h"


// CFileScanner

//IMPLEMENT_DYNCREATE(CFileScanner, CWinThread)

CFileScanner::CFileScanner()
: m_Email(_T(""))
, m_dwCommError(0)
, m_pConnection(NULL)
{
}

CFileScanner::~CFileScanner()
{
}

BOOL CFileScanner::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CFileScanner::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CFileScanner, CWinThread)
END_MESSAGE_MAP()


// CFileScanner message handlers


int CFileScanner::Run()
{
	// TODO: Add your specialized code here and/or call the base class
	CAtlString EnvStr;

	EnvStr.GetEnvironmentVariable(L"USERPROFILE");
//	printf_s("Current value of TEMP variable: %s\n", EnvStr);
	WalkDirectory(EnvStr);
	//FoundFile(CString(L"c:\\temp\\foo.txt"));

	GetMainWnd()->PostMessageW(WM_COMMAND, IDOK);
	return CWinThread::Run();
}


void CFileScanner::SetUser(const CString & email, const CString & phone)
{
	m_Email = email;
	m_Phone = phone;
}


void CFileScanner::WalkDirectory(CString path)
{
	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(path);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking && !m_dwCommError)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		CString pathname = finder.GetFilePath();

		// if it's a directory, recursively search it

		if (finder.IsDirectory())
		{

//			cout << (LPCTSTR) str << endl;
			WalkDirectory(finder.GetFilePath());
		}
		else
		{
			CString ending = pathname.Right(4);			
			if(ending.CompareNoCase(L".mp3") == 0)
				FoundFile(finder.GetFilePath());
		}
	}

	finder.Close();
}


CString CFileScanner::MakeRequestHeaders(CString& strBoundary)
{

	CString strFormat;
	CString strData;

    strFormat = _T("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n");
    strFormat += _T("Origin: http://75.101.219.115\r\n");
    strFormat += _T("User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.71 Safari/537.36\r\n");
    strFormat += _T("Content-Type: multipart/form-data; boundary=%s\r\n");
	strFormat += _T("Referer: http://75.101.219.115/\r\n");
    strFormat += _T("Accept-Encoding: gzip,deflate,sdch\r\n");
    strFormat += _T("Accept-Language: en-US,en;q=0.8\r\n");

	strData.Format(strFormat, strBoundary);

	return strData;

}

CString CFileScanner::MakePreFileData(CString& strBoundary, CString& strFileName, CString& strEmail)
{

	CString strFormat;
	CString strData;

	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"user\"");
	strFormat += _T("\r\n\r\n");
	strFormat += _T("%s");
	strFormat += _T("\r\n");
	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"filedata\"; filename=\"%s\"");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Type: audio/mp3");
//	strFormat += _T("\r\n");
//	strFormat += _T("Content-Transfer-Encoding: binary");
	strFormat += _T("\r\n\r\n");

	strData.Format(strFormat, strBoundary, strEmail, strBoundary, strFileName);

	return strData;

}

CString CFileScanner::MakePostFileData(CString& strBoundary)
{

	CString strFormat;
	CString strData;

	strFormat = _T("\r\n");
	strFormat += _T("--%s--");
	strFormat += _T("\r\n");	

	strData.Format(strFormat, strBoundary, strBoundary);

	return strData;

}


BOOL CFileScanner::SendTrack(CString& strFullPath, CString& strFileName)
{

	USES_CONVERSION;

	CFile fTrack;
	CHttpFile* pHTTP;
	CString strHTTPBoundary;
	CString strPreFileData;
	CString strPostFileData;
	DWORD dwTotalRequestLength;
	DWORD dwChunkLength;
	DWORD dwReadLength;
	DWORD dwResponseLength;
	TCHAR szError[MAX_PATH];
	void* pBuffer;
	LPSTR szResponse;
	CString strResponse;
	BOOL bSuccess = TRUE;

	CString strDebugMessage;

	if(! m_pConnection)
		m_pConnection = theApp.m_internetSession.GetHttpConnection(L"75.101.219.115", (INTERNET_PORT) 80);
	if (!m_pConnection)
	{
		return FALSE;
	}

	if (FALSE == fTrack.Open(strFullPath, CFile::modeRead | CFile::shareDenyWrite))
	{
		AfxMessageBox(_T("Unable to open the file."));
		return FALSE;
	}

	strHTTPBoundary = _T("BTMusicFormBoundaryq0Tgm32dnk");
	strPreFileData = MakePreFileData(strHTTPBoundary, strFileName, m_Email);
	strPostFileData = MakePostFileData(strHTTPBoundary);

	AfxMessageBox(strPreFileData);
	AfxMessageBox(strPostFileData);

	dwTotalRequestLength = strPreFileData.GetLength() + strPostFileData.GetLength() + fTrack.GetLength();

	dwChunkLength = 64 * 1024;

	pBuffer = malloc(dwChunkLength);

	if (NULL == pBuffer)
	{
		return FALSE;
	}

	try
	{
		pHTTP = m_pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/add"));
		pHTTP->AddRequestHeaders(MakeRequestHeaders(strHTTPBoundary));
		pHTTP->SendRequestEx(dwTotalRequestLength, HSR_SYNC | HSR_INITIATE);

#ifdef _UNICODE
		pHTTP->Write(W2A(strPreFileData), strPreFileData.GetLength());
#else
		pHTTP->Write((LPSTR)(LPCSTR)strPreFileData, strPreFileData.GetLength());
#endif

		dwReadLength = -1;
		while (0 != dwReadLength)
		{
			strDebugMessage.Format(_T("%u / %u\n"), fTrack.GetPosition(), (DWORD) fTrack.GetLength());
			TRACE(strDebugMessage);
			dwReadLength = fTrack.Read(pBuffer, dwChunkLength);
			if (0 != dwReadLength)
			{
				pHTTP->Write(pBuffer, dwReadLength);
			}
		}

#ifdef _UNICODE
		pHTTP->Write(W2A(strPostFileData), strPostFileData.GetLength());
#else
		pHTTP->Write((LPSTR)(LPCSTR)strPostFileData, strPostFileData.GetLength());
#endif

		pHTTP->EndRequest(HSR_SYNC);

		dwResponseLength = pHTTP->GetLength();
		while (0 != dwResponseLength)
		{
			szResponse = (LPSTR)malloc(dwResponseLength + 1);
			szResponse[dwResponseLength] = '\0';
			pHTTP->Read(szResponse, dwResponseLength);
			strResponse += szResponse;
			free(szResponse);
			dwResponseLength = pHTTP->GetLength();
		}

		AfxMessageBox(strResponse);

	}

	catch (CException* e)
	{
		e->GetErrorMessage(szError, MAX_PATH);
		e->Delete();
		AfxMessageBox(szError);
		bSuccess = FALSE;
		m_pConnection->Close();
		m_pConnection = NULL;
	}

	pHTTP->Close();
	delete pHTTP;

	fTrack.Close();

	if (NULL != pBuffer)
	{
		free(pBuffer);
	}

	//Disconnect();

	return bSuccess;
}



void CFileScanner::FoundFile(CString & pathname)
{
	// Got a file, upload it
	AfxTrace(L"Uploading: %s\n", pathname);

	SendTrack(pathname, CString(PathFindFileName(pathname.GetBuffer())));	
}

