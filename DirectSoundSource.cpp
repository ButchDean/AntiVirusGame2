/*!\file DirectSoundSource.cpp
 * \brief Functions to handle sound using DirectSound.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

#include <windows.h>
#include <winbase.h>
#include <dsound.h>
#include "DirectSoundIncludes.h"
#include "MMIO.h"
#include <fstream>			//  To comply with the new VC++ 2003 modifications.
using namespace std;

const int NUMSOUNDS = 6;

/*! Check if direct sound is playing or not and if the sound buffer was lost. */
DWORD dwStatus;
/*! Direct sound handle. */
LPDIRECTSOUND		lpDS;
/*! Buffers for all sounds handled by DirectSound. */
LPDIRECTSOUNDBUFFER lpDSBSounds[NUMSOUNDS];
extern HWND ghwnd;

CWave::CWave() : m_dwImageLen(0), m_bResource(false),
    m_pImageData(NULL)
{
}

CWave::CWave(const char* pszFileName) : m_dwImageLen(0),
    m_bResource(false), m_pImageData(NULL)
{
    Create(pszFileName);
}

CWave::CWave(UINT uiResID, HMODULE hmod) : m_dwImageLen(0),
    m_bResource(true), m_pImageData(NULL)
{
    Create(uiResID, hmod);
}

CWave::~CWave() {
    // Free the wave image data
    Free();
}

bool CWave::Create(const char* pszFileName)
{
    // Free any previous wave image data
    Free();

    // Flag as regular memory
    m_bResource = false;

    // Open the wave file
    ifstream fileWave(pszFileName);

    // Get the file length
    fileWave.seekg(0, ios::end);
    m_dwImageLen = (DWORD)fileWave.tellg();

    // Allocate and lock memory for the image data
    m_pImageData = (BYTE*)GlobalLock(GlobalAlloc(GMEM_MOVEABLE |
        GMEM_SHARE, m_dwImageLen));
    if (!m_pImageData)
        return false;

    // Read the image data from the file
    fileWave.seekg(0, ios::beg);
    fileWave.read((char*)m_pImageData, m_dwImageLen);

    return true;
}

bool CWave::Create(UINT uiResID, HMODULE hmod)
{
    // Free any previous wave image data
    Free();

    // Flag as resource memory
    m_bResource = true;

    // Find the wave resource
    HRSRC hresInfo;
    hresInfo = FindResource(hmod, MAKEINTRESOURCE(uiResID),
        "WAVE");
    if (!hresInfo)
        return false;

    // Load the wave resource
    HGLOBAL hgmemWave = LoadResource(hmod, hresInfo);

    if (hgmemWave)
    {
        // Get pointer to and length of the wave image data
        m_pImageData= (BYTE*)LockResource(hgmemWave);
        m_dwImageLen = SizeofResource(hmod, hresInfo);
    }

    return (m_pImageData ? true : false);
}

bool CWave::GetFormat(WAVEFORMATEX& wfFormat) const
{
    // Check validity
    if (!IsValid())
        return false;

    // Setup and open the MMINFO structure
    CMMMemoryIOInfo mmioInfo((HPSTR)m_pImageData, m_dwImageLen);
    CMMIO           mmio(mmioInfo);

    // Find the WAVE chunk
    CMMTypeChunk mmckParent('W','A','V','E');
    mmio.Descend(mmckParent, MMIO_FINDRIFF);

    // Find and read the format subchunk
    CMMIdChunk mmckSubchunk('f','m','t',' ');
    mmio.Descend(mmckSubchunk, mmckParent, MMIO_FINDCHUNK);
    mmio.Read((HPSTR)&wfFormat, sizeof(WAVEFORMATEX));
    mmio.Ascend(mmckSubchunk);

    return true;
}

DWORD CWave::GetDataLen() const
{
    // Check validity
    if (!IsValid())
        return (DWORD)0;

    // Setup and open the MMINFO structure
    CMMMemoryIOInfo mmioInfo((HPSTR)m_pImageData, m_dwImageLen);
    CMMIO           mmio(mmioInfo);

    // Find the WAVE chunk
    CMMTypeChunk mmckParent('W','A','V','E');
    mmio.Descend(mmckParent, MMIO_FINDRIFF);

    // Find and get the size of the data subchunk
    CMMIdChunk mmckSubchunk('d','a','t','a');
    mmio.Descend(mmckSubchunk, mmckParent, MMIO_FINDCHUNK);
    return mmckSubchunk.cksize;
}

