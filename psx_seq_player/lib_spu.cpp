#include "lib_spu.hpp"

#include <sys/types.h>
//#include <libapi.h>
//#include <libetc.h> // ResetCallBack
#include <stdarg.h> // va_list
#include <stdio.h> // printf
#include "psx_stubs.hpp"

extern "C"
{
    #define REPLACE_LIB
    #ifdef REPLACE_LIB
    #define LIBVAR
    #else
    #define LIBVAR extern
    #endif

    typedef struct tagSpuMalloc SPU_MALLOC;
    typedef struct tagSpuControl SPU_RXX;

    // S_RMP0.OBJ
    LIBVAR u32 _spu_rev_startaddr[] =
    {
        0xFFFE, 0xFB28, 0xFC18,	0xF6F8,
        0xF204,	0xEA44,	0xE128,	0xCFF8,
        0xCFF8,	0xF880,	0,		0
    };

    /*
    // S_RMP1.OBJ
    LIBVAR u32 _spu_rev_workareasize[] =
    {
        0,		1240,	984,	2312,
        3580,	5564,	7896,	12296,
        12296,	1920,	0,		0
    };
    */

    // S_INI.OBJ
    LIBVAR u32 _spu_EVdma = 0;
    LIBVAR u32 _spu_keystat = 0;
    LIBVAR u32 _spu_trans_mode = 0;
    LIBVAR u32 _spu_rev_flag = 0;
    LIBVAR u32 _spu_rev_reserve_wa = 0;
    LIBVAR u32 _spu_rev_offsetaddr = 0;
    LIBVAR SpuReverbAttr _spu_rev_attr;
    LIBVAR u32 _spu_RQvoice = 0;
    LIBVAR u32 _spu_RQmask=0;
    LIBVAR u16 _spu_voice_centerNote[24] =
    {
        0xC000, 0xC000, 0xC000, 0xC000,
        0xC000, 0xC000, 0xC000, 0xC000,
        0xC000, 0xC000, 0xC000, 0xC000,
        0xC000, 0xC000, 0xC000, 0xC000,
        0xC000, 0xC000, 0xC000, 0xC000,
        0xC000, 0xC000, 0xC000, 0xC000
    };
    LIBVAR u32 _spu_env = 0;
    LIBVAR u32 _spu_isCalled=0;

    // ZEROBUF.OBJ
    LIBVAR u8 _spu_zerobuf[1024] = { 0 };

    // S_M.obj
    LIBVAR s32 _spu_AllocBlockNum = 0;
    LIBVAR s32 _spu_AllocLastNum = 0;
    LIBVAR SPU_MALLOC *_spu_memList=NULL;

    // private var
    static int _spu_wck;

    // SPU.obj
    LIBVAR u16 _spu_RQ[10];

    LIBVAR volatile SPU_RXX *_spu_RXX = (volatile SPU_RXX*)0x1F801C00;
    LIBVAR u16 _spu_tsa=0;
    LIBVAR u32 _spu_transMode = 0;
    LIBVAR u32 _spu_addrMode = 0;

    LIBVAR u32 _spu_mem_mode = 2;
    LIBVAR u32 _spu_mem_mode_plus = 3;
    LIBVAR u32 _spu_mem_mode_unit = 8;
    LIBVAR u32 _spu_mem_mode_unitM = 7;
    LIBVAR u32 _spu_inTransfer = 1;

    void (*_spu_transferCallback)() = NULL;
    void (*_spu_IRQCallback)() = NULL;


    LIBVAR volatile u32 *SPUDMA_MADR  = (volatile u32*)0x1F8010C0;
    LIBVAR volatile u32 *SPUDMA_BCR   = (volatile u32*)0x1F8010C4;
    LIBVAR volatile u32 *SPUDMA_CHCR  = (volatile u32*)0x1F8010C8;
    LIBVAR volatile u32 *DPCR         = (volatile u32*)0x1F8010F0;
    LIBVAR volatile u32 *SPUSIZE      = (volatile u32*)0x1F801014;

    void _spu_w(void *data, int size);
    void _spu_ssize();
    void _spu_ssize0();

    typedef struct tagSpuVoiceRegister
    {
        SpuVolume volume;	// 0-2
        u16 pitch;			// 4
        u16 addr;			// 6
        u16 adsr[2];		// 8-A
        u16 volumex;		// C
        u16 loop_addr;		// E
    } SPU_VOICE_REG;	// 16 bytes

    typedef struct tagSpuControl
    {
        SPU_VOICE_REG voice[24];
        SpuVolume main_vol;	// 180
        SpuVolume rev_vol;	// 184
        // bit flags
        u16 key_on[2];		// 188
        u16 key_off[2];		// 18C
        u16 chan_fm[2];		// 190
        u16 noise_mode[2];	// 194
        u16 rev_mode[2];	// 198
        u32 chan_on;		// 19C
        u16 unk;			// 1A0
        u16 rev_work_addr;	// 1A2
        u16 irq_addr;		// 1A4
        u16 trans_addr;		// 1A6
        u16 trans_fifo;		// 1A8
        u16 spucnt;			// 1AA SPUCNT
        u16 data_trans;		// 1AC
        u16 spustat;		// 1AE SPUSTAT
        SpuVolume cd_vol;	// 1B0
        SpuVolume ex_vol;	// 1B4
        SpuVolume main_volx;// 1B8
        SpuVolume unk_vol;	// 1BC

        u16 dAPF1; // Starting at 0x1F801DC0
        u16 dAPF2;
        u16 vIIR;
        u16 vCOMB1;
        u16 vCOMB2;
        u16 vCOMB3;
        u16 vCOMB4;
        u16 vWALL;
        u16 vAPF1;
        u16 vAPF2;
        u16 mLSAME;
        u16 mRSAME;
        u16 mLCOMB1;
        u16 mRCOMB1;
        u16 mLCOMB2;
        u16 mRCOMB2;
        u16 dLSAME;
        u16 dRSAME;
        u16 mLDIFF;
        u16 mRDIFF;
        u16 mLCOMB3;
        u16 mRCOMB3;
        u16 mLCOMB4;
        u16 mRCOMB4;
        u16 dLDIFF;
        u16 dRDIFF;
        u16 mLAPF1;
        u16 mRAPF1;
        u16 mLAPF2;
        u16 mRAPF2;
        u16 vLIN;
        u16 vRIN;
    } SPU_RXX;

    typedef struct tagSpuMalloc
    {
        u32 field_0_addr;
        u32 field_4_size;
    } SPU_MALLOC;

    // Forward declares
    int _SpuIsInAllocateArea_(u32 addr);
    void _SpuInit(int mode);

    int  _spu_init(int flag);
    void _spu_FiDMA();
    void _spu_FsetRXX(int l, u32 addr, int flag);
    void _spu_Fw1ts();
    int  _spu_t(int count, ...);
    void SpuStart();

    //extern void _SpuDataCallback(void(*callback)());
    void *_SpuDataCallback(void(*callback)());

    enum
    {
        SPU_T_SYNC0,
        SPU_T_SYNC,
        SPU_T_SET_TRANS,
        SPU_T_TRANSFER,
    };

    #define SPU_TRANSFER_WAIT 1

    void *DMACallback(int index, void(*callback)()); // TODO: Psyq internal?

    // S_DCB.OBJ
    void *_SpuDataCallback(void(*callback)())
    {
        return DMACallback(4, callback);
    }

    //u8 _spu_rev_param[]=
    //{
    //	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    //	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    //	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    //	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    //	   0,  0,  0,  0,  0,  0,  0,  0,125,  0, 91,  0,128,109,184, 84,
    //	 208,190,  0,  0,  0,  0,128,186,  0, 88,  0, 83,214,  4, 51,  3,
    //	 240,  3, 39,  2,116,  3,239,  1, 52,  3,181,  1,  0,  0,  0,  0,
    //	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,180,  1, 54,  1,
    //	 184,  0, 92,  0,  0,128,  0,128,  0,  0,  0,  0, 51,  0, 37,  0,
    //	 240,112,168, 79,224,188, 16, 68,240,192,  0,156,128, 82,192, 78,
    //	 228,  3, 27,  3,164,  3,175,  2,114,  3,102,  2, 28,  3, 93,  2,
    //	  92,  2,142,  1, 47,  2, 53,  1,210,  1,183,  0,143,  1,181,  0,
    //	 180,  0,128,  0, 76,  0, 38,  0,  0,128,  0,128,  0,  0,  0,  0,
    //	 177,  0,127,  0,240,112,168, 79,224,188, 16, 69,240,190,192,180,
    //	 128, 82,192, 78,  4,  9,107,  7, 36,  8, 95,  6,162,  7, 22,  6,
    //	 108,  7,237,  5,236,  5, 46,  4, 15,  5,  5,  3, 98,  4,183,  2,
    //	  47,  4,101,  2,100,  2,178,  1,  0,  1,128,  0,  0,128,  0,128,
    //	   0,  0,  0,  0,227,  0,169,  0, 96,111,168, 79,224,188, 16, 69,
    //	 240,190,128,166,128, 86,192, 82,251, 13, 88, 11,  9, 13, 60, 10,
    //	 217, 11,115,  9, 89, 11,218,  8,217,  8,233,  5,236,  7,176,  4,
    //	 239,  6,210,  3,234,  5, 29,  3, 28,  3, 56,  2, 84,  1,170,  0,
    //	   0,128,  0,128,  0,  0,  0,  0,165,  1, 57,  1,  0, 96,  0, 80,
    //	   0, 76,  0,184,  0,188,  0,192,  0, 96,  0, 92,186, 21,187, 17,
    //	 194, 20,189, 16,188, 17,193, 13,192, 17,195, 13,192, 13,193,  9,
    //	 196, 11,193,  7,  0, 10,205,  6,194,  9,193,  5,192,  5, 26,  4,
    //	 116,  2, 58,  1,  0,128,  0,128,  0,  0,  0,  0, 61,  3, 49,  2,
    //	   0,126,  0, 80,  0,180,  0,176,  0, 76,  0,176,  0, 96,  0, 84,
    //	 214, 30, 49, 26, 20, 29, 59, 24,194, 27,178, 22, 50, 26,239, 21,
    //	 238, 21, 85, 16, 52, 19, 45, 15,246, 17, 93, 12, 86, 16,225, 10,
    //	 224, 10,162,  7,100,  4, 50,  2,  0,128,  0,128,  0,  0,  0,  0,
    //	   1,  0,  1,  0,255,127,255,127,  0,  0,  0,  0,  0,  0,  0,129,
    //	   0,  0,  0,  0,255, 31,255, 15,  5, 16,  5,  0,  0,  0,  0,  0,
    //	   5, 16,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    //	   0,  0,  0,  0,  4, 16,  2, 16,  4,  0,  2,  0,  0,128,  0,128,
    //	   0,  0,  0,  0,  1,  0,  1,  0,255,127,255,127,  0,  0,  0,  0,
    //	   0,  0,  0,  0,  0,  0,  0,  0,255, 31,255, 15,  5, 16,  5,  0,
    //	   0,  0,  0,  0,  5, 16,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    //	   0,  0,  0,  0,  0,  0,  0,  0,  4, 16,  2, 16,  4,  0,  2,  0,
    //	   0,128,  0,128,  0,  0,  0,  0, 23,  0, 19,  0,240,112,168, 79,
    //	 224,188, 16, 69,240,190,  0,133,128, 95,192, 84,113,  3,175,  2,
    //	 229,  2,223,  1,176,  2,215,  1, 88,  3,106,  2,214,  1, 30,  1,
    //	  45,  1,177,  0, 31,  1, 89,  0,160,  1,227,  0, 88,  0, 64,  0,
    //	  40,  0, 20,  0,  0,128,  0,128,  0,  0,  0,  0,  0,  0,  0,  0,
    //};


    #define s _spu_RXX

    // S_I.OBJ
    void SpuInit()
    {
        _SpuInit(0);
    }

    // S_IH.OBJ
    void SpuInitHot()
    {
        _SpuInit(1);
    }

    // S_INI.OBJ
    void _SpuInit(int mode)
    {
        u32 i;
        const int rev_work_addr=0x1A2/2;

        ResetCallback();
        _spu_init(mode);

        for (i = 0; i < _countof(_spu_voice_centerNote); i++)
            _spu_voice_centerNote[i] = 0xC000;

        SpuStart();

        _spu_rev_flag = 0;
        _spu_rev_reserve_wa = 0;
        _spu_rev_attr.mode = 0;
        _spu_rev_attr.depth.left = 0;
        _spu_rev_attr.depth.right = 0;
        _spu_rev_attr.delay = 0;
        _spu_rev_attr.feedback = 0;
        _spu_rev_offsetaddr = _spu_rev_startaddr[0];

        _spu_FsetRXX(rev_work_addr, _spu_rev_startaddr[0], 0);

        _spu_AllocBlockNum = 0;
        _spu_AllocLastNum = 0;
        _spu_memList = NULL;
        _spu_trans_mode = 0;
        _spu_keystat = 0;
        _spu_RQmask = 0;
        _spu_RQvoice = 0;
        _spu_env = 0;
    }

    // S_INI.OBJ
    void SpuStart()
    {
        if (_spu_isCalled) return;
        _spu_isCalled = 1;

        EnterCriticalSection();

        _SpuDataCallback(_spu_FiDMA);
        _spu_EVdma = OpenEvent(HwSPU, EvSpCOMP, EvStACTIVE, NULL);
        EnableEvent(_spu_EVdma);

        ExitCriticalSection();
    }

    // S_ITC.OBJ
    long SpuIsTransferCompleted(long flag)
    {
        int ev;

        if (_spu_trans_mode == 1 || _spu_inTransfer == 1) return 1;

        ev = TestEvent(_spu_EVdma);
        // blocking mode
        if (flag == SPU_TRANSFER_WAIT && !ev)
        {
            // loop until done
            while (!(ev=TestEvent(_spu_EVdma)));
            _spu_inTransfer = 1;
            return 1;
        }
        
        // check if we're done
        if (ev==1) _spu_inTransfer = 1;

        return ev;
    }

    // S_IT.OBJ
    void _spu_setInTransfer(int mode)
    {
        _spu_inTransfer = mode != 1 ? 1 : 0;
    }

    // S_IT.OBJ
    long _spu_getInTransfer(void)
    {
        return _spu_inTransfer ? 0 : 1;
    }

    extern void(*_spu_transferCallback)();
    extern void(*_spu_IRQCallback)();

    // S_Q.OBJ
    void SpuQuit()
    {
        if (_spu_isCalled) return;
        _spu_isCalled = 0;

        EnterCriticalSection();

        _spu_transferCallback = NULL;
        _spu_IRQCallback = NULL;
        _SpuDataCallback(NULL);

        CloseEvent(_spu_EVdma);
        DisableEvent(_spu_EVdma);

        ExitCriticalSection();
    }

    // S_CRWA.OBJ
    long SpuClearReverbWorkArea(long mode)
    {
        u32 m, n;
        int ck, ck2, p, t;
        void (*bk)()=NULL;

        //extern long _SpuIsInAllocateArea_(u32 addr);

        //DEBUGPRINT(("Clear reverb?\n"));

        if (mode < 10 && !_SpuIsInAllocateArea_(_spu_rev_startaddr[mode]))
        {
            //DEBUGPRINT(("Yup!\n"));

            ck = 0;
            if (mode == SPU_REV_MODE_OFF)
            {
                m = 0x10 << _spu_mem_mode_plus;
                n = 0xFFF0 << _spu_mem_mode_plus;
            }
            else
            {
                m = (0x10000-_spu_rev_startaddr[mode]) << _spu_mem_mode_plus;
                n = _spu_rev_startaddr[mode] << _spu_mem_mode_plus;
            }
            if ((t=_spu_transMode)==1)
            {
                _spu_transMode = 0;
                ck = 1;
            }
            ck2 = 1;
            // preserve callback
            if (_spu_transferCallback)
            {
                bk = _spu_transferCallback;
                _spu_transferCallback = NULL;
            }

            while(ck2)
            {
                //DEBUGPRINT(("CRA left %d\n", m));
                if (m > 0x400) 	p = 0x400;
                else
                {
                    p = m;
                    ck2 = 0;
                }

                _spu_t(SPU_T_SET_TRANS, n);
                _spu_t(SPU_T_SYNC);
                _spu_t(SPU_T_TRANSFER, _spu_zerobuf, p);
                m -= 0x400;
                WaitEvent(_spu_EVdma);
            }

            if (ck) _spu_transMode = t; 
            if (bk) _spu_transferCallback = bk;
            
            return 0;
        }

        //DEBUGPRINT(("Nope\n."));
        return -1;
    }

    // S_M_INIT.OBJ
    long SpuInitMalloc(long num, char *top)
    {
        SPU_MALLOC *pTop;

        if (num <= 0) return 0;

        pTop = (SPU_MALLOC*)top;

        _spu_memList = pTop;
        _spu_AllocLastNum = 0;
        _spu_AllocBlockNum = num;
        pTop->field_0_addr = 0x40001010;
        pTop->field_4_size = (0x10000 << _spu_mem_mode_plus) - 0x1010;

        return num;
    }

    // S_M_UTIL.OBJ
    int _SpuIsInAllocateArea(u32 addr)
    {
        u32 /*pos, mode,*/ ptr;
        SPU_MALLOC *pA = _spu_memList;

        if (!pA) return 0;

        for (;; pA++)
        {
            ptr = pA->field_0_addr;
            // hit last entry
            if (ptr & 0x40000000) break;
            // hit unused entry, skip it
            if (ptr & 0x80000000) continue;

            ptr &= 0xFFFFFF;
            if (addr > ptr && addr < pA->field_4_size + ptr)
            {
                return 1;
            }
        }

        return 0;
    }

    // S_M_UTIL.OBJ
    int _SpuIsInAllocateArea_(u32 addr)
    {
        u32 pos;
        u32 ptr;
        SPU_MALLOC *pA = _spu_memList;

        if (!pA) return 0;

        pos = addr << _spu_mem_mode_plus;

        for (;; pA++)
        {
            ptr = pA->field_0_addr;

            // hit last entry
            if (ptr & 0x40000000)
            {
                break;
            }

            // hit unused entry, skip it
            if (ptr & 0x80000000)
            {
                continue;
            }

            ptr &= 0xFFFFFF;
            if (pos > ptr && pos < pA->field_4_size + ptr)
            {
                return 1;
            }
        }

        return 0;
    }

    // SPU.OBJ
    int _spu_init(int flag)
    {
        u32 i;
        static u8 wseq[] = { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 };

        //DEBUGPRINT(("init spu\n"));

        *DPCR |= 0xB0000;
        _spu_transMode = 0;
        _spu_addrMode = 0;
        _spu_tsa = 0;

        _spu_RXX->main_vol.left = 0;
        _spu_RXX->main_vol.right = 0;
        _spu_RXX->spucnt = 0;
        _spu_Fw1ts();

        //DEBUGPRINT(("wasted spu\n"));
        _spu_RXX->main_vol.left = 0;
        _spu_RXX->main_vol.right = 0;

        if (_spu_RXX->spustat & 0x7FF)
        {
            i = 1;
            do
            {
                if (i > 3840)
                {
                    //DEBUGPRINT(("SPU T/O reset\n"));
                    break;
                }
                i++;
            } while (_spu_RXX->spustat & 0x7FF);
        }

        //DEBUGPRINT(("set spu\n"));
        _spu_mem_mode = 2;
        _spu_mem_mode_plus = 3;
        _spu_mem_mode_unit = 8;
        _spu_mem_mode_unitM = 7;
        _spu_RXX->data_trans = 4;
        _spu_RXX->rev_vol.left = 0;
        _spu_RXX->rev_vol.right = 0;
        _spu_RXX->key_off[0] = 0xFF;//-1;	// code actually handles this as 2 u16 writes
        _spu_RXX->key_off[1] = 0xFF;//-1;
        _spu_RXX->rev_mode[0] = 0;	// same write style
        _spu_RXX->rev_mode[1] = 0;

        for (i = 0; i < _countof(_spu_RQ); i++)
            _spu_RQ[i] = 0;

        if (!flag)
        {
            _spu_tsa = 0x200;
            _spu_RXX->chan_fm[0] = 0;		// 2 writes
            _spu_RXX->chan_fm[1] = 0;
            _spu_RXX->noise_mode[0] = 0;	// 2 writes
            _spu_RXX->noise_mode[1] = 0;
            _spu_RXX->cd_vol.left = 0;
            _spu_RXX->cd_vol.right = 0;
            _spu_RXX->ex_vol.left = 0;
            _spu_RXX->ex_vol.right = 0;
            _spu_w(wseq, sizeof(wseq));

            for (i = 0; i < 24; i++)
            {
                _spu_RXX->voice[i].volume.left = 0;
                _spu_RXX->voice[i].volume.right = 0;
                _spu_RXX->voice[i].pitch = 0x3FFF;
                _spu_RXX->voice[i].addr = 0x200;
                _spu_RXX->voice[i].adsr[0] = 0;	// 2 writes
                _spu_RXX->voice[i].adsr[1] = 0;
            }

            _spu_RXX->key_on[0] = 0xFFFF;	// 2 writes
            _spu_RXX->key_on[1] = 0xFF;
            _spu_Fw1ts();
            _spu_Fw1ts();
            _spu_Fw1ts();
            _spu_Fw1ts();

            _spu_RXX->key_off[0] = 0xFFFF;	// 2 writes
            _spu_RXX->key_off[1] = 0xFF;
            _spu_Fw1ts();
            _spu_Fw1ts();
            _spu_Fw1ts();
            _spu_Fw1ts();
        }

        //DEBUGPRINT(("final spu\n"));
        _spu_inTransfer = 1;
        _spu_RXX->spucnt = 0xC000;
        _spu_transferCallback = NULL;
        _spu_IRQCallback = NULL;

        //DEBUGPRINT(("done spu\n"));
        return 0;
    }

    // private ??
    void _spu_w(void *data, int size)
    {
        int i, c;
        u32 ck;
        u16 *d16;

        //DEBUGPRINT(("spu_w\n"));
        ck = s->spustat & 0x7FF;
        s->trans_addr = _spu_tsa;
        _spu_Fw1ts();

        if (size)
        {
            d16 = (u16*)data;
            // write data in blocks of max 64 bytes
            while (size > 0)
            {
                c = size > 64 ? 64 : size;
                //DEBUGPRINT(("write %d bytes\n", c));
                for (i=0; i < c; i += 2)
                    s->trans_fifo = *d16++;
                s->spucnt = (s->spucnt & ~0x30) | 0x10;
                _spu_Fw1ts();
                if (s->spustat & 0x400)
                {
                    i = 1;
                    do
                    {
                        if (i > 3840)
                        {
                            //DEBUGPRINT(("SPU T/O wait ready\n"));
                            break;
                        }
                        i++;
                    } while (s->spustat & 0x400);
                }
                _spu_Fw1ts();
                _spu_Fw1ts();
                size -= c;
            }
        }

        s->spucnt &= ~0x30;
        if ((s->spustat & 0x7FFu) != ck)
        {
            i = 1;
            do
            {
                if (i > 3840)
                {
                    //DEBUGPRINT(("SPU T/O dma clear\n"));
                    break;
                }
                i++;
            } while ((s->spustat & 0x7FFu) != ck);
        }
    }

    // SPU.obj
    void _spu_FiDMA()
    {
        int i;
        if (!_spu_wck) _spu_Fw1ts();

        s->spucnt &= ~0x30;

        // waste time or wait for SPU
        for (i = 1; !(s->spucnt & 0x30); i++)
            if (i>3840)
                break;

        if (_spu_transferCallback)
            _spu_transferCallback();
        else DeliverEvent(HwSPU, EvSpCOMP);
    }

    // SPU.OBJ
    void _spu_Fr_(void *data, int addr, u32 size)
    {
        s->trans_addr = addr;
        _spu_Fw1ts();

        s->spucnt |= 0x30;
        _spu_Fw1ts();
        _spu_ssize();

        *SPUDMA_MADR = (u32)data;
        *SPUDMA_BCR = (size << 16) | 0x10;
        *SPUDMA_CHCR = 0x1000200;
        _spu_wck = 1;
    }

    // SPU.OBJ
    int _spu_t(int count, ...)
    {
        int i;
        u32 ck;
        static u32 _spu_tmp = 0, _spu_bcr = 0;
        va_list args;
        va_start(args, count);

        //DEBUGPRINT(("Tsa %d\n", count));

        switch (count)
        {
        case SPU_T_SET_TRANS:
            ck = va_arg(args, u32) >> _spu_mem_mode_plus;
            _spu_tsa = ck;
            s->trans_addr = ck;
            return 0;
        case SPU_T_SYNC:
            _spu_wck = 0;
            if (s->trans_addr != _spu_tsa)
            {
                i = 1;
                do
                {
                    if (i > 3841) return -2;
                    i++;
                } while (s->trans_addr != _spu_tsa);
            }
            s->spucnt = (s->spucnt & ~0x30) | 0x20;
            return 0;
        case SPU_T_SYNC0:
            _spu_wck = 1;
            if (s->trans_addr != _spu_tsa)
            {
                i = 1;
                do
                {
                    if (i > 3841) return -2;
                    i++;
                } while (s->trans_addr != _spu_tsa);
            }
            s->spucnt |= 0x30;
            return 0;
        case SPU_T_TRANSFER:
            //ck = _spu_wck ? 0x20 : 0x30;
            ck = _spu_wck ? 0x30 : 0x20;
            if ((s->spucnt & 0x30u) != ck)
            {
                i = 1;
                do
                {
                    if (i > 3841)
                    {
                        //DEBUGPRINT(("T/O transfer %x==%x\n", s->spucnt, ck));
                        return -2;
                    }
                    i++;
                } while ((s->spucnt & 0x30u) != ck);
            }
            if (_spu_wck == 1) _spu_ssize();
            else _spu_ssize0();

            _spu_tmp = (u32)va_arg(args, u32);
            ck = (u32)va_arg(args, u32);
            _spu_bcr = (ck / 64) + ((ck % 64) ? 1 : 0);

            *SPUDMA_MADR = _spu_tmp;
            *SPUDMA_BCR = (_spu_bcr << 16) | 0x10;
            *SPUDMA_CHCR = _spu_wck==1 ? 0x1000200 : 0x1000201;
        }

        return 0;
    }

    // SPU.OBJ
    long _spu_Fw(void *addr, u32 size)
    {
        //DEBUGPRINT(("Fw %d\n", size));
        if (!_spu_transMode)
        {
            _spu_t(SPU_T_SET_TRANS, _spu_tsa << _spu_mem_mode_plus);
            _spu_t(SPU_T_SYNC);
            _spu_t(SPU_T_TRANSFER, addr, size);
        }
        else _spu_w(addr, size);

        return size;
    }

    // SPU.OBJ
    void _spu_Fr(void *addr, u32 size)
    {
        //DEBUGPRINT(("Fr %d\n", size));
        _spu_t(SPU_T_SET_TRANS, _spu_tsa << _spu_mem_mode_plus);
        _spu_t(SPU_T_SYNC);
        _spu_t(SPU_T_TRANSFER, addr, size);
    }

    // SPU.OBJ
    void _spu_FsetRXX(int l, u32 addr, int flag)
    {
        u16 *x=&((u16*)s)[l];

        if(!flag) *x = addr;
        else *x = addr >> _spu_mem_mode_plus;
    }

    // SPU.OBJ
    long _spu_FsetRXXa(int l, u32 flag)
    {
        u32 ret;
        u16 *x = &((u16*)s)[l];

        if (_spu_mem_mode && (flag % _spu_mem_mode_unit))
            flag = (flag+_spu_mem_mode_unit) & ~_spu_mem_mode_unitM;

        ret = flag >> _spu_mem_mode_plus;

        switch (l)
        {
        default:
            *x = ret;
            ret = flag;
            break;
        case -1:
            ret &= 0xFFFF;
            break;
        case -2:
            ret = flag;
        }
        //if (l != -2)
        //{
        //	if (l != -1)
        //	{
        //		*x = ret;
        //		ret = flag;
        //	}
        //	else ret &= 0xFFFF;
        //}
        //else ret = flag;

        return ret;
    }

    // SPU.OBJ
    long _spu_FgetRXXa(int l, int flag)
    {
        u32 ret;
        u16 *x=&((u16*)s)[l];

        ret = *x;

        if (flag == -1) return ret;
        
        return ret << _spu_mem_mode_plus;
    }

    // SPU.OBJ
    void _spu_FsetPCR(int flag)
    {
        u32 r;

        *DPCR &= ~0x70000;
        r = flag ? 0x30000 : 0x50000;
        *DPCR|=r;
    }

    // private ??
    void _spu_ssize0()
    {
        *SPUSIZE = (*SPUSIZE & 0xF0FFFFFF) | 0x20000000;
    }

    // private ??
    void _spu_ssize()
    {
        *SPUSIZE = (*SPUSIZE & 0xF0FFFFFF) | 0x22000000;
    }

    // SPU.OBJ spu time waster
    void _spu_Fw1ts()
    {
        int v[2];

        v[1] = 13;
        v[0] = 0;
        goto stuff;

        while (1)
        {
            v[1] += v[1] * 12;
            v[0]++;
            /*
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
            */
    stuff:
            if (v[0] >= 60) break;
        }
    }

    // S_STSA.OBJ
    u32 SpuSetTransferStartAddr(u32 addr)
    {
        // check if we're inside SPU RAM
        if (addr - 0x1010 > SPU_MAX_TRANS - 8) return 0;

        _spu_tsa = _spu_FsetRXXa(-1, addr);
        return _spu_tsa << _spu_mem_mode_plus;
    }

    // S_R.OBJ (not used ??)
    u32 SpuRead(u8 *addr, u32 size)
    {
        if (size > SPU_MAX_TRANS) size = SPU_MAX_TRANS;

        _spu_Fr(addr, size);
        if (!_spu_transferCallback) _spu_inTransfer = 0;

        return size;
    }

    // S_W.OBJ
    u32 SpuWrite(u8* addr, u32 size)
    {
        if(size > SPU_MAX_TRANS) size=SPU_MAX_TRANS;

        _spu_Fw(addr, size);
        if (!_spu_transferCallback) _spu_inTransfer = 0;

        return size;
    }

    // S_W0.OBJ (not used??)
    u32 SpuWrite0(u32 size)
    {
        int ck, ck2;
        u32 bsize;
        u32 written, tsa, old_tmode;
        extern u32 _spu_EVdma;
        void(*bk)();

        if ((old_tmode=_spu_transMode) == 1)
        {
            ck = 1;
            _spu_transMode = 0;
        }
        else ck = 0;

        ck2 = 1;
        // transfer address
        tsa = _spu_tsa << _spu_mem_mode_plus;
        // disable callback and backup
        if (_spu_transferCallback)
        {
            bk = _spu_transferCallback;
            _spu_transferCallback = NULL;
        }
        else bk = NULL;

        written = 0;
        while (ck2)
        {
            if (size >= 1024+1) bsize=1024;
            else
            {
                ck2 = 0;
                bsize = size / 64 * 64;
                if (bsize > size) bsize += 64;
            }
            // write a chunk
            _spu_t(SPU_T_SET_TRANS, tsa);
            _spu_t(SPU_T_SYNC);
            _spu_t(SPU_T_TRANSFER, _spu_zerobuf, bsize);
            size -= 1024;
            tsa += 1024;
            written += bsize;
            WaitEvent(_spu_EVdma);
            // we're writing only one chunk
            //if (!ck2) break;
        }

        // restore stuff
        if (ck) _spu_transMode = old_tmode;
        if (bk) _spu_transferCallback = bk;

        return written;
    }

    // S_WP.OBJ (not used??)
    u32 SpuWritePartly(u8 *addr, u32 size)
    {
        u32 tsa;
        if (size > SPU_MAX_TRANS) size = SPU_MAX_TRANS;

        tsa = _spu_tsa << _spu_mem_mode_plus;
        _spu_Fw(addr, size);
        _spu_tsa=_spu_FsetRXXa(-1, tsa + size);
        if (!_spu_transferCallback) _spu_inTransfer = 0;

        return size;
    }

    // S_F.OBJ (not used ??)
    u32 SpuFlush(u32 ev)
    {
        extern u32 _spu_RQmask, _spu_env, _spu_RQvoice, _spu_keystat;
        int i;

        u32 ret = 0;
        //
        if ((ev == SPU_EVENT_ALL || ev & SPU_EVENT_PITCHLFO) && _spu_RQmask & SPU_EVENT_PITCHLFO)
        {
            ret |= SPU_EVENT_PITCHLFO;
            _spu_RQmask &= ~SPU_EVENT_PITCHLFO;
            s->chan_fm[0] = _spu_RQ[4];
            s->chan_fm[1] = _spu_RQ[5];
        }
        //
        if ((ev == SPU_EVENT_ALL || ev & SPU_EVENT_NOISE) && _spu_RQmask & SPU_EVENT_NOISE)
        {
            ret |= SPU_EVENT_NOISE;
            _spu_RQmask &= ~SPU_EVENT_NOISE;
            s->noise_mode[0] = _spu_RQ[6];
            s->noise_mode[1] = _spu_RQ[7];
        }
        //
        if ((ev == SPU_EVENT_ALL || ev & SPU_EVENT_REVERB) && _spu_RQmask & SPU_EVENT_REVERB)
        {
            ret |= SPU_EVENT_REVERB;
            _spu_RQmask &= ~SPU_EVENT_REVERB;
            s->rev_mode[0] = _spu_RQ[8];
            s->rev_mode[1] = _spu_RQ[9];
        }
        //
        if ((ev == SPU_EVENT_ALL || ev & SPU_EVENT_KEY) && _spu_RQmask & SPU_EVENT_KEY)
        {
            ret |= SPU_EVENT_KEY;
            _spu_RQmask &= ~SPU_EVENT_KEY;
            s->key_on[0] = _spu_RQ[0];
            s->key_on[1] = _spu_RQ[1];
            s->key_off[0] = _spu_RQ[2];
            s->key_off[1] = _spu_RQ[3];
            for (i = 0; i < 4; i++)
                _spu_RQ[i] = 0;
            if (_spu_env & SPU_EVENT_KEY)
                _spu_keystat=_spu_RQvoice;
        }

        return ret;
    }

    // S_SNC.OBJ
    long SpuSetNoiseClock(long n_clock)
    {
        unsigned char n_clock_fixed = 0;
        if (n_clock >= 0)
        {
            n_clock_fixed = n_clock;
            if (n_clock >= 64)
            {
                n_clock_fixed = 63;
            }
        }
        _spu_RXX->spucnt = _spu_RXX->spucnt & 0xC0FF | ((n_clock_fixed & 0x3F) << 8);
        return n_clock_fixed;
    }

    // S_GVEX.OBJ
    void SpuGetVoiceEnvelope(int voiceNum, short *envx)
    {
        *envx = _spu_RXX->voice[voiceNum].volumex;
    }

    // TODO:
    unsigned long _SpuSetAnyVoice(long on_off_flags, unsigned long voice_bits, int word_idx1, int word_idx2);

    // S_SNV.OBJ
    unsigned long SpuSetNoiseVoice(long on_off_flags, unsigned long voice_bits)
    {
        return _SpuSetAnyVoice(on_off_flags, voice_bits, 202, 203);
    }

    // S_SRV.OBJ
    unsigned long SpuSetReverbVoice(long on_off_flags, unsigned long voice_bits)
    {
        return _SpuSetAnyVoice(on_off_flags, voice_bits, 204, 205);
    }

    // TODO:
    unsigned long _SpuGetAnyVoice(int word_idx1, int word_idx2);

    // S_GRV.OBJ
    unsigned long SpuGetReverbVoice(void)
    {
        return _SpuGetAnyVoice(204, 205);
    }

    // S_GNV.OBJ
    unsigned long SpuGetNoiseVoice(void)
    {
        return _SpuGetAnyVoice(202, 203);
    }

    // S_SR.OBJ
    void SpuSetReverb(long on_off)
    {
        if (on_off == 0)
        {
            _spu_rev_flag = 0;
            _spu_RXX->spucnt &= ~0x80;
        }
        else if (on_off == 1)
        {
            if (_spu_rev_reserve_wa == 1 || !_SpuIsInAllocateArea(_spu_rev_offsetaddr))
            {
                _spu_rev_flag = 1;
                _spu_RXX->spucnt |= 0x80;
            }
            else
            {
                _spu_rev_flag = 0;
                _spu_RXX->spucnt &= ~0x80;
            }
        }
    }

    // S_STM.OBJ
    long SpuSetTransferMode(long mode)
    {
        if (mode)
        {
            _spu_transMode = mode == 1 ? 1 : 0;
        }
        else
        {
            _spu_transMode = 0;
        }

        _spu_trans_mode = mode;

        return _spu_transMode;
    }

    // TODO:
    void _spu_gcSPU(void);

    // S_M_F.OBJ
    void SpuFree(unsigned long addr)
    {
        s32 blockCounter = 0;
        if (_spu_AllocBlockNum > 0)
        {
            SPU_MALLOC* pAllocIter = _spu_memList;
            while ((pAllocIter->field_0_addr & 0x40000000) == 0)
            {
                blockCounter++;

                if (pAllocIter->field_0_addr == addr)
                {
                    pAllocIter->field_0_addr = addr | 0x80000000;
                    break;
                }

                pAllocIter++;
                if (blockCounter >= _spu_AllocBlockNum)
                {
                    break;
                }
            }
        }

        _spu_gcSPU();
    }

    // S_SCA.OBJ
    void SpuSetCommonAttr(SpuCommonAttr *pAttr)
    {
        short mvol_left = 0;
        if ((pAttr->mask == 0) || (pAttr->mask & 1))
        {
            int mov_left_part1 = 0;
            if ((pAttr->mask == 0) || (pAttr->mask & 4))
            {
                switch (pAttr->mvolmode.left)
                {
                case 1:
                    mov_left_part1 = 0x8000;
                    break;
                case 2:
                    mov_left_part1 = 0x9000;
                    break;
                case 3:
                    mov_left_part1 = 0xA000;
                    break;
                case 4:
                    mov_left_part1 = 0xB000;
                    break;
                case 5:
                    mov_left_part1 = 0xC000;
                    break;
                case 6:
                    mov_left_part1 = 0xD000;
                    break;
                case 7:
                    mov_left_part1 = 0xE000;
                    break;
                default:
                    mvol_left = pAttr->mvol.left;
                    mov_left_part1 = 0;
                    break;
                }
            }
            else
            {
                mvol_left = pAttr->mvol.left;
                mov_left_part1 = 0;
            }

            short mov_left_part2 = mvol_left & 0x7FFF;
            if (mov_left_part1)
            {
                const int mvol_left_local = pAttr->mvol.left;
                short mvol_left_clamped = 127;
                if (mvol_left_local < 128)
                {
                    mvol_left_clamped = 0;
                    if (mvol_left_local >= 0)
                    {
                        mvol_left_clamped = pAttr->mvol.left;
                    }
                }
                mov_left_part2 = mvol_left_clamped & 0x7FFF;
            }
            _spu_RXX->main_vol.left = mov_left_part2 | mov_left_part1;
        }

        short mvol_right_local_1 = 0;
        if ((pAttr->mask == 0) || (pAttr->mask & 2))
        {
            int mov_right_part1 = 0;
            if ((pAttr->mask == 0) || (pAttr->mask & 8))
            {
                switch (pAttr->mvolmode.right)
                {
                case 1:
                    mov_right_part1 = 0x8000;
                    break;
                case 2:
                    mov_right_part1 = 0x9000;
                    break;
                case 3:
                    mov_right_part1 = 0xA000;
                    break;
                case 4:
                    mov_right_part1 = 0xB000;
                    break;
                case 5:
                    mov_right_part1 = 0xC000;
                    break;
                case 6:
                    mov_right_part1 = 0xD000;
                    break;
                case 7:
                    mov_right_part1 = 0xE000;
                    break;
                default:
                    mvol_right_local_1 = pAttr->mvol.right;
                    mov_right_part1 = 0;
                    break;
                }
            }
            else
            {
                mvol_right_local_1 = pAttr->mvol.right;
                mov_right_part1 = 0;
            }

            short mov_right_part2 = mvol_right_local_1 & 0x7FFF;
            if (mov_right_part1)
            {
                const int mvol_right_local = pAttr->mvol.right;
                short mvol_right_clamped = 127;
                if (mvol_right_local < 128)
                {
                    mvol_right_clamped = 0;
                    if (mvol_right_local >= 0)
                    {
                        mvol_right_clamped = pAttr->mvol.right;
                    }
                }
                mov_right_part2 = mvol_right_clamped & 0x7FFF;
            }

            _spu_RXX->main_vol.right = mov_right_part2 | mov_right_part1;
        }

        if (pAttr->mask || (pAttr->mask & 0x40) != 0)
        {
            _spu_RXX->cd_vol.left = pAttr->cd.volume.left;
        }

        if (pAttr->mask || (pAttr->mask & 0x80) != 0)
        {
            _spu_RXX->cd_vol.right = pAttr->cd.volume.right;
        }

        if (pAttr->mask || (pAttr->mask & 0x400) != 0)
        {
            _spu_RXX->ex_vol.left = pAttr->ext.volume.left;
        }

        if (pAttr->mask || (pAttr->mask & 0x800) != 0)
        {
            _spu_RXX->ex_vol.right = pAttr->ext.volume.right;
        }

        if (pAttr->mask || (pAttr->mask & 0x100) != 0)
        {
            if (pAttr->cd.reverb)
            {
                _spu_RXX->spucnt |= 4;
            }
            else
            {
                _spu_RXX->spucnt &= ~4;
            }
        }

        if (pAttr->mask || (pAttr->mask & 0x200) != 0)
        {
            if (pAttr->cd.mix)
            {
                _spu_RXX->spucnt |= 1;
            }
            else
            {
                _spu_RXX->spucnt &= ~1;
            }
        }

        if (pAttr->mask || (pAttr->mask & 0x1000) != 0)
        {
            if (pAttr->ext.reverb)
            {
                _spu_RXX->spucnt |= 8;
            }
            else
            {
                _spu_RXX->spucnt &= ~8;
            }
        }

        if (pAttr->mask == 0 || (pAttr->mask & 0x2000) != 0)
        {
            if (pAttr->ext.mix)
            {
                _spu_RXX->spucnt |= 2;
            }
            else
            {
                _spu_RXX->spucnt &= ~2;
            }
        }
    }

    void SpuSetKey(long on_off, unsigned long voice_bit)
    {
        u32 voice_hit;

        voice_bit &= 0xFFFFFF;
        voice_hit = voice_bit >> 16;

        if (on_off != SPU_OFF)
        {
            if (_spu_env & 1)
            {
                _spu_RQ[0] = voice_bit;
                _spu_RQ[1] = voice_hit;
                _spu_RQmask |= 1;
                _spu_RQvoice |= voice_bit;

                if (_spu_RQ[2] & voice_bit)
                {
                    _spu_RQ[2] &= ~voice_bit;
                }

                if (_spu_RQ[3] & voice_hit)
                {
                    _spu_RQ[3] &= ~voice_hit;
                }
            }
            else
            {
                s->key_on[0] = voice_bit;
                s->key_on[1] = voice_hit;
                _spu_keystat |= voice_bit;
            }
        }
        else
        {
            if (_spu_env & 1)
            {
                _spu_RQ[2] = voice_bit;
                _spu_RQ[3] = voice_hit;
                _spu_RQmask |= 1;
                _spu_RQvoice &= ~voice_bit;

                if (_spu_RQ[0] & voice_bit)
                {
                    _spu_RQ[0] &= ~voice_bit;
                }

                if (_spu_RQ[1] & voice_hit)
                {
                    _spu_RQ[1] &= ~voice_hit;
                }
            }
            else
            {
                s->key_off[0] = voice_bit;
                s->key_off[1] = voice_hit;
                _spu_keystat &= ~voice_bit;
            }
        }
    }

    u16 _spu_note2pitch(u8 cenNoteHi, u8 cenNoteLo, u8 noteHi, u8 noteLo);

    void SpuSetVoiceAttr(SpuVoiceAttr *pAttr)
    {
        int voice_num;                 // $s4
        unsigned int attr_mask;        // $s1
        u16 *pCentreNoteIter;        // $s5
        int converted_voice_num;       // $s3
        int vol_left_upper;            // $a0
        short vol_left_clamped;      // $a1
        int vol_left;                  // $v1
        int vol_right_upper;           // $a0
        short vol_right_clamped;     // $a1
        int vol_right;                 // $v1
        int voice_num_;                // $v0
        int voice_num__;               // $v0
        unsigned int attr_ar;          // $a1
        short adsr_ar_part;          // $a2
        unsigned int adsr_dr_part;     // $a1
        unsigned int adsr_sr_part;     // $a1
        short converted_s_mode;      // $a2
        int attr_s_mode;               // $v1
        unsigned int adsr_rr_part;     // $a1
        short attr_r_mode_converted; // $a2
        int attr_r_mode;               // $v1
        unsigned int attr_sl;          // $a1
        int i;                         // [sp+10h] [-8h]
        int time_waster;               // [sp+14h] [-4h]

        voice_num = 0;
        attr_mask = pAttr->mask;
        pCentreNoteIter = _spu_voice_centerNote;
        do
        {
            if ((pAttr->voice & (1 << voice_num)) != 0)
            {
                converted_voice_num = 8 * voice_num;
                if (!attr_mask || (attr_mask & 0x10) != 0)
                {
                    _spu_RXX->voice[voice_num].pitch = pAttr->pitch;
                }

                if (!attr_mask || (attr_mask & 0x40) != 0)
                {
                    *pCentreNoteIter = pAttr->sample_note;
                }

                if (!attr_mask || (attr_mask & 0x20) != 0)
                {
                    _spu_RXX->voice[voice_num].pitch = _spu_note2pitch(
                        (*pCentreNoteIter) >> 8 & 0xFF,
                        (u8)*pCentreNoteIter,
                        pAttr->note >> 8 & 0xFF,
                        (u8)pAttr->note);
                }

                if (!attr_mask || (attr_mask & 1) != 0)
                {
                    vol_left_upper = 0;
                    vol_left_clamped = pAttr->volume.left & 0x7FFF;
                    if (!attr_mask || (attr_mask & 4) != 0)
                    {
                        switch (pAttr->volmode.left)
                        {
                        case 1:
                            vol_left_upper = 0x8000;
                            break;
                        case 2:
                            vol_left_upper = 0x9000;
                            break;
                        case 3:
                            vol_left_upper = 0xA000;
                            break;
                        case 4:
                            vol_left_upper = 0xB000;
                            break;
                        case 5:
                            vol_left_upper = 0xC000;
                            break;
                        case 6:
                            vol_left_upper = 0xD000;
                            break;
                        case 7:
                            vol_left_upper = 0xE000;
                            break;
                        default:
                            break;
                        }
                    }
                    if (vol_left_upper)
                    {
                        vol_left = pAttr->volume.left;
                        if (vol_left < 128)
                        {
                            if (vol_left < 0)
                            {
                                vol_left_clamped = 0;
                            }
                        }
                        else
                        {
                            vol_left_clamped = 127;
                        }
                    }
                    _spu_RXX->voice[voice_num].volume.left = vol_left_clamped | vol_left_upper;
                }

                if (!attr_mask || (attr_mask & 2) != 0)
                {
                    vol_right_upper = 0;
                    vol_right_clamped = pAttr->volume.right & 0x7FFF;
                    if (!attr_mask || (attr_mask & 8) != 0)
                    {
                        switch (pAttr->volmode.right)
                        {
                        case 1:
                            vol_right_upper = 0x8000;
                            break;
                        case 2:
                            vol_right_upper = 0x9000;
                            break;
                        case 3:
                            vol_right_upper = 0xA000;
                            break;
                        case 4:
                            vol_right_upper = 0xB000;
                            break;
                        case 5:
                            vol_right_upper = 0xC000;
                            break;
                        case 6:
                            vol_right_upper = 0xD000;
                            break;
                        case 7:
                            vol_right_upper = 0xE000;
                            break;
                        default:
                            break;
                        }
                    }

                    if (vol_right_upper)
                    {
                        vol_right = pAttr->volume.right;
                        if (vol_right < 128)
                        {
                            if (vol_right < 0)
                            {
                                vol_right_clamped = 0;
                            }
                        }
                        else
                        {
                            vol_right_clamped = 127;
                        }
                    }
                    _spu_RXX->voice[voice_num].volume.right = vol_right_clamped | vol_right_upper;
                }

                if (!attr_mask || (attr_mask & 0x80) != 0)
                {
                    _spu_FsetRXXa(converted_voice_num | 3, pAttr->addr);
                }

                if (!attr_mask || (attr_mask & 0x10000) != 0)
                {
                    _spu_FsetRXXa(converted_voice_num | 7, pAttr->loop_addr);
                }

                voice_num_ = voice_num;
                if (!attr_mask || (voice_num_ = voice_num, (attr_mask & 0x20000) != 0))
                {
                    _spu_RXX->voice[voice_num_].adsr[0] = pAttr->adsr1;
                }

                voice_num__ = voice_num;
                if (!attr_mask || (voice_num__ = voice_num, (attr_mask & 0x40000) != 0))
                {
                    _spu_RXX->voice[voice_num__].adsr[1] = pAttr->adsr2;
                }

                if (!attr_mask || (attr_mask & 0x800) != 0)
                {
                    attr_ar = pAttr->ar;
                    if (attr_ar >= 128)
                    {
                        attr_ar = 127; // loword
                    }

                    adsr_ar_part = 0;
                    if ((!attr_mask || (attr_mask & 0x100) != 0) && pAttr->a_mode == 5)
                    {
                        adsr_ar_part = 128;
                    }
                    _spu_RXX->voice[voice_num].adsr[0] = (unsigned char)_spu_RXX->voice[voice_num].adsr[0] | (unsigned short)(((unsigned short)attr_ar | (unsigned short)adsr_ar_part) << 8);
                }

                if (!attr_mask || (attr_mask & 0x1000) != 0)
                {
                    adsr_dr_part = pAttr->dr;
                    if (adsr_dr_part >= 0x10)
                    {
                        adsr_dr_part = 15; // loword
                    }
                    _spu_RXX->voice[voice_num].adsr[0] = _spu_RXX->voice[voice_num].adsr[0] & 0xFF0F | (0x10 * adsr_dr_part);
                }

                if (!attr_mask || (attr_mask & 0x2000) != 0)
                {
                    adsr_sr_part = pAttr->sr;
                    if (adsr_sr_part >= 0x80)
                    {
                        adsr_sr_part = 127; //loword
                    }

                    converted_s_mode = 0x100;
                    if (!attr_mask || (attr_mask & 0x200) != 0)
                    {
                        attr_s_mode = pAttr->s_mode;
                        if (attr_s_mode == 5)
                        {
                            converted_s_mode = 0x200;
                        }
                        else if (attr_s_mode >= 6)
                        {
                            if (attr_s_mode == 7)
                            {
                                converted_s_mode = 0x300;
                            }
                        }
                        else if (attr_s_mode == 1)
                        {
                            converted_s_mode = 0;
                        }
                    }
                    _spu_RXX->voice[voice_num].adsr[1] = _spu_RXX->voice[voice_num].adsr[1] & 0x3F | (((unsigned short)adsr_sr_part | (unsigned short)converted_s_mode) << 6);
                }

                if (!attr_mask || (attr_mask & 0x4000) != 0)
                {
                    adsr_rr_part = pAttr->rr;
                    if (adsr_rr_part >= 0x20)
                    {
                        adsr_rr_part = 31; // loword
                    }

                    attr_r_mode_converted = 0;
                    if (!attr_mask || (attr_mask & 0x400) != 0)
                    {
                        attr_r_mode = pAttr->r_mode;
                        if (attr_r_mode != 3 && attr_r_mode == 7)
                        {
                            attr_r_mode_converted = 32;
                        }
                    }
                    _spu_RXX->voice[voice_num].adsr[1] = _spu_RXX->voice[voice_num].adsr[1] & 0xFFC0 | adsr_rr_part | attr_r_mode_converted;
                }

                if (!attr_mask || (attr_mask & 0x8000) != 0)
                {
                    attr_sl = pAttr->sl;
                    if (attr_sl >= 16)
                    {
                        attr_sl = 15; // loword
                    }
                    _spu_RXX->voice[voice_num].adsr[0] = _spu_RXX->voice[voice_num].adsr[0] & 0xFFF0 | attr_sl;
                }
            }
            ++voice_num;
            ++pCentreNoteIter;
        } while (voice_num < 24);

        time_waster = 1;
        for (i = 0; i < 2; ++i)
        {
            time_waster *= 13;
        }
    }

    struct rev_param_entry
    {
        u32 flags;
        u16 dAPF1;
        u16 dAPF2;
        u16 vIIR;
        u16 vCOMB1;
        u16 vCOMB2;
        u16 vCOMB3;
        u16 vCOMB4;
        u16 vWALL;
        u16 vAPF1;
        u16 vAPF2;
        u16 mLSAME;
        u16 mRSAME;
        u16 mLCOMB1;
        u16 mRCOMB1;
        u16 mLCOMB2;
        u16 mRCOMB2;
        u16 dLSAME;
        u16 dRSAME;
        u16 mLDIFF;
        u16 mRDIFF;
        u16 mLCOMB3;
        u16 mRCOMB3;
        u16 mLCOMB4;
        u16 mRCOMB4;
        u16 dLDIFF;
        u16 dRDIFF;
        u16 mLAPF1;
        u16 mRAPF1;
        u16 mLAPF2;
        u16 mRAPF2;
        u16 vLIN;
        u16 vRIN;
    };

    rev_param_entry _spu_rev_param[10] =
        {
            {0,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u
             },
            {0,
             125u,
             91u,
             28032u,
             21688u,
             48848u,
             0u,
             0u,
             47744u,
             22528u,
             21248u,
             1238u,
             819u,
             1008u,
             551u,
             884u,
             495u,
             820u,
             437u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             436u,
             310u,
             184u,
             92u,
             32768u,
             32768u},
            {0,
             51u,
             37u,
             28912u,
             20392u,
             48352u,
             17424u,
             49392u,
             39936u,
             21120u,
             20160u,
             996u,
             795u,
             932u,
             687u,
             882u,
             614u,
             796u,
             605u,
             604u,
             398u,
             559u,
             309u,
             466u,
             183u,
             399u,
             181u,
             180u,
             128u,
             76u,
             38u,
             32768u,
             32768u},
            {0,
             177u,
             127u,
             28912u,
             20392u,
             48352u,
             17680u,
             48880u,
             46272u,
             21120u,
             20160u,
             2308u,
             1899u,
             2084u,
             1631u,
             1954u,
             1558u,
             1900u,
             1517u,
             1516u,
             1070u,
             1295u,
             773u,
             1122u,
             695u,
             1071u,
             613u,
             612u,
             434u,
             256u,
             128u,
             32768u,
             32768u},
            {0,
             227u,
             169u,
             28512u,
             20392u,
             48352u,
             17680u,
             48880u,
             42624u,
             22144u,
             21184u,
             3579u,
             2904u,
             3337u,
             2620u,
             3033u,
             2419u,
             2905u,
             2266u,
             2265u,
             1513u,
             2028u,
             1200u,
             1775u,
             978u,
             1514u,
             797u,
             796u,
             568u,
             340u,
             170u,
             32768u,
             32768u},
            {0,
             421u,
             313u,
             24576u,
             20480u,
             19456u,
             47104u,
             48128u,
             49152u,
             24576u,
             23552u,
             5562u,
             4539u,
             5314u,
             4285u,
             4540u,
             3521u,
             4544u,
             3523u,
             3520u,
             2497u,
             3012u,
             1985u,
             2560u,
             1741u,
             2498u,
             1473u,
             1472u,
             1050u,
             628u,
             314u,
             32768u,
             32768u},
            {0,
             829u,
             561u,
             32256u,
             20480u,
             46080u,
             45056u,
             19456u,
             45056u,
             24576u,
             21504u,
             7894u,
             6705u,
             7444u,
             6203u,
             7106u,
             5810u,
             6706u,
             5615u,
             5614u,
             4181u,
             4916u,
             3885u,
             4598u,
             3165u,
             4182u,
             2785u,
             2784u,
             1954u,
             1124u,
             562u,
             32768u,
             32768u},
            {0,
             1u,
             1u,
             32767u,
             32767u,
             0u,
             0u,
             0u,
             33024u,
             0u,
             0u,
             8191u,
             4095u,
             4101u,
             5u,
             0u,
             0u,
             4101u,
             5u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             4100u,
             4098u,
             4u,
             2u,
             32768u,
             32768u},
            {0,
             1u,
             1u,
             32767u,
             32767u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             8191u,
             4095u,
             4101u,
             5u,
             0u,
             0u,
             4101u,
             5u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             0u,
             4100u,
             4098u,
             4u,
             2u,
             32768u,
             32768u},
            {0,
             23u,
             19u,
             28912u,
             20392u,
             48352u,
             17680u,
             48880u,
             34048u,
             24448u,
             21696u,
             881u,
             687u,
             741u,
             479u,
             688u,
             471u,
             856u,
             618u,
             470u,
             286u,
             301u,
             177u,
             287u,
             89u,
             416u,
             227u,
             88u,
             64u,
             40u,
             20u,
             32768u,
             32768u}};

    void _spu_setReverbAttr(rev_param_entry *pRevParamEntry)
    {
        const unsigned int flags = pRevParamEntry->flags;
        const unsigned int bFlagsAreZero = pRevParamEntry->flags == 0;

        if (!pRevParamEntry->flags || (pRevParamEntry->flags & 1) != 0)
        {
            _spu_RXX->dAPF1 = pRevParamEntry->dAPF1; // 0x1F801DC0
        }

        if (bFlagsAreZero || (flags & 2) != 0)
        {
            _spu_RXX->dAPF2 = pRevParamEntry->dAPF2;
        }

        if (bFlagsAreZero || (flags & 4) != 0)
        {
            _spu_RXX->vIIR = pRevParamEntry->vIIR;
        }

        if (bFlagsAreZero || (flags & 8) != 0)
        {
            _spu_RXX->vCOMB1 = pRevParamEntry->vCOMB1;
        }

        if (bFlagsAreZero || (flags & 0x10) != 0)
        {
            _spu_RXX->vCOMB2 = pRevParamEntry->vCOMB2;
        }

        if (bFlagsAreZero || (flags & 0x20) != 0)
        {
            _spu_RXX->vCOMB3 = pRevParamEntry->vCOMB3;
        }

        if (bFlagsAreZero || (flags & 0x40) != 0)
        {
            _spu_RXX->vCOMB4 = pRevParamEntry->vCOMB4;
        }

        if (bFlagsAreZero || (flags & 0x80) != 0)
        {
            _spu_RXX->vWALL = pRevParamEntry->vWALL;
        }

        if (bFlagsAreZero || (flags & 0x100) != 0)
        {
            _spu_RXX->vAPF1 = pRevParamEntry->vAPF1;
        }

        if (bFlagsAreZero || (flags & 0x200) != 0)
        {
            _spu_RXX->vAPF2 = pRevParamEntry->vAPF2;
        }

        if (bFlagsAreZero || (flags & 0x400) != 0)
        {
            _spu_RXX->mLSAME = pRevParamEntry->mLSAME;
        }

        if (bFlagsAreZero || (flags & 0x800) != 0)
        {
            _spu_RXX->mRSAME = pRevParamEntry->mRSAME;
        }

        if (bFlagsAreZero || (flags & 0x1000) != 0)
        {
            _spu_RXX->mLCOMB1 = pRevParamEntry->mLCOMB1;
        }

        if (bFlagsAreZero || (flags & 0x2000) != 0)
        {
            _spu_RXX->mRCOMB1 = pRevParamEntry->mRCOMB1;
        }

        if (bFlagsAreZero || (flags & 0x4000) != 0)
        {
            _spu_RXX->mLCOMB2 = pRevParamEntry->mLCOMB2;
        }

        if (bFlagsAreZero || (flags & 0x8000) != 0)
        {
            _spu_RXX->mRCOMB2 = pRevParamEntry->mRCOMB2;
        }

        if (bFlagsAreZero || (flags & 0x10000) != 0)
        {
            _spu_RXX->dLSAME = pRevParamEntry->dLSAME;
        }

        if (bFlagsAreZero || (flags & 0x20000) != 0)
        {
            _spu_RXX->dRSAME = pRevParamEntry->dRSAME;
        }

        if (bFlagsAreZero || (flags & 0x40000) != 0)
        {
            _spu_RXX->mLDIFF = pRevParamEntry->mLDIFF;
        }

        if (bFlagsAreZero || (flags & 0x80000) != 0)
        {
            _spu_RXX->mRDIFF = pRevParamEntry->mRDIFF;
        }

        if (bFlagsAreZero || (flags & 0x100000) != 0)
        {
            _spu_RXX->mLCOMB3 = pRevParamEntry->mLCOMB3;
        }

        if (bFlagsAreZero || (flags & 0x200000) != 0)
        {
            _spu_RXX->mRCOMB3 = pRevParamEntry->mRCOMB3;
        }

        if (bFlagsAreZero || (flags & 0x400000) != 0)
        {
            _spu_RXX->mLCOMB4 = pRevParamEntry->mLCOMB4;
        }

        if (bFlagsAreZero || (flags & 0x800000) != 0)
        {
            _spu_RXX->mRCOMB4 = pRevParamEntry->mRCOMB4;
        }

        if (bFlagsAreZero || (flags & 0x1000000) != 0)
        {
            _spu_RXX->dLDIFF = pRevParamEntry->dLDIFF;
        }

        if (bFlagsAreZero || (flags & 0x2000000) != 0)
        {
            _spu_RXX->dRDIFF = pRevParamEntry->dRDIFF;
        }

        if (bFlagsAreZero || (flags & 0x4000000) != 0)
        {
            _spu_RXX->mLAPF1 = pRevParamEntry->mLAPF1;
        }

        if (bFlagsAreZero || (flags & 0x8000000) != 0)
        {
            _spu_RXX->mRAPF1 = pRevParamEntry->mRAPF1;
        }

        if (bFlagsAreZero || (flags & 0x10000000) != 0)
        {
            _spu_RXX->mLAPF2 = pRevParamEntry->mLAPF2;
        }

        if (bFlagsAreZero || (flags & 0x20000000) != 0)
        {
            _spu_RXX->mRAPF2 = pRevParamEntry->mRAPF2;
        }

        if (bFlagsAreZero || (flags & 0x40000000) != 0)
        {
            _spu_RXX->vLIN = pRevParamEntry->vLIN;
        }

        if (bFlagsAreZero || (flags & 0x80000000) != 0)
        {
            _spu_RXX->vRIN= pRevParamEntry->vRIN;
        }
    }

    long SpuSetReverbModeParam(SpuReverbAttr *pAttr)
    {
        rev_param_entry entry;
        entry.flags = 0;

        bool b_rModeInBounds = false; // TODO: Bad name, means something else
        bool bClearReverbWorkArea = false;
        if (pAttr->mask == 0 || (pAttr->mask & 1) != 0)
        {
            long r_mode = pAttr->mode;
            if ((r_mode & 0x100) != 0)
            {
                r_mode &= ~0x100u;
                bClearReverbWorkArea = true;
            }
            
            if (r_mode >= 10)
            {
                return -1;
            }

            b_rModeInBounds = true;

            if (_SpuIsInAllocateArea(_spu_rev_startaddr[r_mode]))
            {
                return -1;
            }

            _spu_rev_attr.mode = r_mode;
            entry = _spu_rev_param[_spu_rev_attr.mode];

            _spu_rev_offsetaddr = _spu_rev_startaddr[_spu_rev_attr.mode];

            if (_spu_rev_attr.mode == 7)
            {
                _spu_rev_attr.feedback = 127;
                _spu_rev_attr.delay = 127;
            }
            else
            {
                _spu_rev_attr.feedback = 0;
                if (_spu_rev_attr.mode == 8)
                {
                    _spu_rev_attr.delay = 127;
                }
                else
                {
                    _spu_rev_attr.delay = 0;
                }
            }
        }

        bool bModeIs7to9_bit0x8 = false;
        if ((pAttr->mask == 0 || (pAttr->mask & 8) != 0) && _spu_rev_attr.mode < 9 && _spu_rev_attr.mode >= 7)
        {
            bModeIs7to9_bit0x8 = true;
            if (!b_rModeInBounds)
            {
                entry = _spu_rev_param[_spu_rev_attr.mode];
                entry.flags = 0xC011C00;
            }

            _spu_rev_attr.delay = pAttr->delay;
            entry.mLSAME = ((_spu_rev_attr.delay & 0xFFFF) << 13) / 127 - entry.dAPF1;
            const u16 delay_converted = (_spu_rev_attr.delay << 12) / 127;
            entry.mRSAME = delay_converted - entry.dAPF2;
            entry.dLSAME = entry.dRSAME + delay_converted;
            entry.mLCOMB1 = entry.mRCOMB1 + delay_converted;
            entry.mRAPF1 = entry.mRAPF2 + delay_converted;
            entry.mLAPF1 = entry.mLAPF2 + delay_converted;
        }

        bool bModeIs7to9_bit0x10 = false;
        if ((pAttr->mask == 0 || (pAttr->mask & 0x10) != 0) && _spu_rev_attr.mode < 9 && _spu_rev_attr.mode >= 7)
        {
            bModeIs7to9_bit0x10 = true;
            if (!b_rModeInBounds)
            {
                u32 flagsTmp = 0;
                if (bModeIs7to9_bit0x8)
                {
                    flagsTmp = entry.flags | 0x80;
                }
                else
                {
                    entry = _spu_rev_param[_spu_rev_attr.mode];
                    flagsTmp = 0x80;
                }
                entry.flags = flagsTmp;
            }
            _spu_rev_attr.feedback = pAttr->feedback;
            entry.vWALL = (0x8100 * _spu_rev_attr.feedback) / 127;
        }

        bool bSet_spucnt = false;
        if (b_rModeInBounds)
        {
            bSet_spucnt = ((_spu_RXX->spucnt >> 7) & 1) ? true : false;
            if (bSet_spucnt)
            {
                _spu_RXX->spucnt &= ~0x80u;
            }
            _spu_RXX->rev_vol.left = 0;
            _spu_RXX->rev_vol.right = 0;
            _spu_rev_attr.depth.left = 0;
            _spu_rev_attr.depth.right = 0;
        }
        else
        {
            if (pAttr->mask == 0 || (pAttr->mask & 2) != 0)
            {
                _spu_RXX->rev_vol.left = pAttr->depth.left;
                _spu_rev_attr.depth.left = pAttr->depth.left;
            }

            if (pAttr->mask == 0 || (pAttr->mask & 4) != 0)
            {
                _spu_RXX->rev_vol.right = pAttr->depth.right;
                _spu_rev_attr.depth.right = pAttr->depth.right;
            }
        }

        if (b_rModeInBounds || bModeIs7to9_bit0x8 || bModeIs7to9_bit0x10)
        {
            _spu_setReverbAttr(&entry);
        }

        if (bClearReverbWorkArea)
        {
            SpuClearReverbWorkArea(_spu_rev_attr.mode);
        }

        if (b_rModeInBounds)
        {
            _spu_FsetRXX(209, _spu_rev_offsetaddr, 0);
            if (bSet_spucnt)
            {
                _spu_RXX->spucnt |= 0x80u;
            }
        }

        return 0;
    }

    // TODO
    void _spu_gcSPU(void);

    long SpuMalloc(long size)
    {
        unsigned int rev_size_zero = 0;
        if (_spu_rev_reserve_wa)
        {
            rev_size_zero = (0x10000 - _spu_rev_offsetaddr) << _spu_mem_mode_plus;
        }
        else
        {
            rev_size_zero = 0;
        }

        int size_adjusted = size;
        if ((size & ~_spu_mem_mode_unitM) != 0)
        {
            size_adjusted = size + _spu_mem_mode_unitM;
        }

        const u32 calc_alloc_size = size_adjusted >> _spu_mem_mode_plus << _spu_mem_mode_plus;
        
        int found_block_idx = -1;
        if ((_spu_memList->field_0_addr & 0x40000000) != 0)
        {
            found_block_idx = 0;
        }
        else
        {
            _spu_gcSPU();

            if (_spu_AllocBlockNum > 0)
            {
                s32 cur_idx = 0;
                SPU_MALLOC* pListIter = _spu_memList;
                while ((pListIter->field_0_addr & 0x40000000) == 0 && ((pListIter->field_0_addr & 0x80000000) == 0 || pListIter->field_4_size < calc_alloc_size))
                {
                    ++cur_idx;
                    ++pListIter;
                    if (cur_idx >= _spu_AllocBlockNum)
                    {
                        goto out_of_blocks;
                    }
                }
                found_block_idx = cur_idx;
            }
        }

    out_of_blocks:
        long pAllocated = -1;

        if (found_block_idx != -1)
        {
            if ((_spu_memList[found_block_idx].field_0_addr & 0x40000000) != 0)
            {
                if (found_block_idx < (int)_spu_AllocBlockNum)
                {
                    if (_spu_memList[found_block_idx].field_4_size - rev_size_zero >= calc_alloc_size)
                    {
                        _spu_AllocLastNum = found_block_idx + 1;

                        SPU_MALLOC* pLastBlock = &_spu_memList[_spu_AllocLastNum];
                        pLastBlock->field_0_addr = ((_spu_memList[found_block_idx].field_0_addr & 0xFFFFFFF) + calc_alloc_size) | 0x40000000;
                        pLastBlock->field_4_size = _spu_memList[found_block_idx].field_4_size - calc_alloc_size;

                        _spu_memList[found_block_idx].field_4_size = calc_alloc_size;
                        _spu_memList[found_block_idx].field_0_addr &= 0xFFFFFFF;

                        _spu_gcSPU();

                        pAllocated = _spu_memList[found_block_idx].field_0_addr;
                    }
                }
            }
            else
            {
                if (calc_alloc_size < _spu_memList[found_block_idx].field_4_size)
                {
                    const u32 pAllocEndAddr = _spu_memList[found_block_idx].field_0_addr + calc_alloc_size;
                    if (_spu_AllocLastNum < _spu_AllocBlockNum)
                    {
                        const u32 last_addr = _spu_memList[_spu_AllocLastNum].field_0_addr;
                        const u32 last_alloc_size = _spu_memList[_spu_AllocLastNum].field_4_size;

                        _spu_memList[_spu_AllocLastNum].field_0_addr = pAllocEndAddr | 0x80000000;
                        _spu_memList[_spu_AllocLastNum].field_4_size = _spu_memList[found_block_idx].field_4_size - calc_alloc_size;
                        
                        _spu_AllocLastNum++;
                        _spu_memList[_spu_AllocLastNum].field_0_addr = last_addr;
                        _spu_memList[_spu_AllocLastNum].field_4_size = last_alloc_size;
                    }
                }

                _spu_memList[found_block_idx].field_4_size = calc_alloc_size;
                _spu_memList[found_block_idx].field_0_addr &= 0xFFFFFFF;

                _spu_gcSPU();

                pAllocated = _spu_memList[found_block_idx].field_0_addr;
            }

        }
        return pAllocated;
    }
}

