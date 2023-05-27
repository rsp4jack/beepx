/*************************************************************
 *  Beeper Device FIX for Windows 7 NTVDM -- Installer
 *
 *  (C)oded by Dipl.-Ing. (FH) Ludwig Ertl, 2010
 *
 * This is the Installation program for the NTVDM Beeper fix
 * It tries to take care of the common installation tasks.
 *
 *************************************************************
 * Module:   installinf.h
 * Descr.:   SetupAPI Installer for .inf files
 * License:  GPL 3
 * Date  :   11.01.2010
 * Changelog:
 *************************************************************/

BOOL InstallINF (HWND hWnd, char *pszInf, char *pszSection, char *pszService);
BOOL StartSvc(HWND hWnd, char *pszService);
