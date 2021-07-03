#define _STRING_H

#include <STDLIB.H>
#include <LIBGTE.H>
#include <LIBGPU.H>
#include <LIBGS.H>
#include <LIBDS.H>
#include <STDIO.H>
#include <LIBETC.H>
#include <STRINGS.H>
#include <KERNEL.H>
#include <LIBSND.H>
#include <LIBSPU.H>
#include "DebugFont.c"
#include "tims/FONT.h"
#include "tims/P1.h"
#include "tims/P2.h"

#define PSX_SOUND_LIB

#ifdef PSX_SOUND_LIB

// = PSYQ reimpl =================

extern "C"
{
    // LIBSPU

    // TODO
    void _SpuInit(int);

    void SpuInit(void)
    {
        _SpuInit(0);
    }

    // TODO
    void _SsInit(void);

    // TODO: SpuClearReverbWorkArea

    void SsInit(void)
    {
        ResetCallback();
        SpuInit();
        SpuClearReverbWorkArea(SPU_REV_MODE_ECHO); // 7
        _SsInit();
    }

    void SsStart(void)
    {
        // Works as a stub, not sure why - emulator issue?
    }

    void SsSetTickMode(long tick_mode)
    {
        // Seems fine to do nothing, probably because we call
        // SsSeqCalledTbyT manually.
    }

    // TODO: SpuIsTransferCompleted
#define STATIC_ASSERT(COND, MSG) typedef char static_assertion_##MSG[(COND) ? 1 : -1]

    struct SeqStruct
    {
        unsigned char *field_0_seq_ptr;
        unsigned char *field_4;
        unsigned char *field_8;
        unsigned char *field_C;
        unsigned char *field_10;
        char field_14_play_mode;
        char field_15;
        unsigned char field_16_running_status;
        unsigned char field_17_channel_idx;
        char field_18;
        char field_19;
        char field_1A_fn_idx;
        char field_1B;
        char field_1C;
        char field_1D;
        char field_1E;
        char field_1F;
        char field_20_l_count;
        char field_21;
        signed char field_22_next_seq;
        char field_23_next_sep;
        char field_24;
        char field_25;
        char field_26_vab_id;
        char field_27_panpot[16];
        char field_37_programs[16];
        char field_47;
        short field_48;
        short field_4A;
        short field_4C;
        short field_4E;
        short field_50;
        short field_52;
        short field_54;
        short field_56;
        short field_58_voll;
        short field_5A_volr;
        short field_5C;
        short field_5E;
        short field_60_vol[16];
        short field_80;
        char field_82;
        char field_83;
        int field_84;
        int field_88;
        int field_8C;
        int field_90_delta_value;
        int field_94; // tempo?
        unsigned int field_98_flags;
        int field_9C;
        int field_A0;
        int field_A4;
        int field_A8;
        int field_AC;
    } __attribute__((packed));

    extern short _snd_seq_s_max;
    extern short _snd_seq_t_max;
    extern SeqStruct *_ss_score[32];
    extern u_long _snd_openflag;

    void SsSetTableSize(char *table, short s_max, short t_max)
    {
        _snd_seq_s_max = s_max; // max 32
        _snd_seq_t_max = t_max; // max 16

        SeqStruct *pTypedTable = (SeqStruct *)table;

        // Point each item to the users supplied buffer
        for (int i = 0; i < _snd_seq_s_max; i++)
        {
            _ss_score[i] = &pTypedTable[i];
        }

        // Set each bit to mark an index that can't be used (cause its between s_max and the upper limit of 32bits)
        for (int i = _snd_seq_s_max; i < 32; i++)
        {
            _snd_openflag |= (1 << i);
        }

        for (int i = 0; i < _snd_seq_s_max; i++)
        {
            for (int j = 0; j < _snd_seq_t_max; j++)
            {
                // Init each structure
                SeqStruct *pItem = &_ss_score[i][j];
                pItem->field_98_flags = 0;
                pItem->field_22_next_seq = -1;
                pItem->field_23_next_sep = 0;
                pItem->field_48 = 0;
                pItem->field_4A = 0;
                pItem->field_9C = 0;
                pItem->field_A0 = 0;
                pItem->field_4C = 0;
                pItem->field_AC = 0;
                pItem->field_A8 = 0;
                pItem->field_A4 = 0;
                pItem->field_4E = 0;
                pItem->field_58_voll = 127;
                pItem->field_5A_volr = 127;
                pItem->field_5E = 127;
            }
        }
    }

    // TODO: SpuSetCommonAttr

    void SsSetMVol(short voll, short volr)
    {
        SpuCommonAttr attr;
        attr.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR; // 3
        attr.mvol.left = 129 * voll;
        attr.mvol.right = 129 * volr;
        SpuSetCommonAttr(&attr);
    }

    // TODO: SsVabOpenHead
    // TODO: SsVabTransBody

    short SsVabTransfer(u_char *vh_addr, u_char *vb_addr, short vabid, short i_flag)
    {
        vabid = SsVabOpenHead(vh_addr, vabid); // when vabid is -1 a new one is allocated
        if (vabid == -1)
        {
            return -1;
        }

        const int trans_body_result = SsVabTransBody(vb_addr, vabid);
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

    short SsVabTransCompleted(short immediateFlag)
    {
        return SpuIsTransferCompleted(immediateFlag);
    }

    // midi handler funcs
    extern _SsFCALL SsFCALL;

    // TODO
    short _SsInitSoundSeq(int seqId, int vabId, u_long *pSeqData);
    int _SsReadDeltaValue(short seq_access_num, short seq_num);

    int _SsReadDeltaValue(short seq_access_num, short seq_num)
    {
        SeqStruct *pStruc = &_ss_score[seq_access_num][seq_num]; // note 14bit access
        int delta_value = *pStruc->field_0_seq_ptr++;
        int result = 0;
        if (delta_value)
        {
            int dv_mul4 = 4 * delta_value;
            if ((delta_value & 0x80) != 0)
            {
                delta_value &= 0x7Fu;
                unsigned char next_byte = 0;
                do
                {
                    next_byte = *pStruc->field_0_seq_ptr++;
                    delta_value = (delta_value << 7) + (next_byte & 0x7F);
                } while ((next_byte & 0x80) != 0);
                dv_mul4 = 4 * delta_value;
            }
            result = 2 * (dv_mul4 + delta_value);
            pStruc->field_88 += result;
        }
        return result;
    }
   
    // DE_DELAY
    void _SsSetNrpnVabAttr19(short vab_id, short prog_no, short tone_no, VagAtr Vag, short a4, u_char attr)
    {
        //DEBUGPRINT(("DE_DELAY\n"));
        SsUtSetReverbDelay(attr);
    }

    // TODO:
    void _SsVmKeyOn(int seq_sep_no, short vabId, short unknown37, short note, short voll, short unknown27);
    void _SsVmKeyOff(int seq_sep_no, short vabId, short unknown37, short note);

    void _SsNoteOn(short seq_no, short sep_no, unsigned char note, unsigned char voll)
    {
        printf("_SsNoteOn seq_no=%d sep_no=%d note=%d voll=%d\n", seq_no, sep_no, static_cast<int>(note), static_cast<int>(voll));

        int seq_no_promoted = seq_no;
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];
        if (voll)
        {
            if (((pStru->field_80 >> pStru->field_17_channel_idx) & 1) == 0)
            {
                _SsVmKeyOn(
                    seq_no_promoted | (sep_no << 8),
                    pStru->field_26_vab_id,
                    pStru->field_37_programs[pStru->field_17_channel_idx],
                    note,
                    voll,
                    pStru->field_27_panpot[pStru->field_17_channel_idx]);
            }
        }
        else
        {
            _SsVmKeyOff(seq_no_promoted | (sep_no << 8), pStru->field_26_vab_id, pStru->field_37_programs[pStru->field_17_channel_idx], note);
        }
    }

    void _SsSetProgramChange(short seq_no, short sep_no, unsigned char programNum)
    {
        printf("_SsSetProgramChange seq_no=%d, sep_no=%d program=%d\n", seq_no, sep_no, static_cast<int>(programNum));
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];
        pStru->field_37_programs[pStru->field_17_channel_idx] = programNum;
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    extern int VBLANK_MINUS;

    // TODO: Not test and some funny stuff happening in here
    void _SsGetMetaEvent(short seq_no, short sep_no, unsigned char ev)
    {
        printf("_SsGetMetaEvent\n");

        SeqStruct* pStru = &_ss_score[seq_no][sep_no];
        pStru->field_0_seq_ptr += 3;

        int read = pStru->field_0_seq_ptr[2] | (pStru->field_0_seq_ptr[0] << 16) | (pStru->field_0_seq_ptr[1] << 8);

        read = 60000000 / read;

        pStru->field_94 = read;
  
        const int v6 = read * pStru->field_50;

        const int v8 = 15 * VBLANK_MINUS;
        const int v9 = 60 * VBLANK_MINUS;
        if (10 * v6 < 60 * VBLANK_MINUS)
        {
            const int v10 = 600 * VBLANK_MINUS / v6;
            pStru->field_52 = v10;
            pStru->field_54 = v10;
        }
        else
        {
            const int v11 = 10 * pStru->field_50 * pStru->field_94 / v9;
            const int v12 = 10 * pStru->field_50 * pStru->field_94 % v9;

            pStru->field_52 = -1;
            pStru->field_54 = v11;

            if (2 * v8 < v12)
            {
                pStru->field_54 = v11 + 1;
            }
        }
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    // TODO
    void _SsVmPitchBend(short seq_sep_no, short vabId, unsigned char program, unsigned char pitch);

    void SsSetPitchBend(short seq_no, short sep_no)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];
        unsigned char midi_byte = *pStru->field_0_seq_ptr;
        pStru->field_0_seq_ptr++;

        _SsVmPitchBend(
            seq_no | (sep_no << 8),
            pStru->field_26_vab_id,
            pStru->field_37_programs[pStru->field_17_channel_idx],
            midi_byte);

        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContBankChange(short seq_no, short sep_no)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];
        pStru->field_26_vab_id = *pStru->field_0_seq_ptr;
        pStru->field_0_seq_ptr++;
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsVmSetVol(short seq_sep_no, short vabId, short program, short voll, short volr);

    void _SsContMainVol(short seq_no, short sep_no, unsigned char vol)
    {
        SeqStruct* pStruc = &_ss_score[seq_no][sep_no];
        _SsVmSetVol(
            seq_no | (sep_no << 8),
            pStruc->field_26_vab_id,
            pStruc->field_37_programs[pStruc->field_17_channel_idx],
            vol,
            pStruc->field_27_panpot[pStruc->field_17_channel_idx]);
        pStruc->field_60_vol[pStruc->field_17_channel_idx] = vol;
        pStruc->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContPanpot(short seq_no, short sep_no, unsigned char panpot)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];
        _SsVmSetVol(
            seq_no | (sep_no << 8),
            pStru->field_26_vab_id,
            pStru->field_37_programs[pStru->field_17_channel_idx],
            pStru->field_60_vol[pStru->field_17_channel_idx],
            panpot);
        pStru->field_27_panpot[pStru->field_17_channel_idx] = panpot;
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    short SsSeqOpen(u_long *pSeqData, short vab_id)
    {
        // All bits used
        if (_snd_openflag == 0xFFFFFFFF)
        {
            printf("Can't Open Sequence data any more\n\n");
            return -1;
        }

        // Find an unused bit
        int openSeqId = 0;
        int openCounter = 0;
        while ((_snd_openflag & (1 << openCounter)) != 0)
        {
            if (++openCounter >= 32)
            {
                break;
            }
        }

        if (openCounter < 32)
        {
            // Seq id is the unused bit index
            openSeqId = openCounter;
        }

        _snd_openflag |= (1 << openSeqId);
        const short seqInit = _SsInitSoundSeq(openSeqId, vab_id, pSeqData);

        SsFCALL.noteon = (void (*)())_SsNoteOn;
        SsFCALL.programchange = (void (*)())_SsSetProgramChange;
        SsFCALL.metaevent = (void (*)())_SsGetMetaEvent;
        SsFCALL.pitchbend = (void (*)())_SsSetPitchBend;

        SsFCALL.control[CC_NUMBER] = (void (*)())_SsSetControlChange;
        SsFCALL.control[CC_BANKCHANGE] = (void (*)())_SsContBankChange;
        SsFCALL.control[CC_MAINVOL] = (void (*)())_SsContMainVol;
        SsFCALL.control[CC_PANPOT] = (void (*)())_SsContPanpot;
        SsFCALL.control[CC_EXPRESSION] = (void (*)())_SsContExpression;
        SsFCALL.control[CC_DAMPER] = (void (*)())_SsContDamper;
        SsFCALL.control[CC_NRPN1] = (void (*)())_SsContNrpn1;
        SsFCALL.control[CC_NRPN2] = (void (*)())_SsContNrpn2;
        SsFCALL.control[CC_RPN1] = (void (*)())_SsContRpn1;
        SsFCALL.control[CC_RPN2] = (void (*)())_SsContRpn2;
        SsFCALL.control[CC_EXTERNAL] = (void (*)())_SsContExternal;
        SsFCALL.control[CC_RESETALL] = (void (*)())_SsContResetAll;
        SsFCALL.control[CC_DATAENTRY] = (void (*)())_SsContDataEntry;

        SsFCALL.ccentry[DE_PRIORITY] = (void (*)())_SsSetNrpnVabAttr0;
        SsFCALL.ccentry[DE_MODE] = (void (*)())_SsSetNrpnVabAttr1;
        SsFCALL.ccentry[DE_LIMITL] = (void (*)())_SsSetNrpnVabAttr2;
        SsFCALL.ccentry[DE_LIMITH] = (void (*)())_SsSetNrpnVabAttr3;
        SsFCALL.ccentry[DE_ADSR_AR_L] = (void (*)())_SsSetNrpnVabAttr4;
        SsFCALL.ccentry[DE_ADSR_AR_E] = (void (*)())_SsSetNrpnVabAttr5;
        SsFCALL.ccentry[DE_ADSR_DR] = (void (*)())_SsSetNrpnVabAttr6;
        SsFCALL.ccentry[DE_ADSR_SL] = (void (*)())_SsSetNrpnVabAttr7;
        SsFCALL.ccentry[DE_ADSR_SR_L] = (void (*)())_SsSetNrpnVabAttr8;
        SsFCALL.ccentry[DE_ADSR_SR_E] = (void (*)())_SsSetNrpnVabAttr9;
        SsFCALL.ccentry[DE_ADSR_RR_L] = (void (*)())_SsSetNrpnVabAttr10;
        SsFCALL.ccentry[DE_ADSR_RR_E] = (void (*)())_SsSetNrpnVabAttr11;
        SsFCALL.ccentry[DE_ADSR_SR] = (void (*)())_SsSetNrpnVabAttr12;
        SsFCALL.ccentry[DE_VIB_TIME] = (void (*)())_SsSetNrpnVabAttr13;
        SsFCALL.ccentry[DE_PORTA_DEPTH] = (void (*)())_SsSetNrpnVabAttr14;
        SsFCALL.ccentry[DE_REV_TYPE] = (void (*)())_SsSetNrpnVabAttr15;
        SsFCALL.ccentry[DE_REV_DEPTH] = (void (*)())_SsSetNrpnVabAttr16;
        SsFCALL.ccentry[DE_ECHO_FB] = (void (*)())_SsSetNrpnVabAttr17;
        SsFCALL.ccentry[DE_ECHO_DELAY] = (void (*)())_SsSetNrpnVabAttr18;
        SsFCALL.ccentry[DE_DELAY] = (void (*)())_SsSetNrpnVabAttr19;

        if (seqInit != -1)
        {
            // BUG: doesn't unset _snd_openflag
            return openSeqId;
        }

        return -1;
    }

    // TODO
    void _SsVmSetSeqVol(short seq_sep_num, short voll, short volr);

    void SsSeqSetVol(short seq_access_num, short voll, short volr)
    {
        SeqStruct *pStru = _ss_score[seq_access_num];
        if (pStru->field_98_flags == 1)
        {
            _SsVmSetSeqVol(seq_access_num, voll, volr);
        }
        else
        {
            pStru->field_58_voll = voll;
            pStru->field_5A_volr = volr;
        }
    }

    void Snd_SetPlayMode(int seq_sep_num, int idx, char play_mode, char l_count)
    {
        SeqStruct* pStru = &_ss_score[seq_sep_num][idx]; // TODO: 14bit access

        pStru->field_0_seq_ptr = pStru->field_4;
        pStru->field_8 = pStru->field_4;
        pStru->field_C = pStru->field_4;

        pStru->field_98_flags &= ~0x200u;
        pStru->field_98_flags &= ~4u;

        pStru->field_20_l_count = l_count;

        if (play_mode == 1)
        {
            pStru->field_98_flags |= 1u;
            pStru->field_14_play_mode = play_mode;
            pStru->field_21 = 0;
            _SsVmSetSeqVol(seq_sep_num | (idx << 8), pStru->field_58_voll, pStru->field_5A_volr); // TODO: Check param1
        }
        else if (play_mode == 0)
        {
            pStru->field_98_flags |= 2u;
        }
    }

    void SsSeqPlay(short seq_access_num, char play_mode, short l_count)
    {
        Snd_SetPlayMode(seq_access_num, 0, play_mode, l_count);
    }

    // TODO
    void _SsVmSeqKeyOff(short seq_idx);

    static void Impl_SsSeqClose(short seq_idx)
    {
        // Volume + voices off
        _SsVmSetSeqVol(seq_idx, 0, 0);
        _SsVmSeqKeyOff(seq_idx);

        // Clear the used bit
        const int seq_idx_int = seq_idx;
        _snd_openflag &= ~(1 << seq_idx_int);

        // Reset all the SEP entries
        for (int i = 0; i < _snd_seq_t_max; i++)
        {
            SeqStruct *pItem = &_ss_score[seq_idx_int][i];

            pItem->field_98_flags = 0;

            pItem->field_22_next_seq = -1;
            pItem->field_23_next_sep = 0;

            pItem->field_48 = 0;
            pItem->field_4A = 0;
            pItem->field_9C = 0;
            pItem->field_A0 = 0;
            pItem->field_4C = 0;
            pItem->field_AC = 0;
            pItem->field_A8 = 0;
            pItem->field_A4 = 0;
            pItem->field_4E = 0;
            pItem->field_58_voll = 127;
            pItem->field_5A_volr = 127;
        }
    }

    void SsSeqClose(short seq_access_num)
    {
        Impl_SsSeqClose(seq_access_num);
    }

    extern volatile int _snd_ev_flag;
    extern short note2pitch2;

    void _SsVmKeyOffNow(void);

    short SsUtKeyOffV(short voice)
    {
        if (_snd_ev_flag == 1)
        {
            return -1;
        }

        _snd_ev_flag = 1;

        short result = -1;
        if (voice < 24)
        {
            note2pitch2 = voice;
            _SsVmKeyOffNow();
            result = 0;
        }

        _snd_ev_flag = 0;
        return result;
    }

    long _SsVmSeKeyOn(u_char vab, u_char program, u_char note, u_char pitch, u_short volL, u_short volR);

    long SsVoKeyOn(long vab_pro, long pitch, u_short volL, u_short volR)
    {
        return _SsVmSeKeyOn(
            (vab_pro >> 8),
            vab_pro & 0xFF,
            (unsigned short)(pitch >> 8),
            pitch & 0xFF,
            volL,
            volR);
    }

    // TODO
    void _SsVmFlush(void);

    void _SsSndCrescendo(short seqNum, short sepNum);
    void _SsSndTempo(short seqNum, short sepNum);
    void _SsSndReplay(short seqNum, short sepNum);
    void _SsVmDamperOff(void);
    void _SsSeqGetEof(short seq_access_num, short sep_num);
    void _SsGetSeqData(short seq_idx, short sep_idx);
    void _SsSeqPlay(short seq_access_num, short seq_num);

    void _SsSndNextSep(int seq_no, short sep_no)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no]; // TODO: 14bit access

        pStru->field_20_l_count = 1; // TODO: Setting field 21 to 0 or 1 also ??
        pStru->field_21 = 0;
        pStru->field_98_flags &= ~0x100u;
        pStru->field_98_flags &= ~8u;
        pStru->field_98_flags &= ~2u;
        pStru->field_98_flags &= ~4u;
        pStru->field_98_flags &= ~0x200u;
        pStru->field_14_play_mode = 1;
        pStru->field_0_seq_ptr = pStru->field_4;
        pStru->field_98_flags |= 1u;
    }

    void _SsSndPause(short seq_no, short sep_no)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no]; // TODO: 14bit access

        _SsVmSeqKeyOff(seq_no | (sep_no << 8));

        pStru->field_14_play_mode = 0;
        pStru->field_98_flags &= ~2u;
    }

