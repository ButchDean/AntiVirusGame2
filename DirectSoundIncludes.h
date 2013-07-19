/*!\file DirectSoundIncludes.h
 * \brief Function prototypes.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

#ifndef _WAVE_
#define _WAVE_

#include <MMSystem.h>

class CWave {
    // Public Constructor(s)/Destructor
public:
            CWave();
            CWave(const char* pszFileName);
            CWave(UINT uiResID, HMODULE hmod);
    virtual ~CWave();

  // Public Methods
public:
    bool    Create(const char* pszFileName);
    bool    Create(UINT uiResID, HMODULE hmod);
    bool    IsValid() const { return (m_pImageData ? TRUE : FALSE); };
    bool    GetFormat(WAVEFORMATEX& wfFormat) const;
    DWORD   GetDataLen() const;
    DWORD   GetData(BYTE*& pWaveData, DWORD dwMaxToCopy) const;

    // Protected Methods
protected:
    bool    Free();

    // Private Data
private:
    BYTE* m_pImageData;
    DWORD m_dwImageLen;
    bool  m_bResource;
};

bool load_sounds();
bool DirectSoundInitialisation();
void SoundEffect(const char *soundWave); // Backup routine for testing
void PlayRebirthSpeechAudio();
void PlaySupremeBlastSpeechAudio();
void PlayInvisibilitySpeechAudio();
void PlaySupaFlySpeechAudio();
void PlayPAGoEMusic();
void PlayStandardMusic();
void PlayRebirthMusic();
void PlaySupremeBlastMusic();
void PlayInvisibilityMusic();
void PlaySupaFlyMusic();
void PlayMenuMusic();
bool RestoreDSBuffers();
bool DeleteDirectSoundResources();

#endif
