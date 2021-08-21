#include <sys/types.h>
#include "lib_snd.hpp"
#include "lib_spu.hpp"
#include "Sound.hpp"

#include "mines.vb.h"
#include "mines.vh.h"
#include "miseq.bsq.h"
#include "SeqChunkParser.hpp"

#include <stdlib.h>


#include "../mednafen/spu.h"
#include "../mednafen/dma.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>
//#include <SDL_main.h>

PS_SPU* SPU = nullptr;

void SetSpuReg(volatile u32* pReg, u32 value)
{
    if (pReg == (volatile u32*)0x1f8010f0)
    {
        MDFN_IEN_PSX::DMA_Write(0, (uint32)pReg, value);
    }
    else
    {
        SPU->Write(0, (uint32_t)pReg, value);
        SPU->Write(0, ((uint32_t)pReg) | 2, value >> 16); // TODO: Check |2 hack works
    }
}

void SetSpuReg(volatile u16* pReg, u16 value)
{
    SPU->Write(0, (uint32_t)pReg, value);
}

void SetSpuReg(volatile short* pReg, short value)
{
    SPU->Write(0, (uint32_t)pReg, value);
}

u32 GetSpuRegU32(volatile u32* pReg)
{
    if (pReg == (volatile u32*)0x1f8010f0)
    {
        return MDFN_IEN_PSX::DMA_Read(0, (uint32)pReg);
    }

    // TODO: needs to be done as 2x reads ??
    const u16 lo = SPU->Read(0, (uint32_t)pReg);
    const u16 hi = SPU->Read(0, ((uint32_t)pReg) | 2); // TODO: Check |2 hack works

    return lo | ((u32)hi >> 16);
}

u16 GetSpuRegU16(volatile u16* pReg)
{
    return SPU->Read(0, (uint32_t)pReg);
}


static Sound gSound;
static SeqChunkParser gChunkParser;

static void VSync(int mode)
{

}

#include <memory.h>

static void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
    memcpy(stream, IntermediateBuffer + IntermediateBufferPos, len);

//    IntermediateBuffer, IntermediateBufferPos
}

int main(int, char**)
{
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec wav_spec = {};
    wav_spec.channels = 2;
    wav_spec.samples = 2048;
    wav_spec.format = AUDIO_S16;
    wav_spec.callback = my_audio_callback;
    wav_spec.userdata = NULL;
    if (SDL_OpenAudio(&wav_spec, NULL) < 0) 
    {
        printf(SDL_GetError());
        return 1;
    }
    SDL_PauseAudio(0);

    MDFN_IEN_PSX::DMA_Init();

    MDFN_IEN_PSX::DMA_Power();


    SPU = new PS_SPU();

    SPU->Power();

    for (int i = 0; i < 900; i++)
    {
        SPU->UpdateFromCDC(400);
        MDFN_IEN_PSX::DMA_Update(0);

    }

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
        SPU->UpdateFromCDC(400);
        MDFN_IEN_PSX::DMA_Update(0);
        VSync(0);
    }

   //  audio_batch_cb((int16_t*)&IntermediateBuffer,IntermediateBufferPos);
    // 

    return 0;
}
