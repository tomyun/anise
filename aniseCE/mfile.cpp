#include <stdlib.h>
#include <string.h>
#include "mfile.h"
#include "ymf262.h"

#define chStatus (chByte(0x03))
#define chByte(x) (chInfo[ch].byte[x])
#define chWord(x) (chInfo[ch].word[x])
#define OPLWrt(x, y) YMF262Write(0, x, y)

//#define MFILE_TEMPO 1.25 // Original
#define MFILE_TEMPO 1.20

const int MFILE_Buffering = 50;

const unsigned char operParam[4] = {0x08, 0x0A, 0x09, 0x0D};
const unsigned char volumeParam[16] = {
	0x32, 0x2D, 0x29, 0x25, 0x21, 0x1E, 0x1B, 0x18,
	0x15, 0x12, 0x0F, 0x0C, 0x09, 0x06, 0x03, 0x00
};
const unsigned char spkParam[4] = {0x00, 0x50, 0xA0, 0xF0};
const unsigned short noteFreqParam[91] = {
	0x0159, 0x016D, 0x0183, 0x019A, 0x01B3, 0x01CC, 0x01E7, 0x0204,
	0x0223, 0x0244, 0x0266, 0x028B, 0x02B2, 0x02DA, 0x0307, 0x0333,
	0x0366, 0x0398, 0x03CF, 0x0409, 0x0446, 0x0487, 0x04CC, 0x0516,
	0x0565, 0x05B4, 0x060D, 0x0667, 0x06CB, 0x072F, 0x079E, 0x0812,
	0x088B, 0x090F, 0x0998, 0x0A2B, 0x0AC9, 0x0B68, 0x0C1B, 0x0CCE,
	0x0D96, 0x0E5E, 0x0F3C, 0x1024, 0x1116, 0x121E, 0x1330, 0x1457,
	0x1593, 0x16CF, 0x1836, 0x199C, 0x1B2C, 0x1CBD, 0x1E78, 0x2047,
	0x222C, 0x243B, 0x265F, 0x28AE, 0x2B26, 0x2D9E, 0x306B, 0x3338,
	0x3659, 0x397A, 0x3CEF, 0x408F, 0x4458, 0x4876, 0x4CBF, 0x515B,
	0x564C, 0x5B3D, 0x60D6, 0x6670, 0x6CB2, 0x72F4, 0x79DE, 0x811D,
	0x88B1, 0x90ED, 0x997D, 0xA2B6, 0xAC98, 0xB679, 0xC1AC, 0xCCDF,
	0xD963, 0xE5E7, 0xF3BD
};
const unsigned char ch_op_Param[4] = {0x00, 0x03, 0x08, 0x0B};

const int default_tempo = 120;

enum {
	NONE_SONG = 0,
	ELF_SONG = 1,
	SILKYS_SONG = 2
};

INLINE void MFile::OutPort(unsigned char reg, unsigned char val){
	OPLWrt(chInfo[ch].port, reg);
	OPLWrt(1, val);
}

INLINE void MFile::OutPortOp(unsigned char reg, unsigned char val){
	OPLWrt(chInfo[ch].port, reg + chInfo[ch].op);
	OPLWrt(1, val);
}

void MFile::initChip(void){
	OPLWrt(0x00, 0x01);
	OPLWrt(0x01, 0x20);
	OPLWrt(0x00, 0x01);
	OPLWrt(0x01, 0x00);
	OPLWrt(0x02, 0x01);
	OPLWrt(0x03, 0x00);
	OPLWrt(0x02, 0x05);
	OPLWrt(0x03, 0x01);
	OPLWrt(0x02, 0x04);
	OPLWrt(0x03, 0x3F);
	OPLWrt(0x00, 0x08);
	OPLWrt(0x01, 0x00);
}

void MFile::initChannel(void){
	chInfo[ch].port = int(ch / 3) * 2;
	chInfo[ch].op = ch % 3;
	OutPortOp(0xB0, 0x00);
	OutPortOp(0x20, 0x00);
	OutPortOp(0x23, 0x00);
	OutPortOp(0x28, 0x00);
	OutPortOp(0x2B, 0x00);
	OutPortOp(0x80, 0x0F);
	OutPortOp(0x83, 0x0F);
	OutPortOp(0x88, 0x0F);
	OutPortOp(0x8B, 0x0F);
}

