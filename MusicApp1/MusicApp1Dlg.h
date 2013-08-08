
// MusicApp1Dlg.h : header file
//

#pragma once
#include "FileScanner.h"

// CMusicApp1Dlg dialog
class CMusicApp1Dlg : public CDialogEx
{
// Construction
public:
	CMusicApp1Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MUSICAPP1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEmail();	
protected:
	CString m_Email;
	CString m_phone;		
	CFileScanner *m_pScanner;
public:
	afx_msg void OnBnClickedMyok();
};
