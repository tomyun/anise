#ifndef ANISE_H
#define ANISE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#ifndef _WIN32_WCE
	#include <assert.h>
#endif
#include "debug.h"

//HACK: isolate experimental code for faster field operation
//#define FIELD_EXPERIMENT

#define NONE			0xFFFF

#define MASK_UPPER_BYTE		0xF0
#define MASK_LOWER_BYTE		0x0F
#define MASK_UPPER_WORD		0xFF00
#define MASK_LOWER_WORD		0x00FF

typedef unsigned char bit;
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

enum BitFlow {
	BIT_FLOW_RIGHT = 0,
	BIT_FLOW_LEFT = 1
};

#endif
