#include "All.h"
#include "Prepare.h"

const uint32 CRC32_TABLE[256] = {0LL,1996959894LL,3993919788LL,2567524794LL,124634137LL,1886057615LL,3915621685LL,2657392035LL,249268274LL,2044508324LL,3772115230LL,2547177864LL,162941995LL,2125561021LL,3887607047LL,2428444049LL,498536548LL,1789927666LL,4089016648LL,2227061214LL,450548861LL,1843258603LL,4107580753LL,2211677639LL,325883990LL,1684777152LL,4251122042LL,2321926636LL,335633487LL,1661365465LL,4195302755LL,2366115317LL,997073096LL,1281953886LL,3579855332LL,2724688242LL,1006888145LL,1258607687LL,3524101629LL,2768942443LL,901097722LL,1119000684LL,3686517206LL,2898065728LL,853044451LL,1172266101LL,3705015759LL,2882616665LL,651767980LL,1373503546LL,3369554304LL,3218104598LL,565507253LL,1454621731LL,3485111705LL,3099436303LL,671266974LL,1594198024LL,3322730930LL,2970347812LL,795835527LL,1483230225LL,3244367275LL,3060149565LL,1994146192LL,31158534LL,2563907772LL,4023717930LL,1907459465LL,112637215LL,2680153253LL,3904427059LL,2013776290LL,251722036LL,2517215374LL,3775830040LL,2137656763LL,141376813LL,2439277719LL,3865271297LL,1802195444LL,476864866LL,2238001368LL,
    4066508878LL,1812370925LL,453092731LL,2181625025LL,4111451223LL,1706088902LL,314042704LL,2344532202LL,4240017532LL,1658658271LL,366619977LL,2362670323LL,4224994405LL,1303535960LL,984961486LL,2747007092LL,3569037538LL,1256170817LL,1037604311LL,2765210733LL,3554079995LL,1131014506LL,879679996LL,2909243462LL,3663771856LL,1141124467LL,855842277LL,2852801631LL,3708648649LL,1342533948LL,654459306LL,3188396048LL,3373015174LL,1466479909LL,544179635LL,3110523913LL,3462522015LL,1591671054LL,702138776LL,2966460450LL,3352799412LL,1504918807LL,783551873LL,3082640443LL,3233442989LL,3988292384LL,2596254646LL,62317068LL,1957810842LL,3939845945LL,2647816111LL,81470997LL,1943803523LL,3814918930LL,2489596804LL,225274430LL,2053790376LL,3826175755LL,2466906013LL,167816743LL,2097651377LL,4027552580LL,2265490386LL,503444072LL,1762050814LL,4150417245LL,2154129355LL,426522225LL,1852507879LL,4275313526LL,2312317920LL,282753626LL,1742555852LL,4189708143LL,2394877945LL,397917763LL,1622183637LL,3604390888LL,2714866558LL,953729732LL,1340076626LL,3518719985LL,2797360999LL,1068828381LL,1219638859LL,3624741850LL,
    2936675148LL,906185462LL,1090812512LL,3747672003LL,2825379669LL,829329135LL,1181335161LL,3412177804LL,3160834842LL,628085408LL,1382605366LL,3423369109LL,3138078467LL,570562233LL,1426400815LL,3317316542LL,2998733608LL,733239954LL,1555261956LL,3268935591LL,3050360625LL,752459403LL,1541320221LL,2607071920LL,3965973030LL,1969922972LL,40735498LL,2617837225LL,3943577151LL,1913087877LL,83908371LL,2512341634LL,3803740692LL,2075208622LL,213261112LL,2463272603LL,3855990285LL,2094854071LL,198958881LL,2262029012LL,4057260610LL,1759359992LL,534414190LL,2176718541LL,4139329115LL,1873836001LL,414664567LL,2282248934LL,4279200368LL,1711684554LL,285281116LL,2405801727LL,4167216745LL,1634467795LL,376229701LL,2685067896LL,3608007406LL,1308918612LL,956543938LL,2808555105LL,3495958263LL,1231636301LL,1047427035LL,2932959818LL,3654703836LL,1088359270LL,936918000LL,2847714899LL,3736837829LL,1202900863LL,817233897LL,3183342108LL,3401237130LL,1404277552LL,615818150LL,3134207493LL,3453421203LL,1423857449LL,601450431LL,3009837614LL,3294710456LL,1567103746LL,711928724LL,3020668471LL,3272380065LL,1510334235LL,755167117LL};