INLINE unsigned char MFile::getByte(void){
	return unsigned char(Music[chInfo[ch].filePos++]);
}

INLINE char MFile::getChar(void){
	return Music[chInfo[ch].filePos++];
}

INLINE unsigned short MFile::getWord(void){
	unsigned short ret = getByte();
	return ret | (unsigned short(getByte()) << 8);
}

INLINE short MFile::getInt16(void){
	unsigned short ret = getByte();
	return short(ret | (short(getByte()) << 8));
}

void MFile::setPreset(int id){
	unsigned char *preset = MusicPreset;

	chByte(0x3C) = id;
	while(preset[0x15] != id)preset += 30;
	chByte(0x3D) = preset[0x14];
	int op_on = operParam[preset[0x14] & 3];
	int base_vol = volumeParam[chByte(0x40)];
	int i, vol;

	for(i=0;i<4;i++){
		OutPortOp(0x20+ch_op_Param[i], *preset++);
		vol = chInfo[ch].subVol[i] = *preset++;
		if(op_on&1){
			vol = (vol&0x3F) + base_vol;
			if(vol > 0x3F)
				vol = 0x3F;
			vol |= chInfo[ch].subVol[i] & 0xC0;
		}
		OutPortOp(0x40+ch_op_Param[i], vol);
		OutPortOp(0x60+ch_op_Param[i], *preset++);
		OutPortOp(0x80+ch_op_Param[i], *preset++);
		OutPortOp(0xE0+ch_op_Param[i], *preset++);
		op_on >>= 1;
	}

	preset += 2;
	if(preset[1] != 0){
		chByte(0x2A) = *preset & 3;
		chByte(0x2F) = preset[1];
		chWord(0x3A) = preset[2] | (preset[3] << 8);
		chByte(0x2C) = preset[4];

		chByte(0x23) |= 2;
		chByte(0x2E) = chByte(0x2F);
		chByte(0x2B) = 0;
		if(chByte(0x2A) == 0){
			chWord(0x34) = chWord(0x3A) / chByte(0x2F);
			chWord(0x36) = chWord(0x3A) % chByte(0x2F);
		} else if(chByte(0x2A) == 1){
			chWord(0x34) = chWord(0x3A);
		} else {
			chWord(0x34) = chWord(0x3A) / chByte(0x2F);
			chWord(0x36) = chWord(0x3A) % chByte(0x2F);
			chWord(0x3A) = 0;
		}
	} else {
		chByte(0x23) &= 0xFD;
	}
	preset += 5;

	if((*preset != 0)&&(preset[1] != 0)){
		chByte(0x4B) = *preset++;
		chByte(0x4D) = *preset++;
		chByte(0x48) = *preset++;

		chByte(0x23) |= 4;
		chByte(0x4F) = 0;

		if(chByte(0x4D) > chByte(0x4B)){
			chByte(0x4D) /= chByte(0x4B);
		} else if(chByte(0x4D) == chByte(0x4B)){
			chByte(0x4D) = 1;
		} else {
			chByte(0x4F) = chByte(0x4B) / chByte(0x4D);
		}
	} else {
		chByte(23) &= 0xFB;
	}

	unsigned char spk = spkParam[chByte(0x3E)];
	unsigned char fb = chByte(0x3D);
	unsigned char cnt = fb & 1;
	fb = (fb >> 1) | spk;
	OutPortOp(0xC3, (fb & 0xFE) | cnt);
	OutPortOp(0xC0, fb);
	return;
}

void MFile::setVolume(void){
	int op_on = operParam[chByte(0x3D) & 3];
	int base_vol = volumeParam[chByte(0x40)];
	chByte(0x3F) = base_vol;
	int i, vol;

	for(i=0;i<4;i++){
		vol = chInfo[ch].subVol[i];
		if(op_on&1){
			vol = (vol&0x3F) + base_vol;
			if(vol > 0x3F)
				vol = 0x3F;
			vol |= chInfo[ch].subVol[i] & 0xC0;
			OutPortOp(0x40+ch_op_Param[i], vol);
		}
		op_on >>= 1;
	}
}

