/*************************************************************
 *  Beeper Device FIX for Windows 7 NTVDM -- Installer
 *
 *  (C)oded by Dipl.-Ing. (FH) Ludwig Ertl, 2010
 *
 * This is the Installation program for the NTVDM Beeper fix
 * It tries to take care of the common installation tasks.
 *
 *************************************************************
 * Module:   installinf.c
 * Descr.:   SetupAPI Installer for .inf files
 * License:  GPL 3
 * Date  :   11.01.2010
 * Changelog:
 *************************************************************/

// ------------------------- INCLUDES -------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <setupapi.h>

// =============================================================
// Public
// =============================================================
BOOL InstallINF (HWND hWnd, char *pszInf, char *pszSection, char *pszService)
{
    HINF hInf;
    HSPFILEQ fQueue;
    UINT uErr=0;
    PVOID cbContext;
    char szBuf[512];
    BOOL bRet;

    if (GetFileAttributes(pszInf) == 0xFFFFFFFF)
    {
        wsprintf (szBuf, "Cannot find driver .inf: %s", pszInf);
        MessageBox (hWnd, szBuf, "Error", MB_ICONSTOP | MB_OK);
        return FALSE;
    }

    hInf = SetupOpenInfFile (pszInf, NULL, INF_STYLE_WIN4, &uErr);
    if (hInf == INVALID_HANDLE_VALUE)
    {
        wsprintf (szBuf, "Cannot process .inf file %s, line %d: %08X", pszInf, 
            uErr, GetLastError());
        MessageBox (hWnd, szBuf, "Error", MB_ICONSTOP | MB_OK);
        return FALSE;
    }

    if (!SetupInstallFromInfSection (hWnd, hInf, pszSection, SPINST_ALL ^ SPINST_FILES,
            NULL, NULL, 0, NULL, NULL, NULL, NULL))
    {
        wsprintf (szBuf, "Installing Registry entries from .INF file failed: %08X",
            GetLastError());
        MessageBox (hWnd, szBuf, "Error", MB_ICONSTOP | MB_OK);
        SetupCloseInfFile(hInf);
        return FALSE;
    }

    if ((fQueue = SetupOpenFileQueue()) == INVALID_HANDLE_VALUE)
    {
        wsprintf (szBuf, "Opening File queue from .INF file failed: %08X",
            GetLastError());
        MessageBox (hWnd, szBuf, "Error", MB_ICONSTOP | MB_OK);
        SetupCloseInfFile(hInf);
        return FALSE;
    }

    cbContext = SetupInitDefaultQueueCallback(hWnd);
    SetupInstallFilesFromInfSection(hInf, NULL, fQueue, pszSection, NULL, 0);
    if (!(bRet = SetupCommitFileQueue(hWnd, fQueue, SetupDefaultQueueCallback, cbContext)))
    {
        wsprintf (szBuf, "Cannot copy files specified in .INF file: %08X",
            GetLastError());
        MessageBox (hWnd, szBuf, "Error", MB_ICONSTOP | MB_OK);
    }

    SetupCloseFileQueue(fQueue);
    SetupTermDefaultQueueCallback(cbContext);

    if (pszService)
    {
        if (!(bRet = SetupInstallServicesFromInfSection(hInf, pszService, 0)))
        {
            wsprintf (szBuf, "Cannot install services from .INF file: %08X",
                GetLastError());
            MessageBox (hWnd, szBuf, "Error", MB_ICONSTOP | MB_OK);
        }
    }

	SetupCloseInfFile(hInf);
    return bRet;
}

// --------------------------------------------------------------

BOOL StartSvc(HWND hWnd, char *pszService)
{
    SC_HANDLE hSCManager, hService;
    char szBuf[128];
    BOOL bRet=FALSE;

    if (hSCManager = OpenSCManager(0, 0, 0))
    {
        if (hService = OpenService (hSCManager, pszService, SERVICE_START))
        {
            if (!(bRet = StartService (hService, 0, NULL)))
            {
                wsprintf (szBuf, "Cannot start Service: %08X", 
                    GetLastError());
                MessageBox (hWnd, szBuf, "Start failed", MB_ICONWARNING | MB_OK);
            }
            CloseServiceHandle (hService);
        }
        else
        {
            wsprintf (szBuf, "Cannot start Service: Opening Service failed: %08X", 
                GetLastError());
            MessageBox (hWnd, szBuf, "Start failed", MB_ICONWARNING | MB_OK);
        }
        CloseServiceHandle (hSCManager);
    }
    else
    {
        wsprintf (szBuf, "Cannot start Service: Opening Service manager failed: %08X", 
            GetLastError());
        MessageBox (hWnd, szBuf, "Start failed", MB_ICONWARNING | MB_OK);
    }
    return bRet;
}
