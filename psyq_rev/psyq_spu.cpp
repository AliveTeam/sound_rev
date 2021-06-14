#include "psyq_spu.h"

/*
enum SPU_Register
{
    MVolLeft = 0x1f801d80,
    MVolRight = 0x1f801d82,
    ReverbDepthLeft= 0x1f801d84,
    ReverbDepthRight = 0x1f801d86,
    SoundPlay0to15 = 0x1f801d88,
    SoundPlay16to23 = 0x1f801d8a,
    StopSoundPlay0to15 = 0x1f801d8c,
    StopSoundPlay16to23 = 0x1f801d8e,
    SetModulation0to15 = 0x1f801d90,
    SetModulation0to15_2 = 0x1f801d92,

};

class Registers
{
public:
    void SetVol(short mVol)
    {

    }
};
*/

void SetRegister(unsigned int reg, short val)
{

}

void SSInit_4FC230()
{
    // Set max vol left/right
    SetRegister(0x1f801d80, 0x3FFF);
    SetRegister(0x1f801d82, 0x3FFF);

    // Set reverb depth left/right
    SetRegister(0x1f801d84, 0);
    SetRegister(0x1f801d86, 0);

    SetRegister(0x1f801d88, 0);
    SetRegister(0x1f801d8a, 0);

    SetRegister(0x1f801d8c, 0);
    SetRegister(0x1f801d8e, 0);

    SetRegister(0x1f801d90, 0);
    SetRegister(0x1f801d92, 0);

    SetRegister(0x1f801d94, 0);
    SetRegister(0x1f801d96, 0);

    SetRegister(0x1f801d98, 0);
    SetRegister(0x1f801d9a, 0);

    SetRegister(0x1f801d9c, 0);
    SetRegister(0x1f801d9e, 0);

    /*
    k32Counter = 0;
    pSsMarkCallback = SsMarkCallback;
    do
    {
        k15Counter = 15;
        v7 = pSsMarkCallback + 60;
        do
        {
            *v7 = 0;
            --k15Counter;
            --v7;
        } while (k15Counter >= 0);
        ++k32Counter;
        pSsMarkCallback += 64;
    } while (k32Counter < 32);
    VBLANK_MINUS = 60; // Default tick mode?
    snd_openflag = 0;
    snd_ev_flag = 0;
    */
}

/*
void SpuStart()
{
    if (!spu_isCalled)
    {
        spu_isCalled = 1;
        EnterCriticalSection();
        SpuDataCallback(&spu_FiDMA);
        spu_EVdma = OpenEvent(0xF0000009, 32, 0x2000, 0);
        EnableEvent();
        ExitCriticalSection();
    }
}
*/

void SpuInit(int a1)
{
    /*
    int k23Counter; // $v1
    _WORD *v3; // $v0

    ResetCallback();
    spu_init(a1);
    if (!a1)
    {
        k23Counter = 23;
        v3 = &unk_46A;
        do
        {
            *v3 = 0xC000;
            --k23Counter;
            --v3;
        } while (k23Counter >= 0);
    }
    SpuStart(0xC000);
    spu_rev_flag = 0;
    spu_rev_reserve_wa = 0;
    dword_424 = 0;
    word_428 = 0;
    word_42A = 0;
    dword_42C = 0;
    dword_430 = 0;
    spu_rev_offsetaddr = spu_rev_startaddr;
    spu_FsetRXX(209, spu_rev_startaddr, 0);
    spu_AllocBlockNum = 0;
    spu_AllocLastNum = 0;
    spu_memList = 0;
    spu_trans_mode = 0;
    spu_transMode = 0;
    spu_keystat = 0;
    spu_RQmask = 0;
    spu_RQvoice = 0;
    spu_env = 0;
    */
}

void SpuInit()
{
    SpuInit(0);
}

void SpuInitHot()
{
    SpuInit(1);
}

void SpuInitHot_4FC320()
{
    //ResetCallback();
    SpuInitHot();
    SSInit_4FC230();
}