void MFile::setSpeaker(unsigned char spk){
	chByte(0x3E) = spk;

	spk = spkParam[chByte(0x3E)];
	unsigned char fb = chByte(0x3D);
	unsigned char cnt = fb & 1;
	fb = (fb >> 1) | spk;
	OutPortOp(0xC3, (fb & 0xFE) | cnt);
	OutPortOp(0xC0, fb);
}

void MFile::setDepth(unsigned char depth){
	depth <<= 6;
	regBD = (regBD & 0x3F) | depth;
	OPLWrt(0x00, 0xBD);
	OPLWrt(0x01, depth);
}

void MFile::OpCode(unsigned char op){
	switch(op){
	case 0x81:
		setPreset(getByte());
		break;
	case 0x83:
		chStatus |= 0x04;
		break;
	case 0x84: // Tempo : (getWord() * 0.8) Hz
		tempo = TempoBase / getWord();
		break;
	case 0x85:
		chByte(0x09) = getByte() & 7;
		break;
	case 0x87:{
		unsigned char val = getByte();
		if(!(chStatus & 20)){
			chByte(0x40) = val;
			setVolume();
		}
		break;
		}
	case 0x88:
		baseNote = getByte();
		break;
	case 0x89:
		setSpeaker(getByte());
		break;
	case 0x8A:
		chByte(0x41) = getByte()-1;
		break;
	case 0x8B:
		chWord(0x10) = getWord();
		break;
	case 0x8C:
		if(chByte(0x41) != 7)
			chByte(0x41)++;
		break;
	case 0x8D:
		if(chByte(0x41) != 0)
			chByte(0x41)--;
		break;
	case 0x8E:
		if((!(chStatus & 20))&&(chByte(0x40) < 0x0F)){
			chByte(0x40)++;
			setVolume();
		}
		break;
	case 0x8F:
		if((!(chStatus & 20))&&(chByte(0x40) > 0x00)){
			chByte(0x40)--;
			setVolume();
		}
		break;

	case 0x90:{
		unsigned char val1 = getByte();
		unsigned char val2 = getByte();
		unsigned char val3 = getByte();
		chByte(0x06) = val3;
		chByte(0x07) = 0;

		val1 += baseNote;
		val2 += baseNote;
		chWord(0x0B) = noteFreqParam[val1*2];
		chWord(0x28) = noteFreqParam[val2*2];

		chWord(0x26) = (chWord(0x28) - chWord(0x0B)) * val3;
		chByte(0x23) |= 0x01;
		chByte(0x03) &= 0xFE;
		chByte(0x03) |= 0x02;
		break;
		}
	case 0x91:{
		unsigned short val1 = getWord();
		unsigned short val2 = getWord();
		if((val1 >> 8) != 0){
			chByte(0x2A) = val1 & 0xFF;
			chByte(0x2F) = val1 >> 8;
			chWord(0x3A) = val2;

			chByte(0x23) |= 2;
			chByte(0x2E) = chByte(0x2F);
			chByte(0x2B) = 0;
			if(chByte(0x2A) == 0){
				chWord(0x34) = chWord(0x3A) / chByte(0x2F);
				chWord(0x36) = chWord(0x3A) % chByte(0x2F);
			} else if(chByte(0x2A) == 1){
				chWord(0x34) = chWord(0x3A);
			} else {
				chWord(0x34) = chWord(0x3A) / chByte(0x2F);
				chWord(0x36) = chWord(0x3A) % chByte(0x2F);
				chWord(0x3A) = 0;
			}
		}
		break;
		}
	case 0x92:
		chByte(0x23) |= 0x02;
		break;
	case 0x93:
		chByte(0x23) &= 0xFD;
		break;
	case 0x94:
		chByte(0x2C) = getByte();
		break;
	case 0x95:
		setDepth(getByte() & 3);
		break;

	case 0xF0:
		chStatus &= 0x7F;
		initChannel();
		break;
	case 0xF1:
		chInfo[ch].filePos += getInt16() - 2;
		break;
	case 0xF2:{
		unsigned short base = chInfo[ch].filePos;
		short int val = getInt16();
		if(unsigned char(val & 0xFF) >= unsigned char(val >> 8)){
			Music[base] = 0;
		} else {
			Music[base]++;
		}

		int cnt = val & 0xFF;
		for(int i=0;i<cnt;i++)
			val = getInt16();

		chInfo[ch].filePos = base + val;
		break;
		}
	case 0xF3:{
		unsigned short val = getWord();
		if((val & 0xFF) > 1){
			Music[chInfo[ch].filePos-2] = unsigned char(val && 0xFF) - 1;
			getWord();
		} else {
			Music[chInfo[ch].filePos-2] = unsigned char(val >> 8);
			chInfo[ch].filePos += getInt16() - 4;
		}
		break;
		}
	case 0xF4:
		chByte(0x12)++;
		chByte(0x13 + chByte(0x12)*2) = getByte();
		break;
	case 0xF5:
		if(chByte(0x13 + chByte(0x12)*2) == 1){
			chInfo[ch].filePos += getInt16() - 2;
			chByte(0x12)--;
		} else {
			getWord();
		}
		break;
	case 0xF6:
		if((--chByte(0x13 + chByte(0x12)*2)) != 0){
			chInfo[ch].filePos += getInt16() - 2;
		} else {
			getWord();
			chByte(0x12)--;
		}
		break;
	case 0xF7:
		if(chByte(0x13 + chByte(0x12)*2) == 1){
			chInfo[ch].filePos += getChar() - 1;
			chByte(0x12)--;
		} else {
			getByte();
		}
		break;
	case 0xF8:
		if((--chByte(0x13 + chByte(0x12)*2)) != 0){
			chInfo[ch].filePos += getChar() - 1;
		} else {
			getByte();
			chByte(0x12)--;
		}
		break;
	case 0xF9:{
		unsigned char val = getByte();
		OutPortOp(getByte(), val);
		break;
	}
	default:
		printf("Not processing OpCode %2x\n", op);
	}
}

