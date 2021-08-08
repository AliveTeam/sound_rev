#include <iostream>
#include "mednafen/spu.h"
//#include "psyq_rev/psyq_spu.h"
#include "psx_seq_player/lib_snd.hpp"
#include "psx_seq_player/lib_spu.hpp"

/*
EXPORT void CC SND_Init_4CA1F0()
{
    SSInit_4FC230();
    SsSetTableSize_4FE0B0(nullptr, 16, 1);
    SsUtSetReverbDepth_4FE380(0, 0);
    SsUtSetReverbType_4FE360(4);
    SsUtReverbOn_4FE340();
    SsSetTickMode_4FDC20(4096);
    VSyncCallback_4F8C40(SND_CallBack_4020A4);
    SpuInitHot_4FC320();
    SsSetMVol_4FC360(100, 100);
    memset(&GetMidiVars()->sSeq_Ids_word(), -1, sizeof(SeqIds));
    GetMidiVars()->sSeqsPlaying_count_word() = 0;
}

EXPORT s32 CC MIDI_Play_Single_Note_4CA1B0(s32 vabIdAndProgram, s32 note, s32 leftVol, s32 rightVol)
{
    // NOTE: word_BB2E40 is used as a guard here, but it is never read anywhere else
    // TODO: This guard flag was seen in some PSX dev ref, investigate what it was for
    return SsVoKeyOn_4FCF10(vabIdAndProgram, note, static_cast<u16>(leftVol), static_cast<u16>(rightVol));
}

 pSoundBlockInfo->field_8_vab_id = SsVabOpenHead_4FC620(reinterpret_cast<VabHeader*>(pSoundBlockInfo->field_C_pVabHeader));

    SsVabTransBody_4FC840(reinterpret_cast<VabBodyRecord*>(*ppVabBody), static_cast<s16>(pSoundBlockInfo->field_8_vab_id));

    SsVabTransCompleted_4FE060(SS_WAIT_COMPLETED);

    SsUtChangePitch

*/


PS_SPU spu;

void SetSpuReg(volatile u32* pReg, u32 value)
{
    spu.Write(0, (uint32_t)pReg, value);
    // TODO: needs to be done as 2x writes ??
}

void SetSpuReg(volatile u16* pReg, u16 value)
{
    spu.Write(0, (uint32_t)pReg, value);
}

void SetSpuReg(volatile short* pReg, short value)
{
    spu.Write(0, (uint32_t)pReg, value);
}

u32 GetSpuRegU32(volatile u32* pReg)
{
    // TODO: needs to be done as 2x reads ??
    return spu.Read(0, (uint32_t)pReg);
}

u16 GetSpuRegU16(volatile u16* pReg)
{
    return spu.Read(0, (uint32_t)pReg);
}

int main(int, char**)
{
    std::cout << "hello" << std::endl;

    spu.Power();

    for (int i = 0; i < 900; i++)
    {
        spu.UpdateFromCDC(400);
    }

    spu.Write(0, 0, 0);

    SsInit();


    // audio_batch_cb((int16_t*)&IntermediateBuffer,IntermediateBufferPos);
    // 

    return 0;
}
