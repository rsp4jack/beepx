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
 * Module:   spkrfix.c
 * Descr.:   Main module and dispatcher. 
 *           Enables / disables the beeper accordingly.
 * License:  GPL 3
 * Date  :   08.01.2010
 * Changelog:
 *			 03.07.2013 - Added Windows 8 LazyBeep support
 *************************************************************/

// ------------------------- INCLUDES -------------------------
#define i386
#include <windows.h> 
#include <vddsvc.h>
#include "beeper.h"
#include "vdmpriv.h"

// ------------------------ LIBRARIES -------------------------
#pragma comment (lib, "ntvdm.lib")

// ------------------------- DEFINES --------------------------
#ifdef _DEBUG
#define DBG_CALLS   // Debug calls on Debug-Console
#endif

/* FLIRT signature of the Windows 7 LazyBeep function */
#define LAZYBEEP_SIG "8BFF558BEC8B4D088B450C83EC103B0D"
/* FLIRT signature of the Windows 8 LazyBeep function */
#define LAZYBEEP_WN8 "8BFF558BEC83EC14563B"

// ------------------------ VARIABLES -------------------------
static HANDLE m_hBeeper = INVALID_HANDLE_VALUE;
static DWORD  m_nDelay  = 0;
static BOOL   m_bChFreq = FALSE;
static char   m_szDriver[MAX_PATH];

// ------------------------ PROTOTYPES ------------------------
int __stdcall LazyBeep(DWORD dwFreq, DWORD dwDuration);
void __fastcall LazyBeepW8(DWORD dwFreq, DWORD dwDuration);

// =============================================================
// Public (Exports)
// =============================================================
__declspec(dllexport) BOOL __cdecl VDDInitialize(IN PVOID DllHandle, IN ULONG Reason, 
		IN PCONTEXT Context OPTIONAL)
{
    char szBuf[MAX_PATH];

    switch (Reason) {

    case DLL_PROCESS_ATTACH:
	{
		BOOL bHookInstalled;

        /* Open Beeper */
        GetPrivateProfileString ("Spkrfix", "Driver", "Beep", 
             m_szDriver, sizeof(m_szDriver), "SPKRFIX.INI");
        m_hBeeper = Beeper_Open(m_szDriver);
        if (m_hBeeper == INVALID_HANDLE_VALUE)
        {
            wsprintf (szBuf, "Spkrfix cannot open Beeper Device %s: %08X", 
                m_szDriver, GetLastError());
            OutputDebugString (szBuf);
            return FALSE;
        }
        m_nDelay = GetPrivateProfileInt ("Spkrfix", "Delay", 0, "SPKRFIX.INI");
        m_bChFreq = GetPrivateProfileInt ("Spkrfix", "Ding", 0, "SPKRFIX.INI");

        /* Install hook */
		if (LOWORD(GetVersion())==0x106)	// W7
			bHookInstalled = VDMhook_Init(LazyBeep, LAZYBEEP_SIG, sizeof(LAZYBEEP_SIG)>>1);
		else	// W8
			bHookInstalled = VDMhook_Init(LazyBeepW8, LAZYBEEP_WN8, sizeof(LAZYBEEP_WN8)>>1);

        if (!bHookInstalled)
        {
            OutputDebugString ("Hooking LazyBeep failed");
        }
        OutputDebugString ("Spkrfix loaded successfully");
        break;
	}
    case DLL_PROCESS_DETACH:
        /* Close Beeper */
        Beeper_Close(m_hBeeper);
        OutputDebugString ("Spkrfix unloaded");
        break;
    default:
        break;
    }
 
    return TRUE;
}

// =============================================================
// Private
// =============================================================
void __fastcall LazyBeepW8(DWORD dwDuration, DWORD dwFreq)
{
	LazyBeep(dwFreq, dwDuration);
}

void __fastcall LazyBeepW81(DWORD dwFreq, DWORD dwDuration)
{
	LazyBeep(dwFreq, dwDuration);
}

int __stdcall LazyBeep(DWORD dwFreq, DWORD dwDuration)
{
    static DWORD dwLastFreq = 0, dwLastTime=0, dwTicks;

    if (dwLastFreq != dwFreq)
    {
#ifdef DBG_CALLS
        char szBuf[128];

        wsprintf (szBuf, "LazyBeep (%d, %d)", dwFreq, dwDuration);
        OutputDebugString(szBuf);
#endif

        if (m_bChFreq && m_nDelay && (dwFreq==877 || (dwFreq == 0 && dwLastFreq==877)) )
        {
            if (dwFreq>0)
            {
                Beeper_Close(m_hBeeper);
                Sleep(100);
                MessageBeep (0);
                m_hBeeper = Beeper_Open(m_szDriver);
            }
        }
        else
        {
                if (m_nDelay && (dwTicks=GetTickCount()-dwLastTime)>=1)
                {
                    Sleep(m_nDelay);
                }

                Beeper_Beep(m_hBeeper, dwFreq, dwDuration);
        }
        dwLastFreq = dwFreq;
        if (m_nDelay) {Sleep(0); dwLastTime = GetTickCount(); }
    }
	return 0;
}