int CPrepare::Prepare(const unsigned char * pRawData, int nBytes, const WAVEFORMATEX * pWaveFormatEx, int * pOutputX, int *pOutputY, unsigned int *pCRC, int *pSpecialCodes, int *pPeakLevel)
{
    // error check the parameters
    if (pRawData == NULL || pWaveFormatEx == NULL)
        return ERROR_BAD_PARAMETER;

    // initialize the pointers that got passed in
    *pCRC = 0xFFFFFFFF;
    *pSpecialCodes = 0;

    // variables
    uint32 CRC = 0xFFFFFFFF;
    const int nTotalBlocks = nBytes / pWaveFormatEx->nBlockAlign;
    int R,L;

    // the prepare code

    if (pWaveFormatEx->wBitsPerSample == 8) 
    {
        if (pWaveFormatEx->nChannels == 2) 
        {
            for (int nBlockIndex = 0; nBlockIndex < nTotalBlocks; nBlockIndex++) 
            {
                R = (int) (*((unsigned char *) pRawData) - 128);
                L = (int) (*((unsigned char *) (pRawData + 1)) - 128);

                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                
                // check the peak
                if (labs(L) > *pPeakLevel)
                    *pPeakLevel = labs(L);
                if (labs(R) > *pPeakLevel)
                    *pPeakLevel = labs(R);

                // convert to x,y
                pOutputY[nBlockIndex] = L - R;
                pOutputX[nBlockIndex] = R + (pOutputY[nBlockIndex] / 2);
            }
        }
        else if (pWaveFormatEx->nChannels == 1) 
        {
            for (int nBlockIndex = 0; nBlockIndex < nTotalBlocks; nBlockIndex++) 
            {
                R = (int) (*((unsigned char *) pRawData) - 128);
                                
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                
                // check the peak
                if (labs(R) > *pPeakLevel)
                    *pPeakLevel = labs(R);

                // convert to x,y
                pOutputX[nBlockIndex] = R;
            }
        }
    }
    else if (pWaveFormatEx->wBitsPerSample == 24) 
    {
        if (pWaveFormatEx->nChannels == 2) 
        {
            for (int nBlockIndex = 0; nBlockIndex < nTotalBlocks; nBlockIndex++) 
            {
                uint32 nTemp = 0;
                
                nTemp |= (*pRawData << 0);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];

                nTemp |= (*pRawData << 8);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];

                nTemp |= (*pRawData << 16);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];

                if (nTemp & 0x800000)
                    R = (int) (nTemp & 0x7FFFFF) - 0x800000;
                else
                    R = (int) (nTemp & 0x7FFFFF);

                nTemp = 0;

                nTemp |= (*pRawData << 0);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                
                nTemp |= (*pRawData << 8);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                
                nTemp |= (*pRawData << 16);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                                
                if (nTemp & 0x800000)
                    L = (int) (nTemp & 0x7FFFFF) - 0x800000;
                else
                    L = (int) (nTemp & 0x7FFFFF);

                // check the peak
                if (labs(L) > *pPeakLevel)
                    *pPeakLevel = labs(L);
                if (labs(R) > *pPeakLevel)
                    *pPeakLevel = labs(R);

                // convert to x,y
                pOutputY[nBlockIndex] = L - R;
                pOutputX[nBlockIndex] = R + (pOutputY[nBlockIndex] / 2);

            }
        }
        else if (pWaveFormatEx->nChannels == 1) 
        {
            for (int nBlockIndex = 0; nBlockIndex < nTotalBlocks; nBlockIndex++) 
            {
                uint32 nTemp = 0;
                
                nTemp |= (*pRawData << 0);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                
                nTemp |= (*pRawData << 8);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                
                nTemp |= (*pRawData << 16);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                
                if (nTemp & 0x800000)
                    R = (int) (nTemp & 0x7FFFFF) - 0x800000;
                else
                    R = (int) (nTemp & 0x7FFFFF);
    
                // check the peak
                if (labs(R) > *pPeakLevel)
                    *pPeakLevel = labs(R);

                // convert to x,y
                pOutputX[nBlockIndex] = R;
            }
        }
    }
    else 
    {
        if (pWaveFormatEx->nChannels == 2) 
        {
            int LPeak = 0;
            int RPeak = 0;
            int nBlockIndex = 0;
            for (nBlockIndex = 0; nBlockIndex < nTotalBlocks; nBlockIndex++) 
            {

                R = (int) *((int16 *) pRawData);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];

                L = (int) *((int16 *) pRawData);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];

                // check the peak
                if (labs(L) > LPeak)
                    LPeak = labs(L);
                if (labs(R) > RPeak)
                    RPeak = labs(R);

                // convert to x,y
                pOutputY[nBlockIndex] = L - R;
                pOutputX[nBlockIndex] = R + (pOutputY[nBlockIndex] / 2);
            }

            if (LPeak == 0) { *pSpecialCodes |= SPECIAL_FRAME_LEFT_SILENCE; }
            if (RPeak == 0) { *pSpecialCodes |= SPECIAL_FRAME_RIGHT_SILENCE; }
            if (max(LPeak, RPeak) > *pPeakLevel) 
            {
                *pPeakLevel = max(LPeak, RPeak);
            }

            // check for pseudo-stereo files
            nBlockIndex = 0;
            while (pOutputY[nBlockIndex++] == 0) 
            {
                if (nBlockIndex == (nBytes / 4)) 
                {
                    *pSpecialCodes |= SPECIAL_FRAME_PSEUDO_STEREO;
                    break;
                }
            }

        }
        else if (pWaveFormatEx->nChannels == 1) 
        {
            int nPeak = 0;
            for (int nBlockIndex = 0; nBlockIndex < nTotalBlocks; nBlockIndex++) 
            {
                R = (int) *((int16 *) pRawData);
                
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *pRawData++];
                
                // check the peak
                if (labs(R) > nPeak)
                    nPeak = labs(R);

                //convert to x,y
                pOutputX[nBlockIndex] = R;
            }

            if (nPeak > *pPeakLevel)
                *pPeakLevel = nPeak;
            if (nPeak == 0) { *pSpecialCodes |= SPECIAL_FRAME_MONO_SILENCE; }
        }
    }

    CRC = CRC ^ 0xFFFFFFFF;

    // add the special code
    CRC >>= 1;

    if (*pSpecialCodes != 0) 
    {
        CRC |= (1 << 31);
    }

    *pCRC = CRC;

    return ERROR_SUCCESS;
}