/*
    void _SsSeqGetEof(short seq_access_num, short sep_num)
    {
        int seq_access_num_ = seq_access_num; // promote to 32bits

        SeqStruct* pStru = &_ss_score[seq_access_num][sep_num]; // Note: 14bit access
        pStru->field_21++;

        if (pStru->field_20_l_count > 0)
        {
            if (pStru->field_21 >= pStru->field_20_l_count)
            {
                // Max loop iterations hit

                pStru->field_98_flags &= ~1u;
                pStru->field_98_flags &= ~8u;
                pStru->field_98_flags &= ~2u;
                pStru->field_98_flags |= 0x200u;
                pStru->field_98_flags |= 4u;
                
                pStru->field_14_play_mode = 0;

                if (pStru->field_98_flags & 0x400)
                {
                    pStru->field_8 = pStru->field_C;
                }
                else
                {
                    pStru->field_8 = pStru->field_4;
                }

                // Start the next track if there is one
                if (pStru->field_22_next_seq != -1)
                {
                    pStru->field_14_play_mode = 0;
                    _SsSndNextSep(pStru->field_22_next_seq, pStru->field_23_next_sep);
                    _SsVmSeqKeyOff(seq_access_num_ | (sep_num << 8));
                }

                // Kill this track (bug: Already done if there is next track?)
                _SsVmSeqKeyOff(seq_access_num_ | (sep_num << 8));
                pStru->field_90_delta_value = pStru->field_54;
            }
            else
            {
                // More loops iterations to do

                pStru->field_88 = 0;
                pStru->field_1C = 0;
                pStru->field_90_delta_value = 0;

                if (pStru->field_98_flags & 0x400)
                {
                    pStru->field_0_seq_ptr = pStru->field_C;
                    pStru->field_8 = pStru->field_C;
                }
                else
                {
                    pStru->field_0_seq_ptr = pStru->field_4;
                    pStru->field_8 = pStru->field_4;

                }
            }
        }
        else
        {
            // No looping
            pStru->field_88 = 0;
            pStru->field_1C = 0;
            
            pStru->field_90_delta_value = 0;

            if (pStru->field_98_flags & 0x400)
            {
                pStru->field_0_seq_ptr = pStru->field_C;
            }
            else
            {
                pStru->field_0_seq_ptr = pStru->field_4;
            }
        }
    }

    void _SsGetSeqData(short seq_idx, short sep_idx)
    {
        SeqStruct* pSeqPtr = &_ss_score[seq_idx][sep_idx];
        const unsigned char midi_byte = *pSeqPtr->field_0_seq_ptr;
        unsigned char* pNext_midi_byte = pSeqPtr->field_0_seq_ptr + 1;
        pSeqPtr->field_0_seq_ptr = pNext_midi_byte;

        unsigned char midi_byte_and_80 = 0;
        if ((pSeqPtr->field_98_flags & 0x401) == 0x401)
        {
            midi_byte_and_80 = midi_byte & 0x80;
            if (pNext_midi_byte == pSeqPtr->field_10 + 1)
            {
                _SsSeqGetEof(seq_idx, sep_idx);
                return;
            }
        }
        else
        {
            midi_byte_and_80 = midi_byte & 0x80;
        }

        if (midi_byte_and_80)
        {
            pSeqPtr->field_17_channel_idx = midi_byte & 0xF; // Channel ?
            
            unsigned char midi_byte_and_F0 = midi_byte & 0xF0;
            if (midi_byte_and_F0 == 0xC0)
            {
                pSeqPtr->field_16_running_status = 0xC0;
                pSeqPtr->field_0_seq_ptr++;
                //SsFCALL.programchange(seq_idx);
                _SsSetProgramChange(seq_idx, sep_idx, midi_byte);
                return;
            }

            if ((midi_byte & 0xF0u) < 0xC1)
            {
                if (midi_byte_and_F0 == 0x90)
                {
                    pSeqPtr->field_16_running_status = 0x90;
                    const unsigned char midi_byte_next_ = *pSeqPtr->field_0_seq_ptr++;
                    const unsigned char midi_byte_ = *pSeqPtr->field_0_seq_ptr++;
                    pSeqPtr->field_90_delta_value = _SsReadDeltaValue(seq_idx, sep_idx);
                    //SsFCALL.noteon(seq_idx, sep_idx, midi_byte_next_, midi_byte_);
                    _SsNoteOn(seq_idx, sep_idx, midi_byte_next_, midi_byte_);
                }
                else if (midi_byte_and_F0 == 0xB0)
                {
                    pSeqPtr->field_16_running_status = 0xB0;
                    pSeqPtr->field_0_seq_ptr++;
                    //SsFCALL.control[CC_NUMBER](seq_idx);
                    _SsSetControlChange(seq_idx, sep_idx, midi_byte);
                }
                return;
            }

            if (midi_byte_and_F0 != 0xE0)
            {
                if (midi_byte_and_F0 != 0xF0)
                {
                    return;
                }

                pSeqPtr->field_16_running_status = 0xFF; // TODO ?? -1;
                const unsigned char midi_byte__ = *pSeqPtr->field_0_seq_ptr;
                pSeqPtr->field_0_seq_ptr++;

                if (midi_byte__ == 0x2F)
                {
                    _SsSeqGetEof(seq_idx, sep_idx);
                    return;
                }

                //SsFCALL.metaevent(seq_idx);
                _SsGetMetaEvent(seq_idx, sep_idx, midi_byte);
                return;
            }

            pSeqPtr->field_16_running_status = 0xE0;
            pSeqPtr->field_0_seq_ptr++;
            //SsFCALL.pitchbend(seq_idx, sep_idx);
            _SsSetPitchBend(seq_idx, sep_idx);
            return;
        }

        const unsigned char running_status = pSeqPtr->field_16_running_status;
        if (running_status == 0xC0)
        {
            //SsFCALL.programchange(seq_idx);
            _SsSetProgramChange(seq_idx, sep_idx, midi_byte); // short seq_no, short sep_no, unsigned char ch
            return;
        }

        if (pSeqPtr->field_16_running_status >= 0xC1)
        {
            if (running_status != 0xE0)
            {
                if (running_status != 0xFF)
                {
                    return;
                }

                if (midi_byte == 0x2F)
                {
                    _SsSeqGetEof(seq_idx, sep_idx);
                    return;
                }

                //SsFCALL.metaevent(seq_idx);
                _SsGetMetaEvent(seq_idx, sep_idx, midi_byte);
                return;
            }

            //SsFCALL.pitchbend(seq_idx, sep_idx);
            _SsSetPitchBend(seq_idx, sep_idx);
            return;
        }

        if (running_status == 0x90)
        {
            unsigned char next_midi_byte__ = *pSeqPtr->field_0_seq_ptr++;
            pSeqPtr->field_90_delta_value = _SsReadDeltaValue(seq_idx, sep_idx);
            //SsFCALL.noteon(seq_idx, sep_idx, midi_byte, next_midi_byte__);
            _SsNoteOn(seq_idx, sep_idx, midi_byte, next_midi_byte__);
        }
        else if (running_status == 0xB0)
        {
            //SsFCALL.control[CC_NUMBER](seq_idx);
            _SsSetControlChange(seq_idx, sep_idx, midi_byte);
        }
    }

    void _SsSeqPlay(short seq_access_num, short seq_num)
    {
        SeqStruct* pSeqStructure = &_ss_score[seq_access_num][seq_num]; // note 14bit access

        const int diff = pSeqStructure->field_90_delta_value - pSeqStructure->field_54;
        if ( diff <= 0)
        {
            int local_90_ = pSeqStructure->field_90_delta_value;
            if (pSeqStructure->field_54 < local_90_)
            {
                return;
            }

            int new_90 = 0;
            do
            {
                do
                {
                    _SsGetSeqData(seq_access_num, seq_num);
                } while (!pSeqStructure->field_90_delta_value);

                local_90_ += pSeqStructure->field_90_delta_value;
                new_90 = local_90_ - pSeqStructure->field_54;
            } while (local_90_ < pSeqStructure->field_54);
            pSeqStructure->field_90_delta_value = new_90;
        }
        else
        {
            if (pSeqStructure->field_52 > 0)
            {
                pSeqStructure->field_52--;
                return;
            }

            if (pSeqStructure->field_52)
            {
                pSeqStructure->field_90_delta_value = diff;
                return;
            }

            pSeqStructure->field_52 = pSeqStructure->field_54;
            pSeqStructure->field_90_delta_value--;
        }
    }
*/

    void _SsSndPlay(short seq_access_num, short seq_num)
    {
        _SsSeqPlay(seq_access_num, seq_num);
    }

    void _SsSndStop(short seq_or_sep_access_num, short seq_num)
    {
        SeqStruct* pStru = &_ss_score[seq_or_sep_access_num][seq_num]; // TODO: 14bit access
        pStru->field_98_flags &= ~1u;
        pStru->field_98_flags &= ~2u;
        pStru->field_98_flags &= ~8u;
        pStru->field_98_flags &= ~0x400u;
        pStru->field_98_flags |= 4u;

        _SsVmSeqKeyOff(seq_or_sep_access_num | (seq_num << 8));
        _SsVmDamperOff();

        pStru->field_14_play_mode = 0;
        pStru->field_88 = 0;
        pStru->field_1C = 0;
        pStru->field_18 = 0;
        pStru->field_19 = 0;
        pStru->field_1E = 0;
        pStru->field_1A_fn_idx = 0;
        pStru->field_1B = 0;
        pStru->field_1F = 0;
        pStru->field_17_channel_idx = 0;
        pStru->field_21 = 0;
        pStru->field_1C = 0;
        pStru->field_1D = 0;
        pStru->field_15 = 0;
        pStru->field_16_running_status = 0;
        pStru->field_90_delta_value = pStru->field_84;
        pStru->field_94 = pStru->field_8C;
        pStru->field_54 = pStru->field_56;
        pStru->field_0_seq_ptr = pStru->field_4;
        pStru->field_8 = pStru->field_4;

        for (int i = 0; i < 16; i++)
        {
            pStru->field_37_programs[i] = i;
            pStru->field_27_panpot[i] = 64;
            pStru->field_60_vol[i] = 127;
        }

        pStru->field_5C = 127;
        pStru->field_5E = 127;
    }

    static void RunTrack(int i, int j)
    {
        SeqStruct *pItem = &_ss_score[i][j];
        if (pItem->field_98_flags & 1)
        {
            _SsSndPlay(i, j);

            if (pItem->field_98_flags & 0x10)
            {
                _SsSndCrescendo(i, j);
            }

            if (pItem->field_98_flags & 0x20)
            {
                _SsSndCrescendo(i, j);
            }

            if (pItem->field_98_flags & 0x40)
            {
                _SsSndTempo(i, j);
            }

            if (pItem->field_98_flags & 0x80)
            {
                _SsSndTempo(i, j);
            }
        }

        if (pItem->field_98_flags & 2)
        {
            _SsSndPause(i, j);
        }

        if (pItem->field_98_flags & 8)
        {
            _SsSndReplay(i, j);
        }

        if (pItem->field_98_flags & 4)
        {
            _SsSndStop(i, j);
            pItem->field_98_flags = 0;
        }
    }

    static void RunTracks()
    {
        // For each SEP block
        for (int i = 0; i < _snd_seq_s_max; i++)
        {
            // Is it on?
            if (_snd_openflag & (1 << i))
            {
                // Go through each SEQ track
                for (int j = 0; j < _snd_seq_t_max; j++)
                {
                    RunTrack(i, j);
                }
            }
        }
    }

    void SsSeqCalledTbyT(void)
    {
        if (_snd_ev_flag != 1)
        {
            _snd_ev_flag = 1;

            _SsVmFlush();

            RunTracks();

            _snd_ev_flag = 0;
        }
    }

} // extern "C"

