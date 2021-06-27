/* Hardware register locations */

#ifndef HWREG_H
#define HWREG_H

/**********/
/* DELAYS */
/**********/

#define DV5_DELAY               ((PsxUInt32 *)0x1F801018)
#define COM_DELAY               ((PsxUInt32 *)0x1F801020)

/**************/
/* SERIAL I/O */
/**************/

#define SIO_DATA                ((PsxUInt8 *)0x1F801040)
#define SIO_STATUS              ((PsxUInt16 *)0x1F801044)
#define SIO_CONTROL             ((PsxUInt16 *)0x1F80104A)
#define SIO_BAUD                ((PsxUInt16 *)0x1F80104E)

/**********************/
/* MEMORY MANAGER ??? */
/**********************/

#define RAM_SIZE                ((PsxUInt32 *)0x1F801060)

/**************/
/* INTERRUPTS */
/**************/

/* Interrupt  0 - VSync */
/* Interrupt  1 - CD interrupt */
/* Interrupt  2 - ? */
/* Interrupt  3 - DMA finished */
/* Interrupt  4 - Root counter 0 */
/* Interrupt  5 - Root counter 1 */
/* Interrupt  6 - Root counter 2 */
/* Interrupt  7 - ? */
/* Interrupt  8 - ? */
/* Interrupt  9 - ? */
/* Interrupt 10 - ? */

/* Interrupt reg - indicates which of 11 interrupts are active */
#define INT_REG                 ((PsxUInt16 *)0x1F801070)
/* On read: */
/* 0x07FF - interrupt channel active bits */

/* Interrupt Mask - selects which of 11 interrupts are allowable */
#define INT_MASK                ((PsxUInt16 *)0x1F801074)
/* 0x07FF - interrupt channel enable bits */

/****************/
/* DMA CHANNELS */
/****************/

/* Channel 0 - 0x1F801080 - CPU->MDEC */
/* Channel 1 - 0x1F801090 - MDEC->CPU */
/* Channel 2 - 0x1F8010A0 - CPU->GPU and GPU->CPU */
/* Channel 3 - 0x1F8010B0 - CD->CPU */
/* Channel 4 - 0x1F8010C0 - CPU->SPU and SPU->CPU */
/* Channel 5 - 0x1F8010D0 - ??? */
/* Channel 6 - 0x1F8010E0 - Ordering table clear */

#define CPU2MDECDMA_MEMADDR     ((PsxUInt32 *)0x1F801080)
#define CPU2MDECDMA_BCR         ((PsxUInt32 *)0x1F801084)
#define CPU2MDECDMA_CHCR        ((PsxUInt32 *)0x1F801088)

#define MDEC2CPUDMA_MEMADDR     ((PsxUInt32 *)0x1F801090)
#define MDEC2CPUDMA_BCR         ((PsxUInt32 *)0x1F801094)
#define MDEC2CPUDMA_CHCR        ((PsxUInt32 *)0x1F801098)

/* Main memory address - totally linear */
#define GPUDMA_MEMADDR          ((PsxUInt32 *)0x1F8010A0)

/* Block count register */
#define GPUDMA_BCR              ((PsxUInt32 *)0x1F8010A4)
/* 0xFFFF0000 - number of bytes to transfer */
/* 0x00000010 - ??? */

/* DMA control register */
#define GPUDMA_CHCR             ((PsxUInt32 *)0x1F8010A8)
/* 0x01000000 - DMA transfer busy (read) */
/* 0x00000400 - DMA transfer of command list (set BCR to 0) */
/* 0x00000200 - DMA transfer of block of memory to VRAM */
/* 0x00000001 - direction: 0 - GPU to CPU, 1 - CPU to GPU */

#define CDDMA_MEMADDR           ((PsxUInt32 *)0x1F8010B0)
#define CDDMA_BCR               ((PsxUInt32 *)0x1F8010B4)
#define CDDMA_CHCR              ((PsxUInt32 *)0x1F8010B8)

/* Main memory address - totally linear */
#define SPUDMA_MEMADDR          ((PsxUInt32 *)0x1F8010C0)

/* Block count register: */
#define SPUDMA_BCR              ((PsxUInt32 *)0x1F8010C4)
/* 0xFFFF0000 - number of 64 byte blocks in upper 16 bits */
/* 0x00000010 - ??? */

/* DMA control register */
#define SPUDMA_CHCR             ((PsxUInt32 *)0x1F8010C8)
/* 0x01000000 - DMA transfer busy (read) */
/* 0x00000200 - DMA start */
/* 0x00000001 - direction: 0 - SPU to CPU, 1 - CPU to SPU */

