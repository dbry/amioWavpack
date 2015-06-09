#include "All.h"
#include "NNFilter.h"

namespace
{
int  CalculateDotProduct ( const short* pA, const short* pB, int nOrder )
{
	int retVal = 0;
#ifdef _WIN32 // Having trouble getting this inline assembly to work on Mac
	__asm
	{
            mov     eax, pA
            mov     ecx, pB
            mov     edx, nOrder
            shr     edx, 4
            pxor    mm7, mm7

loopDot:    movq    mm0, [eax]
            pmaddwd mm0, [ecx]
            paddd   mm7, mm0
            movq    mm1, [eax +  8]
            pmaddwd mm1, [ecx +  8]
            paddd   mm7, mm1
            movq    mm2, [eax + 16]
            pmaddwd mm2, [ecx + 16]
            paddd   mm7, mm2
            movq    mm3, [eax + 24]
            pmaddwd mm3, [ecx + 24]
            add     eax, 32
            add     ecx, 32
            paddd   mm7, mm3
            dec     edx
            jnz     loopDot

            movq    mm6, mm7
            psrlq   mm7, 32
            paddd   mm6, mm7
            movd    pA, mm6
            emms
            mov     eax, pA
	}
#else
	return retVal;
#endif
}
void  Adapt ( short* pM, const short* pAdapt, int nDirection, int nOrder )
{
#ifdef _WIN32 // Having trouble getting this inline assembly to work on Mac
	__asm
	{
;   [esp+16]    nOrder
;   [esp+12]    nDirection
;   [esp+ 8]    pAdapt
;   [esp+ 4]    pM
;   [esp+ 0]    Return Address

            mov  eax, pM
            mov  ecx, pAdapt
            mov  edx, nOrder
            shr  edx, 4

            cmp  nDirection, 0
            jle  AdaptSub

AdaptAddLoop:
            movq  mm0, [eax]
            paddw mm0, [ecx]
            movq  [eax], mm0
            movq  mm1, [eax + 8]
            paddw mm1, [ecx + 8]
            movq  [eax + 8], mm1
            movq  mm2, [eax + 16]
            paddw mm2, [ecx + 16]
            movq  [eax + 16], mm2
            movq  mm3, [eax + 24]
            paddw mm3, [ecx + 24]
            movq  [eax + 24], mm3
            add   eax, 32
            add   ecx, 32
            dec   edx
            jnz   AdaptAddLoop

            emms
            jmp   AdaptDone

AdaptSub:   je    AdaptDone

AdaptSubLoop:
            movq  mm0, [eax]
            psubw mm0, [ecx]
            movq  [eax], mm0
            movq  mm1, [eax + 8]
            psubw mm1, [ecx + 8]
            movq  [eax + 8], mm1
            movq  mm2, [eax + 16]
            psubw mm2, [ecx + 16]
            movq  [eax + 16], mm2
            movq  mm3, [eax + 24]
            psubw mm3, [ecx + 24]
            movq  [eax + 24], mm3
            add   eax, 32
            add   ecx, 32
            dec   edx
            jnz   AdaptSubLoop

            emms
AdaptDone:
	}
#endif
}


} // private namespace

CNNFilter::CNNFilter(int nOrder, int nShift, int nVersion)
{
    if ((nOrder <= 0) || ((nOrder % 16) != 0)) throw(1);
    m_nOrder = nOrder;
    m_nShift = nShift;
    m_nVersion = nVersion;
    
	int localMMXAvailable = false;
#ifdef _WIN32 // Having trouble getting this inline assembly to work on Mac
	__asm
	{
            pushad
            pushfd
            pop     eax
            mov     ecx, eax
            xor     eax, 0x200000
            push    eax
            popfd
            pushfd
            pop     eax
            cmp     eax, ecx
            jz      returnMMX        ; no CPUID command, so no MMX

            mov     eax,1
            CPUID
            test    edx,0x800000
returnMMX:  popad
            setnz   al
            and     eax, 1
			mov		localMMXAvailable, eax
	}
#endif
	m_bMMXAvailable = localMMXAvailable;
    
    m_rbInput.Create(NN_WINDOW_ELEMENTS, m_nOrder);
    m_rbDeltaM.Create(NN_WINDOW_ELEMENTS, m_nOrder);
    m_paryM = new short [m_nOrder];

#ifdef NN_TEST_MMX
    srand(GetTickCount());
#endif
}

CNNFilter::~CNNFilter()
{
    SAFE_ARRAY_DELETE(m_paryM)
}

void CNNFilter::Flush()
{
    memset(&m_paryM[0], 0, m_nOrder * sizeof(short));
    m_rbInput.Flush();
    m_rbDeltaM.Flush();
    m_nRunningAverage = 0;
}

