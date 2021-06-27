// James' PSX SPU lib ver2
// Started 21/12/1999
// last update: 15 May 2000

//#include "psxtypes.h"
typedef unsigned short PsxUInt16;
typedef long unsigned int PsxUInt32;
typedef unsigned char PsxUInt8;

#include "hwreg.h"					// for SPU registers
#include "libspu.h"
//#include <hitmud.h>					// HITMUD stuff
#include "psxwave.c"				// 4 synth waveforms
#include "reverb.c"					// reverb defines and data
//#include "pitchtab.c"				// MOD pitch table
//#include "modlfo.c"					// LFO data for MOD
#include "vablib.c"

// ************************                *************************
// ************************ GENERAL SPU    *************************
// ************************                *************************

// Reset the SPU
void SPU_Reset(void) {
	*SPU_REG0 = 0x0000;
	printf("SPU reset!\n");
}

// Initialise SPU to "sane" values
void SPU_Init(void) {
	PsxUInt16 i;

	printf("Initialising SPU...\n");
	// switch spu dma on
	*DMA_PCR |= 0xB0000;
	// set vol = 50%
	*SPU_MVOL_L = 0x3fff;
	*SPU_MVOL_R = 0x3fff;
	// reset SPU
	*SPU_REG0 = 0x0000;
	// wait for SPU to come back
	SPU_Wait();
	// dunno what this one does - seems the "in" thing
	// (should it be SPU_REG1 ???)
	*SPU_REG1 = 0x0004;
	// set reverb depth
	*SPU_REVERB_L = 0x0000;
	*SPU_REVERB_R = 0x0000;
	// switch all notes (voices) off
	*SPU_KEY_OFF1 = 0xFFFF;
	*SPU_KEY_OFF2 = 0xFFFF;
	// switch FM mode off
	*SPU_KEY_MODEFM1 = 0x0000;
	*SPU_KEY_MODEFM2 = 0x0000;
	*SPU_KEY_MODENOISE1 = 0x0000;
	*SPU_KEY_MODENOISE2 = 0x0000;
	*SPU_KEY_MODEREVERB1 = 0x0000;
	*SPU_KEY_MODEREVERB2 = 0x0000;
	// set CD volume
	*SPU_CDMVOL_L = 0x0000;
	*SPU_CDMVOL_R = 0x0000;
	// set EXT volume
	*SPU_EXTVOL_L = 0x0000;
	*SPU_EXTVOL_R = 0x0000;
	// set volume on all channels to 0
	// set ADSR to default
	for(i=0; i<24; i++) {
		SPU_VoiceVol(i, 0x0000, 0x0000);		
		SPU_VoiceADSRRaw(i, 0x80FF, 0xCFAB);
	}
	// switch off noise and FM on all channels
	SPU_Voice2NoiseMask(0x00000000);
	SPU_Voice2FMMask(0x00000000);
	
	// *** NB more stuff to be done here!

	// wait again
	SPU_Wait();
	// switch SPU on
	*SPU_REG0 = 0x8000;
	*SPU_REVERB_WORKADDR = 0x1FFF;			// * 8

}