void CPrepare::Unprepare(int X, int Y, const WAVEFORMATEX * pWaveFormatEx, unsigned char * pOutput, unsigned int * pCRC)
{
    #define CALCULATE_CRC_BYTE    *pCRC = (*pCRC >> 8) ^ CRC32_TABLE[(*pCRC & 0xFF) ^ *pOutput++];
    // decompress and convert from (x,y) -> (l,r)
    // sort of long and ugly.... sorry
    
    if (pWaveFormatEx->nChannels == 2) 
    {
        if (pWaveFormatEx->wBitsPerSample == 16) 
        {
            // get the right and left values
            int nR = X - (Y / 2);
            int nL = nR + Y;

            // error check (for overflows)
            if ((nR < -32768) || (nR > 32767) || (nL < -32768) || (nL > 32767))
            {
                throw(-1);
            }

            *(int16 *) pOutput = (int16) nR;
            CALCULATE_CRC_BYTE
            CALCULATE_CRC_BYTE
                
            *(int16 *) pOutput = (int16) nL;
            CALCULATE_CRC_BYTE
            CALCULATE_CRC_BYTE
        }
        else if (pWaveFormatEx->wBitsPerSample == 8) 
        {
            unsigned char R = (X - (Y / 2) + 128);
            *pOutput = R;
            CALCULATE_CRC_BYTE
            *pOutput = (unsigned char) (R + Y);
            CALCULATE_CRC_BYTE
        }
        else if (pWaveFormatEx->wBitsPerSample == 24) 
        {
            int32 RV, LV;

            RV = X - (Y / 2);
            LV = RV + Y;
            
            uint32 nTemp = 0;
            if (RV < 0)
                nTemp = ((uint32) (RV + 0x800000)) | 0x800000;
            else
                nTemp = (uint32) RV;    
            
            *pOutput = (unsigned char) ((nTemp >> 0) & 0xFF);
            CALCULATE_CRC_BYTE
            *pOutput = (unsigned char) ((nTemp >> 8) & 0xFF);
            CALCULATE_CRC_BYTE
            *pOutput = (unsigned char) ((nTemp >> 16) & 0xFF);
            CALCULATE_CRC_BYTE

            nTemp = 0;
            if (LV < 0)
                nTemp = ((uint32) (LV + 0x800000)) | 0x800000;
            else
                nTemp = (uint32) LV;    
            
            *pOutput = (unsigned char) ((nTemp >> 0) & 0xFF);
            CALCULATE_CRC_BYTE
            
            *pOutput = (unsigned char) ((nTemp >> 8) & 0xFF);
            CALCULATE_CRC_BYTE
            
            *pOutput = (unsigned char) ((nTemp >> 16) & 0xFF);
            CALCULATE_CRC_BYTE
        }
    }
    else if (pWaveFormatEx->nChannels == 1) 
    {
        if (pWaveFormatEx->wBitsPerSample == 16) 
        {
            int16 R = X;
                
            *(int16 *) pOutput = (int16) R;
            CALCULATE_CRC_BYTE
            CALCULATE_CRC_BYTE
        }
        else if (pWaveFormatEx->wBitsPerSample == 8) 
        {
            unsigned char R = X + 128;
            *pOutput = R;
            CALCULATE_CRC_BYTE
        }
        else if (pWaveFormatEx->wBitsPerSample == 24) 
        {
            int32 RV = X;
            
            uint32 nTemp = 0;
            if (RV < 0)
                nTemp = ((uint32) (RV + 0x800000)) | 0x800000;
            else
                nTemp = (uint32) RV;    
            
            *pOutput = (unsigned char) ((nTemp >> 0) & 0xFF);
            CALCULATE_CRC_BYTE
            *pOutput = (unsigned char) ((nTemp >> 8) & 0xFF);
            CALCULATE_CRC_BYTE
            *pOutput = (unsigned char) ((nTemp >> 16) & 0xFF);
            CALCULATE_CRC_BYTE
        }
    }
}

