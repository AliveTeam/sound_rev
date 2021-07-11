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

    void SpuInit(void)
    {
        _SpuInit(0);
    }

}
