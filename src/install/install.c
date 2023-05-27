/*************************************************************
 *  Beeper Device FIX for Windows 7 NTVDM -- Installer
 *
 *  (C)oded by Dipl.-Ing. (FH) Ludwig Ertl, 2010
 *
 * This is the Installation program for the NTVDM Beeper fix
 * It tries to take care of the common installation tasks.
 *
 *************************************************************
 * Module:   install.c
 * Descr.:   Main application, dispatch Dialog and to the 
 *           requested actions.
 * License:  GPL 3
 * Date  :   11.01.2010
 * Changelog:
 *           03.07.2013 - Added version check for Windows 8
 *************************************************************/

// ------------------------- INCLUDES -------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include "resource.h"
#include "installinf.h"
#include "version.h"
#include "takeown.h"
#include "../spkrfix/beeper.h"

// ------------------------ PROTOTYPES ------------------------
static BOOL CheckForXPBEEP (/*IN/OUT optional*/char *pszPath);

// =============================================================
// Window handling
// =============================================================
BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static BOOL bInitDlg = FALSE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            char szDriver[MAX_PATH], szVDD[1024], szBeep[MAX_PATH]={0};
            int nDelay;
            BOOL bDisableUninstall=TRUE, bIsXPBeep = CheckForXPBEEP(szBeep), bDing=FALSE;
            DWORD cbData, i;
            HKEY hKey;

            bInitDlg = TRUE;
            GetPrivateProfileString ("Spkrfix", "Driver", "Beep", 
                szDriver, sizeof(szDriver), "SPKRFIX.INI"); 
            nDelay = GetPrivateProfileInt ("Spkrfix", "Delay", 0, "SPKRFIX.INI");
			bDing = GetPrivateProfileInt ("Spkrfix", "Ding", 0, "SPKRFIX.INI");

			if (GetPrivateProfileInt ("Spkrfix", "DSBeep", 0, "SPKRFIX.INI"))
				CheckDlgButton (hWnd, IDC_OPT4, BST_CHECKED);
            else if (lstrcmp(szDriver, "BeepXP") == 0) CheckDlgButton (hWnd, IDC_OPT2, BST_CHECKED);
            else if (!bIsXPBeep) 
            {
                CheckDlgButton (hWnd, IDC_OPT3, BST_CHECKED);
                if (!nDelay) nDelay=200;
            }
            else if (lstrcmp(szDriver, "Beep") == 0) CheckDlgButton (hWnd, IDC_OPT1, BST_CHECKED);

            SetDlgItemText (hWnd, IDC_DEVICE, szDriver);
            SetDlgItemInt (hWnd, IDC_DELAY, nDelay, FALSE);
			CheckDlgButton (hWnd, IDC_DING, bDing?BST_CHECKED:BST_UNCHECKED);
				
            if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\BeepXP", 0,
                KEY_EXECUTE, &hKey) == ERROR_SUCCESS)
            {
                BOOL bDelFlag=0;

                cbData = sizeof(bDelFlag);
                if (RegQueryValueEx (hKey, "DeleteFlag", NULL, NULL, (LPBYTE)&bDelFlag, &cbData) == ERROR_SUCCESS &&
                    bDelFlag == 1)
                    EnableWindow (GetDlgItem(hWnd, IDC_UBEEPXP), FALSE);
                EnableWindow (GetDlgItem(hWnd, IDC_IBEEPXP), FALSE);
                RegCloseKey(hKey);
            }
            else EnableWindow (GetDlgItem(hWnd, IDC_UBEEPXP), FALSE);

            EnableWindow (GetDlgItem (hWnd, bIsXPBeep?IDC_IBEEPOLD:IDC_UBEEPOLD), FALSE);

            if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\VirtualDeviceDrivers", 0,
                KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
            {
                cbData = sizeof(szVDD);
                if (RegQueryValueEx (hKey, "VDD", NULL, NULL, szVDD, &cbData) == ERROR_SUCCESS)
                {
                    for (i=0; i<cbData; i+=lstrlen(&szVDD[i])+1)
                    {
                        if (lstrcmpi(&szVDD[i], "SPKRFIX.DLL") == 0)
                        {
                            EnableWindow (GetDlgItem (hWnd, IDC_IVDMFIX), FALSE);
                            bDisableUninstall = FALSE;
                        }
                    }
                }
                RegCloseKey(hKey);
            }
            if (bDisableUninstall)
                EnableWindow (GetDlgItem (hWnd, IDC_UVDMFIX), FALSE);
            
            bInitDlg = FALSE;

            return TRUE;
        }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_TEST:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                HANDLE hBeeper;
                char szDriver[MAX_PATH], szMsg[512];

                GetDlgItemText (hWnd, IDC_DEVICE, szDriver, sizeof(szDriver));
                if ((hBeeper = Beeper_Open(szDriver)) == INVALID_HANDLE_VALUE)
                {
                    wsprintf (szMsg, "Cannot open Beeper device %s: %08X", szDriver, GetLastError());
                    MessageBox (hWnd, szMsg, "Error", MB_ICONSTOP | MB_OK);
                    break;
                }
                Beeper_Beep (hBeeper, 200, 300);
                Sleep(300);
                Beeper_Close(hBeeper);
            }
            break;
        case IDC_OPT1:
            if (HIWORD(wParam) == BN_CLICKED && !bInitDlg && 
                SendMessage ((HWND)lParam, BM_GETCHECK, 0, 0)==BST_CHECKED)
            {
                char szPath[MAX_PATH]={0}, szBuf[512];

                SetDlgItemText (hWnd, IDC_DEVICE, "Beep");
                SetDlgItemInt (hWnd, IDC_DELAY, 0, FALSE);

                if (CheckForXPBEEP(szPath)) break;
                wsprintf (szBuf, "It doesn't seem like you have the Windows XP BEEP.SYS driver installed. "
                    "Please get the BEEP.SYS from Windows XP and replace %s with it. Make a backup copy of "
                    "the original Windows 7 driver first. You may need to reboot your System afterwards. "
                    "If you don't have the Windows XP BEEP.SYS driver ready, you can alternatively choose "
                    "option 2 and use my BEEPXP.SYS.", szPath);
                MessageBox (hWnd, szBuf, "Warning", MB_ICONWARNING | MB_OK);
                
            }
            break;
        case IDC_OPT2:
            if (HIWORD(wParam) == BN_CLICKED && !bInitDlg &&
                SendMessage ((HWND)lParam, BM_GETCHECK, 0, 0)==BST_CHECKED)
            {
                SetDlgItemText (hWnd, IDC_DEVICE, "BeepXP");
                SetDlgItemInt (hWnd, IDC_DELAY, 0, FALSE);
                if (!IsWindowEnabled (GetDlgItem(hWnd, IDC_UBEEPXP)))
                    MessageBox (hWnd, "This option won't work as long as you don't have the BEEPXP Driver installed!", "Warning", MB_ICONWARNING | MB_OK);
            }
            break;
        case IDC_OPT3:
            if (HIWORD(wParam) == BN_CLICKED && !bInitDlg &&
                SendMessage ((HWND)lParam, BM_GETCHECK, 0, 0)==BST_CHECKED)
            {
                char szPath[MAX_PATH]={0};

                SetDlgItemText (hWnd, IDC_DEVICE, "Beep");
                SetDlgItemInt (hWnd, IDC_DELAY, 200, FALSE);
                if (CheckForXPBEEP(szPath))
                {
                    MessageBox (hWnd, "You currently have the Windows XP BEEP.SYS installed, so this option "
                        "won't work until you reinstall the Windows 7 driver.", "Warning", MB_ICONWARNING | MB_OK);
                    break;
                }
            }
            break;
        case IDC_OPT4:
            if (HIWORD(wParam) == BN_CLICKED && !bInitDlg &&
                SendMessage ((HWND)lParam, BM_GETCHECK, 0, 0)==BST_CHECKED)
            {
                SetDlgItemInt (hWnd, IDC_DELAY, 20, FALSE);
            }
            break;
        case IDC_IVDMFIX:
        case IDC_UVDMFIX:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                char szDir[MAX_PATH];
 
                GetCurrentDirectory (sizeof(szDir), szDir);
                lstrcat (szDir, "\\spkrfix\\spkrfix.inf");
                if (InstallINF (hWnd, szDir, 
                    LOWORD(wParam)==IDC_IVDMFIX?"DefaultInstall":"DefaultUninstall",  NULL))
                {
                    BOOL bEnable = LOWORD(wParam)==IDC_IVDMFIX;
                    EnableWindow (GetDlgItem (hWnd, IDC_IVDMFIX), !bEnable);
                    EnableWindow (GetDlgItem (hWnd, IDC_UVDMFIX), bEnable);
                }
            }
            break;
        case IDC_IBEEPXP:
        case IDC_UBEEPXP:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                char szDir[MAX_PATH];
                BOOL bRet;
 
                GetCurrentDirectory (sizeof(szDir), szDir);
                lstrcat (szDir, "\\beepxp\\beepxp.inf");
                if (LOWORD(wParam)==IDC_IBEEPXP)
                    bRet = InstallINF (hWnd, szDir, "DefaultInstall", "DefaultInstall.Services");
                else
                    bRet = InstallINF (hWnd, szDir, "DefaultUninstall", "DefaultUninstall.Services");

                if (bRet)
                {
                    BOOL bEnable = LOWORD(wParam)==IDC_IBEEPXP;
                    EnableWindow (GetDlgItem (hWnd, IDC_IBEEPXP), !bEnable);
                    EnableWindow (GetDlgItem (hWnd, IDC_UBEEPXP), bEnable);

                    if (LOWORD(wParam)==IDC_IBEEPXP)
                    {
                        StartSvc(hWnd, "BeepXP");
                    }
                }               
            }
            break;
        case IDC_IBEEPOLD:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                char szPath[MAX_PATH]={0}, szBackup[MAX_PATH], *pFile;
                OPENFILENAME ofn={0};
                char szFileName[MAX_PATH]={0}, szBuf[1024];


                if (CheckForXPBEEP(szPath))
                {
                    MessageBox (hWnd, "Old Windows XP Beeper is already installed.", "Nothing to do",
                        MB_ICONINFORMATION | MB_OK);
                    break;
                }

                for (pFile=szPath+lstrlen(szPath); *pFile!='\\'; pFile--);
                pFile++;
                lstrcat (szPath, ".WIN7");
                if (GetFileAttributes (szPath) != 0xFFFFFFFF)
                {
                    MessageBox (hWnd, "There is already a backed up version of the BEEP.SYS driver, aborting.",
                        "Cannot backup", MB_ICONSTOP | MB_OK);
                    break;
                }

	            ofn.lStructSize=sizeof(ofn);
	            ofn.hwndOwner=hWnd;
	            ofn.lpstrFilter="Driver files (*.sys)\0*.sys\0";
	            ofn.nMaxFile=MAX_PATH;
	            ofn.lpstrDefExt="sys";
	            ofn.lpstrFile=szFileName;
                ofn.lpstrTitle="Select Windows XP BEEP.SYS driver";
	            ofn.Flags=OFN_NOCHANGEDIR;
	            if (!GetOpenFileName(&ofn) || !*szFileName) break;

                if (!CheckForXPBEEP(szFileName))
                {
                    MessageBox (hWnd, "The selected file isn't the Windows XP BEEP.SYS driver, aborting",
                        "Invalid file", MB_ICONSTOP | MB_OK);
                    break;
                }
                lstrcpy (szBackup, szPath);
                lstrcpy (pFile, "BEEP.SYS");

                if (!TakeOwnership(hWnd, szPath) || ! GrantAllPrivileges(hWnd, szPath))
                    break;
                if (!MoveFile (szPath, szBackup))
                {
                    wsprintf (szBuf, "Cannot backup %s => %s: %08X", szPath, szBackup, GetLastError());
                    MessageBox (hWnd, szBuf, "Error", MB_ICONSTOP | MB_OK);
                    break;
                }

                if (!CopyFile (szFileName, szPath, TRUE))
                {
                    wsprintf (szBuf, "Cannot copy your BEEP.SYS to drivers directory: %08X", GetLastError());
                    MessageBox (hWnd, szBuf, "Error", MB_ICONSTOP | MB_OK);
                    MoveFile (szBackup, szPath);
                    break;
                }

                MessageBox (hWnd, "You need to restart Windows in order to complete the installation of the "
                    "Windows XP Beeper.", "Information", MB_ICONINFORMATION | MB_OK);
                EnableWindow (GetDlgItem (hWnd, IDC_IBEEPOLD), FALSE);
                EnableWindow (GetDlgItem (hWnd, IDC_UBEEPOLD), TRUE);
            }
            break;
        case IDC_UBEEPOLD:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                char szFileName[MAX_PATH]={0}, szTemp[MAX_PATH], szBackup[MAX_PATH], szBuf[1024];

                if (!CheckForXPBEEP(szFileName))
                {
                    MessageBox (hWnd, "You don't have the Windows XP Beep driver installed.", "Nothing to do", 
                        MB_ICONINFORMATION | MB_OK);
                    break;
                }

                lstrcpy (szBackup, szFileName);
                lstrcat (szBackup, ".WIN7");
                if (GetFileAttributes (szBackup) == 0xFFFFFFFF)
                {
                    MessageBox (hWnd, "There is no backup of the BEEP.SYS driver, aborting.",
                        "Cannot restore", MB_ICONSTOP | MB_OK);
                    break;
                }
                lstrcpy (szTemp, szFileName);
                lstrcat (szTemp, ".TMP");
                if (!MoveFile (szFileName, szTemp))
                {
                    MessageBox (hWnd, "Cannot delete BEEP.SYS", "Error", MB_ICONSTOP | MB_OK);
                    break;
                }
                if (!MoveFile (szBackup, szFileName))
                {
                    wsprintf (szBuf, "Cannot restore old driver, move operation failed with %08X", 
                        GetLastError());
                    MessageBox (hWnd, szBuf, "Error", MB_ICONSTOP | MB_OK);
                    break;
                }
                DeleteFile (szTemp);
                MessageBox (hWnd, "You need to restart Windows in order to complete the installation of the "
                    "Old Beeper.", "Information", MB_ICONINFORMATION | MB_OK);
                EnableWindow (GetDlgItem (hWnd, IDC_IBEEPOLD), TRUE);
                EnableWindow (GetDlgItem (hWnd, IDC_UBEEPOLD), FALSE);
            }
            break;
        case IDOK:
            {
                char szDriver[MAX_PATH];

                if (!GetDlgItemText (hWnd, IDC_DEVICE, szDriver, sizeof(szDriver)))
                {
                    MessageBox (hWnd, "You have to specify a driver at least. You can use Beep",
                        "Error", MB_ICONEXCLAMATION | MB_OK);
                    break;
                }
                WritePrivateProfileString("Spkrfix", "Driver", szDriver, "SPKRFIX.INI");
				wsprintf (szDriver, "%d", IsDlgButtonChecked (hWnd, IDC_DING)==BST_CHECKED);
				WritePrivateProfileString("Spkrfix", "Ding", szDriver, "SPKRFIX.INI");
				wsprintf (szDriver, "%d", IsDlgButtonChecked(hWnd,IDC_OPT4)==BST_CHECKED);
				WritePrivateProfileString("Spkrfix", "DSBeep", szDriver, "SPKRFIX.INI");
                wsprintf (szDriver, "%d", GetDlgItemInt(hWnd, IDC_DELAY, NULL, FALSE));
                if (!WritePrivateProfileString("Spkrfix", "Delay", szDriver, "SPKRFIX.INI"))
                {
                    wsprintf (szDriver, "Cannot save settings: %08X", GetLastError());
                    MessageBox (hWnd, szDriver, "Error", MB_ICONSTOP | MB_OK);
                }

            }
        case IDCANCEL:
                EndDialog (hWnd, 0);
                break;
            
        }
        break;
    case WM_DESTROY:
        GetVersionInfo(NULL, NULL);
        break;
    }
    return FALSE;
}