void MFile::NoteToFreq(void){
	chWord(0x0B) = noteFreqParam[chByte(0x0A) + baseNote];
	chStatus |= 0x02;
}

void MFile::setupMusic(void){
	int pos;

	switch(mType){
	case ELF_SONG:
		pos = unsigned short(Music[0x20 + ch*2]) | unsigned short(Music[0x21 + ch*2] << 8);
		break;
	case SILKYS_SONG:
		pos = unsigned short(Music[0x10 + ch*2]) | unsigned short(Music[0x11 + ch*2] << 8);
		break;
	case NONE_SONG:
		return;
	}

	memset(&chInfo[ch], 0, sizeof(chInfo[ch]));
	chInfo[ch].filePos = pos;
	chInfo[ch].port = int(ch / 3) * 2;
	chInfo[ch].op = ch % 3;
	chStatus = 0x80;
	chByte(0x40)= 0x0A;
	chByte(0x3E) = 0x03;

	while(true){
		unsigned char op = getByte();
		if(op > 0x80){
			OpCode(op);
		} else if(op == 0x80){
			chByte(0x06) = getByte();
			break;
		} else {
			chByte(0x0A) = op;
			chByte(0x06) = getByte();
			if(chByte(0x09) == 0){
				chByte(0x07) = 1;
			} else {
				int cnt = int(chByte(0x06)) * int(chByte(0x09));
				cnt = chByte(0x06) - (cnt >> 3);
				chByte(0x07) = cnt;
			}
			NoteToFreq();
			break;
		}
	}

	return;
}


void MFile::NoteOnSub(void){
	int base_vol = char(chByte(0x3F)) - char(chByte(0x4E));
	if(base_vol < 0){
		base_vol = 0;
	} else if (base_vol > 0x3F){
		base_vol = 0x3F;
	}

	int op_on = operParam[chByte(0x3D) & 3];
	int i, vol;

	for(i=0;i<4;i++){
		vol = chInfo[ch].subVol[i];
		if(op_on&1){
			vol = (vol&0x3F) + base_vol;
			if(vol > 0x3F)
				vol = 0x3F;
			vol |= chInfo[ch].subVol[i] & 0xC0;
		}
		OutPortOp(0x40+ch_op_Param[i], vol);
		op_on >>= 1;
	}
}

