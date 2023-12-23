#ifndef _SOUND_HPP_
#define _SOUND_HPP_

#include "lib_snd.hpp"
#include "lib_spu.hpp"
#include "psx_stubs.hpp"
#ifdef PSX
#include <STDIO.H>
#else
#include <stdio.h>
#endif

#define MVOL 127
#define SVOL 64

inline void TickSeq()
{
    SsSeqCalledTbyT();
}

class Sound
{
public:
    Sound()
    {
        mOpenVabId = -1;
        mOpenSeqId = -1;
    }

    void Init()
    {
        SsInit();
        SsSetTableSize(mSeqTable, 4, 5);
        SsSetTickMode(SS_NOTICK);
        SsStart();
        SsSetMVol(MVOL, MVOL);
        VSyncCallback(TickSeq);
    }

    bool LoadVab(unsigned char *vhBuffer, unsigned char *vbBuffer)
    {
        CloseVab();

        mOpenVabId = SsVabTransfer(vhBuffer, vbBuffer, -1, SS_WAIT_COMPLETED);
        if (mOpenVabId < 0)
        {
            if (mOpenVabId == -1)
            {
                printf("ERROR: Invalid VH\n");
                return false;
            }
            else if (mOpenVabId == -2)
            {
                printf("ERROR: Invalid VB\n");
                return false;
            }
            else
            {
                printf("ERROR: Copying to SPU memory\n");
                return false;
            }
        }
        if (SsVabTransCompleted(SS_WAIT_COMPLETED) == 0)
        {
            printf("ERROR: Didn't finish copy to SPU ram\n");
            return false;
        }

        //debug_dump_vh((unsigned long *)vhBuffer, mOpenVabId);

        return true;
    }

    bool PlaySEQ(unsigned char *seqBuffer)
    {
        if (mOpenVabId == -1)
        {
            printf("ERROR: No vab loaded\n");
            return false;
        }

        CloseSeq();

        mOpenSeqId = SsSeqOpen(reinterpret_cast<unsigned long *>(seqBuffer), mOpenVabId);
        if (mOpenSeqId == -1)
        {
            printf("Open SEQ failed\n");
            return false;
        }

        printf("Playing seq [%d]\n", mOpenSeqId);
        SsSeqSetVol(mOpenSeqId, SVOL, SVOL);
        SsSeqPlay(mOpenSeqId, SSPLAY_PLAY, 1);

        return true;
    }

    void CloseSeq()
    {
        if (mOpenSeqId != -1)
        {
            printf("Closing seq [%d]\n", mOpenSeqId);
            SsSeqClose(mOpenSeqId);
            mOpenSeqId = -1;
        }
    }

    short VabId() const
    {
        return mOpenVabId;
    }

private:
    void CloseVab()
    {
        if (mOpenVabId != -1)
        {
            printf("Closing vab [%d]\n", mOpenVabId);
            SsVabClose(mOpenVabId);
            mOpenVabId = -1;
        }
    }

    char mSeqTable[SS_SEQ_TABSIZ * 4 * 5];
    short mOpenVabId;
    short mOpenSeqId;
};

#endif // _SOUND_HPP_