// =============================================================
// Entry point
// =============================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nCmdShow)
{
	WORD wVersion = LOWORD(GetVersion());

    if (wVersion!=0x106 && wVersion!=0x206)
    {
        if (MessageBox (NULL, "You are currently not running on Windows 7 or 8, do you really want to "
            "execute the Installer", "Not Windows 7 or 8", MB_ICONQUESTION | MB_YESNO) != IDYES)
            return 0;
    }
    if (DialogBox (hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, DialogProc) == -1)
    {
        char szBuf[128];

        wsprintf (szBuf, "Cannot create Dialog: %08X", GetLastError());
        MessageBox (NULL, szBuf, "Error", MB_ICONSTOP | MB_OK);
        return -1;
    }
    return 0;
}

// --------------------------------------------------------------

#ifndef DEBUG
int WINAPI WinMainCRTStartup(void)
{
    ExitProcess  (WinMain (GetModuleHandle(NULL), NULL, NULL, 0));
}
#endif

// =============================================================
// Private
// =============================================================
static BOOL CheckForXPBEEP (/*IN/OUT optional*/char *pszPath)
{
    char szPath[MAX_PATH], *pszVersion;

    GetVersionInfo (NULL, NULL);
    if (!pszPath || !*pszPath)
    {
        if (!GetSystemDirectory(szPath, sizeof(szPath)))
        {
            if (!GetWindowsDirectory (szPath, sizeof(szPath)))
                lstrcpy (szPath, "C:\\WINDOWS");
            lstrcat (szPath, "\\SYSTEM32");
        }
        lstrcat(szPath, "\\DRIVERS\\BEEP.SYS");
        if (pszPath) lstrcpy (pszPath, szPath);
        else pszPath=szPath;
    }
    pszVersion = GetVersionInfo (pszPath, "OriginalFilename");
    if (pszVersion && lstrcmp (pszVersion, "beep.sys") == 0 &&
        (pszVersion = GetVersionInfo (pszPath, "ProductVersion")) &&
        pszVersion[0]=='5' && pszVersion[2]<='1')
        return TRUE;
    return FALSE;
}
