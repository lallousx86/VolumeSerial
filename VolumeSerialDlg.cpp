// VolumeSerialDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VolumeSerial.h"
#include "VolumeSerialDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVolumeSerialDlg dialog

CVolumeSerialDlg::CVolumeSerialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVolumeSerialDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVolumeSerialDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVolumeSerialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVolumeSerialDlg)
	DDX_Control(pDX, IDC_EDT_NEWSERIAL, m_edtNewSerial);
	DDX_Control(pDX, IDC_EDT_INFORMATION, m_edtInformation);
	DDX_Control(pDX, IDC_LB_DRIVES, m_lbDrives);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVolumeSerialDlg, CDialog)
	//{{AFX_MSG_MAP(CVolumeSerialDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_LB_DRIVES, OnSelchangeLbDrives)
	ON_BN_CLICKED(IDC_BTN_ABOUT, OnBtnAbout)
	ON_BN_CLICKED(IDC_BTN_CHANGESERIAL, OnBtnChangeserial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolumeSerialDlg message handlers

BOOL CVolumeSerialDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Populate the drives
  PopulateDriveListBox();
  
  m_edtNewSerial.SetWindowText(_T("12345678"));

/*
  if (GetVersion() > 0x80000000)
  {
    MessageBox(_T("This program will only work in Windows NT based OS"), _T("Info"), MB_ICONINFORMATION);
    GetDlgItem(IDC_BTN_CHANGESERIAL)->EnableWindow(FALSE);
  }
*/

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVolumeSerialDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVolumeSerialDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//-------------------------------------------------------------------------
void CVolumeSerialDlg::ShowErrorString(const DWORD nErr)
{
  TCHAR szErrorMsg[100];

  ::FormatMessage(
     FORMAT_MESSAGE_FROM_SYSTEM,
     NULL,
     nErr,
     LANG_USER_DEFAULT,
     (LPTSTR)szErrorMsg,
     sizeof(szErrorMsg),
     NULL);
  
  MessageBox(szErrorMsg, _T("Error"), MB_ICONERROR);
}

//-------------------------------------------------------------------------
void CVolumeSerialDlg::OnOK() 
{
  CDialog::OnOK();
}

//-------------------------------------------------------------------------
void CVolumeSerialDlg::PopulateDriveListBox()
{
  // clear old list
  m_lbDrives.ResetContent();

  // add a special flag that will allow the list to be refreshed
  m_lbDrives.AddString(_T("-- refresh list --"));
  m_lbDrives.SetItemData(0, 0);

  // get all mapped drives (in form of bits)
  DWORD iLogicalDrives = ::GetLogicalDrives();

  int sel_drive_c = -1;
  for (DWORD j=1, i=0; i< 26; i++)
  {
    // is drive bit set?
    if (((1 << i) & iLogicalDrives) != 0)
    {
      // Add drive to list
	  int drive = 'A'+i;
	  
	  CString str;
      str.Format(_T("%c"), drive);

      m_lbDrives.AddString(str);
      m_lbDrives.SetItemData(j, drive);

	  if (drive == 'C')
		  sel_drive_c = j;
      j++;
    }
  }

  // try to select drive 'C' (assuming that drive C is the 2nd drive in the system)
  if (sel_drive_c != -1)
	m_lbDrives.SetCurSel(sel_drive_c);

  //
  OnSelchangeLbDrives();
}

//-------------------------------------------------------------------------
void CVolumeSerialDlg::OnSelchangeLbDrives() 
{
  int curSel = m_lbDrives.GetCurSel();
  if (curSel == 0)
  {
    PopulateDriveListBox();
    return;
  }
  GetDriveInfo(m_lbDrives.GetItemData(curSel));	
}

//-------------------------------------------------------------------------
void CVolumeSerialDlg::GetDriveInfo(DWORD Drive)
{
  DWORD serialNumber, maxComponentLength, fsFlags;

  TCHAR szFileSystem[12];
  TCHAR szVolumeName[24];
  TCHAR szRoot[4] = "C:\\";

  szRoot[0] = (TCHAR)(Drive & 0xFF);

  if (!::GetVolumeInformation(
     szRoot,
     szVolumeName,
     sizeof(szVolumeName),
     &serialNumber,
     &maxComponentLength,
     &fsFlags,
     szFileSystem,
     sizeof(szFileSystem)
   ))
  {
    m_edtInformation.SetWindowText(_T("Failed to retrieve drive information"));
    return;
  }

  CString info;

  info.Format(_T("File system: %s\r\n")
              _T("Serial Number: %04X-%04X\r\n")
              _T("Label: %s"), szFileSystem, serialNumber >> 16, serialNumber & 0xFFFF, szVolumeName);
  m_edtInformation.SetWindowText(info);
}

//-------------------------------------------------------------------------
void CVolumeSerialDlg::ChangeSerialNumber(DWORD Drive, const DWORD newSerial)
{
  const size_t max_pbsi = 3;

  struct partial_boot_sector_info
  {
    LPSTR Fs; // file system name
    DWORD FsOffs; // offset of file system name in the boot sector
    DWORD SerialOffs; // offset of the serial number in the boot sector
  };

  static const partial_boot_sector_info pbsi[max_pbsi] =
  {
   {"FAT32", 0x52, 0x43},
   {"FAT",   0x36, 0x27},
   {"NTFS",  0x03, 0x48}
  };

  TCHAR szDrive[12];
  _snprintf(szDrive, _countof(szDrive), "%c:\\", Drive & 0xFF);

  if (!disk.Open(szDrive))
  {
    ShowErrorString("Could not open disk!");
    return;
  }

  // read sector
  char Sector[512];
  if (!disk.ReadSector(0, Sector))
  {
    ShowErrorString("Could not read sector!");
    return;
  }

  // Try to search for a valid boot sector
  DWORD i;
  for (i=0;i<max_pbsi;i++)
  {
    if (strncmp(pbsi[i].Fs, Sector+pbsi[i].FsOffs, strlen(pbsi[i].Fs)) == 0)
    {
      // We found a valid signature
      break;
    }
  }

  if (i >= max_pbsi)
  {
    MessageBox(_T("Cannot change serial number of this file system!"), _T("Error"), MB_ICONERROR);
    return;
  }
  
  // Patch the serial number
  *(PDWORD)(Sector+pbsi[i].SerialOffs) = newSerial;

  // write boot sector
  if (!disk.WriteSector(0, Sector))
  {
    ShowErrorString("Could not write sector!");
    return;
  }

  ShowErrorString("Volume serial number changed successfully!\r"
                  "You might want to restart your system for changes to take effect!");
}

//-------------------------------------------------------------------------
void CVolumeSerialDlg::OnBtnAbout() 
{
  MessageBox(_T("Change Volume Serial Number v1.3\n")
             _T("(c) Elias Bachaalany <lallousz-x86@yahoo.com>\n"), 
			 _T("About"), 
			 MB_ICONINFORMATION);	
}

//-------------------------------------------------------------------------
void CVolumeSerialDlg::OnBtnChangeserial() 
{
  if (MessageBox(_T("Are you sure you want to do this?"), _T("Question"), MB_ICONQUESTION | MB_ICONWARNING | MB_YESNO) == IDNO)
    return;

  int drive = m_lbDrives.GetItemData(m_lbDrives.GetCurSel() & 0xFF);

  if (drive != 0)
  {
    TCHAR szNewSerial[10] = {0};
    LPTSTR *temp = NULL;
    m_edtNewSerial.GetWindowText(szNewSerial, sizeof(szNewSerial));
    if (_tcslen(szNewSerial) != 8)
    {
      MessageBox(_T("Please enter a valid 8 digit hex number!"));
      return;
    }

    DWORD newSerial = _tcstoul(szNewSerial, temp, 16);
    ChangeSerialNumber(drive, newSerial);
  }
}

//-------------------------------------------------------------------------
void CVolumeSerialDlg::ShowErrorString(const char *msg)
{
  MessageBox(msg, "Info", MB_ICONINFORMATION);
}