void MFile::NoteOn(void){
	if(!(chStatus & 0x80))return;
	unsigned short note = 0;
	if(!(chStatus & 0x02)){
		if(chByte(0x23) & 0x04){
			if((chByte(0x49) == 0)||(chByte(0x49) == 1)){
				chByte(0x49) = 0;
				if(--chByte(0x4A)){
					if((chByte(0x50) == 0)||(chByte(0x50) == 1)){
						chByte(0x50) = chByte(0x4F);
						chByte(0x4E) += chByte(0x4C);
						NoteOnSub();
					} else {
						chByte(0x50)--;
					}
				} else {
					chByte(0x50) = chByte(0x4F);
					chByte(0x4A) = chByte(0x4B);
					chByte(0x4C) *= -1;
					chByte(0x4E) += chByte(0x4C);
					NoteOnSub();
				}
			} else {
				chByte(0x49)--;
			}
		}
		if(chByte(0x23) & 0x02){
			if((chByte(0x2D) == 0)||(chByte(0x2D) == 1)){
				chByte(0x2D) = 0;
				switch(chByte(0x2A)){
				case 0:
					if(--chByte(0x2E)){
						chWord(0x38) += chWord(0x30);
						note += chWord(0x38);
					} else {
						chByte(0x2E) = chByte(0x2F);
						chWord(0x38) += chWord(0x3A);
						note += chWord(0x38);
					}
					break;
				case 1:
					if(!(--chByte(0x2E))){
						chByte(0x2E) = chByte(0x2F);
						chWord(0x3A) *= -1;
					}
					note += chWord(0x3A);
					break;
				case 2:
					if(!(--chByte(0x2E))){
						chWord(0x38) += chWord(0x30);
						note += chWord(0x38);
					} else {
						chByte(0x2E) = chByte(0x2F);
						chByte(0x2B) = (chByte(0x2B) + 1) & 3;
						if(!(chByte(0x2B)&1)){
							chWord(0x38) = 0;
						} else {
							chWord(0x38) += chWord(0x30) + chWord(0x32);
							note += chWord(0x38);
							chWord(0x30) *= -1;
							chWord(0x32) *= -1;
						}
					}
					break;
				}
			} else {
				chByte(0x2D)--;
			}

		}
		if(chByte(0x23) & 0x01){
			if(chByte(0x06) > 1){
				chWord(0x24) += chWord(0x26);
				note += chWord(0x24);
			} else {
				chWord(0x0B) = chWord(0x28);
				chByte(0x23) &= 0xFE;
			}
		}
		if(!note)return;
	} else {
		chStatus = (chStatus & 0xFD) | 1;
		if(chByte(0x23) & 2){
			chByte(0x2B) = 0;
			chByte(0x2E) = chByte(0x2F);
			chWord(0x30) = chWord(0x34);
			chWord(0x32) = chWord(0x36);
			chByte(0x2D) = chByte(0x2C);
			chWord(0x38) = chWord(0x3A);
		}
		if(chByte(0x34) & 4){
			chByte(0x4A) = chByte(0x4B);
			chByte(0x49) = chByte(0x48);
			chByte(0x50) = chByte(0x4F);
			chByte(0x4C) = chByte(0x4D);
			chByte(0x4E) = 0;
		}
		chWord(0x24) = 0;
	}
	note += chWord(0x10) + chWord(0x0B);
	unsigned char oct = 0;
	while(note > 0x3FF){
		note >>= 1;
		oct++;
	}
	chByte(0x0E) = note & 0xFF;
	OutPortOp(0xA0, chByte(0x0E));
	chByte(0x0F) = (note >> 8) | (oct << 2);
	if(chStatus & 0x01){
		OutPortOp(0xB0, chByte(0x0F) | 0x20);
	} else {
		OutPortOp(0xB0, chByte(0x0F));
	}
}

void MFile::NoteOff(void){
	if(chStatus & 0x01){
		chStatus &= 0xF8;
		OutPortOp(0xA0, chByte(0x0E));
		OutPortOp(0xB0, chByte(0x0F));
	}
}

