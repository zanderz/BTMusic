#pragma once



// CFileScanner

class CFileScanner : public CWinThread
{
//	DECLARE_DYNCREATE(CFileScanner)

public:
	CFileScanner();
	virtual ~CFileScanner();


	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
	CString m_Email;
	CString m_Phone;
	DWORD	m_dwCommError;
public:
	virtual int Run();
	void SetUser(const CString & email, const CString & phone);
protected:
	void WalkDirectory(CString path);
	void FoundFile(CString & pathname);
};


