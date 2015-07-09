
// MapTest.h : main header file for the MapTest application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMapTestApp:
// See MapTest.cpp for the implementation of this class
//

class CMapTestApp : public CWinApp
{
public:
	CMapTestApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount) override;

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMapTestApp theApp;
