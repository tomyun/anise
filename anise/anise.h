#ifndef ANISE_H
#define ANISE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <assert.h>
#include "debug.h"

#define NONE				0xFFFF

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