#define DMA5_MEMADDR            ((PsxUInt32 *)0x1F8010D0)
#define DMA5_BCR                ((PsxUInt32 *)0x1F8010D4)
#define DMA5_CHCR               ((PsxUInt32 *)0x1F8010D8)

#define DMA6_MEMADDR            ((PsxUInt32 *)0x1F8010E0)
#define DMA6_BCR                ((PsxUInt32 *)0x1F8010E4)
#define DMA6_CHCR               ((PsxUInt32 *)0x1F8010E8)

/* DMA control register */
#define DMA_PCR                 ((PsxUInt32 *)0x1F8010F0)
/* On write: */
/* 0x00000800 - Reset */


/* Interrupt channel register - indicates which channels are
 *                              active and bus error condition
 */
#define DMA_ICR                 ((PsxUInt32 *)0x1F8010F4)
/* On read: */
/* 0x80000000 - bus error */
/* 0x7F000000 - bits indicating which channels have caused an interrupt */

/*****************/
/* ROOT COUNTERS */
/*****************/

/* Mode registers */
/* BIt 0 - timer enable */
/* Bit 1 - */
/* Bit 2 - */
/* Bit 3 - */
/* Bit 4 - interrupt enable */
/* Bit 5 - */
/* Bit 6 - interrupt enable */
/* Bit 7 - */
/* Bit 8 - vorteiler: ?*/

/* Root counter 0 = Processor tick */
/* Root counter 1 = HSync count */
/* Root counter 2 = ? */

/* Root counter 0 count value */
#define T0_COUNT                ((PsxUInt32 *)0x1F801100)

/* Root counter 0 configuration register */
#define T0_MODE                 ((PsxUInt32 *)0x1F801104)
/* 

/* Root counter 0 target value (generate interrupt??) */
#define T0_TARGET               0x1F801108

/* Root counter 1 count value */
#define T1_COUNT                ((PsxUInt32 *)0x1F801110)

/* Root counter 1 configuration register */
#define T1_MODE                 ((PsxUInt32 *)0x1F801114)

/* Root counter 1 target value (generate interrupt??) */
#define T1_TARGET               0x1F801118

/* Root counter 2 count value */
#define T2_COUNT                ((PsxUInt32 *)0x1F801120)

/* Root counter 2 configuration register */
#define T2_MODE                 ((PsxUInt32 *)0x1F801124)

/* Root counter 2 target value (generate interrupt??) */
#define T2_TARGET               0x1F801128

/*********/
/* CDROM */
/*********/

#define CDROM_REG0              ((PsxUInt8 *)0x1F801800)

/* Command register */
/* Command 00 : */
/* Command 01 : --- */
/* Command 02 : --- */
/* Command 03 : --- */
/* Command 04 : --- */
/* Command 05 : --- */
/* Command 06 : --- */
/* Command 07 : --- */
/* Command 08 : --- */
/* Command 09 : --- */
/* Command 10 : --- */
/* Command 11 : --- */
/* Command 12 : --- */
/* Command 13 : --- */
/* Command 14 : --- */
/* Command 15 : */
/* Command 16 : --- */
/* Command 17 : --- */
/* Command 18 : */
/* Command 19 : --- */
/* Command 20 : --- */
/* Command 21 : */
/* Command 22 : --- */
/* Command 23 : */
/* Command 24 : */
/* Command 25 : */
/* Command 26 : */
/* Command 27 : --- */
/* Command 28 : */
/* Command 29 : */
/* Command 30 : */
/* Command 31 : */

#define CDROM_REG1              ((PsxUInt8 *)0x1F801801)

#define CDROM_REG2              ((PsxUInt8 *)0x1F801802)

#define CDROM_REG3              ((PsxUInt8 *)0x1F801803)

/*******/
/* GPU */
/*******/

