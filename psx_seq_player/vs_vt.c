#include "lib_snd.hpp"

short SsVabTransfer(unsigned char *vh_addr, unsigned char *vb_addr, short vabid, short i_flag)
{
    int trans_body_result;

    vabid = SsVabOpenHead(vh_addr, vabid); // when vabid is -1 a new one is allocated
    if (vabid == -1)
    {
        return -1;
    }

    trans_body_result = SsVabTransBody(vb_addr, vabid);
    if (trans_body_result == -1)
    {
        return -2;
    }

    if (i_flag == SS_WAIT_COMPLETED) // OG checked i_flag << 16 ??
    {
        SsVabTransCompleted(SS_WAIT_COMPLETED);
    }

    return trans_body_result;
}