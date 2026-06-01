#ifndef UniversalDef_H
#define UniversalDef_H

#include <math.h>
#include <string.h>

/* 兼容一般程序员的常用写法 */
typedef unsigned char	uchar;
typedef unsigned int	uint;
typedef unsigned long	ulong;
typedef signed char		schar;
typedef signed int		sint;
typedef signed long		slong;

/* 为方便移植，建议使用下面写法 */
typedef unsigned char	bool;
typedef unsigned char	uint8;
typedef unsigned int	uint16;
typedef unsigned long   uint32;
typedef signed  char	sint8;
typedef signed	int		sint16;
typedef signed	long	sint32;
typedef signed  char	int8;
typedef signed  int		int16;
typedef signed  long	int32;
#endif