/* GPU command register 0 */
#define GPU_REG0                ((PsxUInt32 *)0x1F801810)
/* On write: (Primitive command words here) */
/* 0x80000000 - Move image (VRAM to VRAM) */
/*   YYYYXXXX - Top left of source */
/*   YYYYXXXX - Top left of dest */
/*   HHHHWWWW - Size of rectangle */
/* 0xE1000000 - Drawing mode */
/*            - 0x00000400 - Drawing on display area flag */
/*            - 0x00000200 - Dither enable */
/*            - 0x000009FF - Texture page (each texture page is 256x256) */
/* 0xE2000000 - Texture window (within texture page) */
/*            - 0x0000001F - Width of texture window: ((0-w) >> 3) */
/*            - 0x000003E0 - Height of texture window: ((0-h) >> 3) */
/*            - 0x00007C00 - Top of texture window: (y >> 3) */
/*            - 0x000F8000 - Left of texture window: (x >> 3) */
/* 0xE3000000 - Clip area start */
/*            - 0x000003FF - Left of clip area */
/*            - 0x000FFC00 - Top of clip area */
/* 0xE4000000 - Clip area end */
/*            - 0x000003FF - Right of clip area (inclusive: x+w-1) */
/*            - 0x000FFC00 - Bottom of clip area (inclusive: x+h-1) */
/* 0xE5000000 - Drawing offset */
/*            - 0x000007FF - Offset x */
/*            - 0x003FF800 - Offset y */
/* 0xE6000000 - Priority ??? */
/* See main docs for primitive commands... */

/* GPU command register 1 and status register */
#define GPU_REG1                ((PsxUInt32 *)0x1F801814)
/* On write: (Control command words here) */
/* 0x00000000 - All reset */
/* 0x01000000 - Reset command */
/* 0x02000000 - Reset IRQ */
/* 0x03000000 - Display mask */
/*            - 0x00000000 - Enable display */
/*            - 0x00000001 - Disable display */
/* 0x04000000 - Transfer mode */
/*            - 0x00000000 - Feed GPU with I/O */
/*            - 0x00000002 - DMA slice mode - CPU to GPU */
/*            - 0x00000003 - DMA slice mode - GPU to CPU */
/* 0x05000000 - Display offset - where the picture is grabbed from in VRAM */
/*            - 0x000003FF - Offset X */
/*            - 0x000FFC00 - Offset Y */
/* 0x06000000 - Horizontal start/end - where the image is displayed on the screen */
/*            - 0x00000FFF - Horizontal start (608 + (x * 10)) */
/*            - 0x00FFF000 - Horizontal end (608 + ((x + w) * 10)) */
/* 0x07000000 - Vertical start/end - where the image is displayed on the screen */
/*            - 0x000003FF - Vertical start (y + (PAL=19, NTSC=16)) */
/*            - 0x000FFC00 - Vertical end (y + h + (PAL=10, NTSC=16)) */
/* 0x08000000 - Display mode (how the image is constructed) */
/*            - 0x00000000 -                Width <= 280 */
/*            - 0x00000001 - Width > 280 && Width <= 352 */
/*            - 0x00000002 - Width > 400 && Width <= 560 */
/*            - 0x00000003 - Width > 560 */
/*            - 0x00000040 - Width > 352 && Width <= 400 */
/*            - 0x00000008 - Video system: 1 - PAL, 0 - NTSC */
/*            - 0x00000010 - Depth select: 1 - 24 bits, 0 - 16 bits */
/*            - 0x00000020 - Interlaced select: 1 - interlaced, 0 - non interlaced */
/*            - 0x00000024 - Display height > (PAL=288 NTSC=256) (selects interlaced) */
/*            - 0x00000080 - Reverse select: 1 - reversed, 0 - non-reversed */
/* On read: */
/* 0x80000000 - Interlace flag - test for frame switch */
/* 0x10000000 - Ready to receive commands: 0 - not ready, 1 - ready */
/* 0x04000000 - GPU idle: 0 - not idle, 1 - idle */

/********/
/* MDEC */
/********/

#define MDEC_REG0               ((PsxUInt32 *)0x1F801820)
#define MDEC_REG1               ((PsxUInt32 *)0x1F801824)

/*******/
/* SPU */
/*******/

#define SPU_DELAY               ((PsxUInt16 *)0x1F801014)

/* Voices */
#define SPU_VOICE_BASE          ((PsxUInt16 *)0x1F801C00)
/* Voices are 16 bytes each, as array of 8 PsxUInt16s: */
/* voice[0] - left voice volume  - bits 0-13 - volume (0-0x3FFF) */
/*                                 bit 14    - sign */
/* voice[1] - right voice volume - bits 0-13 - volume (0-0x3FFF) */
/*                                 bit 14    - sign */
/* voice[2] - voice frequency */
/* voice[3] - sound buffer address (SPU memory address >> 3) */
/* voice[4] - Sustain level, Decay rate and Attack rate: */
/*            0x000F - sustain level */
/*            0x00F0 - decay rate */
/*            0xFF00 - Attack rate - bits 7-14 - attack rate */
/*                                 - bit 15 - 1 means exponential */
/* voice[5] - Release rate and Sustain rate */
/*            0x003F - Release rate */
/*            0xFFC0 - Sustain rate */
/* voice[6] - Main volume (test to see if voice has finished) */
/* voice[7] - Sound buffer repeat address (SPU memory address >> 3) */

