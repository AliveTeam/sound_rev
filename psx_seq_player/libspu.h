// James' PSX SPU lib ver2
// header file for lib functions
// 3:02 pm 21/12/99

// reverb patch defines
#define RVB_NONE			0
#define RVB_ROOM			1
#define RVB_STUDIO_SMALL	2
#define RVB_STUDIO_MEDIUM	3
#define RVB_STUDIO_LARGE	4
#define RVB_HALL			5
#define RVB_SPACE_ECHO		6
#define RVB_ECHO			7
#define RVB_DELAY			8
#define RVB_HALF_ECHO		9

// defines for making life easier with ADSR envelopes
#define ATTACK_LINEAR	0
#define ATTACK_EXP		0x80
#define SUSTAIN_LINEAR	0
#define SUSTAIN_EXP		0x200
#define SUSTAIN_INC		0
#define SUSTAIN_DEC		0x100
#define RELEASE_LINEAR	0
#define RELEASE_EXP		0x20

// --------------------- General SPU lib functions ---------------------

void SPU_Reset(void);

// Enable the SPU
void SPU_Enable(void);

// Switch reverb on
void SPU_ReverbOn(void);

// Wait for SPU to become available
int SPU_Wait(void);

// Transfer Wavetable data from main mem to sound buffer
void SPU_UploadData(PsxUInt32 *src_addr, PsxUInt32 sb_addr, PsxUInt32 size);

// set main volume (0 -> 0x3fff)
// NB: differing discriptions
void SPU_Volume(PsxUInt16 vol_left, PsxUInt16 vol_right);

// Switch a single voice on
void SPU_VoiceOn(PsxUInt8 voice);

// Switch a single voice off
void SPU_VoiceOff(PsxUInt8 voice);

// Switch multiple voices ON
void SPU_VoiceOnMask(PsxUInt32 mask);

// Switch multiple voices OFF
void SPU_VoiceOffMask(PsxUInt32 mask);

// Set Voice n Volume (0 -> 0x3fff)
void SPU_VoiceVol(PsxUInt8 voice, PsxUInt16 vol_left, PsxUInt16 vol_right);

// Set Voice n Pitch (0x1000 = 44100 samples/sec)
void SPU_VoicePitch(PsxUInt8 voice, PsxUInt16 pitch);

// Get Voice n current ADSR Volume
PsxUInt16 SPU_GetADSRVol(PsxUInt8 voice);

// Set Voice n wave data offset
void SPU_VoiceSetData(PsxUInt8 voice, PsxUInt32 sb_addr);

// Set a voice to Noise mode
void SPU_Voice2Noise(PsxUInt8 voice);

// Set Voices to Noise mode (Mask version)
void SPU_Voice2NoiseMask(PsxUInt32 mask);

// Set voice to FM mode
void SPU_Voice2FM(PsxUInt8 voice);

// Set voices to FM mode (mask version)
void SPU_Voice2FMMask(PsxUInt32 mask);

// Raw ADSR setting
void SPU_VoiceADSRRaw(PsxUInt8 voice, PsxUInt16 adsr1, PsxUInt16 adsr2);

// Cooked ADSR setting
void SPU_VoiceADSR(PsxUInt8 voice, PsxUInt8 ar, PsxUInt8 dr, PsxUInt8 sl,
			PsxUInt16 sr, PsxUInt8 rr);

// Check which voices are active
// NB: doesn't work properly (why???)
PsxUInt32 SPU_VoiceActiveMask(void);



// ----------------------- VAB Handling stuff ---------------------

// Load VAB to SPU RAM and initialise VAB struct
PsxUInt8 SPU_LoadVAB(PsxUInt8 *vab, PsxUInt32 sb_addr);

// Set voice data pointer to a sample in VAB bank
// (NB: VAB must have been loaded first!)
void SPU_VoiceVABSample(PsxUInt8 voice, PsxUInt8 sample);


