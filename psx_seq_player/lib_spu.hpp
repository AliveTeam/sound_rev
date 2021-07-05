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

    extern long SpuClearReverbWorkArea (long mode);

    extern void SpuSetCommonAttr (SpuCommonAttr *attr);
    extern long SpuIsTransferCompleted (long flag);


}

#endif // _LIB_SPU_HPP_
