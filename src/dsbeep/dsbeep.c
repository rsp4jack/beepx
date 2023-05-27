/*************************************************************
 *  Directsound Beeper AppInit-Hook for Windows XP
 *
 *  (C)oded by Dipl.-Ing. (FH) Ludwig Ertl, 2013
 *
 * This DLL is a helper module for spkrfix.dll for
 * systems with boards with broken PC speaker support.
 * It generates a square wave Beep via Directsound output.
 * This sounds more like the "original" PC Speaker, as
 * the Beep() command by Windows generates a sine wave.
 *
 * Work is based on
 * http://www.codeproject.com/Articles/32892/DirectSound-Beep-Implementation
 *
 *************************************************************
 * Module:   scbeep.c
 * Descr.:   Main module 
 * License:  Public domain
 * Date  :   15.09.2013
 * Changelog:
 *************************************************************/

// ------------------------- INCLUDES -------------------------

#define WIN32_LEAN_AND_MEAN
#define _WIN32_DCOM
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

// ------------------------ LIBRARIES -------------------------

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")

// ------------------------- DEFINES --------------------------

#define MAX_FREQ 32767
#define MIN_FREQ 37
#define BITS_PER_SAMPLE  16
#define NUM_CHANNELS 1
#define SAMPLING_RATE 44100
#define BUFFER_SIZE  (SAMPLING_RATE * NUM_CHANNELS)
#define SCALE ((1 << (BITS_PER_SAMPLE - 1))-1)

// ------------------------ VARIABLES -------------------------

typedef struct
{
	LPDIRECTSOUNDBUFFER lpDSBuffer;
	LPDIRECTSOUND lpDS;
	DSBUFFERDESC dsbdesc;
	WAVEFORMATEX wfx;
} DS_DATA, *PDS_DATA;

typedef struct
{
	HANDLE hEvent;
	BOOL bShutdown;
	CRITICAL_SECTION cs;
	DWORD dwFreq;
	DWORD dwDuration;
} THREAD_DATA, *PTHREAD_DATA;

static THREAD_DATA m_Thread = {0};
static HANDLE m_hBeepThread = NULL;

// =============================================================
// Private
// =============================================================

/* Init
 * 
 * Description: Initializes global DirectSound instance
 * Returns    : TRUE on success, FALSE on failure
 */
