/*************************************************************
 *  Beeper Device FIX for Windows 7 NTVDM -- Installer
 *
 *  (C)oded by Dipl.-Ing. (FH) Ludwig Ertl, 2010
 *
 * This is the Installation program for the NTVDM Beeper fix
 * It tries to take care of the common installation tasks.
 *
 *************************************************************
 * Module:   addacl.h
 * Descr.:   Changes access Control to a given file.
 *           Based on Microsoft KB102102 
 * License:  GPL 3
 * Date  :   11.01.2010
 * Changelog:
 *************************************************************/

BOOL AddAccessRights(HWND hWnd, LPSTR lpszFileName, LPVOID pUserSID, 
      DWORD dwAccessMask);
