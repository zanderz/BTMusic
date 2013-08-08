
// MusicApp1.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMusicApp1App:
// See MusicApp1.cpp for the implementation of this class
//

class CMusicApp1App : public CWinApp
{
public:
	CMusicApp1App();
	
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	CInternetSession  m_internetSession;
	DECLARE_MESSAGE_MAP()
};

extern CMusicApp1App theApp;