/*

int SPUgly_init (void)
{
 short status,c,*p;

 *d_pcr|=0xB0000;  // enable data transfer through SPU dma channels  
 *spu_mvol_r=0x3FFF; // set main volume  
 *spu_mvol_l=0x3FFF;
 *sp0=0x0; // reset spu
SPUgly_wait();
 *spu_u_B=0x4;
 *spu_u_1=0x0;  // set reverberation depth
 *spu_u_2=0x0; 
 *spu_keyOFF_1 = 0xFFFF ; // set all voices off 
 *spu_keyOFF_2 = 0xFFFF ; 
 *spu_u_3=0x0; 
 *spu_u_4=0x0;
 *spu_u_7=0x0; 
 *spu_u_8=0x0; 
 *spu_u_9=0x0; 
 *spu_u_A=0x0;
 *cd_mvol_l=0x0; // set cd main volume 
 *cd_mvol_r=0x0;
 *spu_u_5=0x0; 
 *spu_u_6=0x0; 
 status=*sp1 & 0x7FF;
 *spu_sb_addr=0x200;
SPUgly_wait();
 for (c=0;c<8;c++) *spu_u_D=0x0707;
SPUgly_wait();
 if (*sp1 & 0x400)
  for (c=0;*sp1 & 0x400;c++)
   if (c>3841)
    {
     _printf ("SPU:T/O ... (wait [WRDY H-> L])");
    return -1;
    }
SPUgly_wait();
SPUgly_wait();
 *sp0 = *sp0 & 0xFFCF ;			// clear bits 2 & 3 
 if (status!=*sp1 & 0x7FF)
  for (c=0;status!=*sp1 & 0x7FF;c++)
   if (c>3841)
    {
     _printf ("SPU:T/O ... (wait [DMAF CLEAR/W])"); // DMA flag clear wait 
     return -1;
    }

// set voices on off
 *spu_keyON_1 = 0xFFFF ;
 *spu_keyON_2 = 0xFF ;
SPUgly_wait();
SPUgly_wait();
SPUgly_wait();
SPUgly_wait();
 *spu_keyOFF_1 = 0xFFFF ;
 *spu_keyOFF_2 = 0x00FF ;
SPUgly_wait();
SPUgly_wait();
SPUgly_wait();
SPUgly_wait();
 *sp0 = 0x8000 ;// set spu on
 *spu_u_E = 0x1FFF;
}

*/

// Enable the SPU
void SPU_Enable(void) {
	*SPU_REG0 |= 0xC000;
	printf("SPU enabled!\n");
}

// Shut down the SPU
void SPU_Quit(void) {
	*DMA_ICR = (*DMA_ICR & 0x00dfffff) | 0x00800000;
	*SPU_REG0 = 0x0000;
}

// Switch reverb on
void SPU_ReverbOn(void) {
	*SPU_REG0 |= 0x80;
	printf("SPU reverb ON.\n");
}

// Switch reverb off
void SPU_ReverbOff(void) {
	*SPU_REG0 &= 0xFF7F;
	printf("SPU reverb OFF.\n");
}

// Wait for SPU to become available
int SPU_Wait(void) {
	PsxUInt32 c, v;
	
	// check if SPU busy
	v = 0;
	while(*SPU_STATUS & 0x7FF) {
		for(c=0; c<50000; c++);
		//printf("%4X  ", *SPU_STATUS);
		if(v > 10) {
			printf("SPU hanging. Exiting...\n");
			return 7;
		}
		v++;
	}
	return 0;
}

// Set SPU Noise Rate
void SPU_SetNoiseRate(PsxUInt8 rate) {
	*SPU_REG0 |= (rate & 0x3F) << 8;
}

// Transfer Wavetable data from main mem to sound buffer
void SPU_UploadData(PsxUInt32 *src_addr, PsxUInt32 sb_addr, PsxUInt32 size) {
	PsxUInt16 i;

	printf("Uploading sample data from %8X to SPU RAM address %X ...\n",
			src_addr, sb_addr);
	// specify destination address in SPU sound buffer
	*SPU_SB_ADDR = (PsxUInt16)(sb_addr >> 3);
	//printf("Dest addr >> 3 = %4x\n", *SPU_SB_ADDR);
	// specify xfer mode (CPU->SPU) part 1
	*SPU_REG0 = (*SPU_REG0 & 0xFFCF) | 0x0020;
	// wait for xfer ready
	printf("Waiting for xfer ready...\n");
	while((*SPU_REG0 & 0x30) != 0x20) ;
	// set xfer mode (CPU->SPU) part 2
	*SPU_DELAY = (*SPU_DELAY & 0xF0FFFFFF) | 0x20000000;
	// specify mem addr (source address of data)
	*SPUDMA_MEMADDR = (PsxUInt32)src_addr; // TODO: WARNING FIX - might break the code
	//printf("Source addr = %8x\n", *SPUDMA_MEMADDR);
	// specify size of data (extra block if needed)
	if(size & 0x3F > 0) i = 1; else i = 0;
	*SPUDMA_BCR= (size >> 6 + i) << 16 | 0x10;
	//printf("Size of data = %8x\n", *SPUDMA_BCR);
	// start xfer
	*SPUDMA_CHCR = 0x01000201;
	printf("Starting xfer...\n");
	// wait for xfer to finish
	while(*SPUDMA_CHCR & 0x01000000) ;
	printf("Xfer complete.\n");

	
	// debug stuff
	//printf("Sample wave data sent to SB!\n");
	//printf("Source address was %8x\n", *SPUDMA_MEMADDR);
	//printf("Size of data was %8x\n", *SPUDMA_BCR);

}

