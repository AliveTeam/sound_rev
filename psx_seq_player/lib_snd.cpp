#include "lib_snd.hpp"
#include "lib_spu.hpp"

#include <stdio.h> // printf
#include <sys/types.h>
#include <libetc.h> // ResetCallback

extern "C"
{

    extern short note2pitch2;
    extern short _svm_damper;
    extern ProgAtr** _svm_pg;
    extern unsigned char _svm_vab_used[16];
    extern short _svm_vab_count;
    extern unsigned long _svm_vab_start[16];

    // TODO

    short SsUtSetReverbType(short);
    void SsUtSetReverbFeedback(short);

    extern short SsVabOpenHead(unsigned char*, short);
    extern short SsVabTransBody(unsigned char*, short);
    extern short SsVabTransCompleted(short);

    extern void SsUtSetReverbDelay(short);
    extern short SsUtSetVagAtr(short, short, short, VagAtr*);
    extern short SsUtGetVagAtr(short, short, short, VagAtr*);
    extern void SsUtSetReverbDepth(short, short);
    extern void SsUtReverbOff(void);

    void _SsVmSetVol(short seq_sep_no, short vabId, short program, short voll, short volr);
    
    int _SsVmVSetUp(short vabId, short program);

    void _SsVmInit(int); // many unknown globals, inits voice structures
    short _SsInitSoundSeq(int seqId, int vabId, unsigned long *pSeqData);
    void _SsVmKeyOn(int seq_sep_no, short vabId, short unknown37, short note, short voll, short unknown27);
    void _SsVmKeyOff(int seq_sep_no, short vabId, short unknown37, short note);
    void _SsVmPitchBend(short seq_sep_no, short vabId, unsigned char program, unsigned char pitch); // unknown func + globals
    extern void _SsContDataEntry(short, short, unsigned char); // med

    void _SsVmSetSeqVol(short seq_sep_num, short voll, short volr); // high
    void _SsVmSeqKeyOff(short seq_idx); // unknown var/struct (voice struct?)
    void _SsVmKeyOffNow(void); // many vars
    long _SsVmSeKeyOn(unsigned char vab, unsigned char program, unsigned char note, unsigned char pitch, unsigned short volL, unsigned short volR); // low
    void _SsVmFlush(void); // many vars
    void _SsSndCrescendo(short seqNum, short sepNum);
    void _SsSndTempo(short seqNum, short sepNum);

    void _SsSeqGetEof(short seq_access_num, short sep_num); // wip
    void _SsGetSeqData(short seq_idx, short sep_idx); // wip
    void _SsSeqPlay(short seq_access_num, short seq_num); // wip

    void SsVabClose(short vabId)
    {
        if (vabId < 16)
        {
            if (_svm_vab_used[vabId] < 3 && _svm_vab_used[vabId] != 0)
            {
                _svm_vab_used[vabId] = 0;
                
                SpuFree(_svm_vab_start[vabId]);

                _svm_vab_count--;

                if (_spu_getInTransfer())
                {
                    _spu_setInTransfer(0);
                }
            }
        }
    }

    void _SsVmSetProgVol(short vabId, short program, unsigned char vol)
    {
        if (!_SsVmVSetUp(vabId, program))
        {
            _svm_pg[program]->mvol = vol;
        }
    }
    void SsUtReverbOn(void)
    {
        SpuSetReverb(1);
    }
    
    void SsUtReverbOff(void)
    {
        SpuSetReverb(0);
    }

    void _SsVmDamperOn(void)
    {
        _svm_damper = 2;
    }

    void _SsVmDamperOff(void)
    {
        _svm_damper = 0;
    }

    void _SsSndReplay(short seqNum, short sepNum)
    {
        SeqStruct* pStruc = &_ss_score[seqNum][sepNum]; // note: 14bit access
        pStruc->field_14_play_mode = 1;
        pStruc->field_98_flags &= ~8u;
    }

    void _SsSetNrpnVabAttr0(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char priority)
    {
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        vagAtr.prior = priority;
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr1(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char mode)
    {
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        vagAtr.mode = mode;
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        if (mode)
        {
            if (mode == 4)
            {
                SsUtReverbOn();
            }
        }
        else
        {
            SsUtReverbOff();
        }
    }

    void _SsSetNrpnVabAttr2(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char minKey)
    {
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        vagAtr.min = minKey;
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr3(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char keyMax)
    {
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        vagAtr.max = keyMax;
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }


    void _SsUtResolveADSR(unsigned short adsr1, unsigned short adsr2, unsigned short *pResolvedADSR)
    {
        pResolvedADSR[5] = adsr1 & 0x8000;
        pResolvedADSR[6] = adsr2 & 0x8000;
        pResolvedADSR[8] = adsr2 & 0x4000;
        pResolvedADSR[7] = adsr2 & 0x20;
        //pResolvedADSR[0] = HIBYTE(adsr1) & 0x7F;
        pResolvedADSR[0] = (adsr1 >> 8) & 0x7F;

        pResolvedADSR[1] = (unsigned char)adsr1 >> 4;
        pResolvedADSR[2] = adsr1 & 0xF;
        pResolvedADSR[3] = (adsr2 >> 6) & 0x7F;
        pResolvedADSR[4] = adsr2 & 0x1F;
    }

    void _SsUtBuildADSR(unsigned short *pResolvedADSR, unsigned short *pAdsr1, unsigned short *pAdsr2)
    {
        unsigned int v4 = (pResolvedADSR[5] != 0 ? 0xFFFF8000 : 0);
        unsigned int v5 = (pResolvedADSR[6] != 0 ? 0xFFFF8000 : 0);

        if (pResolvedADSR[8])
        {
            v5 |= 0x4000;
        }

        if (pResolvedADSR[7])
        {
            v5 |= 0x20u;
        }

        *pAdsr1 = v4 | (pResolvedADSR[0] << 8) & 0x7F00 | (16 * pResolvedADSR[1]) & 0xF0 | pResolvedADSR[2] & 0xF;
        *pAdsr2 = v5 | (pResolvedADSR[3] << 6) & 0x1FC0 | pResolvedADSR[4] & 0x1F;
    }

    void _SsSetNrpnVabAttr4(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char asdr)
    {
        unsigned short resolvedADSR[12];
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        _SsUtResolveADSR(vagAtr.adsr1, vagAtr.adsr2, resolvedADSR);
        resolvedADSR[5] = 0;
        resolvedADSR[0] = asdr;
        _SsUtBuildADSR(resolvedADSR, &vagAtr.adsr1, &vagAtr.adsr2);
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr5(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char adsr)
    {
        unsigned short resolvedADSR[12];
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        _SsUtResolveADSR(vagAtr.adsr1, vagAtr.adsr2, resolvedADSR);
        resolvedADSR[5] = 1;
        resolvedADSR[0] = adsr;
        _SsUtBuildADSR(resolvedADSR, &vagAtr.adsr1, &vagAtr.adsr2);
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr6(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char adsr)
    {
        unsigned short resolvedADSR[12];
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        _SsUtResolveADSR(vagAtr.adsr1, vagAtr.adsr2, resolvedADSR);
        resolvedADSR[1] = adsr;
        _SsUtBuildADSR(resolvedADSR, &vagAtr.adsr1, &vagAtr.adsr2);
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr7(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char adsr)
    {
        unsigned short resolvedADSR[12];
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        _SsUtResolveADSR(vagAtr.adsr1, vagAtr.adsr2, resolvedADSR);
        resolvedADSR[2] = adsr;
        _SsUtBuildADSR(resolvedADSR, &vagAtr.adsr1, &vagAtr.adsr2);
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr8(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char adsr)
    {
        unsigned short resolvedADSR[12];
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        _SsUtResolveADSR(vagAtr.adsr1, vagAtr.adsr2, resolvedADSR);
        resolvedADSR[6] = 0;
        resolvedADSR[3] = adsr;
        _SsUtBuildADSR(resolvedADSR, &vagAtr.adsr1, &vagAtr.adsr2);
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr9(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char adsr)
    {
        unsigned short resolvedADSR[12];
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        _SsUtResolveADSR(vagAtr.adsr1, vagAtr.adsr2, resolvedADSR);
        resolvedADSR[6] = 1;
        resolvedADSR[3] = adsr;
        _SsUtBuildADSR(resolvedADSR, &vagAtr.adsr1, &vagAtr.adsr2);
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr10(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char asdr)
    {
        unsigned short resolvedADSR[12];
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        _SsUtResolveADSR(vagAtr.adsr1, vagAtr.adsr2, resolvedADSR);
        resolvedADSR[7] = 0;
        resolvedADSR[4] = asdr;
        _SsUtBuildADSR(resolvedADSR, &vagAtr.adsr1, &vagAtr.adsr2);
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr11(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char adsr)
    {
        unsigned short resolvedADSR[12];
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        resolvedADSR[7] = 1;
        resolvedADSR[4] = adsr;
        _SsUtBuildADSR(resolvedADSR, &vagAtr.adsr1, &vagAtr.adsr2);
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr12(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char attr)
    {
        unsigned short asdrBuffer[12];
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        if ((attr - 1) >= 63)
        {
            if ((attr - 64) < 64)
            {
                asdrBuffer[8] = 1;
            }
        }
        else
        {
            asdrBuffer[8] = 0;
        }
        _SsUtBuildADSR(asdrBuffer, &vagAtr.adsr1, &vagAtr.adsr2);
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr13(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char vibT)
    {
        unsigned short asdrBuffer[12];
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        vagAtr.vibT = vibT;
        _SsUtBuildADSR(asdrBuffer, &vagAtr.adsr1, &vagAtr.adsr2); // bugged ??
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr14(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char porW)
    {
        unsigned short asdrBuffer[12];
        SsUtGetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
        vagAtr.porW = porW;
        _SsUtBuildADSR(asdrBuffer, &vagAtr.adsr1, &vagAtr.adsr2); // bugged ??
        SsUtSetVagAtr(vab_id, prog_no, tone_no, &vagAtr);
    }

    void _SsSetNrpnVabAttr15(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char attr)
    {
        SsUtSetReverbType(attr);
    }

    void _SsSetNrpnVabAttr16(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char attr)
    {
        SsUtSetReverbDepth(attr, attr);
    }

    void _SsSetNrpnVabAttr17(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char attr)
    {
        SsUtSetReverbFeedback(attr);
    }

    void _SsSetNrpnVabAttr18(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char attr)
    {
        SsUtSetReverbDelay(attr);
    }

    void _SsSetNrpnVabAttr19(short vab_id, short prog_no, short tone_no, VagAtr vagAtr, short fn_idx, unsigned char attr)
    {
        SsUtSetReverbDelay(attr);
    }

    void _SsInit(void); // TODO: Impl can't link due to redef of global vars

/*
    // TODO: Can't link as obj has some globals in there
    void _SsInit(void)
    {
        // Init regs
        const static short private_8001D0A4[16] = { 0x3FFF, 0x3FFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        volatile short* pReg = (volatile short*)0x1F801D80;
        for (int i = 0; i < 16; i++)
        {
            pReg[i] = private_8001D0A4[i];
        }

        _SsVmInit(24);

        // Init mark data
        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                _SsMarkCallback[i].field_0_entries[j] = 0;
            }
        }

        VBLANK_MINUS = 60;
        _snd_openflag = 0;
        _snd_ev_flag = 0;
    }
*/

    // TODO: SpuClearReverbWorkArea

    void SsInit(void)
    {
        ResetCallback();
        SpuInit();
        SpuClearReverbWorkArea(SS_REV_TYPE_ECHO); // 7
        _SsInit();
    }

    // TODO
    void SsStart(void)
    {
        // Works as a stub, not sure why - emulator issue?
    }

    // TODO
    void SsSetTickMode(long tick_mode)
    {
        // Seems fine to do nothing, probably because we call
        // SsSeqCalledTbyT manually.
    }

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

    void SsSetMVol(short voll, short volr)
    {
        SpuCommonAttr attr;
        attr.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR; // 3
        attr.mvol.left = 129 * voll;
        attr.mvol.right = 129 * volr;
        SpuSetCommonAttr(&attr);
    }

    short SsVabTransfer(unsigned char *vh_addr, unsigned char *vb_addr, short vabid, short i_flag)
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

    // TODO: Not tested and some funny stuff happening in here - probably totally bugged
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

    void _SsSetPitchBend(short seq_no, short sep_no)
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

    void _SsContExpression(short seq_no, short sep_no, unsigned char progVol)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];
        _SsVmSetProgVol(pStru->field_26_vab_id, pStru->field_37_programs[pStru->field_17_channel_idx], progVol);
        _SsVmSetVol(
            seq_no | (sep_no << 8),
            pStru->field_26_vab_id,
            pStru->field_37_programs[pStru->field_17_channel_idx],
            pStru->field_60_vol[pStru->field_17_channel_idx],
            pStru->field_27_panpot[pStru->field_17_channel_idx]);
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContExternal(short seq_no, short sep_no, unsigned char reverbDepth)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];
        SsUtSetReverbDepth(reverbDepth, reverbDepth);
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContDamper(short seq_no, short sep_no, unsigned char damper)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];
        if (damper >= 64)
        {
            _SsVmDamperOn();
        }
        else
        {
            _SsVmDamperOff();
        }
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContNrpn1(short seq_no, short sep_no, unsigned char func_idx)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];

        if (pStru->field_1B == 40)
        {
            SndSsMarkCallbackProc pFunc = _SsMarkCallback[seq_no].field_0_entries[sep_no];
            if (pFunc)
            {
                pFunc(seq_no, sep_no, func_idx);
            }
        }

        if (pStru->field_1B != 30 && pStru->field_1B != 20) // TODO: goto removed
        {
            if (pStru->field_1B != 40)
            {
                pStru->field_1A_fn_idx = func_idx;
                pStru->field_1C = 0;
                pStru->field_1F++;
            }
        }

        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContNrpn2(short seq_no, short sep_no, unsigned char a3)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];

        if (a3 == 20)
        {
            pStru->field_1B = a3;
            pStru->field_1C = 1;
            pStru->field_8 = pStru->field_0_seq_ptr;
            pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
            return;
        }

        if (a3 != 30)
        {
            pStru->field_1B = a3;
            pStru->field_1F++;
            pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
            return;
        }

        pStru->field_1B = a3;

        if (!pStru->field_1D)
        {
            pStru->field_15 = 0;
            pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
            return;
        }

        if (pStru->field_1D >= 127)
        {
            _SsReadDeltaValue(seq_no, sep_no);
            pStru->field_90_delta_value = 0;
            pStru->field_0_seq_ptr = pStru->field_8;
        }
        else
        {
            pStru->field_1D--;
            pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
            if (pStru->field_1D)
            {
                pStru->field_0_seq_ptr = pStru->field_8;
            }
            else
            {
                pStru->field_15 = 0;
            }
        }
    }

    void _SsContRpn1(short seq_no, short sep_no, unsigned char a3)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];
        pStru->field_18 = a3;
        pStru->field_1E++;
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContRpn2(short seq_no, short sep_no, unsigned char a3)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];
        pStru->field_19 = a3;
        pStru->field_1E++;
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContResetAll(short seq_no, short sep_no)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];

        SsUtReverbOff();
        _SsVmDamperOff();

        pStru->field_37_programs[pStru->field_17_channel_idx] = pStru->field_17_channel_idx;
        pStru->field_18 = 0;
        pStru->field_19 = 0;
        pStru->field_60_vol[pStru->field_17_channel_idx] = 127;
        pStru->field_27_panpot[pStru->field_17_channel_idx] = 64;
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }


    // TODO: Can't link till _SsVmSeqKeyOff and _SsVmSetSeqVol impl
    /*
    void _SsVmGetSeqVol(short seq_sep_no, short *pVolL, short *pVolR)
    {
        SeqStruct* pStru = &_ss_score[seq_sep_no & 0xFF][(seq_sep_no & 0xFF00) >> 8];
        _svm_cur = seq_sep_no;
        *pVolL = pStru->field_58_voll;
        *pVolR = pStru->field_5A_volr;
    }
    */

    void _SsSetControlChange(short seq_no, short sep_no, unsigned char control)
    {
        SeqStruct* pStru = &_ss_score[seq_no][sep_no];
        const unsigned char control_value = *pStru->field_0_seq_ptr++;

        switch (control)
        {
            case 0:
                pStru->field_26_vab_id = control_value; // or 0
                pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
                break;

            case 6:
                SsFCALL.control[2](seq_no, sep_no, control_value);
                break;

            case 7:
                SsFCALL.control[3](seq_no, sep_no, control_value);
                break;

            case 10:
                SsFCALL.control[4](seq_no, sep_no, control_value);
                break;

            case 11:
                SsFCALL.control[5](seq_no, sep_no, control_value);
                break;

            case 64:
                SsFCALL.control[6](seq_no, sep_no, control_value);
                break;

            case 91:
                SsFCALL.control[11](seq_no, sep_no, control_value);
                break;

            case 98:
                SsFCALL.control[7](seq_no, sep_no, control_value);
                break;

            case 99:
                SsFCALL.control[8](seq_no, sep_no, control_value);
                break;

            case 100:
                SsFCALL.control[9](seq_no, sep_no, control_value);
                break;

            case 101:
                SsFCALL.control[10](seq_no, sep_no, control_value);
                break;
            
            case 121:
                SsFCALL.control[12](seq_no, sep_no, control_value);
                break;

            default:
                pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
                break;
        }
    }

    short SsSeqOpen(unsigned long *pSeqData, short vab_id)
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

        SsFCALL.control[CC_NUMBER] = _SsSetControlChange;
        SsFCALL.control[CC_BANKCHANGE] = (void (*)(short int, short int, unsigned char))_SsContBankChange; // sony lol
        SsFCALL.control[CC_MAINVOL] = _SsContMainVol;
        SsFCALL.control[CC_PANPOT] = _SsContPanpot;
        SsFCALL.control[CC_EXPRESSION] = _SsContExpression;
        SsFCALL.control[CC_DAMPER] = _SsContDamper;
        SsFCALL.control[CC_NRPN1] = _SsContNrpn1;
        SsFCALL.control[CC_NRPN2] = _SsContNrpn2;
        SsFCALL.control[CC_RPN1] = _SsContRpn1;
        SsFCALL.control[CC_RPN2] = _SsContRpn2;
        SsFCALL.control[CC_EXTERNAL] = _SsContExternal;
        SsFCALL.control[CC_RESETALL] = (void (*)(short int, short int, unsigned char))_SsContResetAll;
        SsFCALL.control[CC_DATAENTRY] = _SsContDataEntry;

        SsFCALL.ccentry[DE_PRIORITY] = _SsSetNrpnVabAttr0;
        SsFCALL.ccentry[DE_MODE] = _SsSetNrpnVabAttr1;
        SsFCALL.ccentry[DE_LIMITL] = _SsSetNrpnVabAttr2;
        SsFCALL.ccentry[DE_LIMITH] =  _SsSetNrpnVabAttr3;
        SsFCALL.ccentry[DE_ADSR_AR_L] = _SsSetNrpnVabAttr4;
        SsFCALL.ccentry[DE_ADSR_AR_E] = _SsSetNrpnVabAttr5;
        SsFCALL.ccentry[DE_ADSR_DR] = _SsSetNrpnVabAttr6;
        SsFCALL.ccentry[DE_ADSR_SL] = _SsSetNrpnVabAttr7;
        SsFCALL.ccentry[DE_ADSR_SR_L] = _SsSetNrpnVabAttr8;
        SsFCALL.ccentry[DE_ADSR_SR_E] = _SsSetNrpnVabAttr9;
        SsFCALL.ccentry[DE_ADSR_RR_L] = _SsSetNrpnVabAttr10;
        SsFCALL.ccentry[DE_ADSR_RR_E] = _SsSetNrpnVabAttr11;
        SsFCALL.ccentry[DE_ADSR_SR] = _SsSetNrpnVabAttr12;
        SsFCALL.ccentry[DE_VIB_TIME] = _SsSetNrpnVabAttr13;
        SsFCALL.ccentry[DE_PORTA_DEPTH] = _SsSetNrpnVabAttr14;
        SsFCALL.ccentry[DE_REV_TYPE] = _SsSetNrpnVabAttr15;
        SsFCALL.ccentry[DE_REV_DEPTH] = _SsSetNrpnVabAttr16;
        SsFCALL.ccentry[DE_ECHO_FB] = _SsSetNrpnVabAttr17;
        SsFCALL.ccentry[DE_ECHO_DELAY] = _SsSetNrpnVabAttr18;
        SsFCALL.ccentry[DE_DELAY] = _SsSetNrpnVabAttr19;

        if (seqInit != -1)
        {
            // BUG: doesn't unset _snd_openflag
            return openSeqId;
        }

        return -1;
    }

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

    long SsVoKeyOn(long vab_pro, long pitch, unsigned short volL, unsigned short volR)
    {
        return _SsVmSeKeyOn(
            (vab_pro >> 8),
            vab_pro & 0xFF,
            (unsigned short)(pitch >> 8),
            pitch & 0xFF,
            volL,
            volR);
    }

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
