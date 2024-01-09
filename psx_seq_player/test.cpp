#ifdef PSX
#include <SYS/TYPES.H>
#include <STDLIB.H>
#else
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#endif

#include "types.h"

#include "lib_snd.hpp"

extern "C"
{
void _SsGetSeqData(short seq_idx, short sep_idx);
short Our_SsPitchFromNote(short note, short fine, unsigned char centre, unsigned char shift);
short SsPitchFromNote(short note, short fine, unsigned char centre, unsigned char shift);
void _SsVmDoAllocate(void);
short _SsVmSelectToneAndVag(unsigned char *pVagAttrIdx, unsigned char *pVagNums);

extern unsigned char _svm_sreg_dirty[24];
extern RegBufStruct _svm_sreg_buf[24];
extern unsigned int _svm_envx_hist[16];
extern SpuVoice _svm_voice[24];
extern short _svm_damper;
extern VagAtr *_svm_tn;
extern ProgAtr *_svm_pg;
extern struct_svm _svm_cur;
}

void PcsxReduxExit(short exitCode)
{
    printf("Exiting with code %d\n", exitCode);
    #ifdef PSX
    (*(volatile unsigned short *)0x1f802082) = exitCode;
    #else
    exit(exitCode);
    #endif
}

#define ASSERT_EQ(value1, value2)                                                           \
    if ((value1) != (value2))                                                               \
    {                                                                                       \
        printf("%d != %d in %s %s:%d\n", value1, value2, __FUNCTION__, __FILE__, __LINE__); \
        PcsxReduxExit(1);                                                                   \
    }

#define ASSERT_EQ_HEX(value1, value2)                                                           \
    if ((value1) != (value2))                                                               \
    {                                                                                       \
        printf("%08X != %08X in %s %s:%d\n", value1, value2, __FUNCTION__, __FILE__, __LINE__); \
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

    printf("Test_SsVmSelectToneAndVag: pass\n");
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

    printf("Test_SsVmDoAllocate: pass\n");
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

void Test_SsGetSeqData()
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

    printf("Test_SsGetSeqData: pass\n");
}

void Test_SsPitchFromNote()
{
    for (int note = 0; note < 127; note++)
    {
        for (int fine = 0; fine < 127; fine++)
        {
            for (int centre = 0; centre < 127; centre++)
            {
                for (int shift = 0; shift < 127; shift++)
                {
                    const short realRet = SsPitchFromNote(note, fine, centre, shift);
                    const short ourRet = Our_SsPitchFromNote(note, fine, centre, shift);
                    if (ourRet != realRet)
                    {
                        printf("Failed to match result on %d %d %d %d\n", note, fine, centre, shift);
                        ASSERT_EQ(ourRet, realRet);
                    }
                }
            }
        }
    }
}

void test_vabhdr()
{
    extern unsigned char MINES_VH[];
    VabHdr * vabHdr = (VabHdr*)MINES_VH;
    ASSERT_EQ_HEX(vabHdr->form, 0x56414270);
    ASSERT_EQ_HEX(vabHdr->ver, 0x00000007);
    ASSERT_EQ_HEX(vabHdr->id, 0x00000000);
    ASSERT_EQ_HEX(vabHdr->fsize, 0x00075680);
    ASSERT_EQ_HEX(vabHdr->reserved1, 0xFFFFFFFF);
    printf("test_vabhdr: pass\n");
}

void test_progatr()
{
    extern unsigned char MINES_VH[];
    ProgAtr * progAtr = (ProgAtr*)(MINES_VH  + sizeof(VabHdr));
    ASSERT_EQ_HEX(progAtr->tones, 0xF);        
    ASSERT_EQ_HEX(progAtr->reserved1, 0xFFFFFFFF);
    ASSERT_EQ_HEX(progAtr->reserved3, 0xFFFF);
    printf("test_progatr: pass\n");
}

void test_types()
{
    ASSERT_EQ(sizeof(u8), 1);
    ASSERT_EQ(sizeof(s8), 1);
    ASSERT_EQ(sizeof(u16), 2);
    ASSERT_EQ(sizeof(s16), 2);
    ASSERT_EQ(sizeof(u32), 4);
    ASSERT_EQ(sizeof(s32), 4);
    ASSERT_EQ(sizeof(u64), 8);
    ASSERT_EQ(sizeof(s64), 8);
    printf("test_types: passed\n");
}

void run_unit_tests()
{
    printf("Tests start\n");
    // slow test
    // Test_SsPitchFromNote(); 
    Test_SsVmSelectToneAndVag();
    Test_SsVmDoAllocate();
    Test_SsGetSeqData();
    test_vabhdr();
    test_progatr();
    test_types();
    printf("Tests end\n");
    PcsxReduxExit(0);
}