static BOOL Init(PDS_DATA lpDS)
{
	ZeroMemory(&lpDS->wfx, sizeof(WAVEFORMATEX));
	ZeroMemory(&lpDS->dsbdesc, sizeof(DSBUFFERDESC));

	// All we do here is create the IDirectSound object
	if(FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
		return FALSE;

	if(FAILED(DirectSoundCreate(NULL, &lpDS->lpDS, NULL)))
	{
		lpDS->lpDS = NULL;
		return FALSE;
	}
	if(FAILED(lpDS->lpDS->lpVtbl->SetCooperativeLevel(lpDS->lpDS, GetDesktopWindow(), DSSCL_NORMAL)))
	{
		lpDS->lpDS->lpVtbl->Release(lpDS->lpDS);
		lpDS->lpDS = NULL;
		return FALSE;
	}
	
	lpDS->wfx.wFormatTag = WAVE_FORMAT_PCM;
	lpDS->wfx.nChannels = NUM_CHANNELS;
	lpDS->wfx.wBitsPerSample = BITS_PER_SAMPLE;
	lpDS->wfx.nSamplesPerSec = SAMPLING_RATE;
	lpDS->wfx.nBlockAlign = lpDS->wfx.nChannels * lpDS->wfx.wBitsPerSample / 8;
	lpDS->wfx.nAvgBytesPerSec = lpDS->wfx.nSamplesPerSec * lpDS->wfx.nBlockAlign;
	lpDS->wfx.cbSize = 0;
	
	lpDS->dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	lpDS->dsbdesc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY
		            | DSBCAPS_CTRLFREQUENCY
		            | DSBCAPS_GLOBALFOCUS;
	lpDS->dsbdesc.lpwfxFormat = &lpDS->wfx;
	return TRUE;
}

/* Beep_
 * 
 * Description: Does a Beep via DirectSound as a Squarewave
 * Returns    : TRUE on success, FALSE on failure
 */
static BOOL Beep_(PDS_DATA lpDS, DWORD dwFreq, DWORD dwDuration)
{
	int half_period, buffer_size;
	DWORD play_freq;
	LPVOID lpvWrite;
	DWORD dwLen;

	if(dwFreq < MIN_FREQ) dwFreq = MIN_FREQ;
	if(dwFreq > MAX_FREQ) dwFreq = MAX_FREQ;
	
	if ((half_period = SAMPLING_RATE * NUM_CHANNELS / (2*dwFreq))<1) half_period = 1;
	buffer_size = 2*half_period * BITS_PER_SAMPLE/8;

	// Make buffer
	lpDS->dsbdesc.dwBufferBytes = buffer_size;
	if(FAILED(lpDS->lpDS->lpVtbl->CreateSoundBuffer(lpDS->lpDS, &lpDS->dsbdesc, &lpDS->lpDSBuffer, NULL)))
		return FALSE;

	if(FAILED(lpDS->lpDSBuffer->lpVtbl->GetFrequency(lpDS->lpDSBuffer, &play_freq)))
	{
		lpDS->lpDSBuffer->lpVtbl->Release(lpDS->lpDSBuffer);
		return FALSE;
	}
	play_freq = MulDiv(play_freq, 2*dwFreq*half_period, SAMPLING_RATE * NUM_CHANNELS);
	if(FAILED(lpDS->lpDSBuffer->lpVtbl->SetFrequency(lpDS->lpDSBuffer, play_freq)))
	{
		lpDS->lpDSBuffer->lpVtbl->Release(lpDS->lpDSBuffer);
		return FALSE;
	}

	if(SUCCEEDED(lpDS->lpDSBuffer->lpVtbl->Lock(lpDS->lpDSBuffer, 0, buffer_size, &lpvWrite, &dwLen,
		                            NULL, NULL, DSBLOCK_ENTIREBUFFER)))
	{
		short *p = (short*)lpvWrite;
		int i = half_period, j;

		while(i--)
		{
			for(j = 0; j < NUM_CHANNELS; ++j)
			{
				*p = -SCALE; p++;
			}
		}
		i = half_period;
		while(i--)
		{
			for(j = 0; j < NUM_CHANNELS; ++j)
			{
				*p = SCALE; p++;
			}
		}

		if(SUCCEEDED(lpDS->lpDSBuffer->lpVtbl->Unlock(lpDS->lpDSBuffer, lpvWrite, dwLen, NULL, 0)))
		{
			if(SUCCEEDED(lpDS->lpDSBuffer->lpVtbl->SetCurrentPosition(lpDS->lpDSBuffer, 0)))
			{
				if(SUCCEEDED(lpDS->lpDSBuffer->lpVtbl->Play(lpDS->lpDSBuffer, 0, 0, DSBPLAY_LOOPING)))
				{
					WaitForSingleObject(m_Thread.hEvent, dwDuration);
					lpDS->lpDSBuffer->lpVtbl->Stop(lpDS->lpDSBuffer);
				}
			}
		}
	}

	// Delete buffer
	if(NULL != lpDS->lpDSBuffer) lpDS->lpDSBuffer->lpVtbl->Release(lpDS->lpDSBuffer);

	return TRUE;
}


DWORD WINAPI BeepThread(LPVOID lpParameter)
{
	DWORD dwFreq, dwDuration;
	DS_DATA ds={0};

	EnterCriticalSection(&m_Thread.cs);
	Init(&ds);
	LeaveCriticalSection(&m_Thread.cs);
	WaitForSingleObject(m_Thread.hEvent, INFINITE);
	if (ds.lpDS)
	{
		while(!m_Thread.bShutdown)
		{
			EnterCriticalSection(&m_Thread.cs);
			dwFreq = m_Thread.dwFreq;
			dwDuration = m_Thread.dwDuration;
			m_Thread.dwFreq = 0;
			LeaveCriticalSection(&m_Thread.cs);
			if (dwFreq == 0)
			{
				WaitForSingleObject(m_Thread.hEvent, INFINITE);
				continue;
			}
			Beep_(&ds, dwFreq, dwDuration);
		}
		ds.lpDS->lpVtbl->Release(ds.lpDS);
	}
	m_Thread.bShutdown = TRUE;
	CoUninitialize();
	return 0;
}

BOOL __declspec(dllexport) __cdecl DSBeep(DWORD dwFreq, DWORD dwDuration)
{
	if (m_Thread.bShutdown) return FALSE;
	EnterCriticalSection(&m_Thread.cs);
	m_Thread.dwFreq = dwFreq;
	m_Thread.dwDuration = dwDuration;
	LeaveCriticalSection(&m_Thread.cs);
	return SetEvent(m_Thread.hEvent);
}


// =============================================================
// Entry point
// =============================================================
#ifdef _DEBUG
BOOL APIENTRY DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpReserved)
#else
BOOL WINAPI _DllMainCRTStartup(HANDLE hDllHandle, DWORD dwReason, LPVOID lpReserved)
#endif
{
	static unsigned int refs = 0;
	DWORD dwThreadId;

	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		refs++;

		if (refs == 1)
		{
			InitializeCriticalSection(&m_Thread.cs);
			m_Thread.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			m_hBeepThread = CreateThread(NULL, 0, BeepThread, NULL, 0, &dwThreadId);
		}
		break;
	case DLL_PROCESS_DETACH:
		refs--;
		if (refs==0 && m_hBeepThread) 
		{
			m_Thread.bShutdown = TRUE;
			SetEvent(m_Thread.hEvent);
			WaitForSingleObject(m_hBeepThread, 5000);
			DeleteCriticalSection(&m_Thread.cs);
			CloseHandle(m_Thread.hEvent);
		}
		break;
	}

	return TRUE;
}
