// VolumeSerialDlg.h : header file
//

#if !defined(AFX_VOLUMESERIALDLG_H__7F208745_AB49_4A1B_BFD7_86AD2F42CAB7__INCLUDED_)
#define AFX_VOLUMESERIALDLG_H__7F208745_AB49_4A1B_BFD7_86AD2F42CAB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVolumeSerialDlg dialog

#include "DiskSector.h"

class CVolumeSerialDlg : public CDialog
{
// Construction
public:
	CVolumeSerialDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CVolumeSerialDlg)
	enum { IDD = IDD_VOLUMESERIAL_DIALOG };
	CEdit	m_edtNewSerial;
	CEdit	m_edtInformation;
	CComboBox	m_lbDrives;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVolumeSerialDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVolumeSerialDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnSelchangeLbDrives();
	afx_msg void OnBtnAbout();
	afx_msg void OnBtnChangeserial();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
  DiskSector disk;
	void ChangeSerialNumber(DWORD Drive, const DWORD newSerial);
	void GetDriveInfo(DWORD Drive);
	void PopulateDriveListBox();
	void ShowErrorString(const DWORD nErr);
  void ShowErrorString(const char *msg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOLUMESERIALDLG_H__7F208745_AB49_4A1B_BFD7_86AD2F42CAB7__INCLUDED_)