void _spu_gcSPU(void)
{
    int last_alloc_idx;           // $v0
    int counter;                  // $t1
    SPU_MALLOC *pMemList;         // $t0
    int last_alloc_idx_;          // $t5
    SPU_MALLOC *pMemList_Iter;    // $a3
    int list_idx;                 // $a2
    SPU_MALLOC *pCurBlock;        // $v1
    bool bIsntMagicAddr;          // dc
    SPU_MALLOC *pCurBlock_;       // $a1
    int counter_;                 // $t1
    SPU_MALLOC *pMemList__;       // $v1
    int last_alloc_idx__;         // $v1
    int counter__;                // $t1
    SPU_MALLOC *pMemList___;      // $t5
    SPU_MALLOC *pMemListIter_;    // $t2
    int counter_next;             // $a2
    int last_alloc_idx___;        // $t3
    SPU_MALLOC *pNextBlock_;      // $a0
    int mem_addr;                 // $a3
    int mem_size;                 // $v1
    int last_alloc_idx____;       // $a1
    int idx;                      // $t1
    SPU_MALLOC *pMemListIter;     // $a0
    SPU_MALLOC *pCurBlock__;      // $v0
    SPU_MALLOC *pPrevBlock;       // $a0

    last_alloc_idx = _spu_AllocLastNum;
    counter = 0;
    if (_spu_AllocLastNum >= 0)
    {
        pMemList = _spu_memList;
        last_alloc_idx_ = _spu_AllocLastNum;
        pMemList_Iter = _spu_memList;
        do
        {
            list_idx = counter + 1;
            if ((pMemList_Iter->field_0_addr & 0x80000000) == 0)
            {
                goto next_item;
            }

            pCurBlock = &pMemList[list_idx];
            while (1)
            {
                bIsntMagicAddr = pCurBlock->field_0_addr != 0x2FFFFFFF;
                ++pCurBlock;
                if (bIsntMagicAddr)
                {
                    break;
                }
                ++list_idx;
            }
            pCurBlock_ = &pMemList[list_idx];
            if ((pCurBlock_->field_0_addr & 0x80000000) != 0 && (pCurBlock_->field_0_addr & 0xFFFFFFF) == (pMemList_Iter->field_0_addr & 0xFFFFFFF) + pMemList_Iter->field_4_size)
            {
                pCurBlock_->field_0_addr = 0x2FFFFFFF;
                pMemList_Iter->field_4_size += pCurBlock_->field_4_size;
            }
            else
            {
            next_item:
                ++pMemList_Iter;
                ++counter;
            }
        } while (last_alloc_idx_ >= counter);
        last_alloc_idx = _spu_AllocLastNum;
    }

    counter_ = 0;
    if (last_alloc_idx >= 0)
    {
        pMemList__ = _spu_memList;
        do
        {
            if (!pMemList__->field_4_size)
            {
                pMemList__->field_0_addr = 0x2FFFFFFF;
            }
            ++counter_;
            ++pMemList__;
        } while (last_alloc_idx >= counter_);
    }

    last_alloc_idx__ = _spu_AllocLastNum;
    counter__ = 0;
    if (_spu_AllocLastNum >= 0)
    {
        pMemList___ = _spu_memList;
        pMemListIter_ = _spu_memList;
        do
        {
            if ((pMemListIter_->field_0_addr & 0x40000000) != 0)
            {
                break;
            }
            counter_next = counter__ + 1;
            if (last_alloc_idx__ >= counter__ + 1)
            {
                last_alloc_idx___ = _spu_AllocLastNum;
                pNextBlock_ = &pMemList___[counter__ + 1];
                do
                {
                    if ((pNextBlock_->field_0_addr & 0x40000000) != 0)
                    {
                        break;
                    }

                    mem_addr = pMemListIter_->field_0_addr;
                    if ((pNextBlock_->field_0_addr & 0xFFFFFFFu) < (pMemListIter_->field_0_addr & 0xFFFFFFFu))
                    {
                        pMemListIter_->field_0_addr = pNextBlock_->field_0_addr;
                        mem_size = pMemListIter_->field_4_size;
                        pMemListIter_->field_4_size = pNextBlock_->field_4_size;
                        pNextBlock_->field_0_addr = mem_addr;
                        pNextBlock_->field_4_size = mem_size;
                    }
                    ++counter_next;
                    ++pNextBlock_;
                } while (last_alloc_idx___ >= counter_next);
            }
            last_alloc_idx__ = _spu_AllocLastNum;
            ++counter__;
            ++pMemListIter_;
        } while (_spu_AllocLastNum >= counter__);
    }

    last_alloc_idx____ = _spu_AllocLastNum;
    idx = 0;
    if (_spu_AllocLastNum >= 0)
    {
        pMemListIter = _spu_memList;
        while ((pMemListIter->field_0_addr & 0x40000000) == 0) // not last entry
        {
            if (pMemListIter->field_0_addr == 0x2FFFFFFF)
            {
                pCurBlock__ = &_spu_memList[last_alloc_idx____];
                pMemListIter->field_0_addr = pCurBlock__->field_0_addr;
                pMemListIter->field_4_size = pCurBlock__->field_4_size;
                _spu_AllocLastNum = idx;
                break;
            }
            last_alloc_idx____ = _spu_AllocLastNum;
            ++idx;
            ++pMemListIter;
            if (_spu_AllocLastNum < idx)
            {
                break;
            }
        }
    }

    // Merged tail unused blocks
    if (_spu_AllocLastNum - 1 >= 0)
    {
        pPrevBlock = &_spu_memList[_spu_AllocLastNum - 1];
        do
        {
            if ((pPrevBlock->field_0_addr & 0x80000000) == 0)
            {
                break;
            }
            // Found unused block, merge it and set as last entry
            pPrevBlock->field_0_addr = pPrevBlock->field_0_addr & 0xFFFFFFF | 0x40000000;
            pPrevBlock->field_4_size += _spu_memList[_spu_AllocLastNum].field_4_size;
            _spu_AllocLastNum--;
            pPrevBlock--;
        } while (_spu_AllocLastNum >= 0);
    }
}

