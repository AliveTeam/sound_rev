#include "lib_snd.hpp"
#include "lib_spu.hpp"

#include <stdio.h> // printf
#include <sys/types.h>
//#include <libetc.h> // ResetCallback
#include "psx_stubs.hpp"


#define AddBytes(T, ptr, bytes) reinterpret_cast<T *>(reinterpret_cast<unsigned char *>(ptr) + bytes)

/*
template <class T, class Y>
static inline T *AddBytes(Y ptr, int bytes)
{
    return reinterpret_cast<T *>(reinterpret_cast<unsigned char *>(ptr) + bytes);
}
*/
#define REPLACE_LIB
#ifdef REPLACE_LIB
#define LIBVAR
#define VOID_STUB {}
#define INT_STUB { return 0; }
#else
#define LIBVAR extern
#define VOID_STUB ;
#define INT_STUB ;
#endif

extern "C"
{
    LIBVAR _SsFCALL SsFCALL;
    LIBVAR struct SeqMarker _SsMarkCallback[32];
    LIBVAR int VBLANK_MINUS;
    LIBVAR unsigned long _snd_openflag;
    LIBVAR int _snd_ev_flag;
    LIBVAR struct SeqStruct *_ss_score[32];
    LIBVAR short _snd_seq_s_max;
    LIBVAR short _snd_seq_t_max;

    LIBVAR short _svm_damper;
    LIBVAR VagAtr *_svm_tn;
    LIBVAR VabHdr *_svm_vh;
    LIBVAR ProgAtr *_svm_pg;

    LIBVAR short _svm_vab_count;

    LIBVAR ProgAtr *_svm_vab_pg[16];
    LIBVAR unsigned char _svm_vab_used[16];
    LIBVAR long _svm_vab_start[16];
    LIBVAR VagAtr *_svm_vab_tn[16];
    LIBVAR int _svm_vab_total[16];
    LIBVAR VabHdr *_svm_vab_vh[16];

    LIBVAR int _svm_vab_not_send_size;
    LIBVAR short kMaxPrograms;

    LIBVAR SpuReverbAttr _svm_rattr;

    LIBVAR char _SsVmMaxVoice;

    LIBVAR unsigned short _svm_okon1;
    LIBVAR unsigned short _svm_okon2;
    LIBVAR unsigned short _svm_okof1;
    LIBVAR unsigned short _svm_okof2;
    LIBVAR unsigned short _svm_orev1;
    LIBVAR unsigned short _svm_orev2;
    LIBVAR unsigned short _svm_onos1;
    LIBVAR unsigned short _svm_onos2;

    LIBVAR unsigned char _svm_auto_kof_mode;

    LIBVAR short _svm_stereo_mono;

    LIBVAR unsigned int _snd_vmask;

    struct RegBufStruct
    {
        short field_0_vol_left;
        short field_2_vol_right;
        short field_4_pitch;
        unsigned short field_6_vagAddr;
        unsigned short field_8_adsr1;
        unsigned short field_A_adsr2;
        short field_0xc;
        short field_0xe;
    };
    LIBVAR RegBufStruct _svm_sreg_buf[24];
    LIBVAR unsigned char _svm_sreg_dirty[24];

    LIBVAR int _svm_envx_ptr;
    LIBVAR unsigned int _svm_envx_hist[16];

    struct SpuVoice
    {
        short field_0_vag_idx;
        short field_0x2;
        short field_4_pitch;
        short field_6_keyStat;
        short field_8_voll;
        char field_0xa;
        char pad5;
        short field_C_channel_idx;
        short field_E_note;
        short field_10_seq_sep_no;
        short field_12_fake_program;
        short field_14_program;
        short field_16_vag_num; // TODO: char
        short field_18_vabId;
        short field_1A_priority;

        char pad8;
        char field_0x1d;

        short field_1E_bAutoVol;
        short field_20_autoVolAmount;
        short field_22_autoVol_dt1;
        short field_24_autoVol_dt2;
        short field_26_autoVol_Start;
        short field_28_autoVol_End;
        short field_2A_bAutoPan;

        short field_0x2c; // amount ?
        short field_0x2e; // dt1?
        short field_0x30; // dt2?
        short field_0x32; // start ?
        short pad1;       // end ?

        short field_36_voll;
    };

    LIBVAR SpuVoice _svm_voice[24];

    struct struct_svm
    {
        char field_0_sep_sep_no_tonecount;
        char field_1_vabId;
        char field_2_note;
        char field_0x3;
        char field_4_voll;
        char field_0x5;
        char field_6_program;
        char field_7_fake_program;
        char field_8_unknown;
        char field_0x9;
        char field_A_mvol;
        char field_B_mpan;
        char field_C_vag_idx;
        char field_D_vol;
        char field_E_pan;
        char field_F_prior;
        char field_10_centre;
        unsigned char field_11_shift;
        char field_12_mode;
        char field_0x13;
        short field_14_seq_sep_no;
        short field_16_vag_idx;
        short field_18_voice_idx;
        int field_0x1a;
    }; // 26 bytes, can't be bigger than 28 ?

    LIBVAR struct_svm _svm_cur;

    typedef void (*AutoVolPanCallBack)(unsigned int voiceNum);
    LIBVAR AutoVolPanCallBack _autovol;
    LIBVAR AutoVolPanCallBack _autopan;

    // protos
    int _SsVmVSetUp(short vabId, short program);
    int _SsReadDeltaValue(short seq_access_num, short seq_num);
    short note2pitch2(short note, short fine);
    void _SsVmGetSeqVol(short seq_sep_no, unsigned short *pVolL, unsigned short *pVolR);
    void _SsVmSetSeqVol(short seq_sep_num, short volL, short volR);
    void SsUtGetVagAtr(short vabId, short progNum, short toneNum, VagAtr *pVagAttr);
    short SsUtSetVagAtr(short vabId, short progNum, short toneNum, VagAtr *pVagAttr);

    void debug_dump_vh(unsigned long *pAddr, short vabId)
    {
        VabHdr *pHeader = (VabHdr *)pAddr;
        printf("Dump %p for vabId %d\n", pAddr, vabId);    // id ok
        printf("Vab total = %d\n", _svm_vab_total[vabId]); // ok

        ProgAtr *pProgTable = _svm_vab_pg[vabId];
        for (int i = 0; i < pHeader->vs; i++)
        {
            if ((i & 1) == 0)
            {
                printf("Idx %d IdxD2 %d rev0 %d rev3 %d\n", i, i / 2, (int)pProgTable[i].reserved1, pProgTable[i / 2].reserved3);
            }
            else
            {
                printf("Idx %d IdxD2 %d rev0 %d rev3 %d\n", i, i / 2, (int)pProgTable[i].reserved1, pProgTable[i / 2].reserved2);
            }
        }

        printf("Tone start %p\n", _svm_vab_tn[vabId]); // ok
    }

    int _SsInitSoundSeq(int seqId, int vabId, unsigned char *pSeqData)
    {
        SeqStruct *pSeq = &_ss_score[seqId][0];
        pSeq->field_26_vab_id = vabId;
        pSeq->field_50_res_of_quarter_note = 0;
        pSeq->field_18 = 0;
        pSeq->field_19 = 0;
        pSeq->field_1E = 0;
        pSeq->field_1A_fn_idx = 0;
        pSeq->field_1B = 0;
        pSeq->field_1F = 0;
        pSeq->field_17_channel_idx = 0;
        pSeq->field_84 = 0;
        pSeq->field_88 = 0;
        pSeq->field_8C_tempo = 0;
        pSeq->field_56 = 0;
        pSeq->field_21 = 0;
        pSeq->field_20_l_count = 1;
        pSeq->field_14_play_mode = 0;
        pSeq->field_90_delta_value = 0;
        pSeq->field_1C = 0;
        pSeq->field_1D = 0;
        pSeq->field_15 = 0;
        pSeq->field_16_running_status = 0;
        pSeq->field_80 = 0;
        pSeq->field_24_rhythm_n = 0;
        pSeq->field_25_rhythm_d = 0;
        
        for (int i=0; i < 16; i++)
        {
            pSeq->field_37_programs[i] = i;
            pSeq->field_27_panpot[i] = 64;
            pSeq->field_60_vol[i] = 127;
        }

        pSeq->field_52 = 1;
        pSeq->field_0_seq_ptr = pSeqData;

        // Magic
        if ((pSeqData[0] != 'S') && (pSeqData[0] != 'p'))
        {
            printf("This is an old SEQ Data Format.\n");
            return 0;
        }

        // Version
        if (pSeqData[7] != 1)
        {
            printf("This is not SEQ Data.\n");
            return -1;
        }

        // Quarter note
        pSeq->field_50_res_of_quarter_note = pSeqData[9] | (pSeqData[8] << 8);

        // Tempo
        const unsigned int tempo = pSeqData[12] | pSeqData[10] << 0x10 | pSeqData[11] << 8;
        pSeq->field_8C_tempo = tempo;
        if (tempo >> 1 < 60000000U % tempo)
        {
            pSeq->field_8C_tempo = 60000000 / tempo + 1;
        }
        else
        {
            pSeq->field_8C_tempo = 60000000 / tempo;
        }
        
        pSeq->field_94 = pSeq->field_8C_tempo;

        // Rhythm
        pSeq->field_24_rhythm_n = pSeqData[13];
        pSeq->field_25_rhythm_d = pSeqData[14];

        pSeq->field_0_seq_ptr = &pSeqData[15];

        const int iVar4 = _SsReadDeltaValue(seqId, 0);
        const int uVar8 = pSeq->field_50_res_of_quarter_note * pSeq->field_8C_tempo;
        pSeq->field_8 = pSeq->field_0_seq_ptr;
        pSeq->field_84 = iVar4;
        pSeq->field_90_delta_value = iVar4;
        pSeq->field_10 = 0;
        pSeq->field_C = pSeq->field_0_seq_ptr;
        pSeq->field_4 = pSeq->field_0_seq_ptr;

        const int tmp = VBLANK_MINUS * 60;
        if (uVar8 * 10 < tmp)
        {
            pSeq->field_52 = (VBLANK_MINUS * 600) / uVar8;
        }
        else
        {
            pSeq->field_52 = -1;
            pSeq->field_54 = (uVar8 * 10) / tmp;
            if ((uVar8 * 10) % tmp > (VBLANK_MINUS * 30))
            {
                pSeq->field_54++;
            }
        }

        pSeq->field_56 = pSeq->field_54;
        return 0;
    }

    void _SsVmKeyOnNow(short /*vagCount*/, short pitch)
    {
        SeqStruct *pSeq = &_ss_score[_svm_cur.field_14_seq_sep_no & 0xff][(_svm_cur.field_14_seq_sep_no & 0xff00) >> 8];

        int uVar1 = (((_svm_cur.field_4_voll * _svm_vh->mvol * 0x3fff) / 0x3f01) * _svm_cur.field_A_mvol * _svm_cur.field_D_vol) / 0x3f01;

        int left = uVar1;
        if (_svm_cur.field_14_seq_sep_no != 0x21)
        {
            left = (uVar1 * pSeq->field_58_voll) / 127;
            uVar1 = (uVar1 * pSeq->field_5A_volr) / 127;
        }

        if (_svm_cur.field_E_pan < 64)
        {
            uVar1 = (uVar1 * _svm_cur.field_E_pan) / 63;
        }
        else
        {
            left = (left * (127 - _svm_cur.field_E_pan)) / 63;
        }

        if (_svm_cur.field_B_mpan < 64)
        {
            uVar1 = (uVar1 * _svm_cur.field_B_mpan) / 63;
        }
        else
        {
            left = (left * (127 - _svm_cur.field_B_mpan)) / 63;
        }

        if (_svm_cur.field_0x5 < 64)
        {
            uVar1 = (uVar1 * _svm_cur.field_0x5) / 63;
        }
        else
        {
            left = (left * (127 - _svm_cur.field_0x5)) / 63;
        }

        int right = uVar1;
        if (_svm_stereo_mono == 1)
        {
            if (left < right)
            {
                right = left;
            }
            left = uVar1;
            right = uVar1;
        }

        if (_svm_cur.field_14_seq_sep_no != 0x21)
        {
            left = (left * left) / 0x3fff;
            right = (right * right) / 0x3fff;
        }

        _svm_sreg_buf[_svm_cur.field_18_voice_idx].field_4_pitch = pitch;
        _svm_sreg_buf[_svm_cur.field_18_voice_idx].field_0_vol_left = left;
        _svm_sreg_buf[_svm_cur.field_18_voice_idx].field_2_vol_right = right;
        _svm_sreg_dirty[_svm_cur.field_18_voice_idx] |= 7;
        _svm_voice[_svm_cur.field_18_voice_idx].field_4_pitch = pitch;

        unsigned short bitsLower =0;
        unsigned short bitsUpper;
        if (_svm_cur.field_18_voice_idx < 16)
        {
            bitsLower = (1 << (_svm_cur.field_18_voice_idx & 31));
            bitsUpper = 0;
        }
        else
        {
            bitsLower = 0;
            bitsUpper = (1 << (_svm_cur.field_18_voice_idx - 16 & 31));
        }

        if ((_svm_cur.field_12_mode & 4) == 0)
        {
            _svm_orev1 = _svm_orev1 & ~bitsLower;
            _svm_orev2 = _svm_orev2 & ~bitsUpper;
        }
        else
        {
            _svm_orev1 = _svm_orev1 | bitsLower;
            _svm_orev2 = _svm_orev2 | bitsUpper;
        }

        _svm_okon2 = _svm_okon2 | bitsUpper;
        _svm_okon1 = _svm_okon1 | bitsLower;
        _svm_onos1 = _svm_onos1 & ~bitsLower;
        _svm_onos2 = _svm_onos2 & ~bitsUpper;
        _svm_okof1 = _svm_okof1 & ~_svm_okon1;
        _svm_okof2 = _svm_okof2 & ~_svm_okon2;
    }

    short SsUtGetProgAtr(short vabId, short programNum, ProgAtr* pProgAttr)
    {
        if (_svm_vab_used[vabId] == 1)
        {
            _SsVmVSetUp(vabId, programNum);
            pProgAttr->tones = _svm_pg[programNum].tones;
            pProgAttr->mvol = _svm_pg[programNum].mvol;
            pProgAttr->prior = _svm_pg[programNum].prior;
            pProgAttr->mode = _svm_pg[programNum].mode;
            pProgAttr->mpan = _svm_pg[programNum].mpan;
            pProgAttr->attr = _svm_pg[programNum].attr;
            return 0;
        }
        return -1;
    }

    void _SsVmPBVoice(short voiceNum, short seq_sep_num, short vabId, short program, short pitch)
    {
        if ((_svm_voice[voiceNum].field_10_seq_sep_no == seq_sep_num &&
             _svm_voice[voiceNum].field_18_vabId == vabId) &&
            _svm_voice[voiceNum].field_14_program == program)
        {
            const short note = _svm_voice[voiceNum].field_E_note;
            const short vagIdx = _svm_voice[voiceNum].field_16_vag_num + (_svm_cur.field_7_fake_program * 16);

            short bendMin = note;
            short bendMax = 0;

            const int pitch_converted = (pitch - 64);
            if (pitch_converted < 0)
            {
                int pbmin_tmp = pitch_converted * _svm_tn[vagIdx].pbmin;
                
                bendMin = note + pbmin_tmp / 64 - 1;
                bendMax = 2 * (pbmin_tmp % 64) + 127;
            }
            else if (pitch_converted > 0)
            {
                const int pbmax_tmp = pitch_converted * _svm_tn[vagIdx].pbmax;
                bendMin = note + pbmax_tmp / 63;
                bendMax = (pbmax_tmp % 63) * 2;
            }
            else if (pitch_converted == 0)
            {
                bendMax = 0;
            }
            _svm_cur.field_C_vag_idx = _svm_voice[voiceNum].field_16_vag_num & 0xFF;
            _svm_cur.field_18_voice_idx = voiceNum;
            _svm_sreg_buf[voiceNum].field_4_pitch = note2pitch2(bendMin, bendMax);
            _svm_sreg_dirty[voiceNum] |= 4;
        }
    }

    void _SsVmPitchBend(short seq_sep_no, short vabId, unsigned char program, unsigned char pitch)
    {
        _SsVmVSetUp(vabId, program);

        _svm_cur.field_14_seq_sep_no = seq_sep_no;

        for (int i = 0; i < _SsVmMaxVoice; i++)
        {
            _SsVmPBVoice(i, seq_sep_no, vabId, program, pitch);
        }
    }

    void _SsSndCrescendo(short seq_idx, short sep_idx)
    {
        SeqStruct* pStru = &_ss_score[seq_idx][sep_idx]; // note: 14bit access
        pStru->field_A0++;

        if (pStru->field_9C < pStru->field_A0)
        {
            // done ?
            pStru->field_98_flags &= ~0x10u;
        }
        else
        {
            const int new_field_4a = (pStru->field_48 * pStru->field_A0) / pStru->field_9C;
            const int volIncBy = new_field_4a - pStru->field_4A;

            if (new_field_4a != pStru->field_4A)
            {
                pStru->field_4A = new_field_4a;

                unsigned short seqLeftVol = 0;
                unsigned short seqRightVol = 0;
                _SsVmGetSeqVol(seq_idx | (sep_idx << 8), &seqLeftVol, &seqRightVol);
                
                int voll_clamped = seqLeftVol + volIncBy;
                if (voll_clamped > 127)
                {
                    voll_clamped = 127;
                }
                if (voll_clamped < 0)
                {
                    voll_clamped = 0;
                }

                int volr_clamped = seqRightVol + volIncBy;
                if (volr_clamped > 127)
                {
                    volr_clamped = 127;
                }
                if (volr_clamped < 0)
                {
                    volr_clamped = 0;
                }

                _SsVmSetSeqVol(seq_idx | (sep_idx << 8), voll_clamped, volr_clamped);
                if (voll_clamped == 127 && volr_clamped == 127 && voll_clamped == 0 && volr_clamped == 0)
                {
                    // done ?
                    pStru->field_98_flags &= ~0x10u;
                }
            }
        }

        _SsVmGetSeqVol(seq_idx | (sep_idx << 8), &pStru->field_5C, &pStru->field_5E);
    }

    void _SsSndTempo(short seqNum, short sepNum)
    {
        SeqStruct* pSeq = &_ss_score[seqNum][sepNum]; // note: 14bit access
        pSeq->field_A8--;

        if (pSeq->field_A8 < 0)
        {
            pSeq->field_98_flags &= ~0x40u;
            pSeq->field_98_flags &= ~0x80u;
            return;
        }

        if (pSeq->field_4E <= 0)
        {
            if (pSeq->field_AC < pSeq->field_94)
            {
                pSeq->field_94 -= pSeq->field_4E;
                if (pSeq->field_AC < pSeq->field_94)
                {
                    pSeq->field_94 = pSeq->field_AC;
                }
            }
            else if (pSeq->field_AC > pSeq->field_94)
            {
                pSeq->field_94 += pSeq->field_4E;
                if (pSeq->field_94 < pSeq->field_AC)
                {
                    pSeq->field_94 = pSeq->field_AC;
                }
            }
        }
        else
        {
            if (pSeq->field_A8 % pSeq->field_4E)
            {
                return;
            }

            int local_94_dec = pSeq->field_94 - 1;
            if (pSeq->field_AC < pSeq->field_94 || pSeq->field_94 < pSeq->field_AC)
            {
                if (pSeq->field_AC >= pSeq->field_94) // TODO: Logic check
                {
                    local_94_dec = pSeq->field_94 + 1;
                }
                pSeq->field_94 = local_94_dec;
            }
        }

        const int calc = (pSeq->field_50_res_of_quarter_note * pSeq->field_94) / (60 * VBLANK_MINUS);
        pSeq->field_54 = calc;

        if ((calc << 16) <= 0) // TODO: Is this shifting wrong ?
        {
            pSeq->field_54 = 1;
        }

        if (!pSeq->field_A8 || pSeq->field_94 == pSeq->field_AC)
        {
            pSeq->field_98_flags &= ~0x40u;
            pSeq->field_98_flags &= ~0x80u;
        }
    }

    // TODO: Taken from googling SsPitchFromNote along with SsPitchFromNote's impl
    const unsigned short _svm_ptable[] =
        {
            4096, 4110, 4125, 4140, 4155, 4170, 4185, 4200,
            4216, 4231, 4246, 4261, 4277, 4292, 4308, 4323,
            4339, 4355, 4371, 4386, 4402, 4418, 4434, 4450,
            4466, 4482, 4499, 4515, 4531, 4548, 4564, 4581,
            4597, 4614, 4630, 4647, 4664, 4681, 4698, 4715,
            4732, 4749, 4766, 4783, 4801, 4818, 4835, 4853,
            4870, 4888, 4906, 4924, 4941, 4959, 4977, 4995,
            5013, 5031, 5050, 5068, 5086, 5105, 5123, 5142,
            5160, 5179, 5198, 5216, 5235, 5254, 5273, 5292,
            5311, 5331, 5350, 5369, 5389, 5408, 5428, 5447,
            5467, 5487, 5507, 5527, 5547, 5567, 5587, 5607,
            5627, 5648, 5668, 5688, 5709, 5730, 5750, 5771,
            5792, 5813, 5834, 5855, 5876, 5898, 5919, 5940,
            5962, 5983, 6005, 6027, 6049, 6070, 6092, 6114,
            6137, 6159, 6181, 6203, 6226, 6248, 6271, 6294,
            6316, 6339, 6362, 6385, 6408, 6431, 6455, 6478,
            6501, 6525, 6549, 6572, 6596, 6620, 6644, 6668,
            6692, 6716, 6741, 6765, 6789, 6814, 6839, 6863,
            6888, 6913, 6938, 6963, 6988, 7014, 7039, 7064,
            7090, 7116, 7141, 7167, 7193, 7219, 7245, 7271,
            7298, 7324, 7351, 7377, 7404, 7431, 7458, 7485,
            7512, 7539, 7566, 7593, 7621, 7648, 7676, 7704,
            7732, 7760, 7788, 7816, 7844, 7873, 7901, 7930,
            7958, 7987, 8016, 8045, 8074, 8103, 8133, 8162,
            8192};

    short SsPitchFromNote(short note, short fine, unsigned char center, unsigned char shift)
    {

        unsigned int pitch;
        short calc, type;
        signed int add, sfine; //, ret;

        sfine = fine + shift;
        if (sfine < 0)
            sfine += 7;
        sfine >>= 3;

        add = 0;
        if (sfine > 15)
        {
            add = 1;
            sfine -= 16;
        }

        calc = add + (note - (center - 60)); //((center + 60) - note) + add;
        pitch = _svm_ptable[16 * (calc % 12) + (short)sfine];
        type = calc / 12 - 5;

        // regular shift
        if (type > 0)
            return pitch << type;
        // negative shift
        if (type < 0)
            return pitch >> -type;

        return pitch;
    }
    short note2pitch2(short note, short fine)
    {
        VagAtr* pVag = &_svm_tn[_svm_cur.field_C_vag_idx + (_svm_cur.field_7_fake_program * 16)];
        return SsPitchFromNote(note, fine, pVag->center, pVag->shift);
    }

    short note2pitch(void)
    {
        short pitchShift = _svm_cur.field_11_shift;
        if (_svm_cur.field_11_shift > 127)
        {
            pitchShift = 127;
        }
        return SsPitchFromNote(_svm_cur.field_2_note, 0, _svm_cur.field_10_centre, pitchShift);
    }

    void vmNoiseOn(short voiceNum)
    {
        int rightVolCalc = (((_svm_cur.field_4_voll * _svm_vh->mvol * 0x3fff) / 0x3f01) * 
                         _svm_cur.field_A_mvol * _svm_cur.field_D_vol) / 0x3f01;

        SeqStruct *pSeq = &_ss_score[_svm_cur.field_14_seq_sep_no & 0xff][(_svm_cur.field_14_seq_sep_no & 0xff00) >> 8];

        int leftVolCalc = rightVolCalc;
        if (_svm_cur.field_14_seq_sep_no != 0x21)
        {
            leftVolCalc = (rightVolCalc * pSeq->field_58_voll) / 127;
            rightVolCalc = (rightVolCalc * pSeq->field_5A_volr) / 127;
        }

        if (_svm_cur.field_E_pan < 64)
        {
            rightVolCalc = (rightVolCalc * _svm_cur.field_E_pan) / 63;
        }
        else
        {
            leftVolCalc = (leftVolCalc * (127 - _svm_cur.field_E_pan)) / 63;
        }

        if (_svm_cur.field_B_mpan < 64)
        {
            rightVolCalc = (rightVolCalc * _svm_cur.field_B_mpan) / 63;
        }
        else
        {
            leftVolCalc = (leftVolCalc * (127 - _svm_cur.field_B_mpan)) / 63;
        }

        if (_svm_cur.field_0x5 < 64)
        {
            rightVolCalc = (_svm_cur.field_0x5 * rightVolCalc) / 63;
        }
        else
        {
            leftVolCalc = (leftVolCalc * (127 - _svm_cur.field_0x5)) / 63;
        }

        int rightVolFinal = rightVolCalc;
        if (_svm_stereo_mono == 1)
        {
            rightVolFinal = leftVolCalc; // TODO: Check logic
            if (leftVolCalc < rightVolCalc)
            {
                rightVolFinal = rightVolCalc;
                leftVolCalc = rightVolCalc;
            }
        }

        if (_svm_cur.field_14_seq_sep_no != 0x21)
        {
            leftVolCalc = (leftVolCalc * leftVolCalc) / 0x3fff;
            rightVolFinal = (rightVolFinal * rightVolFinal) / 0x3fff;
        }

        SpuSetNoiseClock(_svm_cur.field_2_note - _svm_cur.field_10_centre & 0x3f);

        _svm_sreg_buf[voiceNum].field_2_vol_right = rightVolFinal;
        _svm_sreg_buf[voiceNum].field_0_vol_left = leftVolCalc;
        _svm_sreg_dirty[voiceNum] |= 3;

        if (voiceNum < 16)
        {
            _svm_onos1 = (1 << (voiceNum & 0x1f));
            _svm_onos2 = 0;
        }
        else
        {
            _svm_onos1 = 0;
            _svm_onos2 = (1 << (voiceNum - 16 & 0x1f));
        }
        _svm_voice[voiceNum].field_4_pitch = 10;

        for (int i = 0; i < _SsVmMaxVoice; i++)
        {
            if ((_snd_vmask & 1 << (i & 0x1f)) == 0)
            {
                _svm_voice[i].field_0x1d &= 1;
            }
        }
        _svm_voice[voiceNum].field_0x1d = 2;

        _svm_okon1 = _svm_okon1 | _svm_onos1;
        _svm_okon2 = _svm_okon2 | _svm_onos2;

        if ((_svm_cur.field_12_mode & 4) == 0)
        {
            _svm_orev1 = _svm_orev1 & ~_svm_onos1;
            _svm_orev2 = _svm_orev2 & ~_svm_onos2;
        }
        else
        {
            _svm_orev1 = _svm_orev1 | _svm_onos1;
            _svm_orev2 = _svm_orev2 | _svm_onos2;
        }

        _svm_okof1 = _svm_okof1 & ~_svm_okon1;
        _svm_okof2 = _svm_okof2 & ~_svm_okon2;
    }

    void vmNoiseOff(short voiceNum)
    {
        _svm_voice[voiceNum].field_0x1d = 0;
        _svm_voice[voiceNum].field_0_vag_idx = 0;
        _svm_voice[voiceNum].field_4_pitch = 0;
    }
    
    short _SsVmAlloc(void)
    {
        int lowest_keyStat = 0xffff;
        int lowest_match = 99;
        int lowest_prior = _svm_cur.field_F_prior;
        int lowest_field2 = 0;

        int match_counter = 0;
        int voice_to_alloc_idx = 99;
        for (int i = 0; i < _SsVmMaxVoice; i++)
        {
            if ((_snd_vmask & (1 << i)) == 0)
            {
                if (_svm_voice[i].field_0x1d == 0 && _svm_voice[i].field_6_keyStat == 0)
                {
                    voice_to_alloc_idx = i;
                    break;
                }

                if (_svm_voice[i].field_1A_priority == lowest_prior)
                {
                    match_counter++;

                    if (_svm_voice[i].field_6_keyStat == lowest_keyStat)
                    {
                        if (lowest_field2 < _svm_voice[i].field_0x2)
                        {
                            lowest_field2 = _svm_voice[i].field_0x2;
                            lowest_match = i;
                        }
                    }
                    else if (_svm_voice[i].field_6_keyStat < lowest_keyStat)
                    {
                        lowest_field2 = _svm_voice[i].field_0x2;
                        lowest_keyStat = _svm_voice[i].field_6_keyStat;
                        lowest_match = i;
                    }
                }
                else if (_svm_voice[i].field_1A_priority < lowest_prior)
                {
                    lowest_prior = _svm_voice[i].field_1A_priority;
                    lowest_keyStat = _svm_voice[i].field_6_keyStat;
                    lowest_field2 = _svm_voice[i].field_0x2;
                    match_counter = 1;
                    lowest_match = i;
                }
            }
        }

        if (voice_to_alloc_idx == 99)
        {
            voice_to_alloc_idx = lowest_match;
            if (match_counter == 0)
            {
                voice_to_alloc_idx = _SsVmMaxVoice;
            }
        }

        if (voice_to_alloc_idx < _SsVmMaxVoice)
        {
            for (int i = 0; i < _SsVmMaxVoice; i++)
            {
                if ((_snd_vmask & 1 << (i & 31)) == 0)
                {
                    _svm_voice[i].field_0x2++;
                }
            }
            _svm_voice[voice_to_alloc_idx].field_0x2 = 0;
            _svm_voice[voice_to_alloc_idx].field_2A_bAutoPan = 0;
            _svm_voice[voice_to_alloc_idx].field_1E_bAutoVol = 0;
            _svm_voice[voice_to_alloc_idx].field_1A_priority = _svm_cur.field_F_prior;
        }

        return voice_to_alloc_idx;
    }

    void _SsContDataEntry(short seq_no, short sep_no, unsigned char dataEntry)
    {
        SeqStruct* pSeq = &_ss_score[seq_no][sep_no];

        // Do this first even though its never used till later
        ProgAtr progAttr;
        SsUtGetProgAtr(pSeq->field_26_vab_id, pSeq->field_37_programs[pSeq->field_17_channel_idx], &progAttr);

        if (pSeq->field_1C == 1 && pSeq->field_15 == 0)
        {
            pSeq->field_1D = dataEntry;
            pSeq->field_1C = 0;
            pSeq->field_15 = 1;
            pSeq->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
            return;
        }

        VagAtr oldVagAtr;
        if (pSeq->field_1E == 2)
        {
            if (pSeq->field_19 || progAttr.tones == 0)
            {
                pSeq->field_1E = 0;
                pSeq->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
                return;
            }

            for (int tone_idx = 0; tone_idx < progAttr.tones; tone_idx++)
            {
                SsUtGetVagAtr(pSeq->field_26_vab_id, pSeq->field_37_programs[pSeq->field_17_channel_idx], tone_idx, &oldVagAtr);
                if (pSeq->field_18 != 1 && pSeq->field_18 < 2 && pSeq->field_18 == 0) // todo: uh.. what ?
                {
                    oldVagAtr.pbmax = dataEntry & 127;
                    oldVagAtr.pbmin = dataEntry & 127;
                }

                // Redundant unless above we true, oh well
                SsUtSetVagAtr(pSeq->field_26_vab_id, pSeq->field_37_programs[pSeq->field_17_channel_idx], tone_idx, &oldVagAtr);
            }

            pSeq->field_1E = 0;
            pSeq->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
            return;
        }

        if (pSeq->field_1F == 2)
        {
            int tone_no = 0;
            if (pSeq->field_1B == 16)
            {
                tone_no = 0;
            }
            else
            {
                tone_no = pSeq->field_1B;
            }

            SsFCALL.ccentry[pSeq->field_1A_fn_idx](
                pSeq->field_26_vab_id,
                pSeq->field_37_programs[pSeq->field_17_channel_idx],
                tone_no,
                oldVagAtr,
                pSeq->field_1A_fn_idx,
                dataEntry);

            pSeq->field_1F = 0;
        }

        pSeq->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsVmSetSeqVol(short seq_sep_num, short volL, short volR)
    {
        SeqStruct *pSeq = &_ss_score[seq_sep_num & 0xff][(seq_sep_num & 0xff00) >> 8];

        pSeq->field_58_voll = volL;
        pSeq->field_5A_volr = volR;

        if (pSeq->field_58_voll > 127)
        {
            pSeq->field_58_voll = 127;
        }

        if (pSeq->field_5A_volr > 127)
        {
            pSeq->field_5A_volr = 127;
        }

        for (int i = 0; i < _SsVmMaxVoice; i++)
        {
            SpuVoice& voice = _svm_voice[i];

            if ((_snd_vmask & 1 << (i & 31)) == 0 &&
                 voice.field_10_seq_sep_no == seq_sep_num &&
                 voice.field_18_vabId == pSeq->field_26_vab_id)
            {
                _SsVmVSetUp(voice.field_18_vabId, voice.field_12_fake_program);
                const short vol_factor = (((_svm_vh->mvol *
                                     ((voice.field_8_voll * pSeq->field_60_vol[voice.field_C_channel_idx]) /
                                      127) *
                                     0x3fff) / 0x3f01) *
                              _svm_pg[voice.field_14_program].mvol *
                              _svm_tn[voice.field_12_fake_program * 16 + voice.field_16_vag_num].vol) / 0x3f01;

                short voll_val = (vol_factor * pSeq->field_58_voll) / 127;
                const short pan = _svm_tn[(voice.field_12_fake_program * 16) + voice.field_16_vag_num].pan;

                short volr_val = (vol_factor * pSeq->field_5A_volr) / 127;
                if (pan < 64)
                {
                    volr_val = (volr_val * pan) / 63;
                }
                else
                {
                    voll_val = (voll_val * (127 - pan)) / 63;
                }

                const short mpan = _svm_pg[voice.field_14_program].mpan;
                if (mpan < 64)
                {
                    //volr_val = (uint)((ulonglong)((longlong)(int)((volr_val) * mpan) * 0x82082083) >> 0x25);
                    volr_val = (volr_val * mpan) / 63;
                }
                else
                {
                    voll_val = ((voll_val) * (127 - mpan)) / 63;
                }

                const short voice_field_A = voice.field_0xa;
                if (voice_field_A < 64)
                {
                    //volr_val = (uint)((ulonglong)((longlong)(int)((volr_val) * voice_field_A) * 0x82082083) >> 0x25);
                    volr_val = (volr_val * voice_field_A) / 63;
                }
                else
                {
                    voll_val = ((voll_val) * (127 - voice_field_A)) / 63;
                }

                short left_final = voll_val;
                if (_svm_stereo_mono == 1)
                {
                    if (left_final < (volr_val))
                    {
                        voll_val = volr_val;
                    }
                    left_final = voll_val;
                    volr_val = voll_val;
                }

                _svm_sreg_buf[i].field_0_vol_left = (left_final * left_final) / 0x3fff;
                _svm_sreg_buf[i].field_2_vol_right = (volr_val * volr_val) / 0x3fff;
                _svm_sreg_dirty[i] |= 3;
            }
        }
    }

    void _SsVmKeyOffNow(void)
    {
        unsigned short bitsUpper = 0;
        unsigned short bitsLower = 0;

        if (_svm_cur.field_18_voice_idx < 16)
        {
            bitsLower = (1 << (_svm_cur.field_18_voice_idx & 31));
            bitsUpper = 0;
        }
        else
        {
            bitsLower = 0;
            bitsUpper = (1 << ((_svm_cur.field_18_voice_idx - 16) & 31));
        }

        _svm_voice[_svm_cur.field_18_voice_idx].field_0x1d = 0;
        _svm_voice[_svm_cur.field_18_voice_idx].field_4_pitch = 0;
        _svm_voice[_svm_cur.field_18_voice_idx].field_0_vag_idx = 0;

        _svm_okof1 = _svm_okof1 | bitsLower;
        _svm_okof2 = _svm_okof2 | bitsUpper;

        _svm_okon1 = _svm_okon1 & ~_svm_okof1;
        _svm_okon2 = _svm_okon2 & ~_svm_okof2;
    }

    void _SsVmSeqKeyOff(short seq_sep_no)
    {
        for (int i = 0; i < _SsVmMaxVoice; i++)
        {
            if ((_snd_vmask & 1 << (i & 31)) == 0 && _svm_voice[i].field_10_seq_sep_no == seq_sep_no)
            {
                _svm_cur.field_18_voice_idx = i;
                _SsVmKeyOffNow();
            }
        }
    }

    void _SsVmGetSeqVol(short seq_sep_no, unsigned short *pVolL, unsigned short *pVolR)
    {
        SeqStruct* pStru = &_ss_score[seq_sep_no & 0xFF][(seq_sep_no & 0xFF00) >> 8];
        _svm_cur.field_0_sep_sep_no_tonecount = seq_sep_no;
        *pVolL = pStru->field_58_voll;
        *pVolR = pStru->field_5A_volr;
    }
/*
    void _SsSeqGetEof(short seq_access_num, short sep_num) VOID_STUB // wip
    void _SsGetSeqData(short seq_idx, short sep_idx) VOID_STUB       // wip
    void _SsSeqPlay(short seq_access_num, short seq_num) VOID_STUB   // wip
*/
    long _SsVmSeKeyOn(unsigned char vab, unsigned char program, unsigned char note, unsigned char pitch, unsigned short volL, unsigned short volR); // done
    void _SsVmKeyOff(int seq_sep_no, short vabId, short program, short note); // done
    int _SsVmKeyOn(int seq_sep_no, short vabId, short unknown37, short note, short voll, short unknown27); // done

    //void _SsVmFlush(void) VOID_STUB // TODO: Can't link due to other globals being required
    void _SsVmFlush(void)
    {
        _svm_envx_ptr = (_svm_envx_ptr + 1) & 15;
        _svm_envx_hist[_svm_envx_ptr] = 0;
        for (int i = 0; i < _SsVmMaxVoice; i++)
        {
            SpuGetVoiceEnvelope(i, &_svm_voice[i].field_6_keyStat);
            if (_svm_voice[i].field_6_keyStat == 0)
            {
                _svm_envx_hist[_svm_envx_ptr] |= 1 << (i & 31);
            }
        }

        if (_svm_auto_kof_mode == 0)
        {
            unsigned int voiceBits = 0xffffffff;
            for (int i = 0; i < 15; i++)
            {
                voiceBits &= _svm_envx_hist[i];
            }

            for (int i = 0; i < _SsVmMaxVoice; i++)
            {
                unsigned int mask = 1 << (i & 31);
                if ((voiceBits & mask) != 0)
                {
                    if (_svm_voice[i].field_0x1d == 2)
                    {
                        unsigned int mask2 = 0;
                        if (15 < i)
                        {
                            mask = 0;
                            mask2 = 1 << (i - 0x10 & 31);
                        }
                        SpuSetNoiseVoice(0, (mask2 & 0xff) << 0x10 | mask);
                    }
                    _svm_voice[i].field_0x1d = 0;
                }
            }
        }

        _svm_okon1 = _svm_okon1 & ~_svm_okof1;
        _svm_okon2 = _svm_okon2 & ~_svm_okof2;

        for (int i = 0; i < 24; i++)
        {
            if (_svm_voice[i].field_1E_bAutoVol != 0)
            {
                _autovol(i);
            }
            if (_svm_voice[i].field_2A_bAutoPan != 0)
            {
                _autopan(i);
            }
        }

        for (int i = 0; i < 24; i++)
        {
            SpuVoiceAttr voiceAttr;
            voiceAttr.voice = 1 << (i & 31);
            voiceAttr.mask = 0;

            if ((_svm_sreg_dirty[i] & 1) != 0)
            {
                voiceAttr.mask |= 3;
                voiceAttr.volume.left = _svm_sreg_buf[i].field_0_vol_left;
                voiceAttr.volume.right = _svm_sreg_buf[i].field_2_vol_right;
            }

            if ((_svm_sreg_dirty[i] & 4) != 0)
            {
                voiceAttr.mask |= 0x10;
                voiceAttr.pitch = _svm_sreg_buf[i].field_4_pitch;
            }

            if ((_svm_sreg_dirty[i] & 8) != 0)
            {
                voiceAttr.mask |= 0x80;
                voiceAttr.addr = (_svm_sreg_buf[i].field_6_vagAddr) << 3;
            }

            if ((_svm_sreg_dirty[i] & 0x10) != 0)
            {
                voiceAttr.mask |= 0x60000;
                voiceAttr.adsr1 = _svm_sreg_buf[i].field_8_adsr1;
                voiceAttr.adsr2 = _svm_sreg_buf[i].field_A_adsr2;
            }

            if (voiceAttr.mask != 0)
            {
                SpuSetVoiceAttr(&voiceAttr);
            }

            _svm_sreg_dirty[i] = 0;
        }

        SpuSetKey(0, (_svm_okof2 << 16) | _svm_okof1); // TODO: Check is correct
        SpuSetKey(1, (_svm_okon2 << 16) | _svm_okon1);

        const int mask = 0xFFFFFF >> (24 - _SsVmMaxVoice);

        const int v21 = ((_svm_orev2 << 16) | _svm_orev1) & mask;
        SpuSetReverbVoice(8, v21 | SpuGetReverbVoice() & ~mask);

        const int v22 = ((_svm_onos2 << 16) | _svm_onos1) & mask;
        SpuSetNoiseVoice(8, v22 | SpuGetNoiseVoice() & ~mask);

        _svm_okof1 = 0;
        _svm_okon1 = 0;
        _svm_okof2 = 0;
        _svm_okon2 = 0;
        _svm_onos2 = 0;
        _svm_onos1 = 0;
    }

    void _SsVmSetVol(short seq_sep_no, short vabId, short program, short voll, short volr)
    {
        SeqStruct *pSeq = &_ss_score[seq_sep_no & 0xff][(seq_sep_no & 0xff00) >> 8];
        _SsVmVSetUp(vabId, program);

        if (volr == 0)
        {
            volr = 1;
        }

        if (voll == 0)
        {
            voll = 1;
        }

        _svm_cur.field_14_seq_sep_no = seq_sep_no;

        for (int i = 0; i < _SsVmMaxVoice; i++)
        {
            SpuVoice &voice = _svm_voice[i];
            if ((_snd_vmask & 1 << (i & 31)) == 0 &&
                voice.field_10_seq_sep_no == seq_sep_no &&
                voice.field_14_program == program &&
                voice.field_18_vabId == vabId)
            {
                if ((pSeq->field_60_vol[pSeq->field_17_channel_idx] != voll) && (pSeq->field_60_vol[pSeq->field_17_channel_idx] == 0))
                {
                    pSeq->field_60_vol[pSeq->field_17_channel_idx] = 1;
                }

                VagAtr &vagAtr = _svm_tn[(voice.field_12_fake_program * 16) + voice.field_16_vag_num];
                int v1 = ((voice.field_8_voll * voll / 127) * _svm_vh->mvol * 0x3fff) / 0x3f01;
                v1 = (v1 * _svm_pg[program].mvol * vagAtr.vol) / 0x3f01;

                const int vagPan = vagAtr.pan;

                int left = (v1 * pSeq->field_58_voll) / 127;
                int right = (v1 * pSeq->field_5A_volr) / 127;

                if (vagPan < 64)
                {
                    right = (right * vagPan) / 63;
                }
                else
                {
                    left = (left * (127 - vagPan)) / 63;
                }

                const int progPan = _svm_pg[_svm_voice[seq_sep_no].field_14_program].mpan;
                if (progPan < 64)
                {
                    right = (right * progPan) / 63;
                }
                else
                {
                    left = (left * (127 - progPan)) / 63;
                }

                int tmp = volr & 0xff;
                if (tmp < 64)
                {
                    right = (right * tmp) / 63;
                }
                else
                {
                    left = (left * (127 - tmp)) / 63;
                }

                if (_svm_stereo_mono == 1)
                {
                    if (left < right)
                    {
                        left = right;
                    }
                    right = left;
                }

                _svm_sreg_buf[i].field_0_vol_left = (short)((left * left) / 0x3fff);
                _svm_sreg_buf[i].field_2_vol_right = (short)((right * right) / 0x3fff);
                _svm_sreg_dirty[i] |= 3;
            }
        }
    }

    static char spuHeapBookKeeping[264]; // (8*(32+1))

    void debug_dump_ssinit()
    {
        printf("_SsVmMaxVoice = %d\n", _SsVmMaxVoice);
        for (unsigned int i = 0; i < _SsVmMaxVoice; i++)
        {
            printf("idx = %d\n", i);
            /*
            printf("field_0x2 = %d\n", _svm_voice[i].field_0x2);
            printf("field_0x0 = %d\n",_svm_voice[i].field_0x0);
            printf("field_0x1d = %d\n",_svm_voice[i].field_0x1d);
            printf("field_0x4 = %d\n", _svm_voice[i].field_0x4);
            printf("field_0x6 = %d\n", _svm_voice[i].field_0x6);
            printf("field_0x10 = %d\n", _svm_voice[i].field_0x10);
            printf("field_0x12 = %d\n", _svm_voice[i].field_0x12);
            printf("field_0x14 = %d\n", _svm_voice[i].field_0x14);
            printf("field_0x16 = %d\n", _svm_voice[i].field_0x16);
            printf("field_0x8 = %d\n", _svm_voice[i].field_0x8);
            printf("field_0xc = %d\n", _svm_voice[i].field_0xc);
            printf("field_0xa = %d\n", _svm_voice[i].field_0xa);
            printf("field_0x36 = %d\n", _svm_voice[i].field_0x36);
            printf("field_0x1e = %d\n", _svm_voice[i].field_0x1e);
            printf("field_0x20 = %d\n", _svm_voice[i].field_0x20);
            printf("field_0x22 = %d\n", _svm_voice[i].field_0x22);
            printf("field_0x24 = %d\n", _svm_voice[i].field_0x24);
            printf("field_0x2a = %d\n", _svm_voice[i].field_0x2a);
            printf("field_0x2c = %d\n", _svm_voice[i].field_0x2c);
            printf("field_0x2e = %d\n", _svm_voice[i].field_0x2e);
            printf("field_0x30 = %d\n", _svm_voice[i].field_0x30);
            printf("field_0x32 = %d\n", _svm_voice[i].field_0x32);
            printf("field_0x26 = %d\n", _svm_voice[i].field_0x26);
            */
            printf("\n");
        }

        //printf("_svm_rattr.mask %d\n", _svm_rattr.mask);
        printf("_svm_rattr.depth.left %d\n", _svm_rattr.depth.left);
        printf("_svm_rattr.depth.right %d\n", _svm_rattr.depth.right);
        //printf("_svm_rattr.mode %d\n", _svm_rattr.mode);

        printf("_svm_okon1 %d\n", _svm_okon1);
        printf("_svm_okon2 %d\n", _svm_okon2);
        printf("_svm_okof1 %d\n", _svm_okof1);
        printf("_svm_orev1 %d\n", _svm_orev1);
        printf("_svm_orev2 %d\n", _svm_orev2);
        printf("_svm_onos1 %d\n", _svm_onos1);
        printf("_svm_onos2 %d\n", _svm_onos2);

        printf("_svm_auto_kof_mode %d\n", _svm_auto_kof_mode);

        printf("_svm_stereo_mono %d\n", _svm_stereo_mono);
        printf("_svm_vab_not_send_size %d\n", _svm_vab_not_send_size);

        printf("kMaxPrograms %d\n", kMaxPrograms);
    }

    //void _SsVmInit(int voiceCount);

    void _SsVmInit(int voiceCount)
    {
        // TODO: Duckstation warns about 2 bad spu reg writes that comes from this func
        _spu_setInTransfer(0);

        _svm_damper = 0;

        SpuInitMalloc(32, spuHeapBookKeeping);

        for (int i = 0; i < 24; i++)
        {
            _svm_sreg_buf[i].field_0_vol_left = 0;
            _svm_sreg_buf[i].field_2_vol_right = 0;
            _svm_sreg_buf[i].field_4_pitch = 0;
            _svm_sreg_buf[i].field_6_vagAddr = 0;
            _svm_sreg_buf[i].field_8_adsr1 = 0;
            _svm_sreg_buf[i].field_A_adsr2 = 0;
            _svm_sreg_buf[i].field_0xc = 0;
            _svm_sreg_buf[i].field_0xe = 0;
        }

        for (int i = 0; i < 24; i++)
        {
            _svm_sreg_dirty[i] = 0;
        }

        _svm_vab_count = 0;

        for (int i = 0; i < 16; i++)
        {
            _svm_vab_used[i] = 0;
        }

        _SsVmMaxVoice = voiceCount;

        if (voiceCount > 24)
        {
            _SsVmMaxVoice = 24;
        }

        SpuVoiceAttr voiceAttr;
        voiceAttr.mask = 0x60093; // TODO: Flags constants
        voiceAttr.pitch = 0x1000;
        voiceAttr.addr = 0x1000;

        voiceAttr.volume.left = 0;
        voiceAttr.volume.right = 0;

        voiceAttr.adsr1 = 0x80FF;
        voiceAttr.adsr2 = 0x4000;

        for (short i = 0; i < _SsVmMaxVoice; i++)
        {
            voiceAttr.voice = 1 << (i & 0x1f);
            _svm_voice[i].field_0x2 = 24;
            _svm_voice[i].field_0_vag_idx = 255;
            _svm_voice[i].field_0x1d = 0;
            _svm_voice[i].field_4_pitch = 0;
            _svm_voice[i].field_6_keyStat = 0;
            _svm_voice[i].field_8_voll = 0;
            _svm_voice[i].field_10_seq_sep_no = -1;
            _svm_voice[i].field_12_fake_program = 0;
            _svm_voice[i].field_14_program = 0;
            _svm_voice[i].field_16_vag_num = 255;
            _svm_voice[i].field_8_voll = 0; // BUG: Already set ??
            _svm_voice[i].field_C_channel_idx = 0;
            _svm_voice[i].field_0xa = 64;
            _svm_voice[i].field_36_voll = 0;

            _svm_voice[i].field_1E_bAutoVol = 0;
            _svm_voice[i].field_20_autoVolAmount = 0;
            _svm_voice[i].field_22_autoVol_dt1 = 0;
            _svm_voice[i].field_24_autoVol_dt2 = 0;

            _svm_voice[i].field_2A_bAutoPan = 0;
            _svm_voice[i].field_0x2c = 0;
            _svm_voice[i].field_0x2e = 0;
            _svm_voice[i].field_0x30 = 0;
            _svm_voice[i].field_0x32 = 0;
            _svm_voice[i].field_26_autoVol_Start = 0;
            SpuSetVoiceAttr(&voiceAttr);
            _svm_cur.field_18_voice_idx = i;
            _SsVmKeyOffNow();
        }

        _svm_rattr.mask = 0;
        _svm_rattr.depth.left = 0x3fff;
        _svm_rattr.depth.right = 0x3fff;
        _svm_rattr.mode = 0;

        _svm_okon1 = 0;
        _svm_okon2 = 0;
        _svm_okof1 = 0;
        _svm_orev1 = 0;
        _svm_orev2 = 0;
        _svm_onos1 = 0;
        _svm_onos2 = 0;

        _svm_auto_kof_mode = 0;

        _svm_stereo_mono = 0;
        _svm_vab_not_send_size = 0;

        kMaxPrograms = 128;

        _SsVmFlush();
    }

    int _SsVmVSetUp(short vabId, short program)
    {
        if (vabId < 16)
        {
            if (_svm_vab_used[vabId] != 1)
            {
                return -1;
            }

            if (program < kMaxPrograms)
            {
                _svm_cur.field_1_vabId = vabId;
                _svm_cur.field_6_program = program;
                _svm_cur.field_7_fake_program = _svm_vab_pg[vabId][program].reserved1; // fake program index
                _svm_tn = _svm_vab_tn[vabId];
                _svm_vh = _svm_vab_vh[vabId];
                _svm_pg = _svm_vab_pg[vabId];
                return 0;
            }
        }
        return -1;
    }

    short SsUtSetVagAtr(short vabId, short progNum, short toneNum, VagAtr *pVagAttr)
    {
        if (_svm_vab_used[vabId] == 1)
        {
            _SsVmVSetUp(vabId, progNum);
            printf("SsUtSetVagAtr expect crash\n");

            // 0x10 = sizeof(ProgAtr)
            // TODO FIX ME
            const int iVar2 = (toneNum + _svm_cur.field_7_fake_program * 0x10) << 16 >> 11;
            _svm_tn[iVar2].prior = pVagAttr->prior;
            _svm_tn[iVar2].mode = pVagAttr->mode;
            _svm_tn[iVar2].vol = pVagAttr->vol;
            _svm_tn[iVar2].pan = pVagAttr->pan;
            _svm_tn[iVar2].center = pVagAttr->center;
            _svm_tn[iVar2].shift = pVagAttr->shift;
            _svm_tn[iVar2].max = pVagAttr->max;
            _svm_tn[iVar2].min = pVagAttr->min;
            _svm_tn[iVar2].vibW = pVagAttr->vibW;
            _svm_tn[iVar2].vibT = pVagAttr->vibT;
            _svm_tn[iVar2].porW = pVagAttr->porW;
            _svm_tn[iVar2].porT = pVagAttr->porT;
            _svm_tn[iVar2].pbmin = pVagAttr->pbmin;
            _svm_tn[iVar2].pbmax = pVagAttr->pbmax;
            _svm_tn[iVar2].adsr1 = pVagAttr->adsr1;
            _svm_tn[iVar2].adsr2 = pVagAttr->adsr2;
            _svm_tn[iVar2].prog = pVagAttr->prog;
            _svm_tn[iVar2].vag = pVagAttr->vag;
            return 0;
        }
        return -1;
    }

    void SsUtGetVagAtr(short vabId, short progNum, short toneNum, VagAtr *pVagAttr)
    {
        if (_svm_vab_used[vabId] == 1)
        {
            _SsVmVSetUp(vabId, progNum);
            printf("SsUtGetVagAtr expect crash\n");
            // 0x10 = sizeof(ProgAtr)
            // TODO FIX ME
            const int iVar2 = (toneNum + _svm_cur.field_7_fake_program * 0x10) << 16 >> 11;
            pVagAttr->prior = _svm_tn[iVar2].prior;
            pVagAttr->mode = _svm_tn[iVar2].mode;
            pVagAttr->vol = _svm_tn[iVar2].vol;
            pVagAttr->pan = _svm_tn[iVar2].pan;
            pVagAttr->center = _svm_tn[iVar2].center;
            pVagAttr->shift = _svm_tn[iVar2].shift;
            pVagAttr->max = _svm_tn[iVar2].max;
            pVagAttr->min = _svm_tn[iVar2].min;
            pVagAttr->vibW = _svm_tn[iVar2].vibW;
            pVagAttr->vibT = _svm_tn[iVar2].vibT;
            pVagAttr->porW = _svm_tn[iVar2].porW;
            pVagAttr->porT = _svm_tn[iVar2].porT;
            pVagAttr->pbmin = _svm_tn[iVar2].pbmin;
            pVagAttr->pbmax = _svm_tn[iVar2].pbmax;
            pVagAttr->adsr1 = _svm_tn[iVar2].adsr1;
            pVagAttr->adsr2 = _svm_tn[iVar2].adsr2;
            pVagAttr->prog = _svm_tn[iVar2].prog;
            pVagAttr->vag = _svm_tn[iVar2].vag;
        }
    }
    void SsUtSetReverbDepth(short leftDepth, short rightDepth)
    {
        _svm_rattr.mask = SPU_REV_DEPTHL | SPU_REV_DEPTHR;
        _svm_rattr.depth.left = (leftDepth * 0x7fff) / 127;
        _svm_rattr.depth.right = (rightDepth * 0x7fff) / 127;
        SpuSetReverbModeParam(&_svm_rattr);
    }

    void SsUtSetReverbDelay(short delay)
    {
        _svm_rattr.delay = delay;
        _svm_rattr.mask = SPU_REV_DELAYTIME;
        SpuSetReverbModeParam(&_svm_rattr);
    }

    void SsUtSetReverbFeedback(short feedback)
    {
        _svm_rattr.feedback = feedback;
        _svm_rattr.mask = SPU_REV_FEEDBACK;
        SpuSetReverbModeParam(&_svm_rattr);
    }

    short SsUtSetReverbType(short type)
    {
        const bool bNegativeMode = type < 0;
        if (bNegativeMode)
        {
            type = -type;
        }

        if (type < 10)
        {
            _svm_rattr.mask = SPU_REV_MODE;
            int reverbMode = type;
            if (bNegativeMode)
            {
                reverbMode = type | SPU_REV_MODE_CLEAR_WA; // clear work area
            }

            _svm_rattr.mode = reverbMode;

            if (type == SPU_REV_MODE_OFF)
            {
                SpuSetReverb(0);
            }

            SpuSetReverbModeParam(&_svm_rattr);
        }
        else
        {
            type = -1;
        }
        return type;
    }

    short SsVabTransBody(unsigned char *pVbData, short vabId)
    {
        if (vabId < 16)
        {
            if (_svm_vab_used[vabId] == 2)
            {
                long spuUploadAddress = _svm_vab_start[vabId];

                // Use DMA
                SpuSetTransferMode(SPU_TRANSFER_BY_DMA);

                // Set DMA src
                spuUploadAddress = SpuSetTransferStartAddr(spuUploadAddress);
                if (spuUploadAddress != 0)
                {
                    // DMA from RAM to SPU
                    SpuWrite(pVbData, _svm_vab_total[vabId]);
                    _svm_vab_used[vabId] = 1;
                    return vabId;
                }
            }
        }
        _spu_setInTransfer(0);
        return -1;
    }

    typedef long (*VabAllocateCallBack)(unsigned int sizeInBytes, long mode, short vabId);

    // obj also needs SsVabOpenHeadSticky SsVabFakeHead but we seem to get away with it because those are never called ??
    int _SsVabOpenHeadWithMode(unsigned char *pAddr, int vabId, VabAllocateCallBack pFn, long mode)
    {
        int vagLens[256]; // max vags

        if (_spu_getInTransfer() == 1)
        {
            // Already uploading to SPU
            printf("Already xfter in progress\n");
            return -1;
        }

        _spu_setInTransfer(1);
        if (vabId < 16)
        {
            if (vabId == -1)
            {
                // Find a free vabId
                vabId = 16; // set to not found
                for (int i = 0; i < 16; i++)
                {
                    if (_svm_vab_used[i] == 0)
                    {
                        _svm_vab_used[i] = 1;
                        _svm_vab_count++;
                        vabId = i;
                        printf("alloc vabid %d\n", vabId);
                        break;
                    }
                }
            }
            else
            {
                // Use the given vabId
                if (_svm_vab_used[vabId] == 0)
                {
                    _svm_vab_used[vabId] = 1;
                    _svm_vab_count++;
                }
                else
                {
                    vabId = 0; // TODO: check
                }
            }

            if (vabId < 16)
            {
                VabHdr *pHeader = (VabHdr *)pAddr;

                _svm_vab_vh[vabId] = pHeader;
                _svm_vab_not_send_size = 0;
                if ((pHeader->form >> 8) == 0x564142) // check for VAB magic marker
                {
                    int maxPrograms = 64; // old versions had 64 progs only?
                    if ((pHeader->form & 0xff) == 'p' && pHeader->ver >= 4)
                    {
                        printf(">= v4 VAB\n");
                        maxPrograms = 128;
                    }
                    else
                    {
                        printf("Old style VAB\n");
                    }

                    int progCount = maxPrograms;
                    if (pHeader->ps > progCount)
                    {
                        // too many progs in header
                        printf("VAB header program count too high\n");
                        _svm_vab_used[vabId] = 0;
                    }
                    else
                    {
                        _svm_vab_pg[vabId] = AddBytes(ProgAtr, pAddr, sizeof(VabHdr));

                        _svm_vab_tn[vabId] = AddBytes(VagAtr, _svm_vab_pg[vabId], progCount * sizeof(ProgAtr)); // 128 program attributes

                        unsigned short *pVagOffTable = AddBytes(unsigned short, _svm_vab_tn[vabId], 16 * pHeader->ps * sizeof(VagAtr)); // 16 tones per program

                        unsigned int fakeProgIdx = 0;
                        ProgAtr *pProgTable = _svm_vab_pg[vabId];
                        for (int i = 0; i < progCount; i++)
                        {
                            pProgTable[i].reserved1 = fakeProgIdx;
                            if (pProgTable[i].tones != 0)
                            {
                                fakeProgIdx++;
                            }
                        }

                        unsigned int totalVagsSize = 0;
                        for (int i = 0; i < 256; i++)
                        {
                            if (i <= pHeader->vs) // vag count
                            {
                                if (pHeader->ver >= 4)
                                {
                                    vagLens[i] = pVagOffTable[i] << 3;
                                }
                                else
                                {
                                    vagLens[i] = pVagOffTable[i] << 2; // TODO: Why is this bit needed?
                                }
                                totalVagsSize += vagLens[i]; // calc total vag size
                            }
                        }

                        const unsigned int roundedSize = totalVagsSize + 63 & ~63; // Round to multiple of 64
                        const long spuAllocMem = (*pFn)(roundedSize, mode, vabId);

                        if (spuAllocMem == -1)
                        {
                            printf("Spu alloc failed size 0x%X id %d\n", roundedSize, vabId); // 0x1C5FC0
                            return -1;
                        }

                        if (spuAllocMem + roundedSize <= 0x80000) // less than spu ram size ??
                        {
                            _svm_vab_start[vabId] = spuAllocMem;
                            int vagIdx = 0;
                            int totalVagSize = 0;
                            pProgTable = _svm_vab_pg[vabId];

                            do
                            {
                                totalVagSize += vagLens[vagIdx]; // incremental total
                                // 256 vags, but 128 progs, so alternate the storage
                                if ((vagIdx & 1) == 0)
                                {
                                    pProgTable[vagIdx / 2].reserved2 = (spuAllocMem + totalVagSize) >> 3;
                                }
                                else
                                {
                                    pProgTable[vagIdx / 2].reserved3 = (spuAllocMem + totalVagSize) >> 3;
                                }

                                vagIdx++; // vag counter
                            } while (vagIdx <= pHeader->vs);

                            _svm_vab_total[vabId] = totalVagSize;
                            _svm_vab_used[vabId] = 2;
                            printf("OK!\n");
                            return vabId;
                        }

                        printf("Bigger than spu ram\n");
                        _svm_vab_used[vabId] = 0;
                    }
                }
                else
                {
                    // bad file magic
                    printf("Bad file magic\n");
                    _svm_vab_used[vabId] = 0;
                }

                _spu_setInTransfer(0);
                _svm_vab_count--;
                return -1;
            }
        }
        _spu_setInTransfer(0);
        return -1;
    }

    static long _SsVabSpuMallocLoader(unsigned int sizeInBytes, long /*mode*/, short vabId)
    {
        long pSpuMem = SpuMalloc(sizeInBytes);
        if (pSpuMem == -1)
        {
            _svm_vab_used[vabId] = 0;
            _spu_setInTransfer(0);
            _svm_vab_count--;
        }
        return pSpuMem;
    }

    short SsVabOpenHead(unsigned char *addr, short vabId)
    {
        return _SsVabOpenHeadWithMode(addr, vabId, _SsVabSpuMallocLoader, 0);
    }

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
            _svm_pg[program].mvol = vol;
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
        SeqStruct *pStruc = &_ss_score[seqNum][sepNum]; // note: 14bit access
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

    void _SsSetNrpnVabAttr15(short /*vab_id*/, short /*prog_no*/, short /*tone_no*/, VagAtr /*vagAtr*/, short /*fn_idx*/, unsigned char attr)
    {
        SsUtSetReverbType(attr);
    }

    void _SsSetNrpnVabAttr16(short /*vab_id*/, short /*prog_no*/, short /*tone_no*/, VagAtr /*vagAtr*/, short /*fn_idx*/, unsigned char attr)
    {
        SsUtSetReverbDepth(attr, attr);
    }

    void _SsSetNrpnVabAttr17(short /*vab_id*/, short /*prog_no*/, short /*tone_no*/, VagAtr /*vagAtr*/, short /*fn_idx*/, unsigned char attr)
    {
        SsUtSetReverbFeedback(attr);
    }

    void _SsSetNrpnVabAttr18(short /*vab_id*/, short /*prog_no*/, short /*tone_no*/, VagAtr /*vagAtr*/, short /*fn_idx*/, unsigned char attr)
    {
        SsUtSetReverbDelay(attr);
    }

    void _SsSetNrpnVabAttr19(short /*vab_id*/, short /*prog_no*/, short /*tone_no*/, VagAtr /*vagAtr*/, short /*fn_idx*/, unsigned char attr)
    {
        SsUtSetReverbDelay(attr);
    }

    //void _SsInit(void) VOID_STUB // TODO: Impl can't link due to redef of global vars
    // TODO: Can't link as obj has some globals in there
    void _SsInit(void)
    {
        // Init regs
        static const short private_8001D0A4[16] = { 0x3FFF, 0x3FFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
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

    // TODO: SpuClearReverbWorkArea

    void SsInit(void)
    {
        ResetCallback();
        SpuInit();
        SpuClearReverbWorkArea(SS_REV_TYPE_ECHO); // 7
        _SsInit();
        debug_dump_ssinit();
    }

    // TODO
    void SsStart(void)
    {
        // Works as a stub, not sure why - emulator issue?
    }

    // TODO
    void SsSetTickMode(long /*tick_mode*/)
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
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];
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

    void _SsGetMetaEvent(short seq_no, short sep_no, unsigned char ev)
    {
        // This could be any meta event bar EOF/0x2F, yet code treats it as if it must always be
        // a tempo change of 0x51 ??
        printf("_SsGetMetaEvent %d\n", (int)ev);

        SeqStruct *pStru = &_ss_score[seq_no][sep_no];

        const int tempoValue = pStru->field_0_seq_ptr[2] | (pStru->field_0_seq_ptr[0] << 16) | (pStru->field_0_seq_ptr[1] << 8);
        pStru->field_0_seq_ptr += 3;

        const int v7 = 60000000 / tempoValue; // TODO: Negative constant ?
        const int uVar2 = pStru->field_50_res_of_quarter_note * v7;

        pStru->field_94 = v7;

        const int v9 = 15 * VBLANK_MINUS;
        const int v10 = 60 * VBLANK_MINUS;
        if ((uVar2 * 10) < v10)
        {
            pStru->field_52 = (600 * VBLANK_MINUS) / uVar2;
            pStru->field_54 = (600 * VBLANK_MINUS) / uVar2;
        }
        else
        {
            const int v12 = (10 * pStru->field_50_res_of_quarter_note * pStru->field_94) / v10;
            const int v13 = (10 * pStru->field_50_res_of_quarter_note * pStru->field_94) % v10;

            pStru->field_52 = -1;
            pStru->field_54 = v12;

            if (v9 * 2 < v13)
            {
                pStru->field_54++;
            }
        }
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsSetPitchBend(short seq_no, short sep_no)
    {
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];
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
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];
        pStru->field_26_vab_id = *pStru->field_0_seq_ptr;
        pStru->field_0_seq_ptr++;
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContMainVol(short seq_no, short sep_no, unsigned char vol)
    {
        SeqStruct *pStruc = &_ss_score[seq_no][sep_no];
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
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];
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
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];
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
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];
        SsUtSetReverbDepth(reverbDepth, reverbDepth);
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContDamper(short seq_no, short sep_no, unsigned char damper)
    {
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];
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
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];

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
            pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
            pStru->field_8 = pStru->field_0_seq_ptr;
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

        if (pStru->field_1D == 0)
        {
            pStru->field_15 = 0;
            pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
            return;
        }

        if (pStru->field_1D >= 127u)
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
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];
        pStru->field_18 = a3;
        pStru->field_1E++;
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContRpn2(short seq_no, short sep_no, unsigned char a3)
    {
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];
        pStru->field_19 = a3;
        pStru->field_1E++;
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsContResetAll(short seq_no, short sep_no)
    {
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];

        SsUtReverbOff();
        _SsVmDamperOff();

        pStru->field_37_programs[pStru->field_17_channel_idx] = pStru->field_17_channel_idx;
        pStru->field_18 = 0;
        pStru->field_19 = 0;
        pStru->field_60_vol[pStru->field_17_channel_idx] = 127;
        pStru->field_27_panpot[pStru->field_17_channel_idx] = 64;
        pStru->field_90_delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }

    void _SsSetControlChange(short seq_no, short sep_no, unsigned char control)
    {
        SeqStruct *pStru = &_ss_score[seq_no][sep_no];
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
        const int seqInit = _SsInitSoundSeq(openSeqId, vab_id, (unsigned char*)pSeqData);

        SsFCALL.noteon = _SsNoteOn;
        SsFCALL.programchange = _SsSetProgramChange;
        SsFCALL.metaevent = _SsGetMetaEvent;
        SsFCALL.pitchbend = _SsSetPitchBend;

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
        SsFCALL.ccentry[DE_LIMITH] = _SsSetNrpnVabAttr3;
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
        SeqStruct *pStru = &_ss_score[seq_sep_num][idx]; // TODO: 14bit access

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
            _svm_cur.field_18_voice_idx = voice;
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
        SeqStruct *pStru = &_ss_score[seq_no][sep_no]; // TODO: 14bit access

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
        SeqStruct *pStru = &_ss_score[seq_no][sep_no]; // TODO: 14bit access

        _SsVmSeqKeyOff(seq_no | (sep_no << 8));

        pStru->field_14_play_mode = 0;
        pStru->field_98_flags &= ~2u;
    }

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
        // TODO: flag 0x400 is never set, only cleared ??
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
                unsigned char program = *pSeqPtr->field_0_seq_ptr;
                pSeqPtr->field_0_seq_ptr++;
                SsFCALL.programchange(seq_idx, sep_idx, program);
                //_SsSetProgramChange(seq_idx, sep_idx, midi_byte);
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
                    SsFCALL.noteon(seq_idx, sep_idx, midi_byte_next_, midi_byte_);
                    //_SsNoteOn(seq_idx, sep_idx, midi_byte_next_, midi_byte_);
                }
                else if (midi_byte_and_F0 == 0xB0)
                {
                    pSeqPtr->field_16_running_status = 0xB0;
                    unsigned char controlChange = *pSeqPtr->field_0_seq_ptr;
                    pSeqPtr->field_0_seq_ptr++;
                    SsFCALL.control[CC_NUMBER](seq_idx, sep_idx, controlChange);
                    //_SsSetControlChange(seq_idx, sep_idx, midi_byte);
                }
                return;
            }

            if (midi_byte_and_F0 == 0xE0) // pitch bend
            {
                pSeqPtr->field_16_running_status = 0xE0;
                pSeqPtr->field_0_seq_ptr++;
                SsFCALL.pitchbend(seq_idx, sep_idx);
                //_SsSetPitchBend(seq_idx, sep_idx);
                return;
            }
            else
            {
                if (midi_byte_and_F0 == 0xF0) // meta
                {
                    pSeqPtr->field_16_running_status = 0xFF; // TODO ?? -1;
                    const unsigned char midi_byte__ = *pSeqPtr->field_0_seq_ptr;
                    pSeqPtr->field_0_seq_ptr++;

                    if (midi_byte__ == 0x2F)
                    {
                        _SsSeqGetEof(seq_idx, sep_idx);
                        return;
                    }

                    SsFCALL.metaevent(seq_idx, sep_idx, midi_byte);
                    //_SsGetMetaEvent(seq_idx, sep_idx, midi_byte);
                }
                return;
            }
        }

        const unsigned char running_status = pSeqPtr->field_16_running_status;
        if (running_status == 0xC0)
        {
            SsFCALL.programchange(seq_idx, sep_idx, midi_byte);
            //_SsSetProgramChange(seq_idx, sep_idx, midi_byte); // short seq_no, short sep_no, unsigned char ch
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

                SsFCALL.metaevent(seq_idx, sep_idx, midi_byte);
                //_SsGetMetaEvent(seq_idx, sep_idx, midi_byte);
                return;
            }

            SsFCALL.pitchbend(seq_idx, sep_idx);
            //_SsSetPitchBend(seq_idx, sep_idx);
            return;
        }

        if (running_status == 0x90)
        {
            unsigned char next_midi_byte__ = *pSeqPtr->field_0_seq_ptr++;
            pSeqPtr->field_90_delta_value = _SsReadDeltaValue(seq_idx, sep_idx);
            SsFCALL.noteon(seq_idx, sep_idx, midi_byte, next_midi_byte__);
            //_SsNoteOn(seq_idx, sep_idx, midi_byte, next_midi_byte__);
        }
        else if (running_status == 0xB0)
        {
            SsFCALL.control[CC_NUMBER](seq_idx, sep_idx, midi_byte);
            //_SsSetControlChange(seq_idx, sep_idx, midi_byte);
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

    void _SsSndPlay(short seq_access_num, short seq_num)
    {
        _SsSeqPlay(seq_access_num, seq_num);
    }

    void _SsSndStop(short seq_or_sep_access_num, short seq_num)
    {
        SeqStruct *pStru = &_ss_score[seq_or_sep_access_num][seq_num]; // TODO: 14bit access
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
        pStru->field_94 = pStru->field_8C_tempo;
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

    short _SsVmSelectToneAndVag(unsigned char *pVagAttrIdx, unsigned char *pVagNums)
    {
        short idx = 0;
        for (unsigned int i = 0; i < _svm_cur.field_0_sep_sep_no_tonecount; i++)
        {
            VagAtr *pVagAttr = &_svm_tn[(_svm_cur.field_7_fake_program * 16) + i];
            // Get all the vags that are within this note range
            if (pVagAttr->min <= _svm_cur.field_2_note && _svm_cur.field_2_note <= pVagAttr->max)
            {
                pVagNums[idx] = pVagAttr->vag;
                pVagAttrIdx[idx] = i;
                idx++;
            }
        }
        return idx;
    }

    void _SsVmDoAllocate(void)
    {
        _svm_voice[_svm_cur.field_18_voice_idx].field_6_keyStat = 0x7fff;

        for (int i = 0; i < 16; i++)
        {
            _svm_envx_hist[i] &= ~(1 << (_svm_cur.field_18_voice_idx & 31));
        }

        unsigned short vag_spu_addr = 0;
        if ((_svm_cur.field_16_vag_idx & 1) == 0)
        {
            vag_spu_addr = _svm_pg[(_svm_cur.field_16_vag_idx + -1) / 2].reserved3;
        }
        else
        {
            vag_spu_addr = _svm_pg[(_svm_cur.field_16_vag_idx + -1) / 2].reserved2;
        }

        _svm_sreg_buf[_svm_cur.field_18_voice_idx].field_6_vagAddr = vag_spu_addr;
        _svm_sreg_dirty[_svm_cur.field_18_voice_idx] |= 8;

        VagAtr *pVag = _svm_tn + (_svm_cur.field_7_fake_program * 16) + _svm_cur.field_C_vag_idx;
        _svm_sreg_buf[_svm_cur.field_18_voice_idx].field_8_adsr1 = pVag->adsr1;
        unsigned short damper = _svm_damper + (pVag->adsr2 & 0x1f);
        if (31 < damper)
        {
            damper = 31;
        }
        _svm_sreg_buf[_svm_cur.field_18_voice_idx].field_A_adsr2 = damper | pVag->adsr2 & 0xffe0;
        _svm_sreg_dirty[_svm_cur.field_18_voice_idx] |= 0x30;
    }

    void _SsVmKeyOff(int seq_sep_no, short vabId, short program, short note)
    {
        for (int i = 0; i < _SsVmMaxVoice; i++)
        {
            if ((_snd_vmask & 1) << (i & 31) == 0 &&
                 _svm_voice[i].field_E_note == note &&
                 _svm_voice[i].field_14_program == program &&
                 _svm_voice[i].field_10_seq_sep_no == seq_sep_no &&
                 _svm_voice[i].field_18_vabId == vabId)
            {
                if (_svm_voice[i].field_0_vag_idx == 0xff)
                {
                    vmNoiseOff(i);
                }
                else
                {
                    _svm_cur.field_18_voice_idx = i;
                    _SsVmKeyOffNow();
                }
            }
        }
    }

    void _SsVmSeKeyOff(short vabId, short program, short note)
    {
        _SsVmKeyOff(0x21, vabId, program, note);
    }

    int _SsVmKeyOn(int seq_sep_no, short vabId, short program, short note, short voll, short unknown27)
    {
        SeqStruct *pSeq = &_ss_score[seq_sep_no & 0xff][(seq_sep_no & 0xff00) >> 8];
        int onKeysMask = -1;

        if (_SsVmVSetUp(vabId, program) == 0)
        {
            _svm_cur.field_2_note = note;
            _svm_cur.field_0x3 = 0;

            if (seq_sep_no == 0x21)
            {
                _svm_cur.field_4_voll = voll;
            }
            else
            {
                _svm_cur.field_4_voll = (voll * pSeq->field_60_vol[pSeq->field_17_channel_idx]) / 127;
            }

            ProgAtr *pProgram = _svm_pg + program;
            _svm_cur.field_0x5 = unknown27;
            _svm_cur.field_A_mvol = pProgram->mvol;
            _svm_cur.field_B_mpan = pProgram->mpan;
            _svm_cur.field_0_sep_sep_no_tonecount = pProgram->tones;
            _svm_cur.field_14_seq_sep_no = seq_sep_no;

            if (_svm_cur.field_7_fake_program < _svm_vh->ps)
            {
                if (voll == 0)
                {
                    _SsVmKeyOff(seq_sep_no, vabId, program, note);
                }
                else
                {
                    // Get all the VAGs for this key
                    unsigned char vagIndexNums[256];
                    unsigned char vagNums[256];
                    const int selectedVagCount = _SsVmSelectToneAndVag(vagIndexNums, vagNums);

                    onKeysMask = 0;

                    for (int i = 0; i < selectedVagCount; i++)
                    {
                        _svm_cur.field_16_vag_idx = vagNums[i];
                        _svm_cur.field_C_vag_idx = vagIndexNums[i];
                        VagAtr *pVag = _svm_tn + (_svm_cur.field_C_vag_idx + (_svm_cur.field_7_fake_program * 16));

                        _svm_cur.field_F_prior = pVag->prior;
                        _svm_cur.field_D_vol = pVag->vol;
                        _svm_cur.field_E_pan = pVag->pan;
                        _svm_cur.field_10_centre = pVag->center;
                        _svm_cur.field_11_shift = pVag->shift;
                        _svm_cur.field_12_mode = pVag->mode;

                        // Allocate a voice
                        _svm_cur.field_18_voice_idx = _SsVmAlloc();

                        if (_svm_cur.field_18_voice_idx < _SsVmMaxVoice)
                        {
                            _svm_voice[_svm_cur.field_18_voice_idx].field_0x1d = 1;
                            _svm_voice[_svm_cur.field_18_voice_idx].field_0x2 = 0;
                            _svm_voice[_svm_cur.field_18_voice_idx].field_10_seq_sep_no = seq_sep_no;
                            _svm_voice[_svm_cur.field_18_voice_idx].field_18_vabId = _svm_cur.field_1_vabId;
                            _svm_voice[_svm_cur.field_18_voice_idx].field_12_fake_program = _svm_cur.field_7_fake_program;
                            _svm_voice[_svm_cur.field_18_voice_idx].field_14_program = program;

                            if (seq_sep_no != 0x21)
                            {
                                _svm_voice[_svm_cur.field_18_voice_idx].field_8_voll = voll;
                                _svm_voice[_svm_cur.field_18_voice_idx].field_C_channel_idx = pSeq->field_17_channel_idx;
                            }

                            _svm_voice[_svm_cur.field_18_voice_idx].field_0xa = unknown27;
                            _svm_voice[_svm_cur.field_18_voice_idx].field_36_voll = _svm_cur.field_4_voll;
                            _svm_voice[_svm_cur.field_18_voice_idx].field_16_vag_num = _svm_cur.field_C_vag_idx;
                            _svm_voice[_svm_cur.field_18_voice_idx].field_E_note = note;
                            _svm_voice[_svm_cur.field_18_voice_idx].field_1A_priority = _svm_cur.field_F_prior;
                            _svm_voice[_svm_cur.field_18_voice_idx].field_0_vag_idx = _svm_cur.field_16_vag_idx;

                            _SsVmDoAllocate();

                            // Key the allocated voice on
                            if (_svm_cur.field_16_vag_idx == 0xff)
                            {
                                vmNoiseOn(_svm_cur.field_18_voice_idx);
                            }
                            else
                            {
                                _SsVmKeyOnNow(selectedVagCount, note2pitch());
                            }
                            onKeysMask |= 1 << (_svm_cur.field_18_voice_idx & 31);
                        }
                        else
                        {
                            onKeysMask = -1;
                        }
                    }
                }
            }
        }
        return onKeysMask;
    }

    long _SsVmSeKeyOn(unsigned char vabId, unsigned char program, unsigned char note, unsigned char /*pitch*/, unsigned short voll, unsigned short param_6)
    {
        char unknown27;
        unsigned int volr_;
        unsigned int voll_;

        voll_ = voll;
        volr_ = param_6;
        if (voll_ == volr_)
        {
            unknown27 = 64;
        }
        else
        {
            if (volr_ < voll_)
            {
                unknown27 = (char)((volr_ << 6) / voll_);
            }
            else
            {
                unknown27 = 127 - (char)((voll_ << 6) / volr_);
                voll = param_6;
            }
        }
        return _SsVmKeyOn(0x21, vabId, program, note, voll, unknown27);
    }

} // extern "C"

static void PcsxReduxExit(short exitCode)
{
    printf("Exiting with code %d\n", exitCode);
    (*(volatile unsigned short *)0x1f802082) = exitCode;
}

#define ASSERT_EQ(value1, value2)                                                           \
    if ((value1) != (value2))                                                               \
    {                                                                                       \
        printf("%d != %d in %s %s:%d\n", value1, value2, __FUNCTION__, __FILE__, __LINE__); \
        PcsxReduxExit(1);                                                                   \
    }

static void Test_SsVmSelectToneAndVag()
{
    // Set up
    _svm_cur.field_0_sep_sep_no_tonecount = 5;
    _svm_cur.field_7_fake_program = 3;
    _svm_cur.field_2_note = 4;

    VagAtr testVags[(3 * 16) + 16] = {};
    testVags[(3 * 16) + 0].min = 4;
    testVags[(3 * 16) + 0].max = 4;
    testVags[(3 * 16) + 0].vag = 55;

    testVags[(3 * 16) + 3].min = 5;
    testVags[(3 * 16) + 3].max = 6;
    testVags[(3 * 16) + 3].vag = 11;

    testVags[(3 * 16) + 3].min = 2;
    testVags[(3 * 16) + 3].max = 3;
    testVags[(3 * 16) + 3].vag = 33;

    testVags[(3 * 16) + 4].min = 4;
    testVags[(3 * 16) + 4].max = 5;
    testVags[(3 * 16) + 4].vag = 22;

    _svm_tn = testVags;

    unsigned char vags[256] = {};
    unsigned char vagNums[256] = {};
    const short ret = _SsVmSelectToneAndVag(vags, vagNums);

    ASSERT_EQ(ret, 2);

    ASSERT_EQ(0, vags[0]);
    ASSERT_EQ(4, vags[1]);

    ASSERT_EQ(55, vagNums[0]);
    ASSERT_EQ(22, vagNums[1]);
}

static void Test_SsVmDoAllocate()
{
    for (int i = 0; i < 24; i++)
    {
        SpuVoice blank = {};
        _svm_voice[i] = blank;
    }

    _svm_cur.field_18_voice_idx = 3;
    _svm_cur.field_C_vag_idx = 0;
    _svm_cur.field_7_fake_program = 0;

    for (int i = 0; i < 16; i++)
    {
        _svm_envx_hist[i] = 0xFFFFFFFF;
    }

    ProgAtr progs[16] = {};
    _svm_pg = progs;
    _svm_pg[(_svm_cur.field_16_vag_idx - 1) / 2].reserved3 = 1234;
    for (int i = 0; i < 24; i++)
    {
        _svm_sreg_dirty[i] = 0;
    }

    VagAtr vags[16 * 5] = {};
    _svm_tn = vags;
    VagAtr *pVag = _svm_tn + (_svm_cur.field_7_fake_program * 16) + _svm_cur.field_C_vag_idx;
    pVag->adsr1 = 4455;
    pVag->adsr2 = 0xFFe1;

    _svm_damper = 31;

    _SsVmDoAllocate();

    ASSERT_EQ(_svm_voice[_svm_cur.field_18_voice_idx].field_6_keyStat, 0x7fff);
    for (int i = 0; i < 16; i++)
    {
        ASSERT_EQ(_svm_envx_hist[i], 0xFFFFFFF7); // check voice bit is cleared
    }

    ASSERT_EQ(_svm_sreg_buf[_svm_cur.field_18_voice_idx].field_6_vagAddr, 1234); // check vagAddr is copied as is
    ASSERT_EQ(_svm_sreg_buf[_svm_cur.field_18_voice_idx].field_8_adsr1, 4455);   // check adsr1 is copied as is
    ASSERT_EQ(_svm_sreg_buf[_svm_cur.field_18_voice_idx].field_A_adsr2, 0xFFFF);

    ASSERT_EQ(_svm_sreg_dirty[_svm_cur.field_18_voice_idx], 0x30 | 0x8); // check correct regs are set
}

struct MidiTest
{
    int mNoteOnCalls;
    int mNoteOnNote;
    int mNoteOnVol;

    int mProgramChangeCalls;
    int mProgramChangeValue;

    int mControlChangeCalls;
    int mControlChangeValue;

    int mPitchBendCalls;
};
static MidiTest gMidiTest = {};

static void Test_SsNoteOn(short seq_no, short sep_no, unsigned char note, unsigned char voll)
{
    gMidiTest.mNoteOnCalls++;
    gMidiTest.mNoteOnNote = note;
    gMidiTest.mNoteOnVol = voll;
}

static void Test_SsSetProgramChange(short seq_no, short sep_no, unsigned char programNum)
{
    gMidiTest.mProgramChangeCalls++;
    gMidiTest.mProgramChangeValue = programNum;
}

static void Test_ControlChange(short seq_no, short sep_no, unsigned char change)
{
    gMidiTest.mControlChangeCalls++;
    gMidiTest.mControlChangeValue = change;
}

static void Test_SsSetPitchBend(short seq_no, short sep_no)
{
    gMidiTest.mPitchBendCalls++;
}

static void ClearTestData()
{
    MidiTest cleared = {};
    gMidiTest = cleared;
}

static int DvConv(int dv)
{
    return 2 * ((dv*4) + dv);
}

static void Test_SsGetSeqData()
{
    SsFCALL.noteon = Test_SsNoteOn;
    SsFCALL.programchange = Test_SsSetProgramChange;
    SsFCALL.control[0] = Test_ControlChange;
    SsFCALL.pitchbend = Test_SsSetPitchBend;

    // Note off test
    {
        ClearTestData();

        SeqStruct seq = {};
        _ss_score[0] = &seq;

        unsigned char midiData[] = 
        {
            0x95, // Note on, channel 5
            0xA, // Note
            0x0, // Velocity (off)
            0x2, // Delta value
        };
        seq.field_0_seq_ptr = midiData;

        _SsGetSeqData(0, 0);

        ASSERT_EQ(seq.field_17_channel_idx, 5);
        ASSERT_EQ(gMidiTest.mNoteOnCalls, 1);
        ASSERT_EQ(gMidiTest.mNoteOnNote, 0xA);
        ASSERT_EQ(gMidiTest.mNoteOnVol, 0x0);
        ASSERT_EQ(seq.field_16_running_status, 0x90);
        ASSERT_EQ(seq.field_90_delta_value, DvConv(0x2));
    }

    // Note off test running status
    {
        ClearTestData();

        SeqStruct seq = {};
        _ss_score[0] = &seq;
        seq.field_16_running_status = 0x90;
        seq.field_17_channel_idx = 5;
        unsigned char midiData[] = 
        {
            0xA, // Note
            0x0, // Velocity (off)
            0x2, // Delta value
        };
        seq.field_0_seq_ptr = midiData;

        _SsGetSeqData(0, 0);

        ASSERT_EQ(seq.field_17_channel_idx, 5);
        ASSERT_EQ(gMidiTest.mNoteOnCalls, 1);
        ASSERT_EQ(gMidiTest.mNoteOnNote, 0xA);
        ASSERT_EQ(gMidiTest.mNoteOnVol, 0x0);
        ASSERT_EQ(seq.field_16_running_status, 0x90);
        ASSERT_EQ(seq.field_90_delta_value, DvConv(0x2));
    }

    // Note on test
    {
        ClearTestData();

        SeqStruct seq = {};
        _ss_score[0] = &seq;
        seq.field_16_running_status = 0x90;
        seq.field_17_channel_idx = 5;

        unsigned char midiData[] = 
        {
            0xA, // Note
            0x22, // Velocity (on)
            0x2, // Delta value
        };
        seq.field_0_seq_ptr = midiData;

        _SsGetSeqData(0, 0);

        ASSERT_EQ(seq.field_17_channel_idx, 5);
        ASSERT_EQ(gMidiTest.mNoteOnCalls, 1);
        ASSERT_EQ(gMidiTest.mNoteOnNote, 0xA);
        ASSERT_EQ(gMidiTest.mNoteOnVol, 0x22);
        ASSERT_EQ(seq.field_16_running_status, 0x90);
        ASSERT_EQ(seq.field_90_delta_value, DvConv(0x2));
    }

    // Running status Note on test
    {
        ClearTestData();

        SeqStruct seq = {};
        _ss_score[0] = &seq;

        unsigned char midiData[] = 
        {
            0x95, // Note on, channel 5
            0xA, // Note
            0x22, // Velocity (on)
            0x2, // Delta value
        };
        seq.field_0_seq_ptr = midiData;

        _SsGetSeqData(0, 0);

        ASSERT_EQ(seq.field_17_channel_idx, 5);
        ASSERT_EQ(gMidiTest.mNoteOnCalls, 1);
        ASSERT_EQ(gMidiTest.mNoteOnNote, 0xA);
        ASSERT_EQ(gMidiTest.mNoteOnVol, 0x22);
        ASSERT_EQ(seq.field_16_running_status, 0x90);
        ASSERT_EQ(seq.field_90_delta_value, DvConv(0x2));
    }

    // Program change test
    {
        ClearTestData();

        SeqStruct seq = {};
        _ss_score[0] = &seq;

        unsigned char midiData[] = 
        {
            0xC5, // Program change channel 5
            0xA, // Change
        };
        seq.field_0_seq_ptr = midiData;

        _SsGetSeqData(0, 0);

        ASSERT_EQ(seq.field_17_channel_idx, 5);
        ASSERT_EQ(gMidiTest.mProgramChangeCalls, 1);
        ASSERT_EQ(gMidiTest.mProgramChangeValue, 0xA);
        ASSERT_EQ(seq.field_16_running_status, 0xC0);
        ASSERT_EQ(seq.field_90_delta_value, DvConv(0));
    }

    // Running status program change test
    {
        ClearTestData();

        SeqStruct seq = {};
        _ss_score[0] = &seq;
        seq.field_16_running_status = 0xC0;
        seq.field_17_channel_idx = 5;

        unsigned char midiData[] = 
        {
            0xA, // Change
        };
        seq.field_0_seq_ptr = midiData;

        _SsGetSeqData(0, 0);

        ASSERT_EQ(seq.field_17_channel_idx, 5);
        ASSERT_EQ(gMidiTest.mProgramChangeCalls, 1);
        ASSERT_EQ(gMidiTest.mProgramChangeValue, 0xA);
        ASSERT_EQ(seq.field_16_running_status, 0xC0);
        ASSERT_EQ(seq.field_90_delta_value, DvConv(0));
    }

    // Control change test
    {
        ClearTestData();

        SeqStruct seq = {};
        _ss_score[0] = &seq;

        unsigned char midiData[] = 
        {
            0xB5, // Control change channel 5
            0xA, // Change
        };
        seq.field_0_seq_ptr = midiData;

        _SsGetSeqData(0, 0);

        ASSERT_EQ(seq.field_17_channel_idx, 5);
        ASSERT_EQ(gMidiTest.mControlChangeCalls, 1);
        ASSERT_EQ(gMidiTest.mControlChangeValue, 0xA);
        ASSERT_EQ(seq.field_16_running_status, 0xB0);
        ASSERT_EQ(seq.field_90_delta_value, DvConv(0));
    }

    // Running status Control change test
    {
        ClearTestData();

        SeqStruct seq = {};
        _ss_score[0] = &seq;
        seq.field_16_running_status = 0xB0;
        seq.field_17_channel_idx = 5;

        unsigned char midiData[] = 
        {
            0xA, // Change
        };
        seq.field_0_seq_ptr = midiData;

        _SsGetSeqData(0, 0);

        ASSERT_EQ(seq.field_17_channel_idx, 5);
        ASSERT_EQ(gMidiTest.mControlChangeCalls, 1);
        ASSERT_EQ(gMidiTest.mControlChangeValue, 0xA);
        ASSERT_EQ(seq.field_16_running_status, 0xB0);
        ASSERT_EQ(seq.field_90_delta_value, DvConv(0));
    }

    // TODO: Meta event test
    {

    }

    // TODO: Running status Meta event test
    {

    }

    // TODO: EOF meta event test
    {

    }

    // TODO: Running status EOF meta event test
    {

    }

    // Pitch bend test
    {
        ClearTestData();

        SeqStruct seq = {};
        _ss_score[0] = &seq;

        unsigned char midiData[] = 
        {
            0xE5, // Pitch bend channel 5
            // Bend value read in a helper
        };
        seq.field_0_seq_ptr = midiData;

        _SsGetSeqData(0, 0);

        ASSERT_EQ(seq.field_17_channel_idx, 5);
        ASSERT_EQ(gMidiTest.mPitchBendCalls, 1);
        ASSERT_EQ(seq.field_16_running_status, 0xE0);
        ASSERT_EQ(seq.field_90_delta_value, DvConv(0));
    }

    // Running status Pitch bend test
    {
        ClearTestData();

        SeqStruct seq = {};
        _ss_score[0] = &seq;
        seq.field_16_running_status = 0xE0;
        seq.field_17_channel_idx = 5;

        unsigned char midiData[] = 
        {
            0x0, // Nothing
        };
        seq.field_0_seq_ptr = midiData;

        _SsGetSeqData(0, 0);

        ASSERT_EQ(seq.field_17_channel_idx, 5);
        ASSERT_EQ(gMidiTest.mPitchBendCalls, 1);
        ASSERT_EQ(seq.field_16_running_status, 0xE0);
        ASSERT_EQ(seq.field_90_delta_value, DvConv(0));
    }

    // TODO: EOF test
    {

    }
}

void DoTests()
{
    printf("Tests start\n");
    Test_SsVmSelectToneAndVag();
    Test_SsVmDoAllocate();
    Test_SsGetSeqData();
    printf("Tests end\n");
    PcsxReduxExit(0);
}