// set main volume (0 -> 0x3fff)
// NB: differing discriptions
void SPU_Volume(PsxUInt16 vol_left, PsxUInt16 vol_right) {
	*SPU_MVOL_L = vol_left;
	*SPU_MVOL_R = vol_right;
}



// ************************                *************************
// ************************ VOICE STUFF    *************************
// ************************                *************************

// Switch a single voice on
void SPU_VoiceOn(PsxUInt8 voice) {
	PsxUInt32 mask;
	if(voice > 0) mask = 1 << voice;
	else mask = 1;
	*SPU_KEY_ON1 = mask & 0xFFFF;
	*SPU_KEY_ON2 = mask >> 16;
	//SPU_Wait();
}

// Switch a single voice off
void SPU_VoiceOff(PsxUInt8 voice) {
	PsxUInt32 mask;
	if(voice > 0) mask = 1 << voice;
	else mask = 1;
	*SPU_KEY_OFF1 = mask & 0xFFFF;
	*SPU_KEY_OFF2 = mask >> 16;
	//SPU_Wait();
}

// Switch multiple voices ON
void SPU_VoiceOnMask(PsxUInt32 mask) {
	SPU_Wait();
	*SPU_KEY_ON1 = mask & 0xFFFF;
	*SPU_KEY_ON2 = mask >> 16;

	printf("VoiceOnMask %8X\n", mask);
	//SPU_Wait();
}

// Switch multiple voices OFF
void SPU_VoiceOffMask(PsxUInt32 mask) {
	*SPU_KEY_OFF1 = mask & 0xFFFF;
	*SPU_KEY_OFF2 = mask >> 16;
	//SPU_Wait();
}

// Set a single voice reverb on (for next note on)
// (NB: reverb for this voice is turned off when
// the sample finishes playing)
void SPU_VoiceReverb(PsxUInt8 voice) {
	PsxUInt32 mask;
	if(voice > 0) mask = 1 << voice;
	else mask = 1;
	*SPU_KEY_MODEREVERB1 = mask & 0xFFFF;
	*SPU_KEY_MODEREVERB2 = mask >> 16;
	//SPU_Wait();
}

// Set multiple voices reverb on (for next note on)
// NB: switches off after sample finished
void SPU_VoiceReverbMask(PsxUInt32 mask) {
	*SPU_KEY_MODEREVERB1 = mask & 0xFFFF;
	*SPU_KEY_MODEREVERB2 = mask >> 16;
	//SPU_Wait();
}

// Set Voice n Volume (0 -> 0x3fff)
void SPU_VoiceVol(PsxUInt8 voice, PsxUInt16 vol_left, PsxUInt16 vol_right) {
	PsxUInt16 *v_base = (PsxUInt16 *)(SPU_VOICE_BASE + 8 * voice);

	SPU_Wait();
	v_base[0] = vol_left;
	v_base[1] = vol_right;
	//printf("VoiceVol: voice = %d  v_base = %8X\n", voice, v_base);
}

// Set Voice n Pitch
void SPU_VoicePitch(PsxUInt8 voice, PsxUInt16 pitch) {
	PsxUInt16 *v_base = (PsxUInt16 *)(SPU_VOICE_BASE + 8 * voice);

	SPU_Wait();
	*(v_base + 2) = pitch;
	printf("VoicePitch: voice = %d  v_base = %8X\n", voice, v_base);
}

// Set Voice n Repeat Addr
void SPU_VoiceRepeatAddress(PsxUInt8 voice, PsxUInt16 rep_addr) {
	PsxUInt16 *v_base = (PsxUInt16 *)(SPU_VOICE_BASE + 8 * voice);
	
	*(v_base + 7) = rep_addr;
}


// Get Voice n current ADSR Volume
PsxUInt16 SPU_GetADSRVol(PsxUInt8 voice) {
	PsxUInt16 us1;
	PsxUInt16 *v_base = (PsxUInt16 *)(SPU_VOICE_BASE + 8 * voice);
	
	us1 = *(v_base + 6);
	return us1;
}