static volatile unsigned long* _GetVoiceImpl(int word_idx1, int word_idx2)
{
    if (word_idx1 == 204 && word_idx2 == 205)
    {
        // 0x1F801D98 4  Voice 0..23 Channel Reverb mode (R/W)
        return (volatile unsigned long*)0x1F801D98;
    }
    else if (word_idx1 == 202 && word_idx2 == 203)
    {
        // 0x1F801D94 4  Voice 0..23 Channel Noise mode (R/W)
        return (volatile unsigned long*)0x1F801D94;
    }
    else
    {
        printf("Unknown reg combo\n");
        while(1) {}
    }
}

unsigned long _SpuGetAnyVoice(int word_idx1, int word_idx2)
{
    return (*_GetVoiceImpl(word_idx1, word_idx2)) & 0xFFFFFF;
}

// note: partial impl cause _spu_env stuff ignored as its never set in the funcs I care about
unsigned long _SpuSetAnyVoice(long on_off_flags, unsigned long voice_bits, int word_idx1, int word_idx2)
{
    volatile unsigned long* pRegister = _GetVoiceImpl(word_idx1, word_idx2);

    // Note: _spu_env branch removed
    unsigned long ret_bits = *pRegister;

    if (on_off_flags == 1)
    {
        // Note: _spu_env branch removed
        *pRegister |= voice_bits & 0xFFFFFF;
        ret_bits |= voice_bits & 0xFFFFFF;
    }
    else if (on_off_flags >= 2)
    {
        if (on_off_flags == 8)
        {
            // Note: _spu_env branch removed
            *pRegister = voice_bits & 0xFFFFFF;
            ret_bits = voice_bits & 0xFFFFFF;
        }
    }
    else if (on_off_flags == 0)
    {
        // Note: _spu_env branch removed
        *pRegister = ~(voice_bits & 0xFFFFFF);
        ret_bits &= ~(voice_bits & 0xFFFFFF);
    }
    return ret_bits & 0xFFFFFF;
}

