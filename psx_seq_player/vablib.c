/*

VAG file reader
(version for spulib)
Jum Hig
V0.1

*/

//#include <stdio.h>
//#include <string.h>
//#include "include\psxtypes.h"

typedef struct {
	unsigned long magicnum;
	unsigned long version;
	unsigned long vab_id;
	unsigned long wave_size;
	unsigned short num_progs;
	unsigned short num_tones;
	unsigned short num_vags;
	unsigned char master_vol;
	unsigned char master_pan;
	unsigned char ba1;
	unsigned char ba2;
	unsigned short vag_length[256];
	unsigned short vag_offset[256];
	unsigned long wavedatasize;
	unsigned long sb_addr;
} VABHEADER;	

VABHEADER vab_info;

extern void SPU_UploadData(PsxUInt32 *src_addr, PsxUInt32 sb_addr, PsxUInt32 size);
extern void SPU_VoiceSetData(PsxUInt8 voice, PsxUInt32 sb_addr);

// helper function 1
// derived from file-scanning code
// (may not have to reverse-endian)
unsigned long get_quad(PsxUInt8 *p) {
	unsigned long t;
	unsigned char c1, c2, c3, c4;

    c4 = *p++;
	c3 = *p++;
	c2 = *p++;
	c1 = *p;
	t = (c1 << 24) + (c2 << 16) + (c3 << 8) + c4;
	//printf("u_long t = %08x\n", t);
	return t;
}

// helper function 2
// derived from file-scanning code
// (may not have to reverse-endian)
unsigned short get_bi(PsxUInt8 *p) {
	unsigned short t;
	unsigned char c1, c2;
	c2 = *p++;
	c1 = *p;
	t = (c1 << 8) + c2;
	//printf("u_short t = %04x\n", t);
	return t;
}
	
// Upload an entire VAB bank to SPU
// and store offsets in VABHEADER
PsxUInt8 SPU_LoadVAB(PsxUInt8 *vab, PsxUInt32 sb_addr)
{
    unsigned long quad;
    unsigned short bi;
    unsigned char uc;
    unsigned char data[520000];
    unsigned short total;

    int predict_nr, shift_factor, flags;

    int i, j, k;

    int d, s;



	// check for header string
	printf("Reading VAB header...\n");
	quad = get_quad(vab); vab += 4;
	printf("Read: %8x\n", quad);
	if(quad == 0x56414270) {
		printf("Header VAGp found. Little-endian.\n");
	}
	else {
		if(quad == 0x70424156) {
			printf("Header VAGp found. Big-endian.\n");
		}
		else {
			printf("Header VAGp *NOT* found - exiting...\n");
			exit(1);
		}
	}
	vab_info.magicnum = quad;

	// read version
	quad = get_quad(vab); vab += 4;
	printf("Version = %d\n", quad);
	vab_info.version = quad;

	// read VAB ID
	quad = get_quad(vab); vab += 4;
	printf("VAB ID = %8x\n", quad);
	vab_info.vab_id = quad;

	// read waveform size
	quad = get_quad(vab); vab += 4;
	printf("Waveform size = %d\n", quad);
	vab_info.wave_size = quad;

	// get sys_reserved1
	bi = get_bi(vab); vab += 2;

	// get no. of programs
	bi = get_bi(vab); vab += 2;
	printf("Number of programs = %d\n", bi);
	vab_info.num_progs = bi;

	// get no. of tones
	bi = get_bi(vab); vab += 2;
	printf("Number of tones = %d\n", bi);
	vab_info.num_tones = bi;

	// get VAG count
	bi = get_bi(vab); vab += 2;
	printf("VAG count = %d\n", bi);
	vab_info.num_vags = bi;

	// get master volume
	uc = *vab; vab++;
	printf("Master volume = %d\n", uc);
	vab_info.master_vol = uc;

	// get master pan
	uc = *vab; vab++;
	printf("Master pan = %d\n", uc);
	vab_info.master_pan = uc;

	// get bank attr 1
	uc = *vab; vab++;
	printf("Bank attr 1 = %d\n", uc);
	vab_info.ba1 = uc;

	// get bank attr 2
	uc = *vab; vab++;
	printf("Bank attr 2 = %d\n", uc);
	vab_info.ba2 = uc;

	// skip system reserved bytes
	quad = get_quad(vab); vab += 4;

	// read program attr table
	printf("Reading Programs...\n");
	for(i=0 ; i < vab_info.num_progs; i++) {
		//printf("Program %d settings:\n", i);
		for(j = 0; j < 4; j++) {
			quad = get_quad(vab);
			vab += 4;
		}
	}

	// skip unused part of program attr table
	printf("Skipping rest of program attr table...\n");
	for(i=0 ; i < (128 - vab_info.num_progs); i++) {
		for(j = 0; j < 4; j++) {
			quad = get_quad(vab);
			vab += 4;
		}
	}

	// read tone attr table
	printf("Reading Tones...\n");
	for(i=0; i < vab_info.num_progs; i++) {
		for(k=0; k < 32; k++) {
			for(j = 0; j < 4; j++) {
				quad = get_quad(vab);
				vab += 4;
			}
		}
	}

	// read VAG offset table
	// (stores in original /8 format)
	printf("Reading VAG offset table...\n");
	total = 0;
	for(i=0; i<256; i++) {
		bi = get_bi(vab); vab += 2;
		vab_info.vag_length[i] = bi;
		vab_info.vag_offset[i] = total;
		printf("VAG %d  Length %d  Offset = %d\n", i, vab_info.vag_length[i],
               	vab_info.vag_offset[i]);
   		total += bi;
	}
	vab_info.wavedatasize = total << 3;
	printf("Total wave data size = %d\n", vab_info.wavedatasize);


	// Upload VAG data
	SPU_UploadData((PsxUInt32 *)vab, sb_addr, total);

	// record addr VAB was loaded to
	vab_info.sb_addr = sb_addr;
	
	return (PsxUInt8)vab_info.num_vags;			// return no. of vags in vab...
}

void SPU_VoiceVABSample(PsxUInt8 voice, PsxUInt8 sample) {
	PsxUInt32 addr;

	addr = vab_info.sb_addr + (vab_info.vag_offset[sample] << 3);
	printf("SPU_VoiceVABSample %d  addr = %d\n", sample, addr);
	SPU_VoiceSetData(voice, addr);
}
	