// Set Voice n wave data offset
void SPU_VoiceSetData(PsxUInt8 voice, PsxUInt32 sb_addr) {
	PsxUInt16 *v_base = (PsxUInt16 *)(SPU_VOICE_BASE + 8 * voice);

	SPU_Wait();
	////// **** CHECK!!!!!
	*(v_base + 3) = sb_addr >> 3;
	
	// debug
	printf("VoiceSetData voice %d  v_base = %8X  wave offset = %8x\n", voice, v_base, *(v_base + 3));
}


// Switch a single voice to Noise
void SPU_Voice2Noise(PsxUInt8 voice) {
	PsxUInt32 mask;
	if(voice > 0) mask = 1 << voice;
	else mask = 1;
	*SPU_KEY_MODENOISE1 = mask & 0xFFFF;
	*SPU_KEY_MODENOISE2 = mask >> 16;
	SPU_Wait();
}

// Switch multiple voices to Noise
void SPU_Voice2NoiseMask(PsxUInt32 mask) {
	*SPU_KEY_MODENOISE1 = mask & 0xFFFF;
	*SPU_KEY_MODENOISE2 = mask >> 16;
	SPU_Wait();
}


// Switch a single voice to FM
void SPU_Voice2FM(PsxUInt8 voice) {
	PsxUInt32 mask;
	if(voice > 0) mask = 1 << voice;
	else mask = 1;
	*SPU_KEY_MODEFM1= mask & 0xFFFF;
	*SPU_KEY_MODEFM2 = mask >> 16;
	SPU_Wait();
}

// Switch multiple voices to FM
void SPU_Voice2FMMask(PsxUInt32 mask) {
	*SPU_KEY_MODEFM1= mask & 0xFFFF;
	*SPU_KEY_MODEFM2 = mask >> 16;
	SPU_Wait();
}

// Set Voice n ADSR
// NB: linear only at this stage
void SPU_VoiceADSR(PsxUInt8 voice, PsxUInt8 ar, PsxUInt8 dr, PsxUInt8 sl,
			PsxUInt16 sr, PsxUInt8 rr)
{
	PsxUInt16 us1, us2;	
	PsxUInt16 *v_base = (PsxUInt16 *)(SPU_VOICE_BASE + 8 * voice);
	
	us1 = (ar << 8) | ((dr & 0x0f) << 4) | (sl & 0x0f);
	us2 = (sr << 6) | (rr & 0x3f); 
	
	*(v_base + 4) = us1;
	*(v_base + 5) = us2;
}

// Set Voice n ADSR (raw)
// (useful if you know how PSX SPU ADSR works,
// or if you can get the ADSR values somewhere else :)
void SPU_VoiceADSRRaw(PsxUInt8 voice, PsxUInt16 adsr1, PsxUInt16 adsr2)
{
	PsxUInt16 *v_base = (PsxUInt16 *)(SPU_VOICE_BASE + 8 * voice);
	
	*(v_base + 4) = adsr1;
	*(v_base + 5) = adsr2;
}

// check voice active mask
PsxUInt32 SPU_VoiceActiveMask(void) {
	return (*SPU_KEY_CHANNELACTIVE2 << 16 + *SPU_KEY_CHANNELACTIVE1) & 0x00FFFFFF;
}



// ************************                *************************
// ************************ REVERB STUFF   *************************
// ************************                *************************

// Load Reverb data into reverb area
void SPU_LoadReverb_OLD(PsxUInt16 *data, PsxUInt16 work_addr) {
	int i;

	// switch off reverb and set reverb depth = 0
	SPU_ReverbOff();
	*SPU_REVERB_L = 0;
	*SPU_REVERB_R = 0;

	// copy reverb data to reverb area
	for(i=0; i<32; i++) {
		SPU_REVERBCONFIG[i] = data[i];
	}
	printf("Reverb data loaded from %8x\n", data);

	// set sound buffer reverb work area start address
	*SPU_REVERB_WORKADDR = work_addr;
	printf("Reverb work area at %4x << 3 in SB.\n", *SPU_REVERB_WORKADDR);

// NB: must clear reverb work area to 0 before switching reverb back
//     on !!!
// (just upload zeros ???)
	// switch reverb back on and set reverb = 50%
	SPU_ReverbOn();
	*SPU_REVERB_L = 0x4000;
	*SPU_REVERB_R = 0x4000;	
}