int CNNFilter::Compress(int nInput)
{
    // convert the input to a short and store it
    m_rbInput[0] = GetSaturatedShortFromInt(nInput);

    // figure a dot product
    int nDotProduct;
    if (m_bMMXAvailable)
        nDotProduct = CalculateDotProduct(&m_rbInput[-m_nOrder], &m_paryM[0], m_nOrder);
    else
        nDotProduct = CalculateDotProductNoMMX(&m_rbInput[-m_nOrder], &m_paryM[0], m_nOrder);

    // calculate the output
    int nOutput = nInput - ((nDotProduct + (1 << (m_nShift - 1))) >> m_nShift);

    // adapt
    if (m_bMMXAvailable)
        Adapt(&m_paryM[0], &m_rbDeltaM[-m_nOrder], -nOutput, m_nOrder);
    else
        AdaptNoMMX(&m_paryM[0], &m_rbDeltaM[-m_nOrder], nOutput, m_nOrder);

    int nTempABS = abs(nInput);

    if (nTempABS > (m_nRunningAverage * 3))
        m_rbDeltaM[0] = ((nInput >> 25) & 64) - 32;
    else if (nTempABS > (m_nRunningAverage * 4) / 3)
        m_rbDeltaM[0] = ((nInput >> 26) & 32) - 16;
    else if (nTempABS > 0)
        m_rbDeltaM[0] = ((nInput >> 27) & 16) - 8;
    else
        m_rbDeltaM[0] = 0;

    m_nRunningAverage += (nTempABS - m_nRunningAverage) / 16;

    m_rbDeltaM[-1] >>= 1;
    m_rbDeltaM[-2] >>= 1;
    m_rbDeltaM[-8] >>= 1;
        
    // increment and roll if necessary
    m_rbInput.IncrementSafe();
    m_rbDeltaM.IncrementSafe();

    return nOutput;
}

int CNNFilter::Decompress(int nInput)
{
    // figure a dot product
    int nDotProduct;

    if (m_bMMXAvailable)
        nDotProduct = CalculateDotProduct(&m_rbInput[-m_nOrder], &m_paryM[0], m_nOrder);
    else
        nDotProduct = CalculateDotProductNoMMX(&m_rbInput[-m_nOrder], &m_paryM[0], m_nOrder);
    
    // adapt
    if (m_bMMXAvailable)
        Adapt(&m_paryM[0], &m_rbDeltaM[-m_nOrder], -nInput, m_nOrder);
    else
        AdaptNoMMX(&m_paryM[0], &m_rbDeltaM[-m_nOrder], nInput, m_nOrder);

    // store the output value
    int nOutput = nInput + ((nDotProduct + (1 << (m_nShift - 1))) >> m_nShift);

    // update the input buffer
    m_rbInput[0] = GetSaturatedShortFromInt(nOutput);

    if (m_nVersion >= 3980)
    {
        int nTempABS = abs(nOutput);

        if (nTempABS > (m_nRunningAverage * 3))
            m_rbDeltaM[0] = ((nOutput >> 25) & 64) - 32;
        else if (nTempABS > (m_nRunningAverage * 4) / 3)
            m_rbDeltaM[0] = ((nOutput >> 26) & 32) - 16;
        else if (nTempABS > 0)
            m_rbDeltaM[0] = ((nOutput >> 27) & 16) - 8;
        else
            m_rbDeltaM[0] = 0;

        m_nRunningAverage += (nTempABS - m_nRunningAverage) / 16;

        m_rbDeltaM[-1] >>= 1;
        m_rbDeltaM[-2] >>= 1;
        m_rbDeltaM[-8] >>= 1;
    }
    else
    {
        m_rbDeltaM[0] = (nOutput == 0) ? 0 : ((nOutput >> 28) & 8) - 4;
        m_rbDeltaM[-4] >>= 1;
        m_rbDeltaM[-8] >>= 1;
    }

    // increment and roll if necessary
    m_rbInput.IncrementSafe();
    m_rbDeltaM.IncrementSafe();
    
    return nOutput;
}

void CNNFilter::AdaptNoMMX(short * pM, short * pAdapt, int nDirection, int nOrder)
{
    nOrder >>= 4;

    if (nDirection < 0) 
    {    
        while (nOrder--)
        {
            EXPAND_16_TIMES(*pM++ += *pAdapt++;)
        }
    }
    else if (nDirection > 0)
    {
        while (nOrder--)
        {
            EXPAND_16_TIMES(*pM++ -= *pAdapt++;)
        }
    }
}

int CNNFilter::CalculateDotProductNoMMX(short * pA, short * pB, int nOrder)
{
    int nDotProduct = 0;
    nOrder >>= 4;

    while (nOrder--)
    {
        EXPAND_16_TIMES(nDotProduct += *pA++ * *pB++;)
    }
    
    return nDotProduct;
}
