#include "lib_spu.hpp"

#ifdef __cplusplus
extern "C" 
#endif
short SsVabTransCompleted(short immediateFlag)
{
    long ret = SpuIsTransferCompleted(immediateFlag);
    return ret;
}
