#ifndef DEBUG_H
#define DEBUG_H

//#define DEBUG

#if defined(DEBUG)
	#include <stdio.h>
	#define PRINT		printf
	#define PAUSE		{char ch; scanf("%c", &ch);}
#else
	#define PRINT		/##/		//TODO: is this right?
	#define PAUSE
#endif

#define PRINT_ERROR		printf

#endif
