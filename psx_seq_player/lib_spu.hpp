#ifndef _LIB_SPU_HPP_
#define _LIB_SPU_HPP_

extern "C"
{
    #define SPU_TRANSFER_BY_DMA 0L
    #define SPU_TRANSFER_BY_IO 1L

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

    extern long SpuClearReverbWorkArea (long mode);

    extern void SpuSetCommonAttr (SpuCommonAttr *attr);
    extern long SpuIsTransferCompleted (long flag);

    void SpuInit(void);

    void SpuSetReverb(long on_off);

    short _spu_getInTransfer(void);
    void _spu_setInTransfer(short);

    long SpuMalloc(long size);
    void SpuFree(unsigned long addr);

    long SpuSetTransferMode(long mode);
    unsigned long SpuSetTransferStartAddr (unsigned long addr);
    unsigned long SpuWrite (unsigned char *addr, unsigned long size);
}

#endif // _LIB_SPU_HPP_
