#include "stdafx.h"

CMapWholeFile *mwf = NULL;

//--------------------------------------------------------------------------
static BOOL WINAPI Detoured_GetVolumeInformationA(LPCSTR lpRootPathName,LPSTR lpVolumeNameBuffer,DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,LPDWORD lpFileSystemFlags,LPSTR lpFileSystemNameBuffer,DWORD nFileSystemNameSize);
DETOUR_TRAMPOLINE(BOOL WINAPI Trampoline_GetVolumeInformationA(LPCSTR lpRootPathName,LPSTR lpVolumeNameBuffer,DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,LPDWORD lpFileSystemFlags,LPSTR lpFileSystemNameBuffer,DWORD nFileSystemNameSize), GetVolumeInformationA);
static BOOL WINAPI Detoured_GetVolumeInformationW(LPCWSTR lpRootPathName, LPWSTR lpVolumeNameBuffer, DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,LPDWORD lpFileSystemFlags, LPWSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize);
DETOUR_TRAMPOLINE(BOOL WINAPI Trampoline_GetVolumeInformationW( LPCWSTR lpRootPathName, LPWSTR lpVolumeNameBuffer, DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,LPDWORD lpFileSystemFlags, LPWSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize ), GetVolumeInformationW);

//--------------------------------------------------------------------------
const char *InstallHooks()
{
  mwf = new CMapWholeFile();
  if (!mwf->openMemory(SN_MMNAME, sizeof(serialnumber_map_t)))
  {
    delete mwf;
    mwf = NULL;
    return "Make sure that the launcher is still running!";
  }
  DetourFunctionWithTrampoline((PBYTE)Trampoline_GetVolumeInformationA, (PBYTE)Detoured_GetVolumeInformationA);
  DetourFunctionWithTrampoline((PBYTE)Trampoline_GetVolumeInformationW, (PBYTE)Detoured_GetVolumeInformationW);
  return NULL;
}

//--------------------------------------------------------------------------
void UnInstallHooks()
{
  DetourRemove((PBYTE)Trampoline_GetVolumeInformationA, (PBYTE)Detoured_GetVolumeInformationA);
  DetourRemove((PBYTE)Trampoline_GetVolumeInformationW, (PBYTE)Detoured_GetVolumeInformationW);
  delete mwf;
  mwf = NULL;
}

//--------------------------------------------------------------------------
void PatchVolumeInformation(char drive, LPDWORD lpVolumeSerialNumber)
{
  if (drive < 'A' || drive > 'Z')
    return;
  serialnumber_map_t *snm = (serialnumber_map_t *) mwf->Memory();
  *lpVolumeSerialNumber = snm->sn[drive - 'A'];
  LogwindowSendFmt("Program requested serial number for %c and we returned %08X\n", 
    drive, *lpVolumeSerialNumber);
}

//--------------------------------------------------------------------------
BOOL WINAPI Detoured_GetVolumeInformationA(LPCSTR lpRootPathName,LPSTR lpVolumeNameBuffer,DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,LPDWORD lpFileSystemFlags,LPSTR lpFileSystemNameBuffer,DWORD nFileSystemNameSize)
{
  BOOL ok;
  ok = Trampoline_GetVolumeInformationA(lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
  if (!ok || lpRootPathName == NULL || lpVolumeSerialNumber == NULL)
    return ok;
  char drive = toupper(lpRootPathName[0]);
  PatchVolumeInformation(drive, lpVolumeSerialNumber);
  return ok;
}

BOOL WINAPI Detoured_GetVolumeInformationW( LPCWSTR lpRootPathName, LPWSTR lpVolumeNameBuffer, DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,LPDWORD lpFileSystemFlags, LPWSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize )
{
  BOOL ok;
  ok = Trampoline_GetVolumeInformationW(lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
  if (!ok || lpRootPathName == NULL || lpVolumeSerialNumber == NULL)
    return ok;
  char drive = _totupper(lpRootPathName[0]);
  PatchVolumeInformation(drive, lpVolumeSerialNumber);
  return ok;
}

//--------------------------------------------------------------------------
BOOL APIENTRY DllMain(
  HMODULE hModule,
  DWORD  ul_reason_for_call,
  LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
    {
      DisableThreadLibraryCalls(hModule);
      const char *msg = InstallHooks();
      if (msg != NULL)
      {
        LogwindowSendString("Hook dll failed to initialize!");
        return FALSE;
      }
      else
      {
        LogwindowSendFmt("Hook dll installed for process %x", GetCurrentProcessId());
      }
    }
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    UnInstallHooks();
    LogwindowSendFmt("Hook dll uninstalled!");
    break;
  }
  return TRUE;
}