// Load Reverb data into reverb area
void SPU_LoadReverb(PsxUInt16 revnum) {
	int i;

	// switch off reverb and set reverb depth = 0
	SPU_ReverbOff();
	*SPU_REVERB_L = 0;
	*SPU_REVERB_R = 0;

	// copy reverb data (see reverb.c) to reverb area
	for(i=0; i<32; i++) {
		SPU_REVERBCONFIG[i] = rvb_data[(revnum << 5) + i];
	}
	printf("Reverb data loaded from rvb_data offset %4X\n", revnum << 5);

	// set sound buffer reverb work area start address
	*SPU_REVERB_WORKADDR = rvb_work_addr[revnum];
	printf("Reverb work area at %4X << 3 in SB.\n", *SPU_REVERB_WORKADDR);

// NB: must clear reverb work area to 0 before switching reverb back
//     on !!!
// (just upload zeros ???)
	// switch reverb back on and set reverb = 50%
	SPU_ReverbOn();
	*SPU_REVERB_L = 0x4000;
	*SPU_REVERB_R = 0x4000;	
}

// set Reverb depth
void SPU_ReverbDepth(PsxUInt16 left, PsxUInt16 right) {
	
	*SPU_REVERB_L = left;
	*SPU_REVERB_R = right;
}



// ************************                *************************
// ************************ CD & EXT STUFF *************************
// ************************                *************************

// Enable CD audio
void SPU_CDAudioOn(void) {
	*SPU_REG0 |= 0x0001;
}

// Disable CD audio
void SPU_CDAudioOff(void) {
	*SPU_REG0 &= 0xFFFE;
}

// Enable CD reverb
void SPU_CDReverbOn(void) {
	*SPU_REG0 |= 0x0004;
}

// Disable CD reverb
void SPU_CDReverbOff(void) {
	*SPU_REG0 &= 0xFFFB;
}

// Set CD input Volume (0 -> 0x7fff)
void SPU_CDVol(PsxUInt16 vol_left, PsxUInt16 vol_right) {	
	*SPU_CDMVOL_L = vol_left;
	*SPU_CDMVOL_R = vol_right;
}

// Enable EXTernal audio
void SPU_ExtAudioOn(void) {
	*SPU_REG0 |= 0x0002;
}

// Disable EXTernal audio
void SPU_ExtAudioOff(void) {
	*SPU_REG0 &= 0xFFFD;
}

// Enable EXTernal reverb
void SPU_ExtReverbOn(void) {
	*SPU_REG0 |= 0x0008;
}

// Disable EXTernal reverb
void SPU_ExtReverbOff(void) {
	*SPU_REG0 &= 0xFFF7;
}

// Set external input Volume (0 -> 0x7fff)
void SPU_ExternVol(PsxUInt16 vol_left, PsxUInt16 vol_right) {	
	*SPU_EXTVOL_L = vol_left;
	*SPU_EXTVOL_R = vol_right;
}



// ************************               **************************
// ************************ MOD FUNCTIONS **************************
// ************************               **************************