// ===============================

// ok
void Sound_SsInit()
{
    ::SsInit();
}

// ok
void Sound_SsSetTableSize(char *table, short s_max, short t_max)
{
    ::SsSetTableSize(table, s_max, t_max);
}

// ok ?
void Sound_SsSetTickMode(long tick_mode)
{
    ::SsSetTickMode(tick_mode);
}

// ok ?
void Sound_SsStart()
{
    ::SsStart();
}

// ok
void Sound_SsSetMVol(short voll, short volr)
{
    ::SsSetMVol(voll, volr);
}

// ok
int Sound_SsVabTransfer(u_char *vh_addr, u_char *vb_addr, short vabid, short i_flag)
{
    return ::SsVabTransfer(vh_addr, vb_addr, vabid, i_flag);
}

// ok
short Sound_SsVabTransCompleted(short immediateFlag)
{
    return ::SsVabTransCompleted(immediateFlag);
}

// ok
short Sound_SsSeqOpen(u_long *addr, short vab_id)
{
    return ::SsSeqOpen(addr, vab_id);
}

// ok
void Sound_SsSeqSetVol(short seq_access_num, short voll, short volr)
{
    ::SsSeqSetVol(seq_access_num, voll, volr);
}

// ok
void Sound_SsSeqPlay(short seq_access_num, char play_mode, short l_count)
{
    ::SsSeqPlay(seq_access_num, play_mode, l_count);
}

