/*!\file MMIO.h
 * \brief Borrowed from 'Sam's Teach Yourself Directx 7 in 24 Hours'.
 * \date August 2004
 */

#ifndef __MMIO_H__
#define __MMIO_H__

#include <MMSystem.h>

class CMMChunk : public MMCKINFO
{
protected:
    CMMChunk() { };
};

class CMMIdChunk : public CMMChunk
{
public:
    CMMIdChunk(char c0, char c1, char c2, char c3);
    CMMIdChunk(LPCSTR psz, UINT uiFlags = 0u);
};

class CMMTypeChunk : public CMMChunk
{
public:
    CMMTypeChunk(char c0, char c1, char c2, char c3);
    CMMTypeChunk(LPCSTR psz, UINT uiFlags = 0u);
};

class CMMIOInfo : public MMIOINFO
{
public:
    CMMIOInfo();
};


class CMMMemoryIOInfo : public CMMIOInfo
{
public:
    CMMMemoryIOInfo(LONG lBuffer, DWORD dwMinExpansion = 0);
    CMMMemoryIOInfo(HPSTR pBuffer, LONG lBuffer, DWORD
        dwMinExpansion = 0);
};

class CMMIO
{
public:
                CMMIO();
                CMMIO(HMMIO hmmio);
                CMMIO(const char* pszFileName, DWORD dwOpenFlag =
                    MMIO_READ);
                CMMIO(CMMMemoryIOInfo& mmioinfo);
public:
    void        Open(const char* pszFileName,
                    DWORD dwOpenFlags = MMIO_READ);
    void        Open(CMMMemoryIOInfo &mmioinfo);
    MMRESULT    Close(UINT uiFlags = 0u);

    MMRESULT    Ascend(CMMChunk &mmckInfo, UINT uiFlags = 0u);
    MMRESULT    Descend(CMMChunk &mmckInfo, UINT uiFlags = 0u);
    MMRESULT    Descend(CMMChunk &mmckInfo, CMMChunk &mmckParent,
                    UINT uiFlags = 0u);

    LONG        Read(HPSTR pData, LONG lLen);
    LONG        Write(const char* pData, LONG lLen);
    LONG        Seek(LONG lOffset, int iOrigin);

    LRESULT     SendMessage(UINT uiMsg, LPARAM lParam1,
                    LPARAM lParam2);
    MMRESULT    SetBuffer(LPSTR pBuffer, LONG lBuffer,
                    UINT uiFlags = 0u);

    MMRESULT    GetInfo(CMMIOInfo &, UINT uiFlags = 0);
    MMRESULT    SetInfo(CMMIOInfo &, UINT uiFlags = 0);
    MMRESULT    Advance(CMMIOInfo &, UINT uiFlags);
public:
    HMMIO m_hmmio;
};

inline CMMIdChunk::CMMIdChunk(char c0, char c1, char c2, char c3)
{
    ckid = mmioFOURCC(c0, c1, c2, c3);
}

inline CMMIdChunk::CMMIdChunk(LPCSTR psz, UINT uiFlags)
{
    ckid = ::mmioStringToFOURCC(psz, uiFlags);
}

inline CMMTypeChunk::CMMTypeChunk(char c0, char c1, char c2, char c3)
{
    fccType = mmioFOURCC(c0, c1, c2, c3);
}

inline CMMTypeChunk::CMMTypeChunk(LPCSTR psz, UINT uiFlags)
{
    fccType = ::mmioStringToFOURCC(psz, uiFlags);
}

inline CMMIOInfo::CMMIOInfo()
{
    ::ZeroMemory(this, sizeof(MMIOINFO));
}

inline CMMMemoryIOInfo::CMMMemoryIOInfo(LONG lBuffer,
    DWORD dwMinExpansion)
{
    pIOProc = NULL;
    fccIOProc = FOURCC_MEM;
    pchBuffer = NULL;
    cchBuffer = lBuffer;
    adwInfo[0] = dwMinExpansion;
}

inline CMMMemoryIOInfo::CMMMemoryIOInfo(HPSTR pBuffer, LONG cchBuf,
    DWORD dwMinExpansion)
{
    pIOProc = NULL;
    fccIOProc = FOURCC_MEM;
    pchBuffer = pBuffer;
    cchBuffer = cchBuf;
    adwInfo[0] = dwMinExpansion;
}

inline CMMIO::CMMIO() : m_hmmio(NULL)
{
}

inline
CMMIO::CMMIO(HMMIO hmmio) : m_hmmio(hmmio)
{
}

inline CMMIO::CMMIO(const char* pszFileName, DWORD dwOpenFlag)
{
    Open(pszFileName, dwOpenFlag);
}

inline CMMIO::CMMIO(CMMMemoryIOInfo &mmioinfo)
{
    Open(mmioinfo);
}

inline MMRESULT CMMIO::Close(UINT uiFlags)
{
    MMRESULT mmr = ::mmioClose(m_hmmio, uiFlags);  
    m_hmmio = NULL;
    return mmr;
}

inline LONG CMMIO::Read(HPSTR pData, LONG lLen)
{
    return ::mmioRead(m_hmmio, pData, lLen);
}

inline MMRESULT CMMIO::Ascend(CMMChunk &mmckInfo, UINT uiFlags)
{
    return ::mmioAscend(m_hmmio, &mmckInfo, uiFlags);
}

inline MMRESULT CMMIO::Descend(CMMChunk &mmckInfo, UINT uiFlags)
{
    return ::mmioDescend(m_hmmio, &mmckInfo, 0, uiFlags);
}

inline MMRESULT CMMIO::Descend(CMMChunk &mmckInfo,
    CMMChunk &mmckParent, UINT uiFlags)
{
    return ::mmioDescend(m_hmmio, &mmckInfo, &mmckParent, uiFlags);
}

inline LONG CMMIO::Seek(LONG lOffset, int iOrigin)
{
    return ::mmioSeek(m_hmmio, lOffset, iOrigin);
}

inline LRESULT CMMIO::SendMessage(UINT uiMsg, LPARAM lParam1,
    LPARAM lParam2)
{
    return ::mmioSendMessage(m_hmmio, uiMsg, lParam1, lParam2);
}

inline MMRESULT CMMIO::SetBuffer(LPSTR pBuffer, LONG lBuffer,
    UINT uiFlags)
{
    return ::mmioSetBuffer(m_hmmio, pBuffer, lBuffer, uiFlags);  
}

inline LONG CMMIO::Write(const char* pData, LONG lLen)
{
    return ::mmioWrite(m_hmmio, pData, lLen);
}

inline MMRESULT CMMIO::GetInfo(CMMIOInfo &Info, UINT uiFlags)
{
    return ::mmioGetInfo(m_hmmio, &Info, uiFlags);
}

inline MMRESULT CMMIO::SetInfo(CMMIOInfo &Info, UINT uiFlags)
{
    return ::mmioSetInfo(m_hmmio, &Info, uiFlags);
}

inline MMRESULT CMMIO::Advance(CMMIOInfo &Info, UINT uiFlags)
{
    return ::mmioAdvance(m_hmmio, &Info, uiFlags);
}

#endif
