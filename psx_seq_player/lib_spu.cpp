#include "lib_spu.hpp"

extern "C"
{
    // TODO
    extern long SpuClearReverbWorkArea (long mode);
    extern void SpuSetCommonAttr (SpuCommonAttr *attr);
    extern long SpuIsTransferCompleted (long flag);

}
