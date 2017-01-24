
// VSnGui.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VSnGui.h"
#include "VSnGuiDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVSnGuiApp

BEGIN_MESSAGE_MAP(CVSnGuiApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVSnGuiApp construction

CVSnGuiApp::CVSnGuiApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CVSnGuiApp object

CVSnGuiApp theApp;


// CVSnGuiApp initialization

BOOL CVSnGuiApp::InitInstance()
{
	CWinAppEx::InitInstance();

	CVSnGuiDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
