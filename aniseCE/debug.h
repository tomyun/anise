#ifndef DEBUG_H
#define DEBUG_H

//#define DEBUG

#ifdef DEBUG
	#define PRINT			printf
	#define PAUSE			{char ch; scanf("%c", &ch);}
#else
	#if defined(_MSC_VER)||defined(_WIN32_WCE)
		#define PRINT		/##/
	#else
		//HACK: for gcc
		#define PRINT(args...)
	#endif
	#define PAUSE
#endif

#define PRINT_ERROR			printf

#endif