DWORD CWave::GetData(BYTE*& pWaveData, DWORD dwMaxLen) const
{
    // Check validity
    if (!IsValid())
        return (DWORD)0;

    // Setup and open the MMINFO structure
    CMMMemoryIOInfo mmioInfo((HPSTR)m_pImageData, m_dwImageLen);
    CMMIO           mmio(mmioInfo);

    // Find the WAVE chunk
    CMMTypeChunk mmckParent('W','A','V','E');
    mmio.Descend(mmckParent, MMIO_FINDRIFF);

    // Find and get the size of the data subchunk
    CMMIdChunk mmckSubchunk('d','a','t','a');
    mmio.Descend(mmckSubchunk, mmckParent, MMIO_FINDCHUNK);
    DWORD dwLenToCopy = mmckSubchunk.cksize;

    // Allocate memory if the passed in pWaveData was NULL
    if (pWaveData == NULL)
        pWaveData = (BYTE*)GlobalLock(GlobalAlloc(GMEM_MOVEABLE,
            dwLenToCopy));
    else
        // If we didn't allocate our own memory, honor dwMaxLen
        if (dwMaxLen < dwLenToCopy)
            dwLenToCopy = dwMaxLen;
    if (pWaveData)
        // Read waveform data into the buffer
        mmio.Read((HPSTR)pWaveData, dwLenToCopy);

    return dwLenToCopy;
}

bool CWave::Free()
{
    // Free any previous wave data
    if (m_pImageData) {
        HGLOBAL  hgmemWave = GlobalHandle(m_pImageData);

        if (hgmemWave) {
            if (m_bResource)
                // Free resource
                FreeResource(hgmemWave);
            else {
                // Unlock and free memory
                GlobalUnlock(hgmemWave);
                GlobalFree(hgmemWave);
            }

            m_pImageData = NULL;
            m_dwImageLen = 0;
            return true;
        }
    }
    return false;
}

