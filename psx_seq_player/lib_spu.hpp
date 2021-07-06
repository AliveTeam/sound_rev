#ifndef _LIB_SPU_HPP_
#define _LIB_SPU_HPP_

extern "C"
{
    #define	SPU_COMMON_MVOLL		(0x01 <<  0) /* master volume (left) */
    #define	SPU_COMMON_MVOLR		(0x01 <<  1) /* master volume (right) */

    typedef struct {
        short left;	       /* Lch */
        short right;       /* Rch */
    } SpuVolume;

    typedef struct {
        SpuVolume	volume;		  /* volume       */
        long	reverb;		  /* reverb on/off */
        long	mix;		  /* mixing on/off */
    } SpuExtAttr;

    typedef struct {
        unsigned long	mask;	  /* settings mask */
        
        SpuVolume		mvol;	  /* master volume */
        SpuVolume		mvolmode; /* master volume mode */
        SpuVolume		mvolx;	  /* current master volume */
        SpuExtAttr		cd;	  /* CD input attributes */
        SpuExtAttr		ext;	  /* external digital input attributes */
    } SpuCommonAttr;

    typedef struct ProgAtr {        /* Program Headdings */

        unsigned char tones;          /* # of tones */
        unsigned char mvol;           /* program volume */
        unsigned char prior;          /* program priority */
        unsigned char mode;           /* program mode */
        unsigned char mpan;           /* program pan */
        char          reserved0;      /* system reserved */
        short         attr;           /* program attribute */
        unsigned long reserved1;      /* system reserved */
        unsigned long reserved2;      /* system reserved */

    } ProgAtr;			/* 16 byte */

    extern long SpuClearReverbWorkArea (long mode);

    extern void SpuSetCommonAttr (SpuCommonAttr *attr);
    extern long SpuIsTransferCompleted (long flag);

    void SpuInit(void);

    void SpuSetReverb(long on_off);

    short _spu_getInTransfer(void);
    void _spu_setInTransfer(short);

    void SpuFree(unsigned long addr);
}

#endif // _LIB_SPU_HPP_
