/*************************************************************
 *  Beeper Device FIX for Windows 7 NTVDM -- Installer
 *
 *  (C)oded by Dipl.-Ing. (FH) Ludwig Ertl, 2010
 *
 * This is the Installation program for the NTVDM Beeper fix
 * It tries to take care of the common installation tasks.
 *
 *************************************************************
 * Module:   takeown.h
 * Descr.:   Takes ownership of a specified file
 *           Based on Microsoft PSDK sample takeown.c
 * License:  GPL 3
 * Date  :   11.01.2010
 * Changelog:
 *************************************************************/

BOOL TakeOwnership(HWND hWnd, LPSTR lpFileName);
BOOL GrantAllPrivileges (HWND hWnd, LPSTR lpszFileName);
