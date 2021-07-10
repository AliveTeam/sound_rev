#include "lib_spu.hpp"

short SsVabTransCompleted(short immediateFlag)
{
    long ret = SpuIsTransferCompleted(immediateFlag);
    return ret;
}
