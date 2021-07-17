#include "lib_spu.hpp"

extern "C"
{
    // TODO
    extern long SpuClearReverbWorkArea (long mode);
    extern void SpuSetCommonAttr (SpuCommonAttr *attr);
    extern long SpuIsTransferCompleted (long flag);

    // TODO
    void _SpuInit(int);

    void SpuSetReverb(long on_off);
    
    short _spu_getInTransfer(void);
    void _spu_setInTransfer(short);

    long SpuMalloc(long size);
    void SpuFree(unsigned long addr);

    long SpuSetTransferMode(long mode);
    unsigned long SpuSetTransferStartAddr (unsigned long addr);
    unsigned long SpuWrite (unsigned char *addr, unsigned long size);

    extern long SpuSetReverbModeParam (SpuReverbAttr *attr);
    extern long SpuInitMalloc (long num, char *top);


    extern void SpuSetVoiceAttr (SpuVoiceAttr *arg);
    extern void SpuGetVoiceEnvelope (int vNum, short *envx);
    extern unsigned long SpuSetNoiseVoice (long on_off, unsigned long voice_bit);
    extern unsigned long SpuSetReverbVoice (long on_off, unsigned long voice_bit);
    extern void SpuSetKey (long on_off, unsigned long voice_bit);
    extern unsigned long SpuGetReverbVoice (void);
    extern unsigned long SpuGetNoiseVoice (void);

    void SpuInit(void)
    {
        _SpuInit(0);
    }

    long SpuSetNoiseClock(long n_clock);
    
}