const signed short int word_8001D6F8[12] =
{
  32768,
  -30820,
  -28756,
  -26569,
  -24251,
  -21796,
  -19196,
  -16440,
  -13521,
  -10428,
  -7151,
  -3679
};

const signed short int word_8001D710[128] =
{
  32768, // -1 ??
  -32754,
  -32739,
  -32724,
  -32709,
  -32694,
  -32680,
  -32665,
  -32650,
  -32635,
  -32620,
  -32605,
  -32591,
  -32576,
  -32561,
  -32546,
  -32531,
  -32516,
  -32501,
  -32486,
  -32471,
  -32456,
  -32442,
  -32427,
  -32412,
  -32397,
  -32382,
  -32367,
  -32352,
  -32337,
  -32322,
  -32307,
  -32292,
  -32277,
  -32262,
  -32247,
  -32232,
  -32217,
  -32202,
  -32187,
  -32172,
  -32157,
  -32142,
  -32126,
  -32111,
  -32096,
  -32081,
  -32066,
  -32051,
  -32036,
  -32021,
  -32006,
  -31990,
  -31975,
  -31960,
  -31945,
  -31930,
  -31915,
  -31900,
  -31884,
  -31869,
  -31854,
  -31839,
  -31824,
  -31808,
  -31793,
  -31778,
  -31763,
  -31747,
  -31732,
  -31717,
  -31702,
  -31686,
  -31671,
  -31656,
  -31640,
  -31625,
  -31610,
  -31595,
  -31579,
  -31564,
  -31549,
  -31533,
  -31518,
  -31503,
  -31487,
  -31472,
  -31456,
  -31441,
  -31426,
  -31410,
  -31395,
  -31379,
  -31364,
  -31349,
  -31333,
  -31318,
  -31302,
  -31287,
  -31271,
  -31256,
  -31240,
  -31225,
  -31209,
  -31194,
  -31178,
  -31163,
  -31147,
  -31132,
  -31116,
  -31101,
  -31085,
  -31070,
  -31054,
  -31039,
  -31023,
  -31008,
  -30992,
  -30976,
  -30961,
  -30945,
  -30930,
  -30914,
  -30898,
  -30883,
  -30867,
  -30851,
  -30836
};