// ok
void Sound_SsSeqClose(short seq_access_num)
{
    ::SsSeqClose(seq_access_num);
}

// TODO: but isn't in test binary
void Sound_SsVabClose(short vab_id)
{
    ::SsVabClose(vab_id);
}

// ok
short Sound_SsUtKeyOffV(short voice)
{
    return ::SsUtKeyOffV(voice);
}

// ok
long Sound_SsVoKeyOn(long vab_pro, long pitch, u_short volL, u_short volR)
{
    return ::SsVoKeyOn(vab_pro, pitch, volL, volR);
}

void Sound_SsSeqCalledTbyT()
{
    ::SsSeqCalledTbyT();
}
#else
// !
void Sound_SsInit()
{
    SPU_Reset();
}

void Sound_SsSetTableSize(char *table, short s_max, short t_max)
{
}

void Sound_SsSetTickMode(long tick_mode)
{
}

// !
void Sound_SsStart()
{
    SPU_Enable();
}

// !
void Sound_SsSetMVol(short voll, short volr)
{
}

// !
int Sound_SsVabTransfer(u_char *vh_addr, u_char *vb_addr, short vabid, short i_flag)
{
    //SPU_LoadVAB();
    return 0;
}

// !
short Sound_SsVabTransCompleted(short immediateFlag)
{
    return 0;
}

short Sound_SsSeqOpen(u_long *addr, short vab_id)
{
    return 0;
}

void Sound_SsSeqSetVol(short seq_access_num, short voll, short volr)
{
}

void Sound_SsSeqPlay(short seq_access_num, char play_mode, short l_count)
{
}

void Sound_SsSeqClose(short seq_access_num)
{
}

// !
void Sound_SsVabClose(short vab_id)
{
}

// !
short Sound_SsUtKeyOffV(short voice)
{
    SPU_VoiceOff(voice);
    return 0;
}

// !
long Sound_SsVoKeyOn(long vab_pro, long pitch, u_short volL, u_short volR)
{

    return 0;
}

void Sound_SsSeqCalledTbyT()
{
}

#endif

#define OT_LENGTH 40      // the ordertable length
#define PACKETMAX 4000    // the maximum number of objects on the screen
#define SCREEN_WIDTH 368  // screen width
#define SCREEN_HEIGHT 240 // screen height

#define countof(x) sizeof(x) / sizeof(x[0])
class Graphics
{
public:
    Graphics()
    {
        mActiveBuffer = 0;
    }

    void Init()
    {
        //::SetVideoMode(IsPalBios() ? MODE_PAL : MODE_NTSC);

        // NTSC force for correct SEQ timings
        ::SetVideoMode(MODE_NTSC);

        ::GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER | GsOFSGPU, 1, 0); // set the graphics mode resolutions. You may also try using 'GsNONINTER' (read LIBOVR46.PDF in PSYQ/DOCS for detailed information)
        ::GsDefDispBuff(0, 0, 0, SCREEN_HEIGHT);                              // set the top left coordinates of the two buffers in video memory

        // init the ordering tables
        mOrderingTable[0].length = OT_LENGTH;
        mOrderingTable[1].length = OT_LENGTH;
        mOrderingTable[0].org = mOrderingTableTag[0];
        mOrderingTable[1].org = mOrderingTableTag[1];

        ::GsClearOt(0, 0, &mOrderingTable[0]);
        ::GsClearOt(0, 0, &mOrderingTable[1]);

        clearVRAM();

        ::FntLoad(960, 0); // load the font from the BIOS into VRAM/SGRAM

        ::SetDumpFnt(FntOpen(10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20, 0, 1024 * 4)); // screen X,Y | max text length X,Y | automatic background clear 0,1 | max characters

        // Overwrite the debug font with something less ugly
        ::LoadImage(&DbgFnt_ImageRect, reinterpret_cast<u_long *>(DbgFnt_ImageData));
        ::LoadImage(&DbgFnt_ClutRect, reinterpret_cast<u_long *>(DbgFnt_ClutData));
    }

    void PreRender()
    {
        mActiveBuffer = GsGetActiveBuff();                          // get the current buffer
        ::GsSetWorkBase((PACKET *)mGpuPacketBuffer[mActiveBuffer]); // setup the packet workbase
        ::GsClearOt(0, 0, &mOrderingTable[mActiveBuffer]);          // clear the ordering table
    }

    void Render()
    {
        ::DrawSync(0);                                          // wait for all drawing to finish
        ::VSync(0);                                             // wait for v_blank interrupt
        ::GsSwapDispBuff();                                     // flip the double buffers
        ::GsSortClear(0, 0, 0, &mOrderingTable[mActiveBuffer]); // clear the ordering table with a background color.
        ::GsDrawOt(&mOrderingTable[mActiveBuffer]);             // Draw the ordering table for the CurrentBuffer
        ::FntFlush(-1);                                         // refresh the font
    }

    GsOT *OtPtr()
    {
        return &mOrderingTable[mActiveBuffer];
    }

