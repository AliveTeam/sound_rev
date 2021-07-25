#ifndef _LIB_SPU_HPP_
#define _LIB_SPU_HPP_

#ifdef __cplusplus
extern "C"
{
#endif

    #define _countof(x) sizeof(x) / sizeof(x[0])

    #define SPU_TRANSFER_BY_DMA 0L
    #define SPU_TRANSFER_BY_IO 1L

    #define	SPU_COMMON_MVOLL		(0x01 <<  0) /* master volume (left) */
    #define	SPU_COMMON_MVOLR		(0x01 <<  1) /* master volume (right) */

    #define	SPU_REV_MODE		(0x01 <<  0) /* mode setting */
    #define	SPU_REV_DEPTHL		(0x01 <<  1) /* reverb depth (left) */
    #define	SPU_REV_DEPTHR		(0x01 <<  2) /* reverb depth (right) */
    #define	SPU_REV_DELAYTIME	(0x01 <<  3) /* Delay Time  (ECHO, DELAY only) */
    #define	SPU_REV_FEEDBACK	(0x01 <<  4) /* Feedback    (ECHO only) */

    #define SPU_REV_MODE_CHECK	(-1)
    #define SPU_REV_MODE_OFF	0
    #define SPU_REV_MODE_ROOM	1
    #define SPU_REV_MODE_STUDIO_A	2
    #define SPU_REV_MODE_STUDIO_B	3
    #define SPU_REV_MODE_STUDIO_C	4
    #define SPU_REV_MODE_HALL	5
    #define SPU_REV_MODE_SPACE	6
    #define SPU_REV_MODE_ECHO	7
    #define SPU_REV_MODE_DELAY	8
    #define SPU_REV_MODE_PIPE	9
    #define SPU_REV_MODE_MAX	10

    #define SPU_REV_MODE_CLEAR_WA	0x100
    
    #define SPU_MAX_TRANS	0x7EFF0		// ~512 KB

    
    #define SPU_EVENT_KEY      (0x01 << 0)
    #define SPU_EVENT_PITCHLFO (0x01 << 1)
    #define SPU_EVENT_NOISE    (0x01 << 2)
    #define SPU_EVENT_REVERB   (0x01 << 3)

    #define SPU_EVENT_ALL 0

    typedef unsigned int u32;
    typedef unsigned short int u16;
    typedef unsigned char u8;

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

    typedef struct {
        unsigned long	mask;	  /* settings mask */
        long            mode;	  /* reverb mode */
        SpuVolume       depth;	  /* reverb depth */
        long            delay;	  /* Delay Time  (ECHO, DELAY only)   */
        long            feedback; /* Feedback    (ECHO only)          */
    } SpuReverbAttr;

    typedef struct {
        unsigned long	voice;		/* set voice:
                                SpuSetVoiceAttr: each voice is a bit array
                                SpuGetVoiceAttr: voice is a bit value */
        unsigned long	mask;		/* settings attribute bit (invalid with Get) */
        SpuVolume		volume;		/* volume                         */
        SpuVolume		volmode;	/* volume mode                    */
        SpuVolume		volumex;	/* current volume (invalid with Set) */
        unsigned short	pitch;		/* tone (pitch setting) */
        unsigned short	note;		/* tone (note setting) */
        unsigned short	sample_note;	/* tone (note setting) */
        short		envx;		/* current envelope value (invalid with Set) */
        unsigned long	addr;		/* waveform data start address */
        unsigned long	loop_addr;	/* loop start address */
        long		a_mode;		/* Attack rate mode			*/
        long		s_mode;		/* Sustain rate mode			*/
        long		r_mode;		/* Release rate mode			*/
        unsigned short	ar;		/* Attack rate				*/
        unsigned short	dr;		/* Decay rate				*/
        unsigned short	sr;		/* Sustain rate				*/
        unsigned short	rr;		/* Release rate				*/
        unsigned short	sl;		/* Sustain level			*/
        unsigned short	adsr1;		/* adsr1 for `VagAtr' */
        unsigned short	adsr2;		/* adsr2 for `VagAtr' */
    } SpuVoiceAttr;

    extern long SpuClearReverbWorkArea (long mode);
    // _SpuIsInAllocateArea_
    // _spu_t

    extern void SpuSetCommonAttr (SpuCommonAttr *attr);
    // 

    extern long SpuIsTransferCompleted (long flag);
    // TestEvent
    
    void SpuInit(void);

    void SpuSetReverb(long on_off);

    long _spu_getInTransfer(void);
    void _spu_setInTransfer(int mode);

    extern long SpuInitMalloc (long num, char *top);
    long SpuMalloc(long size);
    void SpuFree(unsigned long addr);

    long SpuSetTransferMode(long mode);
    u32 SpuSetTransferStartAddr(u32 addr);

    u32 SpuWrite(u8* addr, u32 size);

    extern long SpuSetReverbModeParam (SpuReverbAttr *attr);

    
    extern void SpuSetVoiceAttr (SpuVoiceAttr *arg);
    extern void SpuGetVoiceEnvelope (int vNum, short *envx);
    extern unsigned long SpuSetNoiseVoice (long on_off, unsigned long voice_bit);
    extern unsigned long SpuSetReverbVoice (long on_off, unsigned long voice_bit);
    extern void SpuSetKey (long on_off, unsigned long voice_bit);
    extern unsigned long SpuGetReverbVoice (void);
    extern unsigned long SpuGetNoiseVoice (void);

    long SpuSetNoiseClock(long n_clock);

#ifdef __cplusplus
}
#endif

#endif // _LIB_SPU_HPP_