bool load_sounds()
{
    // Initialize waves
	const char DSErr[] = "DirectSoundError...";
	const char DirectSoundLoadingError[] = "Direct Sound Initialisation Error...";
	HRESULT dsls;

    CWave waves[NUMSOUNDS];
    waves[0].Create("SupremeBlast.wav");
    waves[1].Create("Rebirth.wav");
    waves[2].Create("Invisibility.wav");
	waves[3].Create("SupaFly.wav");
	waves[4].Create("MenuBeat.wav");
	waves[5].Create("StringLoop.wav");

    // Initialize secondary DS buffers
    for (int i = 0; i < NUMSOUNDS; i++) {
        // Get the wave information
        DWORD         dwDataLen = waves[i].GetDataLen();
        WAVEFORMATEX  wfFormat;
        waves[i].GetFormat(wfFormat);

        // Setup the DS buffer description
        DSBUFFERDESC  dsbdDesc;
        ZeroMemory(&dsbdDesc, sizeof(DSBUFFERDESC));
        dsbdDesc.dwSize = sizeof(DSBUFFERDESC);
        dsbdDesc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN |
					DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC;
        dsbdDesc.dwBufferBytes = dwDataLen;
        dsbdDesc.lpwfxFormat = &wfFormat;

        // Create the DS buffer
	    if ((dsls = lpDS->CreateSoundBuffer(&dsbdDesc, &lpDSBSounds[i], NULL)) != DS_OK)
        {
			switch(dsls)
			{
				case DSERR_ALLOCATED:
				{	
					MessageBox(ghwnd, "Create sound buffer failed... Resources tied up.", DirectSoundLoadingError, MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				case DSERR_BADFORMAT:
				{	
					MessageBox(ghwnd, "Create sound buffer failed... Wave format not supported.", DirectSoundLoadingError, MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				case DSERR_INVALIDPARAM:
				{
					MessageBox(ghwnd, "Create sound buffer failed... Invalid parameter passed.", DirectSoundLoadingError, MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				case DSERR_NOAGGREGATION:
				{	
					MessageBox(ghwnd, "Create sound buffer failed... Aggregation not supported.", DirectSoundLoadingError, MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				case DSERR_OUTOFMEMORY:
				{	
					MessageBox(ghwnd, "Create sound buffer failed... Not enough memory available.", DirectSoundLoadingError, MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				case DSERR_UNINITIALIZED:
				{
					MessageBox(ghwnd, "Create sound buffer failed... IDirectSound::Initialize not called.", DirectSoundLoadingError, MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				case DSERR_UNSUPPORTED:
				{
					MessageBox(ghwnd, "Create sound buffer failed... Function not supported.", DirectSoundLoadingError, MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				default:
				{
					MessageBox(NULL, "Could not create sound buffer.", DSErr, MB_OK);
					break;
				}
			}

 
            return false;
        }

        // Lock the DS buffer
        BYTE* pDSBuffData;
        if (lpDSBSounds[i]->Lock(0, dwDataLen, (void**)&pDSBuffData, &dwDataLen, NULL, 0, 0) != DS_OK)
        {
            MessageBox(NULL, "Could not lock DirectSound Buffer.", DSErr, MB_OK | MB_ICONEXCLAMATION);
            return false;
        }

        // Write wave data to the DS buffer
        dwDataLen = waves[i].GetData(pDSBuffData, dwDataLen);

        // Unlock the DS buffer
        if (lpDSBSounds[i]->Unlock(pDSBuffData, dwDataLen, NULL, 0) !=
            DS_OK)
        {
            MessageBox(NULL, "Could not unlock DirectSound Buffer.", DSErr, MB_OK | MB_ICONEXCLAMATION);
            return false;
        }
    }

    return true;
}

bool DirectSoundInitialisation()
{
	const char DirectSoundInitialisationError[] = "Direct Sound Initialisation Error...";
	HRESULT				dsscl;				// SetCooperativeLevel error description.

	if(DirectSoundCreate(NULL, &lpDS, NULL) != DS_OK)
	{
		MessageBox(ghwnd, "DirectSound Initialisation failed!", DirectSoundInitialisationError, MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if((dsscl = lpDS->SetCooperativeLevel(ghwnd, DSSCL_NORMAL)) != DS_OK)
	{
		switch(dsscl)
		{
			case DSERR_ALLOCATED:
			{	
				MessageBox(ghwnd, "DirectSound cooperative level setting failed... Resources tied up.", DirectSoundInitialisationError, MB_OK | MB_ICONEXCLAMATION);
				break;
			}
			case DSERR_INVALIDPARAM:
			{
				MessageBox(ghwnd, "DirectSound cooperative level setting failed... Invalid parameter passed.", DirectSoundInitialisationError, MB_OK | MB_ICONEXCLAMATION);
				break;
			}
			case DSERR_UNINITIALIZED:
			{
				MessageBox(ghwnd, "DirectSound cooperative level setting failed... IDirectSound::Initialize not called.", DirectSoundInitialisationError, MB_OK | MB_ICONEXCLAMATION);
				break;
			}
			case DSERR_UNSUPPORTED:
			{
				MessageBox(ghwnd, "DirectSound cooperative level setting failed... Function not supported.", DirectSoundInitialisationError, MB_OK | MB_ICONEXCLAMATION);
				break;
			}
			default:
				break;
		}

			return false;
	}

	if(!load_sounds())
	{
		MessageBox(ghwnd, "Sound file loading for DirectSound failed!", DirectSoundInitialisationError, MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}
// Backup default sound playing function...
void SoundEffect(const char *soundWave)
{
	PlaySound(soundWave, NULL, SND_ASYNC | SND_NODEFAULT);
}

void PlayRebirthSpeechAudio()
{
	lpDSBSounds[1]->GetStatus(&dwStatus);
	if(dwStatus & DSBSTATUS_BUFFERLOST)
		RestoreDSBuffers();
	else
	{
		lpDSBSounds[1]->SetFrequency(37500);
		lpDSBSounds[1]->SetVolume(DSBVOLUME_MAX);
		if (!(dwStatus & DSBSTATUS_PLAYING))
			lpDSBSounds[1]->Play(0, 0, 0);
		else
		if(dwStatus & DSBSTATUS_PLAYING)
			lpDSBSounds[1]->Stop();
	}
}

void PlaySupremeBlastSpeechAudio()
{
	// Supreme Blast Speech.
	lpDSBSounds[0]->GetStatus(&dwStatus);
	if(dwStatus & DSBSTATUS_BUFFERLOST)
		RestoreDSBuffers();
	else
	{
		lpDSBSounds[0]->SetFrequency(37500);
		lpDSBSounds[0]->SetVolume(DSBVOLUME_MAX);
		if (!(dwStatus & DSBSTATUS_PLAYING))
			lpDSBSounds[0]->Play(0, 0, 0);
		else
		if(dwStatus & DSBSTATUS_PLAYING)
			lpDSBSounds[0]->Stop();
	}
}

void PlayInvisibilitySpeechAudio()
{
	if(dwStatus & DSBSTATUS_BUFFERLOST)
		RestoreDSBuffers();
	else
	{
		lpDSBSounds[2]->SetFrequency(37500);
		lpDSBSounds[2]->SetVolume(DSBVOLUME_MAX);
		if (!(dwStatus & DSBSTATUS_PLAYING))
			lpDSBSounds[2]->Play(0, 0, 0);
		else
		if(dwStatus & DSBSTATUS_PLAYING)
			lpDSBSounds[2]->Stop();
	}
}

void PlaySupaFlySpeechAudio()
{
	lpDSBSounds[2]->GetStatus(&dwStatus);
	if(dwStatus & DSBSTATUS_BUFFERLOST)
		RestoreDSBuffers();
	else
	{
		lpDSBSounds[3]->SetFrequency(37500);
		lpDSBSounds[3]->SetVolume(DSBVOLUME_MAX);
		if (!(dwStatus & DSBSTATUS_PLAYING))
			lpDSBSounds[3]->Play(0, 0, 0);
		else
		if(dwStatus & DSBSTATUS_PLAYING)
			lpDSBSounds[3]->Stop();
	}
}

void PlayPAGoEMusic()
{
	// Revert to standard sound...
	PlaySound(NULL, NULL, SND_NODEFAULT); // Stop previous sound!
	PlaySound("PAGoE.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);
}

void PlayStandardMusic()
{
	// Play Rebirth music.
	PlaySound(NULL, NULL, SND_NODEFAULT); // Stop previous sound!
	PlaySound("StringLoop.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);
}

void PlayRebirthMusic()
{
	// Play Rebirth music.
	PlaySound(NULL, NULL, SND_NODEFAULT); // Stop previous sound!
	PlaySound("RebirthBeat.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);
}

void PlaySupremeBlastMusic()
{
	// Play Supreme Blast music.
	PlaySound(NULL, NULL, SND_NODEFAULT); // Stop previous sound!
	PlaySound("SupBlastBeat.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);
}

void PlayInvisibilityMusic()
{
	// Play Invisibility music.
	PlaySound(NULL, NULL, SND_NODEFAULT); // Stop previous sound!
	PlaySound("InvisiBeat.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);
}

void PlaySupaFlyMusic()
{
	// Play SupaFly music.
	PlaySound(NULL, NULL, SND_NODEFAULT); // Stop previous sound!
	PlaySound("SupaFlyBeatMachine.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);
}

void PlayMenuMusic()
{
	// Play SupaFly music.
	PlaySound(NULL, NULL, SND_NODEFAULT); // Stop previous sound!
	PlaySound("MenuBeat.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);
}

bool RestoreDSBuffers()
{
	for(int i = 0; i < NUMSOUNDS; i++)
		if(lpDSBSounds[i]->Restore() != DS_OK)
		{
			MessageBox(NULL, "Buffer restoration failed.", "DirectSound Restore Buffers", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

	return load_sounds();
}

bool DeleteDirectSoundResources()
{
	if(lpDS)
	{
		lpDS->Release();
		lpDS = NULL;
		return true;
	}

	// Just incase something should go wrong.
	MessageBox(NULL, "Sound buffer freed without having been acquired.", "DirectSound Error", MB_OK | MB_ICONEXCLAMATION);
	return false;
}