private:
    bool IsPalBios()
    {
        // within the BIOS, if the address 0xBFC7FF52 equals 'E' (SCEE string), set it as PAL otherwise, set it as NTSC
        return *(const char *)0xbfc7ff52 == 'E';
    }

    void clearVRAM()
    {
        RECT rectTL;

        setRECT(&rectTL, 0, 0, 1024, 512);
        ClearImage2(&rectTL, 0, 0, 0);

        DrawSync(0); // ensure that the VRAM is clear before exiting
    }

private:
    GsOT mOrderingTable[2];
    GsOT_TAG mOrderingTableTag[2][PACKETMAX];
    PACKET mGpuPacketBuffer[2][PACKETMAX];
    int mActiveBuffer;
};

//u_long __ramsize   = 0x00200000; // force 2 megabytes of RAM
//u_long __stacksize = 0x00004000; // force 16 kilobytes of stack

#define kNumLvls 33
const char *kLvlFiles[kNumLvls] =
    {
        "\\PM.LVL",
        "\\P2\\C1.LVL",
        "\\P2\\D1.LVL",
        "\\P2\\D2.LVL",
        "\\P2\\D7.LVL",
        "\\P2\\E2.LVL",
        "\\P2\\R2.LVL",
        "\\P2\\R6.LVL",
        "\\E1.LVL",
        "\\F1.LVL",
        "\\F2.LVL",
        "\\F4.LVL",
        "\\L1.LVL",
        "\\R1.LVL",
        "\\S1.LVL",
        "\\BA\\BA.LVL",
        "\\BM\\BM.LVL",
        "\\BR\\BR.LVL",
        "\\BW\\BW.LVL",
        "\\CR\\CR.LVL",
        "\\FD\\FD.LVL",
        "\\MI\\MI.LVL",
        "\\NE\\NE.LVL",
        "\\PV\\PV.LVL",
        "\\ST\\ST.LVL",
        "\\SV\\SV.LVL",

        "\\ABESODSE\\C1.LVL",
        "\\ABESODSE\\E1.LVL",
        "\\ABESODSE\\R1.LVL",
        "\\ABESODSE\\S1.LVL",

        "\\ABE2\\MI.LVL",
        "\\ABE2\\CR.LVL",
        "\\ABE2\\ST.LVL",
};

void my_memcpy(void *dst, void *src, int size)
{
    unsigned char *d = reinterpret_cast<unsigned char *>(dst);
    unsigned char *s = reinterpret_cast<unsigned char *>(src);
    for (int i = 0; i < size; i++)
    {
        d[i] = s[i];
    }
}

class CdRom
{
public:
    void Init(bool debugOn = true)
    {
        printf("DsInit\n");
        ::DsInit();
        ::DsSetDebug(debugOn ? 1 : 0);
    }

    void DeInit()
    {
        ::DsClose();
    }

    bool Ready() const
    {
        return ::DsSystemStatus() == DslReady;
    }

    // Stop the cd spinning
    void StopCdRotation()
    {
        DslLOC pos;
        printf("Calling DsControl...\n");
        ::DsControl(DslStop, reinterpret_cast<u_char *>(&pos), mStatusBuffer);
    }

    // Check if file exists, if yes returns true and populates fileInfo, else returns false and fileInfo is undefined
    bool Find(const char *fileName, DslFILE &fileInfo)
    {
        printf("Calling DsSearchFile...\n");
        if (!::DsSearchFile(&fileInfo, const_cast<char *>(fileName)))
        {
            printf("File %s not found\n", fileName);
            return false;
        }
        else
        {
            printf("File %s found, starts at sector %d size %ld\n", fileName, fileInfo.pos.sector, fileInfo.size);
            return true;
        }
    }

    bool ReadSectors(int sector, int numSectors, void *buffer)
    {
        DslLOC pos;
        ::DsIntToPos(sector, &pos);

        printf("Start read of [%d] sectors from sector [%d]\n", numSectors, sector);
        if (::DsRead(&pos, numSectors, reinterpret_cast<u_long *>(buffer), DslModeSpeed | DslModeSize1) == 0)
        {
            printf("Read failure\n");
            return false;
        }

        int ret = 0;
        do
        {
            ret = ::DsReadSync(mStatusBuffer);
        } while (ret > 0);
        printf("ReadSync [%d]\n", ret);
        return ret == 0;
    }

private:
    u_char mStatusBuffer[8];
};

typedef unsigned int Uint32;
typedef unsigned short int Uint16;
typedef signed short int Sint16;
typedef unsigned char Uint8;
typedef signed char Sint8;

const static Uint32 KMaxLvlArchiveFileNameLength = 12;
const static Uint32 kSectorSize = 2048;
const static Uint32 kMaxFiles = 500;

enum ePads
{
    ePad1 = 1,
    ePad2 = 2,
    eAny = 3
};

// Delects press, e.g is x pressed == yes when x is is down but wasn't previously down
class Controller
{
private:
    u_long GetButtonMask(ePads pad, u_long button)
    {
        switch (pad)
        {
        case ePad1:
            return _PAD(0, button);

        case ePad2:
            return _PAD(1, button);

        case eAny:
            return _PAD(0, button) | _PAD(1, button);
        }

        // Impossible - but have to shut compiler up
        return 0;
    }

    bool ButtonPressed(ePads pad, u_long button)
    {
        return (mPadState & GetButtonMask(pad, button)) && !(mPrevPadState & GetButtonMask(pad, button));
    }

public:
    Controller()
        : mPrevPadState(0), mPadState(0)
    {
    }

    void Init()
    {
        // This API only supports the 16-button controller, which is fine
        ::PadInit(0);
    }

    bool Up(ePads pad = eAny)
    {
        return ButtonPressed(pad, PADLup);
    }

    bool Down(ePads pad = eAny)
    {
        return ButtonPressed(pad, PADLdown);
    }

    bool Left(ePads pad = eAny)
    {
        return ButtonPressed(pad, PADLleft);
    }

    bool Right(ePads pad = eAny)
    {
        return ButtonPressed(pad, PADLright);
    }

    bool Circle(ePads pad = eAny)
    {
        return ButtonPressed(pad, PADRright);
    }

    bool Cross(ePads pad = eAny)
    {
        return ButtonPressed(pad, PADRdown);
    }

    bool Triangle(ePads pad = eAny)
    {
        return ButtonPressed(pad, PADRup);
    }

    bool Square(ePads pad = eAny)
    {
        return ButtonPressed(pad, PADRleft);
    }

    bool Select(ePads pad = eAny)
    {
        return ButtonPressed(pad, PADselect);
    }

    bool Start(ePads pad = eAny)
    {
        return ButtonPressed(pad, PADstart);
    }

    void Update()
    {
        mPrevPadState = mPadState;
        mPadState = ::PadRead(0);
    }

private:
    u_long mPrevPadState;
    u_long mPadState;
};

static Controller gInput;
static CdRom cdRom;

class CdStream
{
public:
    CdStream()
        : mPos(0), mStartSector(0), mSector(0)
    {
    }

    void Reset(int startSector)
    {
        mPos = 0;
        mStartSector = startSector;
        mSector = startSector;
    }

    void ReadUInt8(Uint8 &output)
    {
        ReadBytes(reinterpret_cast<Uint8 *>(&output), sizeof(Uint8));
    }

    void ReadUInt32(Uint32 &output)
    {
        ReadBytes(reinterpret_cast<Uint8 *>(&output), sizeof(Uint32));
    }

    void ReadUInt16(Uint16 &output)
    {
        ReadBytes(reinterpret_cast<Uint8 *>(&output), sizeof(Uint16));
    }

    void ReadSInt16(Sint16 &output)
    {
        ReadBytes(reinterpret_cast<Uint8 *>(&output), sizeof(Sint16));
    }

    void ReadBytes(Sint8 *pDest, size_t destSize)
    {
        ReadBytes(reinterpret_cast<Uint8 *>(pDest), destSize);
    }

    bool ReadBytes(Uint8 *pDest, size_t destSize)
    {
        for (;;)
        {
            const int posWithinSector = mPos % (kSectorSize);
            if (posWithinSector == 0)
            {
                //printf("Read to next sector POS zero [%d]\n", mSector);
                if (!cdRom.ReadSectors(mSector, 1, mSectorBuffer))
                {
                    return false;
                }
                mSector++;
            }

            //assert(posWithinSector <= kSectorSize);

            // If what we will read is more than the remaining data in this sector
            //printf("SECTOR POS: [%d] END [%d]\n", posWithinSector, posWithinSector + destSize);
            if (posWithinSector + destSize > kSectorSize)
            {
                //printf("Partial fit\n");

                const int spaceLeft = 2048 - static_cast<int>(posWithinSector);
                if (spaceLeft < 0)
                {
                    printf("Error: Space left [%d]\n", spaceLeft);
                    for (;;)
                    {
                    }
                }

                const int dataLeftAfterRead = static_cast<int>(destSize) - spaceLeft;
                if (dataLeftAfterRead < 0)
                {
                    printf("Error: dataLeftAfterRead [%d]\n", dataLeftAfterRead);
                    for (;;)
                    {
                    }
                }

                my_memcpy(pDest, mSectorBuffer + posWithinSector, spaceLeft);
                mPos += spaceLeft;

                pDest += spaceLeft;
                destSize = dataLeftAfterRead;
                continue;
            }
            else if (destSize)
            {
                //printf("Fully fits\n");
                my_memcpy(pDest, mSectorBuffer + posWithinSector, destSize);
                mPos += destSize;
            }

            break;
        }
        return true;
    }

