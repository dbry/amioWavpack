#include <windows.h>
#include "All.h"

#ifdef IO_USE_WIN_FILE_IO

#include "WinFileIO.h"
#include "CharacterHelper.h"

CWinFileIO::CWinFileIO()
{
    m_hFile = INVALID_HANDLE_VALUE;
    memset(m_cFileName, 0, MAX_PATH);
    m_bReadOnly = FALSE;
}

CWinFileIO::~CWinFileIO()
{
    Close();
}

int CWinFileIO::Open(const char * pName)
{
    Close();

	const int kNumWideCharacters = 4096;
	WCHAR wcharBuffer[kNumWideCharacters] = {0};
	MultiByteToWideChar(CP_UTF8, 0, pName, -1, wcharBuffer, kNumWideCharacters);

    m_hFile = ::CreateFileW(wcharBuffer, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_hFile == INVALID_HANDLE_VALUE) 
    {
        m_hFile = ::CreateFileW(wcharBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_hFile == INVALID_HANDLE_VALUE) 
        {
            return -1;
        }
        else 
        {
            m_bReadOnly = TRUE;
        }
    }
    else
    {
        m_bReadOnly = FALSE;
    }
    
    strcpy(m_cFileName, pName);

    return 0;
}

int CWinFileIO::Close()
{
    SAFE_FILE_CLOSE(m_hFile);

    return 0;
}
    
int CWinFileIO::Read(void * pBuffer, unsigned int nBytesToRead, unsigned int * pBytesRead)
{
    unsigned int nTotalBytesRead = 0;
    int nBytesLeft = nBytesToRead;
    BOOL bRetVal = TRUE;
    unsigned char * pucBuffer = (unsigned char *) pBuffer;

    *pBytesRead = 1;
    while ((nBytesLeft > 0) && (*pBytesRead > 0) && bRetVal)
    {
        bRetVal = ::ReadFile(m_hFile, &pucBuffer[nBytesToRead - nBytesLeft], nBytesLeft, (unsigned long *) pBytesRead, NULL);
        if (bRetVal == TRUE)
        {
            nBytesLeft -= *pBytesRead;
            nTotalBytesRead += *pBytesRead;
        }
    }
    
    *pBytesRead = nTotalBytesRead;
    
    return (bRetVal == FALSE) ? ERROR_IO_READ : 0;
}

int CWinFileIO::Write(const void * pBuffer, unsigned int nBytesToWrite, unsigned int * pBytesWritten)
{
    BOOL bRetVal = WriteFile(m_hFile, pBuffer, nBytesToWrite, (unsigned long *) pBytesWritten, NULL);

    if ((bRetVal == 0) || (*pBytesWritten != nBytesToWrite))
        return ERROR_IO_WRITE;
    else
        return 0;
}
    
int CWinFileIO::Seek(int nDistance, unsigned int nMoveMode)
{
    SetFilePointer(m_hFile, nDistance, NULL, nMoveMode);
    return 0;
}
    
int CWinFileIO::SetEOF()
{
    BOOL bRetVal = SetEndOfFile(m_hFile);
    if (bRetVal == FALSE)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int CWinFileIO::GetPosition()
{
    return SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
}

int CWinFileIO::GetSize()
{
    return GetFileSize(m_hFile, NULL);
}

int CWinFileIO::GetName(char * pBuffer)
{
    strcpy(pBuffer, m_cFileName);
    return 0;
}

int CWinFileIO::Create(const char * pName)
{
    Close();

	const int kNumWideCharacters = 4096;
	WCHAR wcharBuffer[kNumWideCharacters] = {0};
	MultiByteToWideChar(CP_UTF8, 0, pName, -1, wcharBuffer, kNumWideCharacters);

	m_hFile = ::CreateFileW(wcharBuffer, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_hFile == INVALID_HANDLE_VALUE) { return -1; }

    m_bReadOnly = FALSE;
    
    strcpy(m_cFileName, pName);

    return 0;
}

int CWinFileIO::Delete()
{
    Close();

	const int kNumWideCharacters = 4096;
	WCHAR wcharBuffer[kNumWideCharacters] = {0};
	MultiByteToWideChar(CP_UTF8, 0, m_cFileName, -1, wcharBuffer, kNumWideCharacters);

	return ::DeleteFileW(wcharBuffer) ? 0 : -1;
}

#endif // #ifdef IO_USE_WIN_FILE_IO
