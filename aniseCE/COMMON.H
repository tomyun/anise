#define	SUCCESS		0
#define	FAILURE		1

#ifndef PI
#define PI 3.14159265357989
#endif

#ifndef LOADINTELDWORD
#define	LOADINTELDWORD(a)		(((DWORD)(a)[0]) | ((DWORD)(a)[1] << 8) |	\
								((DWORD)(a)[2] << 16) | ((DWORD)(a)[3] << 24))
#endif

#ifndef LOADINTELWORD
#define	LOADINTELWORD(a)		(((WORD)(a)[0]) | ((WORD)(a)[1] << 8))
#endif

#ifndef STOREINTELDWORD
#define	STOREINTELDWORD(a, b)	*((a)+0) = (BYTE)((b));			\
								*((a)+1) = (BYTE)((b)>>8);		\
								*((a)+2) = (BYTE)((b)>>16);		\
								*((a)+3) = (BYTE)((b)>>24)
#endif

#ifndef STOREINTELWORD
#define	STOREINTELWORD(a, b)	*((a)+0) = (BYTE)((b));			\
								*((a)+1) = (BYTE)((b)>>8)
#endif



#ifndef STRLITERAL
#ifdef UNICODE
#define	STRLITERAL(string)		_T(string)
#else
#define	STRLITERAL(string)		(string)
#endif
#endif

#define	_MEM_INIT()				
#define	_MALLOC(a, b)			malloc(a);
#define	_MFREE(a)				free(a);
#define	_HANDLE_ADD(a, b)		
#define	_HANDLE_REM(a)			
#define	_MEM_USED(a)			