// updates:
// 1. upload MOD sample data to any SB address
// 2. MOD effects 1, 2, 4, 7, 8, 9, A, B, C and D
// 3. LFO stuff
// 4. global instr volume implemented
// 5. MOD master volume implemented
// 6. corrected handling of sample no. without note
// 7. pitch table implemented (for speed-up)
//
// initialise HITMOD structure
// 4, 6, 8 track MOD's
// updated 9/9/1999 to allow upload to any address
/*
void InitMOD(HITMUD *m, PsxUInt8 *modaddr, PsxUInt32 sb_addr) {
        int i;
        PsxUInt32 tsampleaddr;
        PsxUInt32 samplelen;
        PsxUInt8 *ucptr;
        PsxUInt8 t1, t2;

        m->NumTracks = 4;
        m->ModStart = modaddr;

        m->MasterVol = 63;				// set MOD master vol to max.

        // get number of tracks
        t1 = *(m->ModStart + 0x43B);
        if(t1 == '4') m->NumTracks = 4;         
        if(t1 == '6') m->NumTracks = 6;         
        if(t1 == '8') m->NumTracks = 8;
        printf("Initialising %d track MOD...\n", m->NumTracks);       

        ucptr = modaddr + 42;

        tsampleaddr = sb_addr;          // start addr in SB
        // get sample infos             
        for(i=0; i<30; i++) {
                t1 = *ucptr;
                t2 = *(ucptr + 1);
                m->SmpAddress[i] = tsampleaddr; // addr in SB of each sample
                samplelen = ((t1 << 8) + t2); 
                tsampleaddr += samplelen;

                m->FineTunes[i] = *(ucptr + 2) & 0x0f;
                m->Volumes[i] = *(ucptr + 3);
                // printf("Sample %d  length %d  addr %d Finetune %d  Vol %d\n",
                //        i, samplelen, m->SmpAddress[i], m->FineTunes[i], m->Volumes[i]);
                ucptr += 30;
        }

        m->SampleEnd = tsampleaddr;     // addr in SB of end of samples

        printf("Start of mod is %08X\n", modaddr);
        // set songlength
        m->SongLength = *(modaddr + 950);
        printf("Songlength is %d\n", m->SongLength);

        m->CurrOrder = 0;
        m->CurrRow = 0;

        // set CurrentPattern
        t1 = *(modaddr + 952);
        m->CurrPattern = t1;
        printf("First Pattern is %d\n", m->CurrPattern);
        // set RowPointer
        m->RowPointer = (PsxUInt32 *)(modaddr + 1084 + (t1 * m->NumTracks *
256));
        //printf("RowPointer is %x\n", m->RowPointer);
        //printf("Value at RowPointer is %08x\n", *m->RowPointer);
        //printf("Value after is %08x\n", *(m->RowPointer + 1));
        
        // calc no of patterns by reading thru order list
        t2 = 0;
        for(i=0; i<128; i++) {
                t1 = *(modaddr + 952 + i);
                if(t1 > t2) t2 = t1;
        }
        m->NumPatterns = t2;
        printf("Number of patterns is %d\n", t2);
        t2++;
        m->PattDataSize = t2 * m->NumTracks * 256;      // size of pattern data
        
        samplelen = 1084 + m->PattDataSize;
        //printf("Samples offset is %x\n", samplelen);
        ucptr = modaddr + samplelen;
        //printf("Address of samples is %08x\n", ucptr);

        // init stuff
        for(i=0; i < m->NumTracks; i++) {
                m->ChanVol[i] = 0;
                m->LastSmp[i] = 0;
               	m->LFOptr[i] = 0;
    	    	m->LFOSpeed[i] = 0;
    	    	m->LFODepth[i] = 0;
    	    	// init ADSR
				SPU_VoiceADSRRaw(i, 0x80FF, 0xDFFF);
        }
        m->Keys = 0x0000;
        m->Speed = 6;
        m->Tick = 0;            // current tick

        // upload samples to address sb_addr
        samplelen = tsampleaddr - sb_addr;
        printf("Samples length is %d\n", samplelen);

        SPU_UploadData(ucptr, sb_addr, samplelen + 32);
}




// Play next row of mod
// TODO: check not waiting for too long
// TODO: optimise
// TODO: LFO tables (make available to outside sounds)
// TODO: LFO update (every poll)
int PollMOD(HITMUD *m) {
    int i;
    PsxUInt8 uc;
    PsxUInt8 uc1;
    PsxUInt32 w;
    PsxUInt32 nn;
    PsxUInt32 pitch;
    PsxUInt32 offset[8];
    PsxUInt16 *vwa;
    PsxUInt16 mask;
    PsxUInt16 v_on;
    PsxUInt16 v_off;
    PsxUInt16 us1;
    PsxInt8 sc;

    		
    // increment tick, check, return if neccessary
    m->Tick++;
    // update LFO pointers
    for(i=0; i<m->NumTracks; i++) {
    	// NB: *** OPTIMISE - use & to mask range 
		m->LFOptr[i] = (m->LFOptr[i] + m->LFOSpeed[i]) % 64;
    }

	// update inbetween-lines
    if(m->Tick < m->Speed) {

		// update stuff that needs to be done every tick
    	vwa = SPU_VOICE_BASE;
    	for(i=0; i < m->NumTracks; i++) {
    		// check which effect is currently active on this channel
    		switch(m->Effect[i]) {
    			case NO_EFFECT :
    				break;
    			case PORTAMENTO :	// pitch slide
	    			// do pitch slide stuff here
    				m->Period[i] += m->PitchSlide[i];
					nn = m->Period[i];
					//vwa = SPU_VOICE_BASE + i * 8;
					*(vwa + 2) = pitchtab[nn];
    				break;
    			case VOL_SLIDE :	// volume slide
	    			// do volume slide update
    				m->ChanVol[i] += m->VolSlide[i];
    				if(m->ChanVol[i] > 64) m->ChanVol[i] = 64;
    				if(m->ChanVol[i] < 0) m->ChanVol[i] = 0;
    				// write new vol to SPU
					//vwa = SPU_VOICE_BASE + i * 8;
					// set voice vol for this channel
					// *** optimise - precalc m->Volume << 2  ***
					us1 = (m->ChanVol[i] * m->Volumes[m->LastSmp[i]] * m->MasterVol) >> 4;
					*vwa = us1;
					*(vwa + 1) = us1;
    				break;
    			case LFO_VIBRATO :	// check for Vibrato
					sc = modlfo[m->LFOptr[i] + 64 * m->LFODepth[i]];
					nn = m->Period[i] + sc;
					//vwa = SPU_VOICE_BASE + i * 8;
					*(vwa + 2) = pitchtab[nn];
    				break;
				case LFO_TREMOLO :	// check for Tremolo
					sc = modlfo[m->LFOptr[i] + 64 * m->LFODepth[i]];
					uc = m->ChanVol[i] + sc;
    				if(uc > 127) uc = 0;				// clip low
    				if(uc > 64) uc = 64;				// clip high
    				// write new vol to SPU
					//vwa = SPU_VOICE_BASE + i * 8;
					// set voice vol for this channel
					us1 = (uc * m->Volumes[m->LastSmp[i]] * m->MasterVol) >> 4;
					*vwa = us1;
					*(vwa + 1) = us1;
					break;
    		} // end switch Effect
    		vwa += 8;
    	} // next i
    	return 0;
    } // end if tick < speed

    // reset voice on and voice off masks
    v_on = 0; v_off = 0;
    m->Keys = 0;

    // do tracks
    vwa = SPU_VOICE_BASE;
    for(i=0; i < m->NumTracks; i++) {
      	// reset sample offsets
        offset[i] = 0;
		// reset Effect stuff
		m->Effect[i] = NO_EFFECT;
		
        // get data word, increment rowpointer
        w = *m->RowPointer++;

        // parse for samplenum / instrnum
        uc = ((w >> 20) & 0x0f) | (w & 0xf0);
        if(uc > 0) {
            m->ChanVol[i] = 64;		// reset sample play vol to full
            m->LastSmp[i] = uc - 1;
            // *** modification for correctness: so note will trigger even if ***
            // *** no note specified (just samplenum) ***
            // set keyon for this channel
            m->Keys |= 1 << i;
        }

        // parse for effects
        // get effect
        uc = (w >> 16) & 0x0f;
        if(uc != 0) {
            // get effect data
            uc1 = (w >> 24) & 0xff;
            // which effect?
			switch(uc) {
            	case 0x0F :	// song speed
					// set speed (convert from tempo if necc)
                    if(uc1 < 20) m->Speed = uc1;
                    else m->Speed = 800 / uc1;
                    break;
                    
				case 0x01 :	// portamento up
					m->PitchSlide[i] = -uc1;
					m->Effect[i] = PORTAMENTO;
					break;

				case 0x02 :	// portamento down
					m->PitchSlide[i] = uc1;
					m->Effect[i] = PORTAMENTO;
					break;

				case 0x04 :	// vibrato (pitch LFO)
		                    // TODO: vibrato using LFO (sine only)
        		            // format: xy where x = speed, y = depth
							//Vibrato means to "oscillate the sample pitch using
                       		//  a  particular waveform with amplitude yyyy notes,
                       		//  such that (xxxx * speed)/64  full oscillations
							//occur in the line".
							// NB: 1024 byte LFO table needed, then:
							// LFOSpeed = (16 * x) / speed
					m->LFOSpeed[i] = (uc1 & 0xf0) >> 4;
					m->LFODepth[i] = uc1 & 0x0f;
					m->Effect[i] = LFO_VIBRATO;
                    break;

				case 0x07 :	// tremolo (volume LFO)
                            // TODO: tremolo using LFO (sine only)
                            // format: xy where x = speed, y = depthTemolo means to "oscillate
							// the sample volume using a  particular waveform
							// with   amplitude   yyyy*4,   such   that   (xxxx*speed)/64   full
							//oscillations occur in the line".  
					m->LFOSpeed[i] = (uc1 & 0xf0) >> 4;
					m->LFODepth[i] = uc1 & 0x0f;
					m->Effect[i] = LFO_TREMOLO;
					break;

				case 0x08 :	// set panning position
                            // TODO: panning (valid 0 (left) -> 7f (right))
					break;

				case 0x09 :	// set sample offset (wocious!)
                            // note: can only offset to 16-byte blocks
                            // which correspond to 28 samples of original
                            // also: offset = uc1 * 256
					offset[i] = (uc1 << 12) / 28;
					offset[i] &= 0xFFFFFFF0;                // clip to 16-byte boundary
					break;

				case 0x0A :	// volume slide
					m->VolSlide[i] = ((uc1 & 0xf0) >> 4) - (uc1 & 0x0f);
					m->Effect[i] = VOL_SLIDE;
					break;

				case 0x0B :	// pattern jump
                        	// note: still plays the current row                    
					// check if a valid jump destination
					// (is this required?)
					if(uc1 <= m->SongLength) {
						m->CurrRow = 63;
						m->CurrOrder = uc1 - 1;
					}
					break;

				case 0x0C :	// set volume
					if(uc1 > 64) uc1 = 64;
					m->ChanVol[i] = uc1;
					// calc voice write addr (vwa)
    	            //vwa = SPU_VOICE_BASE + i * 8;
        	        // set voice vol for this channel
	        	    us1 = (m->ChanVol[i] * m->Volumes[m->LastSmp[i]] * m->MasterVol) >> 4;
    	        	*vwa = us1;
        	    	*(vwa + 1) = us1;
					break;

				case 0x0D :	// pattern break (simplified version)
                        	// note: still plays the current row
					m->CurrRow = 63;
					break;
					
			} // end switch for effects

        } // end effects
            
		// check if note on
        // and get note period
		nn = ((w & 0x0f) << 8) | ((w >> 8) & 0xff);
		if(nn > 0) {
        	m->Period[i] = nn;			// remember period
            // calc voice write addr (vwa)
            //vwa = SPU_VOICE_BASE + i * 8;
            // set voice vol for this channel
            // note: global volumes implemented - check if works!
            us1 = (m->ChanVol[i] * m->Volumes[m->LastSmp[i]] * m->MasterVol) >> 4;
            *vwa = us1;
            *(vwa + 1) = us1;
            // set pitch from pitch table
            *(vwa + 2) = pitchtab[nn];
            // set sample addr (including "offset" effect)
            us1 = ((m->SmpAddress[m->LastSmp[i]] + offset[i]) >> 3) & 0xffff;
            *(vwa + 3) = us1;                                                               
            // Wait for the SPU   *** optimise - remove? ***
            //SPU_Wait();
            // set keyon for this channel
            m->Keys |= 1 << i;
		} // end if note on

		vwa += 8;
	} // next channel (track)
        
	// reset ticks
	m->Tick = 0;

	// do voice ons
	*SPU_KEY_ON1 = m->Keys;

	// inc row counter
	m->CurrRow++;
        
	// check if end of pattern
	if(m->CurrRow == 64) {
        m->CurrRow = 0;
        m->CurrOrder++;
        // check for end of song and loop
        if(m->CurrOrder > m->SongLength) {
            m->CurrOrder = 0;
        }
        // get pattern
        m->CurrPattern = *(m->ModStart + 952 + m->CurrOrder);
        //printf("Order: %d  Pattern: %d\n", m->CurrOrder, m->CurrPattern);
        // set rowpointer
        m->RowPointer = (PsxUInt32 *)(m->ModStart + 1084 +
                                 (m->CurrPattern * m->NumTracks * 256));
	}

    return 1;
}
*/

/* -------------------------- END OF SPULIB CODE -------------------- */