u16 _spu_note2pitch(u8 cenNoteHi, u8 cenNoteLo, u8 noteHi, u8 noteLo)
{
    unsigned short int maxLo;  // $a3
    int calc_note;           // $a0
    unsigned short int v2_idx; // $a3
    int calc_note_div12;     // $a1
    int calc_note_div12_m2;  // $a2
    int calc_note_mod12;     // $a0
    int v1_idx;              // $v1
    unsigned int calc_pitch; // $a1

    maxLo = noteLo + cenNoteLo;
    calc_note = (u16)(noteHi + (maxLo >> 7) - cenNoteHi);
    v2_idx = maxLo & 127;
    calc_note_div12 = calc_note / 12;
    calc_note_div12_m2 = calc_note / 12 - 2;
    calc_note_mod12 = calc_note % 12;
    v1_idx = calc_note_mod12;

    if (calc_note_mod12 < 0)
    {
        v1_idx = calc_note_mod12 + 12;
        calc_note_div12_m2 = calc_note_div12 - 3;
    }
 
    if (calc_note_div12_m2 < 0)
    {
        unsigned int tmp =  (1 << (-(signed short int)calc_note_div12_m2 - 1)) >> -(signed short int)calc_note_div12_m2;
        calc_pitch = word_8001D6F8[v1_idx] * word_8001D710[v2_idx] +  tmp;
    }
    else
    {
        calc_pitch = 0x3FFF;
    }
    return calc_pitch;
}