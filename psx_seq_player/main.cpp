#include <sys/types.h>
#include "lib_snd.hpp"
#include "lib_spu.hpp"
#include "Sound.hpp"

#include "mines.vb.h"
#include "mines.vh.h"
#include "miseq.bsq.h"
#include "SeqChunkParser.hpp"


/*
#include "mednafen/spu.h"

PS_SPU spu;

void SetSpuReg(volatile u32* pReg, u32 value)
{
    spu.Write(0, (uint32_t)pReg, value);
    spu.Write(0, ((uint32_t)pReg) | 2, value >> 16); // TODO: Check |2 hack works
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
    const u16 lo = spu.Read(0, (uint32_t)pReg);
    const u16 hi = spu.Read(0, ((uint32_t)pReg) | 2); // TODO: Check |2 hack works

    return lo | ((u32)hi >> 16);
}

u16 GetSpuRegU16(volatile u16* pReg)
{
    return spu.Read(0, (uint32_t)pReg);
}
*/

static Sound gSound;
static SeqChunkParser gChunkParser;

int main(int, char**)
{
    /*
    spu.Power();

    for (int i = 0; i < 900; i++)
    {
        spu.UpdateFromCDC(400);
    }
    */

    gSound.Init();

    if (!gChunkParser.Parse(MISEQ_BSQ))
    {
        printf("Chunk parse failure\n");
    }
    
    if (!gSound.LoadVab(MINES_VH, MINES_VB))
    {
        printf("Vab load failure\n");
    }

    if (!gSound.PlaySEQ(gChunkParser.SeqPtr(MISEQ_BSQ, 13) + sizeof(ChunkHeader)))
    {
        printf("PlaySEQ failure\n");
    }

    printf("Enter loop\n");
    for (;;)
    {
        VSync(0);
    }

    // audio_batch_cb((int16_t*)&IntermediateBuffer,IntermediateBufferPos);
    // 

    return 0;
}