#ifdef BACKWARDS_COMPATIBILITY

int CPrepare::UnprepareOld(int *pInputX, int *pInputY, int nBlocks, const WAVEFORMATEX *pWaveFormatEx, unsigned char *pRawData, unsigned int *pCRC, int *pSpecialCodes, int nFileVersion)
{
    // the CRC that will be figured during decompression
    uint32 CRC = 0xFFFFFFFF;

    // decompress and convert from (x,y) -> (l,r)
    // sort of int and ugly.... sorry
    if (pWaveFormatEx->nChannels == 2) 
    {
        // convert the x,y data to raw data
        if (pWaveFormatEx->wBitsPerSample == 16) 
        {
            int16 R;
            unsigned char *Buffer = &pRawData[0];
            int *pX = pInputX;
            int *pY = pInputY;

            for (; pX < &pInputX[nBlocks]; pX++, pY++) 
            {
                R = *pX - (*pY / 2);
                
                *(int16 *) Buffer = (int16) R;
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
                
                *(int16 *) Buffer = (int16) R + *pY;
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
            }
        }
        else if (pWaveFormatEx->wBitsPerSample == 8) 
        {
            unsigned char *R = (unsigned char *) &pRawData[0];
            unsigned char *L = (unsigned char *) &pRawData[1];

            if (nFileVersion > 3830) 
            {
                for (int SampleIndex = 0; SampleIndex < nBlocks; SampleIndex++, L+=2, R+=2) 
                {
                    *R = (unsigned char) (pInputX[SampleIndex] - (pInputY[SampleIndex] / 2) + 128);
                    CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *R];
                    *L = (unsigned char) (*R + pInputY[SampleIndex]);
                    CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *L];
                }
            }
            else 
            {
                for (int SampleIndex = 0; SampleIndex < nBlocks; SampleIndex++, L+=2, R+=2)
                {
                    *R = (unsigned char) (pInputX[SampleIndex] - (pInputY[SampleIndex] / 2));
                    CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *R];
                    *L = (unsigned char) (*R + pInputY[SampleIndex]);
                    CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *L];
                }
            }
        }
        else if (pWaveFormatEx->wBitsPerSample == 24) 
        {
            unsigned char *Buffer = (unsigned char *) &pRawData[0];
            int32 RV, LV;

            for (int SampleIndex = 0; SampleIndex < nBlocks; SampleIndex++)
            {
                RV = pInputX[SampleIndex] - (pInputY[SampleIndex] / 2);
                LV = RV + pInputY[SampleIndex];
                
                uint32 nTemp = 0;
                if (RV < 0)
                    nTemp = ((uint32) (RV + 0x800000)) | 0x800000;
                else
                    nTemp = (uint32) RV;    
                
                *Buffer = (unsigned char) ((nTemp >> 0) & 0xFF);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
                
                *Buffer = (unsigned char) ((nTemp >> 8) & 0xFF);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];

                *Buffer = (unsigned char) ((nTemp >> 16) & 0xFF);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];

                nTemp = 0;
                if (LV < 0)
                    nTemp = ((uint32) (LV + 0x800000)) | 0x800000;
                else
                    nTemp = (uint32) LV;    
                
                *Buffer = (unsigned char) ((nTemp >> 0) & 0xFF);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
                
                *Buffer = (unsigned char) ((nTemp >> 8) & 0xFF);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
                
                *Buffer = (unsigned char) ((nTemp >> 16) & 0xFF);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
            }
        }
    }
    else if (pWaveFormatEx->nChannels == 1) 
    {
        // convert to raw data
        if (pWaveFormatEx->wBitsPerSample == 8) 
        {
            unsigned char *R = (unsigned char *) &pRawData[0];

            if (nFileVersion > 3830) 
            {
                for (int SampleIndex = 0; SampleIndex < nBlocks; SampleIndex++, R++)
                {
                    *R = pInputX[SampleIndex] + 128;
                    CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *R];
                }
            }
            else 
            {
                for (int SampleIndex = 0; SampleIndex < nBlocks; SampleIndex++, R++)
                {
                    *R = (unsigned char) (pInputX[SampleIndex]);
                    CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *R];
                }
            }
        }
        else if (pWaveFormatEx->wBitsPerSample == 24) 
        {
            unsigned char *Buffer = (unsigned char *) &pRawData[0];
            int32 RV;
            for (int SampleIndex = 0; SampleIndex<nBlocks; SampleIndex++) 
            {
                RV = pInputX[SampleIndex];

                uint32 nTemp = 0;
                if (RV < 0)
                    nTemp = ((uint32) (RV + 0x800000)) | 0x800000;
                else
                    nTemp = (uint32) RV;    
                
                *Buffer = (unsigned char) ((nTemp >> 0) & 0xFF);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
                
                *Buffer = (unsigned char) ((nTemp >> 8) & 0xFF);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
                
                *Buffer = (unsigned char) ((nTemp >> 16) & 0xFF);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
            }
        }
        else 
        {
            unsigned char *Buffer = &pRawData[0];

            for (int SampleIndex = 0; SampleIndex < nBlocks; SampleIndex++) 
            {
                *(int16 *) Buffer = (int16) (pInputX[SampleIndex]);
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
                CRC = (CRC >> 8) ^ CRC32_TABLE[(CRC & 0xFF) ^ *Buffer++];
            }
        }
    }

    CRC = CRC ^ 0xFFFFFFFF;

    *pCRC = CRC;

    return 0;
}

#endif // #ifdef BACKWARDS_COMPATIBILITY