#define SPU_MVOL_L              ((PsxUInt16 *)0x1F801D80)
#define SPU_MVOL_R              ((PsxUInt16 *)0x1F801D82)
/* Master volume registers - 0x0000->0x7FFF */

#define SPU_REVERB_L            ((PsxUInt16 *)0x1F801D84)
#define SPU_REVERB_R            ((PsxUInt16 *)0x1F801D86)
/* Reverb depth registers - 0x0000->0x7FFF */

#define SPU_KEY_ON1             ((PsxUInt16 *)0x1F801D88)
#define SPU_KEY_ON2             ((PsxUInt16 *)0x1F801D8A)
/* Used to enable a particular channel */

#define SPU_KEY_OFF1            ((PsxUInt16 *)0x1F801D8C)
#define SPU_KEY_OFF2            ((PsxUInt16 *)0x1F801D8E)
/* Used to disable a particular channel */

#define SPU_KEY_MODEFM1         ((PsxUInt16 *)0x1F801D90)
#define SPU_KEY_MODEFM2         ((PsxUInt16 *)0x1F801D92)
/* Used to select which channels are in FM mode */

#define SPU_KEY_MODENOISE1      ((PsxUInt16 *)0x1F801D94)
#define SPU_KEY_MODENOISE2      ((PsxUInt16 *)0x1F801D96)
/* Used to select which channels are in noise mode */

#define SPU_KEY_MODEREVERB1     ((PsxUInt16 *)0x1F801D98)
#define SPU_KEY_MODEREVERB2     ((PsxUInt16 *)0x1F801D9A)
/* Used to select which channels are in reverb mode */

#define SPU_KEY_CHANNELACTIVE1  ((PsxUInt16 *)0x1F801D9C)
#define SPU_KEY_CHANNELACTIVE2  ((PsxUInt16 *)0x1F801D9E)
/* Indicates which channels are active */

#define SPU_U_E			((PsxUInt16 *)0x1F801DA2)
/* ??? */

/* SPU sound buffer address for transfers from CPU */
#define SPU_SB_ADDR             ((PsxUInt16 *)0x1F801DA6)
/* On 8 byte boundary: (SB_ADDR >> 3) */

#define SPU_DATA                ((PsxUInt16 *)0x1F801DA8)

/* Register 0 - works like flag register for diffreent features */
#define SPU_REG0                ((PsxUInt16 *)0x1F801DAA)
/* 0x8000 - SPU enable: 0 - off, 1 - on */
/* 0x4000 - Playing enable: 0 - mute, 1 - play */
/* 0x3F00 - Noise frequency */
/* 0x0080 - Reverb enable: 0 - off, 1 - on */
/* 0x0030 - 01 - CPU->SPU without DMA */
/*          10 - CPU->SPU with DMA */
/*          11 - SPU->CPU with DMA */
/* 0x0008 - External reverb enable: 0 - off, 1 - on */
/* 0x0004 - CD reverb enable: 0 - off, 1 - on */
/* 0x0002 - External digital input enable */
/* 0x0001 - CD audio enable : 0 - off, 1 - on */

#define SPU_REG1                ((PsxUInt16 *)0x1F801DAC)
#define SPU_STATUS              ((PsxUInt16 *)0x1F801DAE)

#define SPU_CDMVOL_L            ((PsxUInt16 *)0x1F801DB0)
#define SPU_CDMVOL_R            ((PsxUInt16 *)0x1F801DB2)
/* CD audio volume */

#define SPU_EXTVOL_L            ((PsxUInt16 *)0x1F801DB4)
#define SPU_EXTVOL_R            ((PsxUInt16 *)0x1F801DB6)
#define SPU_REVERB_WORKADDR	((PsxUInt16 *)0x1F801DA2)
#define SPU_REVERBCONFIG        ((PsxUInt16 *)0x1F801DC0)

#define SPU_FACTORLEFT          ((PsxUInt16 *)0x1F801DFC)
#define SPU_FACTORRIGHT         ((PsxUInt16 *)0x1F801DFE)

#define INT_2000                0x1F802030
#define DIP_SWITCHES            0x1F802040

#endif /* HWREG_H */