void MFile::PlaySound(void){
	if(!(chStatus & 0x80))return;

	if((chStatus & 0x01)&&(!(chStatus & 0x04))&&
		(chByte(0x06) == chByte(0x07))){
		NoteOff();
	}

	if((--chByte(0x06)) > 0)return;

	if(chStatus & 0x03){
		chByte(0x23) |= 0x08;
		chStatus |= 0x08;
		chStatus &= 0xFB;
	} else {
		chStatus&= 0xF7;
		NoteOff();
	}

	chByte(0x23) &= 0xFE;

	while(true){
		unsigned char op = getByte();
		if (op > 0x80) {
			OpCode(op);
		} else if(op == 0x80){
			chByte(0x06) = getByte();
			chStatus &= 0xF8;
			break;
		} else {
			if(chByte(0x0A) != op){
				chByte(0x23) &= 0xF7;
				chStatus &= 0xF7;
				chByte(0x0A) = op;
			}
			chByte(0x06) = getByte();
			if(!(chStatus & 0x04)){
				if(chByte(0x09) == 0){
					chByte(0x07) = 1;
				} else {
					int cnt = chByte(0x06) * chByte(0x09);
					cnt = chByte(0x06) - (cnt >> 3);
					chByte(0x07) = cnt;
				}
			}
			if(!(chStatus & 0x08)){
				NoteToFreq();
			}
			break;
		}
	}

	return;
}

bool MFile::getbuf(void){
	bool cont;

	if(!(playStatus & 0x80)){
		return false;
	}

	for(int i=0;i<MFILE_Buffering;i++){
		cont = true;

		if(!(playStatus & 0x80)){
			return true;
		}

		if(playStatus & 0x04){
			playStatus &= 0xFB;
			playStatus |= 0x02;
		} else {
			if(!(playStatus & 0x02)){
				cont = false;
			}
		}

		if(cont){
			for(ch=0;ch<6;ch++){
				PlaySound();
				NoteOn();
			}

			if(playStatus & 20){
				for(ch=0;ch<6;ch++){
					if((!(chStatus & 20))&&(chByte(0x40) > 0x00)){
						chByte(0x40)--;
						setVolume();
					}
				}
			}
		}

		OPLWrt(0xFF, tempo);
	}
	
	return true;
}

bool MFile::callback(void *pointer){
	return ((MFile *)pointer)->getbuf();
}

void MFile::getpcm(short *pcm, unsigned int samples) {
	if(active)
		YMF262UpdateOne(0, pcm, samples);
}

bool MFile::load(const char *file_name)
{
	if(memory){
		delete memory;
		memory = NULL;
	}

	file->open(file_name);
	memory = new MemoryBlock(0, file->getSize());
	file->load(memory, 0);
	file->close();

	Music = memory->getRaw();

	if(strncmp((char *)Music, "MUSIC DRV", 9) == 0){
		mType = ELF_SONG;
		MusicPreset = Music + (unsigned short(Music[0x2C]) | unsigned short(Music[0x2D] << 8));
	} else if(strncmp((char *)Music, "OPL3 DATA", 9) == 0){
		mType = SILKYS_SONG;
		MusicPreset = Music + (unsigned short(Music[0x1C]) | unsigned short(Music[0x1D] << 8));
	} else {
		mType = NONE_SONG;
		delete memory;
		memory = NULL;
		stop();
		return false;
	}
	
	playStatus &= 0x7F;

	YMF262BufferClear();

	initChip();

	for(ch=0;ch<6;ch++){
		initChannel();
	}
	setDepth(0);

	for(ch=0;ch<6;ch++)setupMusic();
	playStatus &= 0xDD;
	playStatus |= 0x04;

	return true;
}

void MFile::play(void){
	playStatus |= 0x80;
}

void MFile::stop(void){
	YMF262BufferClear();
	for(ch=0;ch<6;ch++){
		initChannel();
	}
	playStatus &= 0x7F;
}

MFile::MFile(File *file, Option *option) {
	this->file = file;
	this->option = option;

	memory = NULL;
	Music = NULL;
	mType = 0;
	ch = 0;
	regBD = 0;
	baseNote = 0;
	playStatus = 0;

	tempo = TempoBase / default_tempo;
	active = true;
	TempoBase = int(option->sound_freq * MFILE_TEMPO);

	YMF262Init(1, 14400000, option->sound_freq);
	YMF262SetGetBuf(callback, this);
}

MFile::~MFile(void) {
	playStatus &= 0x7F;
	active = false;

	if(memory)
		delete memory;

	YMF262Shutdown();
}
