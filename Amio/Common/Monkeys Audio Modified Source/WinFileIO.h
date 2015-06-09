#ifdef IO_USE_WIN_FILE_IO

#ifndef _winfileio_h_
#define _winfileio_h_

#include "ApeIO.h"

class CWinFileIO : public CIO
{
public:

    // construction / destruction
    CWinFileIO();
    ~CWinFileIO();

    // open / close
    int Open(const char * pName);
    int Close();
    
    // read / write
    int Read(void * pBuffer, unsigned int nBytesToRead, unsigned int * pBytesRead);
    int Write(const void * pBuffer, unsigned int nBytesToWrite, unsigned int * pBytesWritten);
    
    // seek
    int Seek(int nDistance, unsigned int nMoveMode);
    
    // other functions
    int SetEOF();

    // creation / destruction
    int Create(const char * pName);
    int Delete();

    // attributes
    int GetPosition();
    int GetSize();
    int GetName(char * pBuffer);

private:
    
    HANDLE        m_hFile;
    char        m_cFileName[MAX_PATH];
    BOOL        m_bReadOnly;
};


#endif //_winfileio_h_

#endif //IO_USE_WIN_FILE_IO
