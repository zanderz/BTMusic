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



void CFileScanner::FoundFile(CString & pathname)
{
	// Got a file, upload it
	AfxTrace(L"Uploading: %s\n", pathname);
	
	CInternetSession ises;
	CHttpFile* httpf = NULL;
	CHttpConnection *connection = NULL;

	CFile file(pathname.GetBuffer(), CFile::modeRead);

	try{
		connection = ises.GetHttpConnection(L"127.0.0.1:8080");

		httpf = connection->OpenRequest(CHttpConnection::HTTP_VERB_POST, L"upload.php",0,1,0,0,INTERNET_FLAG_KEEP_CONNECTION);

		CString hdrs /* = My appropriate headers to send */;
		httpf->AddRequestHeaders(hdrs);

		CString PreFileData; /* = prefile data */;	// TODO: add email and phone number!
		CString PostFileData; /* = postfile data */;

		httpf->SendRequestEx(DWORD(PreFileData.GetLength()+file.GetLength()+PostFileData.GetLength()), HSR_SYNC | HSR_INITIATE);

		httpf->Write((LPSTR)(LPCSTR)PreFileData.GetBuffer(), PreFileData.GetLength());

		UINT len=1024;
		char buf[1024]; /*BUFFER SIZE = 1024 HOW BIG ACTUALLY SHOULD IT BE?*/
		while(len>0){
			len=file.Read(&buf,1024);
			if(len>0)httpf->Write(&buf,len);
		}

		httpf->Write((LPSTR)(LPCSTR)PostFileData.GetBuffer(), PostFileData.GetLength());
		httpf->EndRequest(HSR_SYNC);
	}
	catch(CInternetException* e) {
		AfxTrace(L"CInternetException: m_dwError = %d\n", e->m_dwError);
		m_dwCommError = e->m_dwError;
		AfxMessageBox(L"Unable to contact server");
		e->Delete();
	}

	file.Close();
	httpf->Close();
	connection->Close();
}

