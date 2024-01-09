#ifndef _LIB_SND_HPP_
#define _LIB_SND_HPP_

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif
    #define SS_WAIT_COMPLETED   1

    #define SS_REV_TYPE_OFF        0
    #define SS_REV_TYPE_ROOM       1
    #define SS_REV_TYPE_STUDIO_A   2
    #define SS_REV_TYPE_STUDIO_B   3
    #define SS_REV_TYPE_STUDIO_C   4
    #define SS_REV_TYPE_HALL       5
    #define SS_REV_TYPE_SPACE      6
    #define SS_REV_TYPE_ECHO       7
    #define SS_REV_TYPE_DELAY      8
    #define SS_REV_TYPE_PIPE       9

    #define CC_NUMBER     0
    #define CC_BANKCHANGE 1
    #define CC_DATAENTRY  2
    #define CC_MAINVOL    3
    #define CC_PANPOT     4
    #define CC_EXPRESSION 5
    #define CC_DAMPER     6
    #define CC_NRPN1      7
    #define CC_NRPN2      8
    #define CC_RPN1       9
    #define CC_RPN2       10
    #define CC_EXTERNAL   11
    #define CC_RESETALL   12

    #define DE_PRIORITY	0	 
    #define DE_MODE  	1 
    #define DE_LIMITL	2
    #define DE_LIMITH	3 
    #define DE_ADSR_AR_L	4 
    #define DE_ADSR_AR_E	5
    #define DE_ADSR_DR  	6 
    #define DE_ADSR_SL  	7 
    #define DE_ADSR_SR_L  	8 
    #define DE_ADSR_SR_E 	9 
    #define DE_ADSR_RR_L	10 
    #define DE_ADSR_RR_E	11
    #define DE_ADSR_SR  	12 
    #define DE_VIB_TIME 	13 
    #define DE_PORTA_DEPTH	14 
    #define DE_REV_TYPE  	15 
    #define DE_REV_DEPTH 	16 
    #define DE_ECHO_FB  	17 
    #define DE_ECHO_DELAY	18 
    #define DE_DELAY  	19 

    #define SS_SEQ_TABSIZ     176

    #define SS_NOTICK	 0x1000

    #define SSPLAY_PLAY      1

    typedef struct VagAtr {         /* VAG Tone Headdings */

        unsigned char  prior;         /* tone priority */
        unsigned char  mode;          /* play mode */
        unsigned char  vol;           /* tone volume*/
        unsigned char  pan;           /* tone panning */
        unsigned char  center;        /* center note */
        unsigned char  shift;         /* center note fine tune */
        unsigned char  min;           /* minimam note limit */
        unsigned char  max;           /* maximam note limit */
        unsigned char  vibW;          /* vibrate depth */
        unsigned char  vibT;          /* vibrate duration */
        unsigned char  porW;          /* portamento depth */
        unsigned char  porT;          /* portamento duration */
        unsigned char  pbmin;         /* under pitch bend max */
        unsigned char  pbmax;         /* upper pitch bend max */
        unsigned char  reserved1;     /* system reserved */
        unsigned char  reserved2;     /* system reserved */
        unsigned short adsr1;         /* adsr1 */
        unsigned short adsr2;         /* adsr2 */
        short          prog;          /* parent program*/
        short          vag;           /* vag reference */
        short          reserved[4];   /* system reserved */

    } VagAtr;			/* 32 byte */

    typedef struct VabHdr {         /* VAB Bank Headdings */

        s32 form;          /* always 'VABp' */
        s32 ver;           /* VAB file version number */
        s32 id;            /* VAB id */
        u32 fsize;         /* VAB file size */
        u16 reserved0;     /* system reserved */
        u16 ps;            /* # of the programs in this bank */
        u16 ts;            /* # of the tones in this bank */
        u16 vs;            /* # of the vags in this bank */
        u8  mvol;          /* master volume for this bank */
        u8  pan;           /* master panning for this bank */
        u8  attr1;         /* bank attributes1 */
        u8  attr2;         /* bank attributes2 */
        u32 reserved1;     /* system reserved */
    } VabHdr;			/* 32 byte */


    typedef struct ProgAtr {        /* Program Headdings */
        u8  tones;          /* # of tones */
        u8  mvol;           /* program volume */
        u8  prior;          /* program priority */
        u8  mode;           /* program mode */
        u8  mpan;           /* program pan */
        s8  reserved0;      /* system reserved */
        s16 attr;           /* program attribute */
        u32 reserved1;      // "fake" program index (skips empties)
        u16 reserved2;      // even vag spu ptr
        u16 reserved3;      // odd vag spu ptr
    } ProgAtr;			/* 16 byte */

    typedef void(*SndSsMarkCallbackProc)(short seq_no, short sep_no, short data);

    struct SeqMarker
    {
        SndSsMarkCallbackProc field_0_entries[16];
    };

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
        unsigned char field_1B;
        char field_1C;
        unsigned char field_1D;
        char field_1E;
        char field_1F;
        char field_20_l_count;
        char field_21;
        signed char field_22_next_seq;
        char field_23_next_sep;
        char field_24_rhythm_n;
        char field_25_rhythm_d;
        char field_26_vab_id;
        char field_27_panpot[16];
        char field_37_programs[16];
        char field_47;
        short field_48;
        short field_4A;
        short field_4C;
        short field_4E;
        short field_50_res_of_quarter_note;
        short field_52;
        short field_54;
        short field_56;
        unsigned short field_58_voll;
        unsigned short field_5A_volr;
        unsigned short field_5C;
        unsigned short field_5E;
        short field_60_vol[16];
        short field_80;
        char field_82;
        char field_83;
        int field_84;
        int field_88;
        int field_8C_tempo;
        int field_90_delta_value;
        int field_94; // tempo?
        unsigned int field_98_flags;
        int field_9C;
        int field_A0;
        int field_A4;
        int field_A8;
        int field_AC;
    };

    typedef struct {
        void (*noteon) (short seq_no, short sep_no, unsigned char note, unsigned char voll);
        void (*programchange) (short seq_no, short sep_no, unsigned char programNum);
        void (*pitchbend) (short seq_no, short sep_no);
        void (*metaevent) (short seq_no, short sep_no, unsigned char ev);
        void (*control[13]) (short, short, unsigned char);
        void (*ccentry[20]) (short, short, short, VagAtr, short, unsigned char);
    } _SsFCALL;
    extern _SsFCALL SsFCALL;

    struct SeqStruct;
    extern struct SeqMarker _SsMarkCallback[32];
    extern int VBLANK_MINUS;
    extern unsigned long _snd_openflag;
    extern int _snd_ev_flag;
    extern struct SeqStruct *_ss_score[32];
    extern short _snd_seq_s_max;
    extern short _snd_seq_t_max;

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

    void SsStart(void);
    void SsInit(void);
    void SsSetTableSize(char *table, short s_max, short t_max);
    void SsSetTickMode(long tick_mode);
    void SsSetMVol(short voll, short volr);
    short SsVabTransfer(unsigned char *vh_addr, unsigned char *vb_addr, short vabid, short i_flag);
    short SsVabTransCompleted(short immediateFlag);
    void SsVabClose(short);

    short SsSeqOpen(unsigned long *pSeqData, short vab_id);
    void SsSeqSetVol(short seq_access_num, short voll, short volr);
    void SsSeqPlay(short seq_access_num, char play_mode, short l_count);
    void SsSeqClose(short seq_access_num);
    
    short SsUtKeyOffV(short voice);
    long SsVoKeyOn(long vab_pro, long pitch, unsigned short volL, unsigned short volR);

    void SsSeqCalledTbyT(void);

    void SsUtReverbOn(void);
    void SsUtReverbOff(void);

    short SsUtSetReverbType(short);
    void SsUtSetReverbFeedback(short);
    short SsVabOpenHead(unsigned char *addr, short vabId);
    short SsVabTransBody(unsigned char* pVbData, short vabId);

    // Debugging
    void debug_dump_vh(unsigned long *pAddr, short vabId);
#ifdef __cplusplus
}
#endif

void DoTests();

#endif // _LIB_SND_HPP_
