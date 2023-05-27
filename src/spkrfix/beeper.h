/*************************************************************
 *  Beeper Device FIX for Windows 7 NTVDM
 *
 *  (C)oded by Dipl.-Ing. (FH) Ludwig Ertl, 2010
 *
 * This little VDD driver fixes a problem with the Windows 7
 * NTVDM where the PC Speaker output is not wrapped correctly.
 * This may be due to the fact that the functionality of the
 * BEEP.SYS System driver was move to the Usermode Sound Agent.
 *
 * For further information, see:
 * http://blogs.msdn.com/larryosterman/archive/2010/01/04/what-s-up-with-the-beep-driver-in-windows-7.aspx
 *
 * This driver tries to fix the problem by capturing the I/O
 * ports of the 8253 and 8254 PIC and using the information
 * to enable/disable the Beeper with the given frequency.
 *
 *************************************************************
 * Module:   beeper.h
 * Descr.:   Implements an Interface to the BEEP.SYS driver
 *           via IOCTLs. This is necessary, as the WIN32 API
 *           Beep() Closes the Beeper device immediately, so
 *           enabling the beeper with -1 doesn't work (even
 *           tough the WIN32 API seems to return without
 *           Sleep() in this case.
 * License:  GPL 3
 * Date  :   08.01.2010
 * Changelog:
 *************************************************************/

HANDLE Beeper_Open (char *pszDriver);
void   Beeper_Close (HANDLE hBeep);
BOOL   Beeper_Beep (HANDLE hBeep, DWORD dwFreq, DWORD dwDuration);
