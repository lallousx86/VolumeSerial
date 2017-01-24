
// VSnGuiDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VSnGui.h"
#include "VSnGuiDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------------
static const char S_SNMAP_FILE[] = "serials.bin";
static const char S_VSNDLL[]     = "\\VSnHook.dll";
//--------------------------------------------------------------------------

// CVSnGuiDlg dialog

//--------------------------------------------------------------------------
CVSnGuiDlg::CVSnGuiDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVSnGuiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

//--------------------------------------------------------------------------
void CVSnGuiDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COMBO_DRIVES, m_cbDrives);
  DDX_Control(pDX, IDC_EDT_SN, m_txtSerial);
  DDX_Control(pDX, IDC_EDT_PROGNAME, m_txtProgramName);
}

//--------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CVSnGuiDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_CBN_SELCHANGE(IDC_COMBO_DRIVES, &CVSnGuiDlg::OnCbnSelchangeComboDrives)
//  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_BUTTON_SET_SN, &CVSnGuiDlg::OnBnClickedButtonSetSn)
//  ON_EN_CHANGE(IDC_EDT_PROGNAME, &CVSnGuiDlg::OnEnChangeEdtProgname)
  ON_BN_CLICKED(IDOK, &CVSnGuiDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL, &CVSnGuiDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


//--------------------------------------------------------------------------
// CVSnGuiDlg message handlers
BOOL CVSnGuiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

  if (!InitMemoryMap())
  {
    AfxMessageBox("Failed to initialize memory map!");
    EndDialog(IDOK);
    return TRUE;
  }
	PopulateSerials();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//--------------------------------------------------------------------------
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CVSnGuiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

//--------------------------------------------------------------------------
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVSnGuiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//--------------------------------------------------------------------------
void CVSnGuiDlg::OnCbnSelchangeComboDrives()
{
  int i = m_cbDrives.GetCurSel();
  if (i < 0)
    return;
  char buf[20];
  _snprintf(buf, sizeof(buf), "%08X", snm->sn[i]);
  m_txtSerial.SetWindowText(buf);
}

//--------------------------------------------------------------------------
// 1. Loads the serial file
//   - if not found, then initiate with values from the system
// 2. Populates the combo box of drives accordingly
void CVSnGuiDlg::PopulateSerials()
{
  if (!LoadSerialMap())
    PopulateSerialMap();

  m_cbDrives.ResetContent();
  char root[10] = "C:\\";
  for (int i=0;i<26;i++)
  {
    root[0] = 'A' + i;
    m_cbDrives.AddString(root);
  }
  // Select drive 'C'
  m_cbDrives.SetCurSel(2);
  OnCbnSelchangeComboDrives();
}

//--------------------------------------------------------------------------
// Updates the entry for the drive serial
void CVSnGuiDlg::OnBnClickedButtonSetSn()
{
  int i = m_cbDrives.GetCurSel();
  if (i < 0)
    return;
  char buf[100] = {0}, *temp;
  m_txtSerial.GetWindowText(buf, sizeof(buf)-1);
  snm->sn[i] = strtoul(buf, &temp, 16);
}

//--------------------------------------------------------------------------
bool CVSnGuiDlg::LoadSerialMap()
{
  FILE *fp = fopen(S_SNMAP_FILE, "rb");
  if (fp == NULL)
    return false;
  size_t nb = fread(snm, sizeof(serialnumber_map_t), 1, fp);
  if (nb == 0)
  {
    fclose(fp);
    return false;
  }
  char txt[MAX_PATH] = {0};
  fread(txt, sizeof(txt), 1, fp);
  m_txtProgramName.SetWindowText(txt);
  fclose(fp);
  return true;
}

//--------------------------------------------------------------------------
bool CVSnGuiDlg::SaveSerialMap()
{
  FILE *fp = fopen(S_SNMAP_FILE, "wb");
  if (fp == NULL)
    return false;
  fwrite(snm, sizeof(serialnumber_map_t), 1, fp);
  char txt[MAX_PATH] = {0};
  m_txtProgramName.GetWindowText(txt, sizeof(txt));
  fwrite(txt, sizeof(txt), 1, fp);
  fclose(fp);
  return true;
}

//--------------------------------------------------------------------------
int CVSnGuiDlg::PopulateSerialMap()
{
  char root[] = "C:\\";
  int ok = 0;
  DWORD fsflags, maxcomp;
  char volumename[MAX_PATH], fsname[MAX_PATH];
  for (int i=0;i<26;i++)
  {
    root[0] = 'A' + i;
    snm->sn[i] = 0;
    if (GetVolumeInformationA(root, volumename, sizeof(volumename), &snm->sn[i], &maxcomp, &fsflags, fsname, sizeof(fsname)))
      ++ok;
  }
  return ok;
}

//--------------------------------------------------------------------------
bool CVSnGuiDlg::InitMemoryMap()
{
  if (!mwf.mapMemory(SN_MMNAME, sizeof(serialnumber_map_t)))
    return false;
  snm = (serialnumber_map_t *) mwf.Memory();
  return true;
}

//--------------------------------------------------------------------------
void CVSnGuiDlg::OnBnClickedOk()
{
  // Form hook DLL name
  CString msg;
  char DllName[MAX_PATH];
  DWORD n = GetCurrentDirectory(sizeof(DllName), DllName);
  const char *postfix = S_VSNDLL;
  if (DllName[n-1] == '\\')
    ++postfix;
  strncat(DllName, postfix, sizeof(DllName));

  // Check its existance
  if (GetFileAttributes(DllName) == INVALID_FILE_ATTRIBUTES)
  {
    msg.Format("Hook dll \"%s\" not found!", DllName);
    AfxMessageBox(msg);
    return;
  }

  // Get application name
  char CmdLine[MAX_PATH];
  m_txtProgramName.GetWindowText(CmdLine, sizeof(CmdLine));

  // Create process
  STARTUPINFOA si = {0};
  PROCESS_INFORMATION pi = {0};
  si.cb = sizeof(si);
  BOOL ok = CreateProcessWithDllA(NULL, CmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi, DllName, NULL);
  if (!ok)
  {
    msg.Format("Failed!\n"
      "Could not run: %s\n"
      "With hook: %s\n", CmdLine, DllName);
    AfxMessageBox(msg);
  }
}

//--------------------------------------------------------------------------
void CVSnGuiDlg::OnBnClickedCancel()
{
  SaveSerialMap();
  OnCancel();
}
