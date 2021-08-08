#ifndef _PSX_STUBS_HPP_
#define _PSX_STUBS_HPP_

#define PSX

#ifdef PSX
    #include <libapi.h>
    #include <libetc.h> // ResetCallBack

inline void SetSpuReg(volatile u32* pReg, u32 value)
{
    *pReg = value;
}

inline void SetSpuReg(volatile u16* pReg, u16 value)
{
    *pReg = value;
}

inline void SetSpuReg(volatile short* pReg, short value)
{
    *pReg = value;
}

inline u32 GetSpuRegU32(volatile u32* pReg)
{
    return *pReg;
}

inline u16 GetSpuRegU16(volatile u16* pReg)
{
    return *pReg;
}

#else

extern "C"
{
#define HwSPU 1
#define EvSpCOMP 2
#define EvStACTIVE 3
#define NULL 0

    inline void SetSpuReg(volatile u32* pReg, u32 value)
    {
        *pReg = value;
    }

    inline void SetSpuReg(volatile u16* pReg, u16 value)
    {
        *pReg = value;
    }

    inline void SetSpuReg(volatile short* pReg, short value)
    {
        *pReg = value;
    }

    inline u32 GetSpuRegU32(volatile u32* pReg)
    {
        return *pReg;
    }

    inline u16 GetSpuRegU16(volatile u16* pReg)
    {
        return *pReg;
    }

    inline void ResetCallback()
    {

    }

    inline void EnterCriticalSection()
    {

    }

    inline void ExitCriticalSection()
    {

    }

    inline void CloseEvent(int)
    {

    }

    inline void DisableEvent(int)
    {

    }

    inline void EnableEvent(int)
    {

    }

    inline int TestEvent(int)
    {
        return 1;
    }

    inline int WaitEvent(int)
    {
        return 1;
    }

    inline int OpenEvent(int, int, int, void*)
    {
        printf("Open event...\n");
        return 1;
    }

    inline void DeliverEvent(int, int)
    {

    }

    inline void *DMACallback(int index, void(*callback)())
    {
        return 0;
    }
}
#endif // WIN32

#endif // _PSX_STUBS_HPP_
