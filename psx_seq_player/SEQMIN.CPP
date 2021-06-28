#include <STDLIB.H>
#include <LIBETC.H>
#include <STDIO.H>
#include <LIBSND.H>

void Sound_SsInit()
{
    ::SsInit();
}

void Sound_SsSetTableSize(char* table, short s_max, short t_max)
{
    ::SsSetTableSize(table, s_max, t_max);
}

void Sound_SsSetTickMode(long tick_mode)
{
    ::SsSetTickMode(tick_mode);
}

void Sound_SsStart()
{
    ::SsStart();
}

void Sound_SsSetMVol(short voll, short volr)
{
    ::SsSetMVol(voll, volr);
}

int Sound_SsVabTransfer(u_char* vh_addr, u_char* vb_addr, short vabid, short i_flag)
{
    return ::SsVabTransfer(vh_addr, vb_addr, vabid, i_flag);
}

short Sound_SsVabTransCompleted(short immediateFlag)
{
    return ::SsVabTransCompleted(immediateFlag);
}

short Sound_SsSeqOpen(u_long* addr, short vab_id)
{
    return ::SsSeqOpen(addr, vab_id);
}

void Sound_SsSeqSetVol(short seq_access_num, short voll, short volr)
{
    ::SsSeqSetVol(seq_access_num, voll, volr);
}

void Sound_SsSeqPlay(short seq_access_num, char play_mode, short l_count)
{
    ::SsSeqPlay(seq_access_num, play_mode, l_count);
}

void Sound_SsSeqClose(short seq_access_num)
{
    ::SsSeqClose(seq_access_num);
}

void Sound_SsVabClose(short vab_id)
{
    ::SsVabClose(vab_id);
}

short Sound_SsUtKeyOffV(short voice)
{
    return ::SsUtKeyOffV(voice);
}

long Sound_SsVoKeyOn(long vab_pro, long pitch, u_short volL, u_short volR)
{
    return ::SsVoKeyOn(vab_pro, pitch, volL, volR);
}

void Sound_SsSeqCalledTbyT()
{
    ::SsSeqCalledTbyT();
}

static void TickSeq()
{
    Sound_SsSeqCalledTbyT();
}

static char mSeqTable[SS_SEQ_TABSIZ * 4 * 5];
static short mOpenVabId;
static short mOpenSeqId;

static void DoSound()
{
    Sound_SsInit();
        Sound_SsSetTableSize(mSeqTable, 4, 5);
        Sound_SsSetTickMode(SS_NOTICK);
        Sound_SsStart();
        Sound_SsSetMVol(127, 127);
        VSyncCallback(TickSeq);

 mOpenVabId = Sound_SsVabTransfer(0, 0, -1, SS_WAIT_COMPLETED);
        if (mOpenVabId < 0)
        {
            if (mOpenVabId == -1)
            {
                printf("ERROR: Invalid VH\n");
                return;
            }
            else if (mOpenVabId == -2)
            {
                printf("ERROR: Invalid VB\n");
                return;
            }
            else
            {
                printf("ERROR: Copying to SPU memory\n");
                return;
            }
        }
        if (Sound_SsVabTransCompleted(SS_WAIT_COMPLETED) == 0)
        {
            printf("ERROR: Didn't finish copy to SPU ram\n");
            return;
        }

        mOpenSeqId = Sound_SsSeqOpen(reinterpret_cast<unsigned long*>(0), mOpenVabId);
        if (mOpenSeqId == -1)
        {
            printf("Open SEQ failed\n");
            return ;
        }

        printf("Playing seq [%d]\n", mOpenSeqId);
        Sound_SsSeqSetVol(mOpenSeqId, 100, 100);
        Sound_SsSeqPlay(mOpenSeqId, SSPLAY_PLAY, 1);

         Sound_SsUtKeyOffV(4);
     Sound_SsVoKeyOn(0, 0, 0, 0);
}

int main()
{
    ::ResetCallback();

    printf("main enter\n");

    DoSound();

    return 0;
}