    // Seek from 0 to file size
    bool Seek(size_t pos)
    {
        printf("Seek to [%d]\n", pos);

        // Add on the start sector position
        mSector = (pos / kSectorSize) + mStartSector;
        mPos = (mStartSector * kSectorSize) + pos;
        if ((mPos % kSectorSize) != 0)
        {
            printf("Seek must fetch sector [%d]\n", mSector);
            // Fetch the buffer now else mSectorBuffer will have stale data
            if (!cdRom.ReadSectors(mSector, 1, mSectorBuffer))
            {
                return false;
            }
        }
        return true;
    }

    int StartSector() const
    {
        return mStartSector;
    }

private:
    Uint32 mPos;
    int mStartSector;
    int mSector;
    char mSectorBuffer[2048];
};

static Uint8 gVbBuffer[2048 * 400];
static Uint8 gVhBuffer[2048 * 100];
static Uint8 gSeqBuffer[2048 * 100];

class Lvl
{
public:
    static Uint32 MakeType(Uint8 b4, Uint8 b3, Uint8 b2, Uint8 b1)
    {
        return (static_cast<Uint32>(b1) << 24) +
               (static_cast<Uint32>(b2) << 16) +
               (static_cast<Uint32>(b3) << 8) +
               static_cast<Uint32>(b4);
    }

    struct ChunkHeader
    {
        Uint32 iSize;     // Size inc header
        Uint32 iRefCount; // Ref count, should be zero when loaded
        Uint32 iType;
        Uint32 iId; // Id
    };

    struct LvlHeader
    {
        Uint32 mFirstFileOffset;
        Uint32 mNull1;
        Uint32 mMagic; // (Indx)
        Uint32 mNull2;
        Uint32 mNumFiles;
        Uint32 mFirstFileOffsetSectorNum;
        Uint32 mUnknown2;
        Uint32 mUnknown3;
    };

    struct FileRecord
    {
        // This array will not be NULL terminated
        char mFileNameBytes[KMaxLvlArchiveFileNameLength];

        Uint32 mStartSector;
        Uint32 mNumSectors;
        Uint32 mFileSize;
    };

    LvlHeader mHeader;
    FileRecord mFiles[kMaxFiles];

public:
    int NumFiles() const
    {
        return mHeader.mNumFiles;
    }

    const FileRecord &FileAt(int index)
    {
        return mFiles[index];
    }

    Lvl()
    {
        printf("lvl ctor\n");
    }

    ~Lvl()
    {
        printf("Lvl dtor\n");
    }

    bool Load(const char *fileName)
    {
        char buffer[256] = {};
        sprintf(buffer, "%s;1", fileName);

        printf("Searching for [%s]...\n", fileName);
        if (cdRom.Find(buffer, mFileInfo))
        {
            // Load the first sector so we can grab the header
            const int sector = ::DsPosToInt(&mFileInfo.pos);
            mStream.Reset(sector);

            if (!mStream.ReadBytes(reinterpret_cast<Uint8 *>(&mHeader), sizeof(LvlHeader)))
            {
                return false;
            }

            Print(mHeader);

            if (mHeader.mNumFiles > countof(mFiles))
            {
                printf("More files in LVL than hard coded array size\n");
                return false;
            }

            for (Uint32 i = 0; i < mHeader.mNumFiles; i++)
            {
                //printf("Reading record [%d] of [%d]\n", i+1, mHeader.mNumFiles);
                if (!mStream.ReadBytes(reinterpret_cast<Uint8 *>(&mFiles[i]), sizeof(FileRecord)))
                {
                    printf("Failed to read file record\n");
                    return false;
                }
                //Print(mFiles[i]);
            }

            return true;
        }
        else
        {
            printf("Lvl not found\n");
            return false;
        }
    }

    bool LoadFile(const char *name, unsigned char *buffer, unsigned int bufferLen, unsigned int &fileSize)
    {
        for (unsigned int i = 0; i < mHeader.mNumFiles; i++)
        {
            if (strncmp(name, mFiles[i].mFileNameBytes, KMaxLvlArchiveFileNameLength) == 0)
            {
                printf("Loading file [%s]\n", name);
                fileSize = mFiles[i].mFileSize;
                return LoadFile(mFiles[i], buffer, bufferLen);
            }
        }
        printf("File [%s] not found\n", name);
        return false;
    }

    bool LoadFile(const FileRecord &rec, unsigned char *buffer, unsigned int bufferLen)
    {
        if (rec.mFileSize > bufferLen)
        {
            printf("File is too big for buffer [%d] vs [%d]\n", rec.mFileSize, bufferLen);
            return false;
        }

        if (!cdRom.ReadSectors(mStream.StartSector() + rec.mStartSector, rec.mNumSectors, buffer))
        {
            printf("Reading data failure\n");
            return false;
        }

        printf("File read completed\n");
        return true;
    }

    static void Print(const LvlHeader &h)
    {
        printf("mFirstFileOffset=%d\n", h.mFirstFileOffset);
        printf("mNull1=%d\n", h.mNull1);
        printf("mMagic=%d\n", h.mMagic);
        printf("mNull2=%d\n", h.mNull2);
        printf("mNumFiles=%d\n", h.mNumFiles);
        printf("mFirstFileOffsetSectorNum=%d\n", h.mFirstFileOffsetSectorNum);
        printf("mUnknown2=%d\n", h.mUnknown2);
        printf("mUnknown3=%d\n", h.mUnknown3);
    }

    static void Print(const FileRecord &f)
    {
        printf("mStartSector=%d\n", f.mStartSector);
        printf("mNumSectors=%d\n", f.mNumSectors);
        printf("mFileSize=%d\n", f.mFileSize);
        char tmpBuffer[KMaxLvlArchiveFileNameLength + 1] = {};
        strncpy(tmpBuffer, f.mFileNameBytes, KMaxLvlArchiveFileNameLength);
        printf("mFileNameBytes=%s\n", tmpBuffer);
    }

private:
    CdStream mStream;
    DslFILE mFileInfo;
};

enum eMainStates
{
    eInitScreen,
    eLvlSelection,
    eCdNotReady,
    eLvlLoading,
    eErrorLoadingLvl,
    eLoadedLvl,
    eErrorLoadingTracks,
    eSfxOrTrackSelection,
    eSfxSelection,
    eTrackSelection
};

static Lvl gLvl;
static eMainStates gMainState = eLvlSelection;

class TextMenu
{
public:
    TextMenu()
    {
    }

    void Init(const char **strings, int numStrings, const char *title = NULL)
    {
        mMenuTitle = title;

        mStrings = strings;
        mNumStrings = numStrings;

        mYSize = mNumStrings / 2;
        mLastRowOdd = mNumStrings % 2;
        if (mLastRowOdd)
        {
            mYSize++;
        }

        mSelectedX = 0;
        mSelectedY = 0;
    }

    void Update()
    {
        if (gInput.Up())
        {
            mSelectedY--;
            if (mSelectedY < 0)
            {
                if (mLastRowOdd && mSelectedX == kXSize - 1)
                {
                    mSelectedY = mYSize - 2;
                }
                else
                {
                    mSelectedY = mYSize - 1;
                }
            }
        }
        else if (gInput.Down())
        {
            mSelectedY++;
            if (mSelectedY > mYSize - 1 || (mLastRowOdd && mSelectedY > mYSize - 2 && mSelectedX == kXSize - 1))
            {
                mSelectedY = 0;
            }
        }
        else if (gInput.Left())
        {
            mSelectedX--;
            if (mSelectedX < 0)
            {
                if (mLastRowOdd && mSelectedY == mYSize - 1)
                {
                    mSelectedX = kXSize - 2;
                }
                else
                {
                    mSelectedX = kXSize - 1;
                }
            }
        }
        else if (gInput.Right())
        {
            mSelectedX++;
            if (mSelectedX > kXSize - 1 || (mLastRowOdd && mSelectedY == mYSize - 1))
            {
                mSelectedX = 0;
            }
        }
    }

    void Render()
    {
        if (mMenuTitle)
        {
            ::FntPrint("%s\n\n", mMenuTitle);
        }

        for (int i = 0; i < mNumStrings; i++)
        {
            if (i == mNumStrings - 1 && mLastRowOdd)
            {
                ::FntPrint("[%s] %s\n", SelectedString(i), mStrings[i]);
            }
            else
            {
                char buffer[40] = {};
                strcpy(buffer, mStrings[i]);

                int len = 0;
                do
                {
                    len = strlen(buffer);
                    if (len < 16)
                    {
                        buffer[len] = ' ';
                        len++;
                    }
                } while (len < 16);

                ::FntPrint("[%s] %s [%s] %s\n", SelectedString(i), buffer, SelectedString(i + 1), mStrings[i + 1]);
                i++;
            }
        }
    }

    int SelectedIndex() const
    {
        const int r = (mSelectedY * kXSize) + mSelectedX;
        return r;
    }

private:
    const char *SelectedString(int pos)
    {
        const int y = pos / kXSize;
        const int x = pos % kXSize;
        if (x == mSelectedX && y == mSelectedY)
        {
            return "X";
        }
        return " ";
    }

private:
    const char *mMenuTitle;
    const char **mStrings;
    int mNumStrings;
    const static int kXSize;
    int mYSize;
    int mSelectedY;
    int mSelectedX;
    bool mLastRowOdd;
};

const int TextMenu::kXSize = 2;

static TextMenu gLevelSelectMenu;

int DisplayLevelSelection()
{
    gLevelSelectMenu.Update();
    gLevelSelectMenu.Render();

    if (gInput.Cross())
    {
        return gLevelSelectMenu.SelectedIndex();
    }
    else if (gInput.Start())
    {
        cdRom.StopCdRotation();
    }
    return -1;
}

#define MVOL 127
#define SVOL 64

