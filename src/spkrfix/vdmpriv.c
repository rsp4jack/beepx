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
 * Module:   vdmpriv.c
 * Descr.:   Hooks the LazyBeep funktion of NTVDM to pass
 *           on the Sound generation to our driver.
 * License:  GPL 3
 * Date  :   08.01.2010
 * Changelog:
 *           03.07.2013 - Added Windows 8 Lazybeep support
 *************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "vdmpriv.h"
#include "beeper.h"

void __fastcall LazyBeepW8(DWORD dwDuration, DWORD dwFreq);
void __fastcall LazyBeepW81(DWORD dwFreq, DWORD dwDuration);

static char HexToNum(char cIn);
static BOOL MatchSig(BYTE *lpBuffer, DWORD cbBuffer, const char *pszSig, DWORD *pdwEndOffset);

BOOL VDMhook_Init(void *LazyBeep, const char *pszSig, int cbSig)
{
    HMODULE hNtvdm;
    DWORD dwOffset;
    PIMAGE_DOS_HEADER mzhead;
	PIMAGE_NT_HEADERS peh;
    PBYTE pNtvdm;
    
    /* Get NTVDM handle */
    if (!(hNtvdm = GetModuleHandle("ntvdm.exe")))
    {
        OutputDebugString ("NTVDM.EXE Module not found in address space");
        return FALSE;
    }

    /* Read PE header */
    pNtvdm=(PBYTE)hNtvdm;
    mzhead = (PIMAGE_DOS_HEADER)hNtvdm;
    if (mzhead->e_magic != IMAGE_DOS_SIGNATURE) return FALSE;
    peh = (PIMAGE_NT_HEADERS32)&pNtvdm[mzhead->e_lfanew];
    if (peh->Signature!=IMAGE_NT_SIGNATURE) return FALSE;

    if (MatchSig(pNtvdm, peh->OptionalHeader.SizeOfImage, pszSig, &dwOffset))
    {
		DWORD dwback, dwAddr=(DWORD)hNtvdm+dwOffset-cbSig;

		/* Special: Windows 8.1 and 10 swapped frequency and duration back to correct param order) */
		if (LazyBeep == LazyBeepW8 && pNtvdm[dwOffset]==0x0D) LazyBeep=LazyBeepW81;

        /* Make an inline hook to our function */
        VirtualProtect((LPVOID)dwAddr, 5, PAGE_READWRITE, &dwback);
        *((PBYTE)(dwAddr))=0xE9;
        *((PDWORD)(dwAddr+1))=((DWORD)LazyBeep)-dwAddr-5;
        VirtualProtect((LPVOID)dwAddr, 5, dwback, &dwback);
        FlushInstructionCache(GetCurrentProcess(), NULL, 0); 
        return TRUE;
    }
    

    OutputDebugString ("Lazybeep signature in NTVDM.EXE not found");
    return FALSE;
}

// ------------------------------------------------------------------------- //

static char HexToNum(char cIn)
{
	return cIn>='A'?cIn-'A'+0xA:cIn-'0';
}

// ------------------------------------------------------------------------- //

static BOOL MatchSig(PBYTE lpBuffer, DWORD cbBuffer, const char *pszSig, PDWORD pdwEndOffset)
{
	BYTE cByte;
	DWORD i, j, iLen = lstrlen(pszSig), iEnd;
	BOOL bRet;
	
	if (cbBuffer<=iLen/2) return FALSE;
	iEnd = cbBuffer-(iLen/2);
	for (i=0, j=0; i<iLen && j<iEnd; i+=2, j++)
	{
		if (pszSig[i]!='.')
		{
			cByte = (HexToNum(pszSig[i])<<4) | HexToNum(pszSig[i+1]);
			if (lpBuffer[j]!=cByte) i=-2;
		}
	}
	if ((bRet=i==iLen) && pdwEndOffset) *pdwEndOffset = j;
	return bRet;
}

