#pragma once

#define EXPORT
#define CC

struct VabHeader;
struct VabBodyRecord;



// ========== used in psx test app

EXPORT void CC SsVabClose_4FC5B0(int vabId);
EXPORT void CC SsSeqClose_4FD8D0(__int16 idx);
EXPORT void CC SsSeqSetVol_4FDAC0(__int16 idx, __int16 volLeft, __int16 volRight);
EXPORT void CC SsSeqPlay_4FD900(unsigned __int16 idx, char playMode, __int16 repeatCount);
EXPORT void SsVabTransCompleted_4FE060(int immediateFlag);
EXPORT void CC SSInit_4FC230();
EXPORT void CC SsSetTableSize_4FE0B0(void* pTable, int sMax, int tMax);
EXPORT void CC SsSetTickMode_4FDC20(int tickMode);
// SsStart
// SsVabTransfer
EXPORT void CC SsSetMVol_4FC360(__int16 left, __int16 right);

// ==========


EXPORT __int16 CC SsVabOpenHead_4FC620(VabHeader* pVabHeader);
EXPORT void CC SsVabTransBody_4FC840(VabBodyRecord* pVabBody, __int16 vabId);



EXPORT void CC SpuInitHot_4FC320();
EXPORT void SsEnd_4FC350();


EXPORT void SsUtSetReverbDepth_4FE380(int leftDepth, int rightDepth);
EXPORT void CC SsUtSetReverbType_4FE360(int);
EXPORT void SsUtReverbOn_4FE340();
EXPORT void SsUtReverbOff_4FE350();
EXPORT void SpuClearReverbWorkArea_4FA690(int reverbMode);

EXPORT int CC SsVoKeyOn_4FCF10(int vabIdAndProgram, int pitch, unsigned __int16 leftVol, unsigned __int16 rightVol);
EXPORT void CC SsUtAllKeyOff_4FDFE0(int mode);
EXPORT __int16 CC SsUtKeyOffV_4FE010(__int16 idx);
EXPORT __int16 CC SsUtChangePitch_4FDF70(__int16 voice, int /*vabId*/, int /*prog*/, __int16 old_note, __int16 old_fine, __int16 new_note, __int16 new_fine);

EXPORT __int16 CC SsSeqOpen_4FD6D0(unsigned char* pSeqData, __int16 seqIdx);

EXPORT void CC SsSeqStop_4FD9C0(__int16 idx);
EXPORT unsigned __int16 CC SsIsEos_4FDA80(__int16 idx, __int16 seqNum);

/*
EXPORT signed int CC MIDI_ParseMidiMessage_4FD100(int idx);
EXPORT int CC MIDI_Read_Var_Len_4FD0D0(MIDI_SeqSong* pMidiStru);
EXPORT BYTE CC MIDI_ReadByte_4FD6B0(MIDI_SeqSong* pData);
EXPORT void CC MIDI_SkipBytes_4FD6C0(MIDI_SeqSong* pData, int length);
EXPORT void CC MIDI_SetTempo_4FDB80(__int16 idx, __int16 kZero, __int16 tempo);
EXPORT int CC MIDI_PlayerPlayMidiNote_4FCE80(int vabId, int program, int note, int leftVol, int rightVol, int volume);
EXPORT signed int CC MIDI_Allocate_Channel_4FCA50(int not_used, int priority);

using TVSyncCallBackFn = void(CC *)();
EXPORT void CC VSyncCallback_4F8C40(TVSyncCallBackFn callBack);
EXPORT void CC SND_CallBack_4020A4(); // TODO: Naming??
*/
EXPORT void CC SsSeqCalledTbyT_4FDC80();

// Most likely PC specific extensions that have been inlined
//void SsExt_CloseAllVabs();
//void SsExt_StopPlayingSamples();
