#include "lib_spu.hpp"

extern "C"
{
    // TODO
    extern long SpuClearReverbWorkArea (long mode);
    extern void SpuSetCommonAttr (SpuCommonAttr *attr);
    extern long SpuIsTransferCompleted (long flag);

    // TODO
    void _SpuInit(int);

    void SpuInit(void)
    {
        _SpuInit(0);
    }

}
