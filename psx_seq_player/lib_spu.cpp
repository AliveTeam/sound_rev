#include "lib_spu.hpp"

#include <sys/types.h>
#include <libapi.h>
#include <libetc.h> // ResetCallBack
#include <stdarg.h> // va_list

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
    LIBVAR u32 _spu_AllocBlockNum = 0;
    LIBVAR u32 _spu_AllocLastNum = 0;
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
    } SPU_RXX;

    typedef struct tagSpuMalloc
    {
        u32 v0, v1;
    } SPU_MALLOC;

    // Forward declares
    int _SpuIsInAllocateArea_(u32 addr);
    void _SpuInit(int mode);

    int  _spu_init();
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
        pTop->v0 = 0x40001010;
        pTop->v1 = (0x10000 << _spu_mem_mode_plus) - 0x1010;

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
            ptr = pA->v0;
            // hit last entry
            if (ptr & 0x40000000) break;
            // hit unused entry, skip it
            if (ptr & 0x80000000) continue;

            ptr &= 0xFFFFFF;
            if (addr > ptr && addr < pA->v1 + ptr)
                return 1;
        }

        return 0;
    }

    // S_M_UTIL.OBJ
    int _SpuIsInAllocateArea_(u32 addr)
    {
        u32 pos, ptr;
        SPU_MALLOC *pA = _spu_memList;

        if (!pA) return 0;

        pos = addr << _spu_mem_mode_plus;

        for (;; pA++)
        {
            ptr = pA->v0;
            // hit last entry
            if (ptr & 0x40000000) break;
            // hit unused entry, skip it
            if (ptr & 0x80000000) continue;

            ptr &= 0xFFFFFF;
            if (pos > ptr && pos < pA->v1 + ptr)
                return 1;
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
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
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
        extern u8 _spu_zerobuf[];
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

    // TODO
    extern void SpuSetCommonAttr (SpuCommonAttr *attr);
    void SpuSetReverb(long on_off);
    long SpuMalloc(long size);
    void SpuFree(unsigned long addr);
    long SpuSetTransferMode(long mode);
    extern void SpuSetVoiceAttr (SpuVoiceAttr *arg);
    extern void SpuSetKey (long on_off, unsigned long voice_bit);

}
