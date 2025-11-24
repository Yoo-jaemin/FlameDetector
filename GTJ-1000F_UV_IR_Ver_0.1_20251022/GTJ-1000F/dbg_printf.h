/*

*/

#ifndef __DBG_PRINTF_H
#define __DBG_PRINTF_H

#include <stdio.h>
//#include <avr/pgmspace.h>

#define _DEBUG_BUILD 

#ifdef _DEBUG_BUILD

#define DPRINT(str) (			\
{					\
	printf(str);			\
}					\
)

#define DPRINT1(str,arg1) (		\
{					\
	printf(str,arg1);		\
}					\
)

#define DPRINT2(str,arg1,arg2) (	\
{					\
	printf(str,arg1,arg2);		\
}					\
)

#define DPRINT3(str,arg1,arg2,arg3) (	\
{					\
	printf(str,arg1,arg2,arg3);	\
}					\
)

#define DPRINT4(str,arg1,arg2,arg3,arg4) (	\
{						\
	printf(str,arg1,arg2,arg3,arg4);	\
}						\
)

#else
	
	#define DPRINT(x) ({})
	
	#define DPRINT1(x,arg1) ({})
	
	#define DPRINT2(x,arg1,arg2) ({})
	
	#define DPRINT3(x,arg1,arg2,arg3) ({})
	
	#define DPRINT4(x,arg1,arg2,arg3,arg4) ({})
	
#endif

#endif