static void TickSeq()
{
    Sound_SsSeqCalledTbyT();
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
        Sound_SsInit();
        Sound_SsSetTableSize(mSeqTable, 4, 5);
        Sound_SsSetTickMode(SS_NOTICK);
        Sound_SsStart();
        Sound_SsSetMVol(MVOL, MVOL);
        VSyncCallback(TickSeq);
    }

    bool LoadVab(unsigned char *vhBuffer, unsigned char *vbBuffer)
    {
        CloseVab();

        mOpenVabId = Sound_SsVabTransfer(vhBuffer, vbBuffer, -1, SS_WAIT_COMPLETED);
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
        if (Sound_SsVabTransCompleted(SS_WAIT_COMPLETED) == 0)
        {
            printf("ERROR: Didn't finish copy to SPU ram\n");
            return false;
        }

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

        mOpenSeqId = Sound_SsSeqOpen(reinterpret_cast<unsigned long *>(seqBuffer), mOpenVabId);
        if (mOpenSeqId == -1)
        {
            printf("Open SEQ failed\n");
            return false;
        }

        printf("Playing seq [%d]\n", mOpenSeqId);
        Sound_SsSeqSetVol(mOpenSeqId, SVOL, SVOL);
        Sound_SsSeqPlay(mOpenSeqId, SSPLAY_PLAY, 1);

        return true;
    }

    void CloseSeq()
    {
        if (mOpenSeqId != -1)
        {
            printf("Closing seq [%d]\n", mOpenSeqId);
            Sound_SsSeqClose(mOpenSeqId);
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
            Sound_SsVabClose(mOpenVabId);
            mOpenVabId = -1;
        }
    }

    char mSeqTable[SS_SEQ_TABSIZ * 4 * 5];
    short mOpenVabId;
    short mOpenSeqId;
};

static Sound gSound;

class SeqChunk
{
public:
    SeqChunk()
    {
        for (int i = 0; i < 200; i++)
        {
            sprintf(mNumbers[i], "SEQ %d", i + 1);
            mNumbersPtrs[i] = &mNumbers[i][0];
        }
    }

    bool Parse(unsigned char *seqChunk)
    {
        mData = seqChunk;
        bool ok = true;
        mNumSeqs = 0;
        unsigned char *ptr = mData;
        for (;;)
        {
            Lvl::ChunkHeader *chunk = reinterpret_cast<Lvl::ChunkHeader *>(ptr);
            if (chunk->iType == Lvl::MakeType('E', 'n', 'd', '!'))
            {
                printf("At end\n");
                break;
            }
            else if (chunk->iType == Lvl::MakeType('S', 'e', 'q', ' '))
            {
                printf("Found SEQ\n");
                mNumSeqs++;
            }
            else
            {
                printf("Unknown chunk type\n");
            }
            ptr += chunk->iSize;
        }

        mMenu.Init(mNumbersPtrs, mNumSeqs);

        return ok;
    }

    unsigned char *SeqPtr(unsigned int index)
    {
        if (index > mNumSeqs)
        {
            return NULL;
        }

        unsigned char *ptr = gSeqBuffer;
        unsigned int count = 0;
        for (;;)
        {
            Lvl::ChunkHeader *chunk = reinterpret_cast<Lvl::ChunkHeader *>(ptr);
            if (chunk->iType == Lvl::MakeType('E', 'n', 'd', '!'))
            {
                return NULL;
            }
            else if (chunk->iType == Lvl::MakeType('S', 'e', 'q', ' '))
            {
                if (count == index)
                {
                    printf("Found SEQ at index %d\n", index);
                    return reinterpret_cast<unsigned char *>(chunk);
                }
                count++;
            }
            ptr += chunk->iSize;
        }
        return NULL;
    }

    int DisplayMenu()
    {
        mMenu.Update();
        mMenu.Render();
        return mMenu.SelectedIndex();
    }

private:
    unsigned char *mData;
    unsigned int mNumSeqs;
    char mNumbers[200][10];
    const char *mNumbersPtrs[200];
    TextMenu mMenu;
};
static SeqChunk gSeqChunk;

#define MAX_VH_FILES 20

class VhMenu
{
public:
    VhMenu()
    {
        mNumVhs = 0;
        for (int i = 0; i < MAX_VH_FILES; i++)
        {
            mStrsPtrs[i] = &mStrs[i][0];
        }
    }

    void Init(Lvl &lvl)
    {
        mNumVhs = 0;
        for (int i = 0; i < lvl.NumFiles(); i++)
        {
            const Lvl::FileRecord &rec = lvl.FileAt(i);
            char tmp[13] = {};
            strncpy(tmp, rec.mFileNameBytes, 12);
            if (strstr(tmp, ".VH"))
            {
                printf("name: %s\n", tmp);
                memcpy(mStrs[mNumVhs], tmp, 13);
                mNumVhs++;
            }
        }

        mMenu.Init((const char **)mStrsPtrs, mNumVhs, "Press X to load soundbank\nPress O to go back\n\n");
    }

    int DisplayMenu()
    {
        mMenu.Update();
        mMenu.Render();
        return mMenu.SelectedIndex();
    }

    char *SelectedName()
    {
        if (mMenu.SelectedIndex() == -1)
        {
            return NULL;
        }
        return mStrsPtrs[mMenu.SelectedIndex()];
    }

private:
    unsigned int mNumVhs;
    char mStrs[MAX_VH_FILES][13];
    char *mStrsPtrs[MAX_VH_FILES];
    TextMenu mMenu;
};
static VhMenu gVhMenu;

static void HandleLvlLoaded()
{
    const int selected = gVhMenu.DisplayMenu();
    if (gInput.Cross() && selected != -1)
    {
        char *vhName = gVhMenu.SelectedName();

        unsigned int vhSize = 0;
        if (!gLvl.LoadFile(vhName, gVhBuffer, sizeof(gVhBuffer), vhSize))
        {
            gMainState = eErrorLoadingTracks;
            return;
        }

        unsigned int vbSize = 0;
        vhName[strlen(vhName) - 1] = 'B';
        if (!gLvl.LoadFile(vhName, gVbBuffer, sizeof(gVbBuffer), vbSize))
        {
            vhName[strlen(vhName) - 1] = 'H';
            gMainState = eErrorLoadingTracks;
            return;
        }
        vhName[strlen(vhName) - 1] = 'H';

        if (!gSound.LoadVab(gVhBuffer, gVbBuffer))
        {
            printf("Failed to load VAB data\n");
            gMainState = eErrorLoadingTracks;
            return;
        }

        unsigned int seqsSize = 0;

        for (int i = 0; i < gLvl.NumFiles(); i++)
        {
            const Lvl::FileRecord &rec = gLvl.FileAt(i);
            char tmp[13] = {};
            strncpy(tmp, rec.mFileNameBytes, 12);
            if (strstr(tmp, ".BSQ"))
            {
                if (!gLvl.LoadFile(tmp, gSeqBuffer, sizeof(gSeqBuffer), seqsSize))
                {
                    gMainState = eErrorLoadingTracks;
                    return;
                }

                if (!gSeqChunk.Parse(gSeqBuffer))
                {
                    gMainState = eErrorLoadingTracks;
                    return;
                }

                gMainState = eSfxOrTrackSelection;
                return;
            }
        }

        gMainState = eErrorLoadingTracks;
    }
    else if (gInput.Circle())
    {
        gMainState = eLvlSelection;
    }
}

static Graphics graphics;

#define MAX_STARS 150
#define star_w 8000 //determines the width of the area that stars 'appear' in

// Seems "brainwlkr" originally came up with this effect
class StarField
{
public:
    StarField()
    {
        speed = 2;
    }

    void Init()
    {
        //inits starfield
        srand(1); //init the randomiser (for creating random star allocations)

        for (int i = 0; i < MAX_STARS; i++)
        {                                                                  //this bit sets up each 'star' (MAX_STARS number of them)
            myStars[i].x = star_w - (rand() * (2 * star_w)) / (32767 + 1); //set stars x-coordinate
            myStars[i].y = star_w - (rand() * (2 * star_w)) / (32767 + 1); //set stars y-coordinate
            myStars[i].z = (rand() * (80)) / (32767 + 1) + 1;              //set stars z-coordinate 1-81 (cant be zero since +1)
            myStars[i].line.r = myStars[i].line.g = myStars[i].line.b = 255;
        } //for
    }

    void Render()
    {
        //this is what makes the stars 'move', it also adds the stars to the OT
        GsOT *otPtr = graphics.OtPtr();

        for (int i = 0; i < MAX_STARS; i++)
        {

            if (myStars[i].z < 2)
            {
                myStars[i].z = 100; //if the star has reached you then put it back to come toward you again
            }

            //divide by c so that as star gets closer it seems to move faster past you
            const int x = (myStars[i].x) / (myStars[i].z);                   //sets x value of stars
            const int y = (myStars[i].y) / (myStars[i].z);                   //sets y value of stars
            myStars[i].z -= speed;                                           //moves stars toward you
            myStars[i].line.x1 = myStars[i].line.x0 = x + SCREEN_WIDTH / 2;  //centres starfields x value onscreen
            myStars[i].line.y1 = myStars[i].line.y0 = y + SCREEN_HEIGHT / 2; //centres starfields y value onscreen

            GsSortLine(&myStars[i].line, otPtr, 0); //sorts stars into OT)
        }
    }

private:
    typedef struct
    {
        int x; //star structure - has and x, y and z coordinate
        int y;
        int z;
        GsLINE line; //GsLINE strcuture that make a star pixel
    } Star;

    Star myStars[MAX_STARS];
    int speed;
};

static StarField gStarField;

class TimImage
{
public:
    explicit TimImage(const unsigned char *timBuffer)
        : mTimBuffer(timBuffer)
    {
    }

