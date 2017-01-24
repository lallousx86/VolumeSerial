
// VSnGuiDlg.h : header file
//

#pragma once
#include "afxwin.h"

// CVSnGuiDlg dialog
class CVSnGuiDlg : public CDialog
{
// Construction
public:
	CVSnGuiDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VSNGUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
  serialnumber_map_t *snm;
  CMapWholeFile mwf;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnCbnSelchangeComboDrives();
  void PopulateSerials();
//  afx_msg void OnClose();
  CComboBox m_cbDrives;
  CEdit m_txtSerial;
  afx_msg void OnBnClickedButtonSetSn();
  bool LoadSerialMap();
  bool SaveSerialMap();
  int PopulateSerialMap();
  bool InitMemoryMap();
  CEdit m_txtProgramName;
//  afx_msg void OnEnChangeEdtProgname();
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCancel();
};
