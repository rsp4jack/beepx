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
 * This driver tries to fix the problem by hooking the LazyBeep
 * function of the NTVDM and using our own function to work 
 * around the problem.
 *
 *************************************************************
 * Module:   beeper.c
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

// ------------------------- INCLUDES -------------------------
#include <windows.h>
#include <ntddbeep.h>
#include "beeper.h"

// ------------------------- DEFINES --------------------------
#ifdef _DEBUG
#define DBG_CALLS   // Debug calls on Debug-Console
#endif

// =============================================================
// Public (Exports)
// =============================================================

/* Beeper_Open
 * 
 * Description: Opens the Beeper Device and returns a handle
 *              to it for subsequent calls to this module
 * Returns    : Either a HANDLE to the Device or 
 *              INVALID_HANDLE_VALUE on failure.
 */
HANDLE Beeper_Open (char *pszDriver)
{
	char szDevice[MAX_PATH], szDevName[MAX_PATH], *pszDev = "\\\\.\\Beep";

    wsprintf (szDevName, "\\\\.\\%s", pszDriver);
    if (!QueryDosDevice (pszDriver, szDevice, sizeof(szDevice)))
    {
        wsprintf (szDevice, "\\Device\\%s", pszDriver);
	    DefineDosDevice (DDD_RAW_TARGET_PATH, pszDriver,
		    szDevice);
    }


    return CreateFile(szDevName,
                       FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL);
}

/* Beeper_Close
 * 
 * Description: Closes the Beeper Device and shuts the Speaker
 * Parameters : hBeep       -   Handle to the Beeper 
 */
void Beeper_Close (HANDLE hBeep)
{
	if (hBeep != INVALID_HANDLE_VALUE)
	{
		Beeper_Beep(hBeep, 0, 0);
		CloseHandle(hBeep);
	}
}

/* Beeper_Beep
 * 
 * Description: Beeps with the given frequency for the
 *              specified duration.
 * Parameters : hBeep       -   Handle to the Beeper 
 *              dwFreq      -   Frequncy to beep with
 *              dwDuration  -   Duration, -1 for INFINITE
 * Returns    : TRUE on success, FALSE on failure.
 */
BOOL Beeper_Beep (HANDLE hBeep, DWORD dwFreq, DWORD dwDuration)
{
    BEEP_SET_PARAMETERS BeepSetParameters;
    DWORD dwReturned;
	
#ifdef DBG_CALLS
    char szBuf[128];

    wsprintf(szBuf, "Beep(%d, %d)", dwFreq, dwDuration);
    OutputDebugString(szBuf);
#endif

	if (hBeep == INVALID_HANDLE_VALUE)
        return FALSE;

    /* Set beep data */
    BeepSetParameters.Frequency = dwFreq;
    BeepSetParameters.Duration  = dwDuration;

    return DeviceIoControl(hBeep,
                    IOCTL_BEEP_SET,
                    &BeepSetParameters,
                    sizeof(BEEP_SET_PARAMETERS),
                    NULL,
                    0,
                    &dwReturned,
                    NULL);
}

