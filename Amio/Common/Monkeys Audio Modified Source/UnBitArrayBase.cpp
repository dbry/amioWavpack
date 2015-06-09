#include "All.h"
#include "UnBitArrayBase.h"
#include "APEInfo.h"
#include "UnBitArray.h"
#ifdef BACKWARDS_COMPATIBILITY
    #include "Old/APEDecompressOld.h"
    #include "Old/UnBitArrayOld.h"
#endif

const uint32 POWERS_OF_TWO_MINUS_ONE[33] = {0LL,1LL,3LL,7LL,15LL,31LL,63LL,127LL,255LL,511LL,1023LL,2047LL,4095LL,8191LL,16383LL,32767LL,65535LL,131071LL,262143LL,524287LL,1048575LL,2097151LL,4194303LL,8388607LL,16777215LL,33554431LL,67108863LL,134217727LL,268435455LL,536870911LL,1073741823LL,2147483647LL,4294967295LL};

CUnBitArrayBase * CreateUnBitArray(IAPEDecompress * pAPEDecompress, int nVersion)
{
    // determine the furthest position we should read in the I/O object
    int nFurthestReadByte = GET_IO(pAPEDecompress)->GetSize();
    if (nFurthestReadByte > 0)
    {
        nFurthestReadByte -= pAPEDecompress->GetInfo(APE_INFO_WAV_TERMINATING_BYTES);

        CAPETag * pAPETag = (CAPETag *) pAPEDecompress->GetInfo(APE_INFO_TAG);
        if (pAPETag != NULL)
            nFurthestReadByte -= pAPETag->GetTagBytes();
    }

    // create the appropriate object
#ifdef BACKWARDS_COMPATIBILITY
    if (nVersion >= 3900)
        return (CUnBitArrayBase * ) new CUnBitArray(GET_IO(pAPEDecompress), nVersion, nFurthestReadByte);
    else
        return (CUnBitArrayBase * ) new CUnBitArrayOld(pAPEDecompress, nVersion, nFurthestReadByte);
#else
    return (CUnBitArrayBase * ) new CUnBitArray(GET_IO(pAPEDecompress), nVersion, nFurthestReadByte);
#endif
}

CUnBitArrayBase::CUnBitArrayBase(int nFurthestReadByte)
{
    m_nFurthestReadByte = nFurthestReadByte;
}

CUnBitArrayBase::~CUnBitArrayBase()
{
}

void CUnBitArrayBase::AdvanceToByteBoundary() 
{
    int nMod = m_nCurrentBitIndex % 8;
    if (nMod != 0) { m_nCurrentBitIndex += 8 - nMod; }
}

uint32 CUnBitArrayBase::DecodeValueXBits(uint32 nBits) 
{
    // get more data if necessary
    if ((m_nCurrentBitIndex + nBits) >= m_nBits)
        FillBitArray();

    // variable declares
    uint32 nLeftBits = 32 - (m_nCurrentBitIndex & 31);
    uint32 nBitArrayIndex = m_nCurrentBitIndex >> 5;
    m_nCurrentBitIndex += nBits;
    
    // if their isn't an overflow to the right value, get the value and exit
    if (nLeftBits >= nBits)
        return (m_pBitArray[nBitArrayIndex] & (POWERS_OF_TWO_MINUS_ONE[nLeftBits])) >> (nLeftBits - nBits);
    
    // must get the "split" value from left and right
    int nRightBits = nBits - nLeftBits;
    
    uint32 nLeftValue = ((m_pBitArray[nBitArrayIndex] & POWERS_OF_TWO_MINUS_ONE[nLeftBits]) << nRightBits);
    uint32 nRightValue = (m_pBitArray[nBitArrayIndex + 1] >> (32 - nRightBits));
    return (nLeftValue | nRightValue);
}

int CUnBitArrayBase::FillAndResetBitArray(int nFileLocation, int nNewBitIndex) 
{
    // seek if necessary
    if (nFileLocation != -1)
    {
        if (m_pIO->Seek(nFileLocation, FILE_BEGIN) != 0)
            return ERROR_IO_READ;
    }

	// fill
	m_nCurrentBitIndex = m_nBits; // position at the end of the buffer
	int nRetVal = FillBitArray();

	// set bit index
	m_nCurrentBitIndex = nNewBitIndex;

    return nRetVal;
}

int CUnBitArrayBase::FillBitArray() 
{
    // get the bit array index
    uint32 nBitArrayIndex = m_nCurrentBitIndex >> 5;
    
    // move the remaining data to the front
    memmove((void *) (m_pBitArray), (const void *) (m_pBitArray + nBitArrayIndex), m_nBytes - (nBitArrayIndex * 4));

    // get the number of bytes to read
    int nBytesToRead = nBitArrayIndex * 4;
    if (m_nFurthestReadByte > 0)
	{
		int nFurthestReadBytes = m_nFurthestReadByte - m_pIO->GetPosition();
		if (nBytesToRead > nFurthestReadBytes)
			nBytesToRead = nFurthestReadBytes;
	}

    // read the new data
    unsigned int nBytesRead = 0;
    int nRetVal = m_pIO->Read((unsigned char *) (m_pBitArray + m_nElements - nBitArrayIndex), nBytesToRead, &nBytesRead);

    // zero anything at the tail we didn't fill
	m_nGoodBytes = ((m_nElements - nBitArrayIndex) * 4) + nBytesRead;
    if (m_nGoodBytes < m_nBytes)
        memset(&((unsigned char *) m_pBitArray)[m_nGoodBytes], 0, m_nBytes - m_nGoodBytes);

    // adjust the m_Bit pointer
    m_nCurrentBitIndex = m_nCurrentBitIndex & 31;
    
    // return
    return (nRetVal == 0) ? 0 : ERROR_IO_READ;
}

int CUnBitArrayBase::CreateHelper(CIO * pIO, int nBytes, int nVersion)
{
    // check the parameters
    if ((pIO == NULL) || (nBytes <= 0)) { return ERROR_BAD_PARAMETER; }

    // save the size
    m_nElements = nBytes / 4;
    m_nBytes = m_nElements * 4;
    m_nBits = m_nBytes * 8;
	m_nGoodBytes = 0;
    
    // set the variables
    m_pIO = pIO;
    m_nVersion = nVersion;
    m_nCurrentBitIndex = 0;
    
    // create the bitarray (we allocate and empty a little extra as buffer insurance, although it should never be necessary)
    m_pBitArray = new uint32 [m_nElements + 64];
    memset(m_pBitArray, 0, (m_nElements + 64) * sizeof(uint32));
    
    return (m_pBitArray != NULL) ? 0 : ERROR_INSUFFICIENT_MEMORY;
}