    void UploadToVram()
    {
        // +4 to skip the DWORD header magic flag
        ::GsGetTimInfo((u_long *)(mTimBuffer + 4), &mGsImage);
        LoadImage(mGsImage.px, mGsImage.py, mGsImage.pw, mGsImage.ph, mGsImage.pixel);
        // Only 4 and 8 bit TIMs have CLUTs

        LoadImage(mGsImage.cx, mGsImage.cy, mGsImage.cw, mGsImage.ch, mGsImage.clut);
    }

    int ClutX() const { return mGsImage.cx; }
    int ClutY() const { return mGsImage.cy; }
    int Width() const { return mGsImage.pw; }
    int Height() const { return mGsImage.ph; }
    int PMode() const { return mGsImage.pmode; }
    int VramX() const { return mGsImage.px; }
    int VramY() const { return mGsImage.py; }

private:
    void LoadImage(u_short x, u_short y, u_short w, u_short h, u_long *data)
    {
        RECT rect = {x, y, w, h};
        printf("Upload RECT: [%d,%d,%d,%d]\n", w, y, w, h);
        ::LoadImage(&rect, data);
    }

    const unsigned char *mTimBuffer;
    GsIMAGE mGsImage;
};

static void InitSprite(GsSPRITE &sprite, const TimImage &tim)
{
    memset(&sprite, 0, sizeof(GsSPRITE));

    // 16 bit CLUT, all options off (0x1 = 8-bit, 0x2 = 16-bit)
    sprite.attribute = 0x1000000;

    // 100% scale
    sprite.scalex = ONE;
    sprite.scaley = ONE;

    // Colour
    sprite.r = 128;
    sprite.g = 128;
    sprite.b = 128;

    // CLUT
    sprite.cx = tim.ClutX();
    sprite.cy = tim.ClutY();

    // TPage
    sprite.tpage = GetTPage(1, 2, tim.VramX(), tim.VramY());

    // Size
    sprite.w = 256; // tim.Width() * 2;
    sprite.h = tim.Height();
}

struct SfxDefinition
{
    char field_0_block_idx;
    char field_1_program;
    char field_2_note;
    char field_3_default_volume;
    short field_4_pitch_min;
    short field_6_pitch_max;
};

const SfxDefinition sSligGameSpeakEntries_560868[16] = {
    {0u, 25u, 60u, 127u, 0, 0},
    {0u, 25u, 62u, 127u, 0, 0},
    {0u, 25u, 61u, 115u, 0, 0},
    {0u, 25u, 38u, 120u, 0, 0},
    {0u, 25u, 63u, 127u, 0, 0},
    {0u, 25u, 66u, 127u, 0, 0},
    {0u, 25u, 37u, 127u, 0, 0},
    {0u, 25u, 67u, 127u, 0, 0},
    {0u, 25u, 57u, 127u, 0, 0},
    {0u, 25u, 58u, 127u, 0, 0},
    {0u, 25u, 59u, 127u, 0, 0},
    {0u, 25u, 39u, 127u, 0, 0},
    {0u, 25u, 64u, 127u, 0, 0},
    {0u, 25u, 65u, 127u, 0, 0},
    {0u, 25u, 68u, 127u, 0, 0},
    {0u, 0u, 36u, 70u, 0, 0},
};

const static char *kSligSfx[] =
    {
        "Hi",
        "HereBoy",
        "Gethim",
        "Laugh",
        "Stay",
        "BullShit",
        "Lookout",
        "Bullshit2",
        "Freeze",
        "What",
        "Help",
        "Blurgh",
        "GotYa",
        "Ouch1",
        "Ouch2",
        "LoadingSfx"};

static void SND_Stop_Channels_Mask_4CA810(int bitMask)
{
    for (int i = 0; i < 24; i++) // TODO: Constant
    {
        // Does the index match a bit in the bitmask?
        if ((1 << i) & bitMask)
        {
            // Turn it off
            Sound_SsUtKeyOffV(i);
        }
    }
}
class SfxPlayer
{
public:
    SfxPlayer()
    {
        mMenu.Init(kSligSfx, 16);
        mOnVoices = 0;
    }

    int DisplayMenu()
    {
        mMenu.Update();
        mMenu.Render();
        return mMenu.SelectedIndex();
    }

    void Play(short vabId, int idx)
    {
        Stop();

        char volLeft = 127;
        char volRight = 127;

        const SfxDefinition &def = sSligGameSpeakEntries_560868[idx];
        mOnVoices = Sound_SsVoKeyOn(def.field_1_program | vabId, def.field_2_note << 8, volLeft, volRight);
    }

    void Stop()
    {
        SND_Stop_Channels_Mask_4CA810(mOnVoices);
        mOnVoices = 0;
    }

private:
    int mOnVoices;
    TextMenu mMenu;
};
static SfxPlayer gSfxPlayer;

int main()
{
    //STATIC_ASSERT(1,sizeof(SeqStruct)==0xB0);

    ::ResetCallback();

    printf("main enter\n");

    graphics.Init();
    gStarField.Init();

    cdRom.Init(true);

    gInput.Init();

    gSound.Init();

    gLevelSelectMenu.Init(kLvlFiles, kNumLvls, "Select a level to load\nPress start to stop/swap CD");

    TimImage copyrightLeft(P1);
    copyrightLeft.UploadToVram();

    TimImage copyrightRight(P2);
    copyrightRight.UploadToVram();

    GsSPRITE copyRight[2];
    InitSprite(copyRight[0], copyrightLeft);
    InitSprite(copyRight[1], copyrightRight);
    copyRight[0].y = -3;
    copyRight[0].x = 0 + 25;
    copyRight[1].y = -3;
    copyRight[1].x = 128 + 25;

    //TestFindAllFiles(cdRom);
    int lvlIndex = -1;
    int timeToShowloading = 0;
    int timeToShowInit = 0;
    for (;;)
    {
        gInput.Update();
        graphics.PreRender();

        if (gMainState != eInitScreen)
        {
            gStarField.Render();
        }

        ::FntPrint("Size = %d\n", sizeof(SeqStruct));

        switch (gMainState)
        {
        case eInitScreen:
            GsSortSprite(&copyRight[0], graphics.OtPtr(), 0);
            GsSortSprite(&copyRight[1], graphics.OtPtr(), 0);
            timeToShowInit++;
            if (timeToShowInit > 300)
            {
                gMainState = eLvlSelection;
            }
            break;

        case eLvlSelection:
        {
            lvlIndex = DisplayLevelSelection();
            if (lvlIndex != -1)
            {
                // TODO: Check CD properly, if CD is switched we never
                // seem to get a new dir listing
                if (!cdRom.Ready())
                {
                    gMainState = eCdNotReady;
                }
                else
                {
                    timeToShowloading = 0;
                    gMainState = eLvlLoading;
                }
            }
        }
        break;

        case eCdNotReady:
            ::FntPrint("CD ROM not ready\n\nPress X go to back");
            if (gInput.Cross())
            {
                gMainState = eLvlSelection;
            }
            break;

        case eLvlLoading:
        {
            timeToShowloading++;
            ::FntPrint("Loading [%s]\n", kLvlFiles[lvlIndex]);
            if (timeToShowloading >= 30)
            {
                printf("Loading %s\n", kLvlFiles[lvlIndex]);
                if (gLvl.Load(kLvlFiles[lvlIndex]))
                {
                    gMainState = eLoadedLvl;
                    gVhMenu.Init(gLvl);
                }
                else
                {
                    gMainState = eErrorLoadingLvl;
                }
                lvlIndex = -1;
            }
        }
        break;

        case eErrorLoadingLvl:
            ::FntPrint("Error loading LVL\n\nPress X go to back");
            if (gInput.Cross())
            {
                gMainState = eLvlSelection;
            }
            break;

        case eLoadedLvl:
            HandleLvlLoaded();
            break;

        case eErrorLoadingTracks:
            ::FntPrint("Error loading tracks\n\nPress X go to back");
            if (gInput.Cross())
            {
                gMainState = eLvlSelection;
            }
            break;

        case eSfxOrTrackSelection:
            ::FntPrint("SFX or track selection");
            ::FntPrint("\n\nPress Triangle for SFX, X for tracks");
            ::FntPrint("\nOr O to go back\n\n");
            if (gInput.Cross())
            {
                gMainState = eTrackSelection;
            }
            else if (gInput.Triangle())
            {
                gMainState = eSfxSelection;
            }
            else if (gInput.Circle())
            {
                gSound.CloseSeq();
                gMainState = eLvlSelection;
            }
            break;

        case eSfxSelection:
        {
            ::FntPrint("SFX selection");
            ::FntPrint("\nPress X to play");
            ::FntPrint("\n start to stop and O to go back\n\n");

            const int sfxNum = gSfxPlayer.DisplayMenu();
            if (gInput.Cross() && sfxNum != -1)
            {
                gSfxPlayer.Play(gSound.VabId(), sfxNum);
            }
            else if (gInput.Start())
            {
                gSfxPlayer.Stop();
            }
            else if (gInput.Circle())
            {
                gSound.CloseSeq();
                gMainState = eSfxOrTrackSelection;
            }
        }
        break;

        case eTrackSelection:
            ::FntPrint("Track selection");
            ::FntPrint("\nPress X to play and O to go back\n\n");

            const int seqNum = gSeqChunk.DisplayMenu();
            if (gInput.Cross() && seqNum != -1)
            {
                unsigned char *seqPtr = gSeqChunk.SeqPtr(seqNum);
                if (seqPtr)
                {
                    if (!gSound.PlaySEQ(seqPtr + sizeof(Lvl::ChunkHeader)))
                    {
                        printf("SEQ play failed for idx [%d]\n", seqNum);
                    }
                }
            }
            else if (gInput.Circle())
            {
                gSound.CloseSeq();
                gMainState = eSfxOrTrackSelection;
            }
            break;
        }

        graphics.Render();
    }

    return 0;
}
