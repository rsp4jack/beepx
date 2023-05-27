/*************************************************************
 *  Beeper Device FIX for Windows 7 NTVDM -- Installer
 *
 *  (C)oded by Dipl.-Ing. (FH) Ludwig Ertl, 2010
 *
 * This is the Installation program for the NTVDM Beeper fix
 * It tries to take care of the common installation tasks.
 *
 *************************************************************
 * Module:   version.c
 * Descr.:   Gets Version Info for a specified file
 * License:  GPL 3
 * Date  :   11.01.2010
 * Changelog:
 *************************************************************/

// ------------------------- INCLUDES -------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// =============================================================
// Public
// =============================================================
void *GetVersionInfo (char *pszFile, char *pszResname)
{
	DWORD nHandle;
	DWORD nResSize;
	UINT cbTranslate, nValueSize;
	char sBuf[64];
	static void *pRetVal;
    static char *pResData=NULL, szOldFile[MAX_PATH]={0};
    static struct LANGANDCODEPAGE {
      WORD wLanguage;
      WORD wCodePage;
    } *lpTranslate;


    if (lstrcmp(szOldFile, pszFile))
    {
		if (pResData) HeapFree (GetProcessHeap(), 0, pResData);
		pResData = NULL;
    }

	if (!pResData || !pszResname)
	{
		if (! pszResname ||
			! (nResSize = GetFileVersionInfoSize (pszFile, &nHandle)) ||
			! (pResData = (char *)HeapAlloc (GetProcessHeap(), 0, nResSize)) ||
			! GetFileVersionInfo (pszFile, 0, nResSize, pResData) ||
			! VerQueryValue (pResData, "\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate) )
		{
			if (pResData) HeapFree (GetProcessHeap(), 0, pResData);
			pResData = NULL;
			return NULL;
		}
        strcpy (szOldFile, pszFile);
	}
    wsprintf(sBuf, "\\StringFileInfo\\%04x%04x\\%s", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage, pszResname);
    if (!VerQueryValue(pResData, sBuf, &pRetVal, &nValueSize) || !nValueSize) return NULL;
	return pRetVal;
}
