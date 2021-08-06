#ifndef _PSX_STUBS_HPP_
#define _PSX_STUBS_HPP_

#define PSX

#ifdef PSX
    #include <libapi.h>
    #include <libetc.h> // ResetCallBack
#else

extern "C"
{
#define HwSPU 1
#define EvSpCOMP 2
#define EvStACTIVE 3
#define NULL 0

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
        return 0;
    }

    inline void WaitEvent(int)
    {

    }

    inline int OpenEvent(int, int, int, void*)
    {
        return 0;
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
