/*!\file MMIO.cpp
 * \brief Borrowed from 'Sam's Teach Yourself Directx 7 in 24 Hours'.
 * \date August 2004
 */

#ifndef _MMIOSUPPORT_
#define _MMIOSUPPORT_

#include <windows.h>
#include "MMIO.h"

// MMIO support functions.

void CMMIO::Open(const char* pszFileName, DWORD dwOpenFlags)
{
    m_hmmio = mmioOpen((char*)pszFileName, NULL, dwOpenFlags);
}

void CMMIO::Open(CMMMemoryIOInfo &mmioinfo)
{
    m_hmmio = mmioOpen(NULL, &mmioinfo, MMIO_READWRITE);
}

#endif

